#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "linkedlist.h"

#define DEFAULT_ITEM_NUM 6
using namespace std;

struct nodeItem {
	float index;
	int blockNum;
};

struct Student {
	char name[20];
	unsigned studentID;   //key for hashing
	float score;   //index for indexing
	unsigned advisorID;
};

class treeNode {
	bool leafNode;
	nodeItem **item;
	treeNode *next;
	treeNode **childs;
	treeNode *upper;
	int size;
	int itemNum;
	int k;;
public:
	treeNode() {
		leafNode = true;
		item = NULL;
		next = NULL;
		childs = NULL;
		upper = NULL;
		size = 0;
		itemNum = 0;
	}
	treeNode(int s) {
		leafNode = true;
		item = NULL;
		next = NULL;
		childs = NULL;
		upper = NULL;
		size = s;
		itemNum = 0;
	}
	bool IsFull();
	//bool IsLeaf();
	int findBlockNum(float i);
	void setSize(int s) {
		size = s;
	}
	void setNext(treeNode *t) {
		next = t;
	}
	void setUpper(treeNode *t) {
		upper = t;
	}
	void setChilds() {
		childs = new treeNode*[DEFAULT_ITEM_NUM + 1];
		for (int i = 0; i < DEFAULT_ITEM_NUM; i++) {
			childs[i] = NULL;
		}
	}
	void setIsLeaf(bool l) {
		leafNode = l;
	}
	treeNode **getChilds() {
		return childs;
	}
	treeNode *getNext() {
		return next;
	}
	treeNode *getUpper() {
		return upper;
	}
	nodeItem **getItem() {
		return item;
	}
	bool getIsLeaf() {
		return leafNode;
	}
	/*
	int getItemNum() {
		return itemNum;
	}*/
	int getSize() {
		return size;
	}
	bool isChildNull() {
		for (int i = 0; i < size + 1; i++) {
			if (childs[i] != NULL) {
				return true;
			}
		}
		return false;
	}
	void insertItem(nodeItem *i);
	void sortNode();
	int checkChildsPtrNum(nodeItem *i);

	int inputK();
	void findKthLeafNode(treeNode *tmp);
};

bool treeNode::IsFull() {
	bool result = false;

	if (itemNum == DEFAULT_ITEM_NUM) {
		result = true;
	}

	return result;
}

int treeNode::findBlockNum(float i) {
	for (int i = 0; i < size; i++) {
		if (item[i]->index == i) {
			return item[i]->blockNum;
		}
	}
}

void treeNode::sortNode() {   //node 내부 item들을 index로 오름차순 sort
	for (int i = 0; i < itemNum - 1; i++) {
		for (int j = i; j < itemNum; j++) {
			float tmp = item[i]->index;
			float tmp2 = item[j]->index;
			if (tmp - tmp2 > 0.000001) {
				nodeItem *t = item[i];
				item[i] = item[j];
				item[j] = t;
			}
		}
	}

	cout << "sort result :";
	for (int i = 0; i < itemNum; i++) {
		cout << " " << item[i]->index;
	}
	cout << endl;
}

void treeNode::insertItem(nodeItem *i) {
	if (item == NULL) {
		item = new nodeItem*[DEFAULT_ITEM_NUM];
		for (int j = 0; j < DEFAULT_ITEM_NUM; j++) { //초기화
			item[j] = NULL;
		}
		item[0] = new nodeItem;
		item[0] = i;
		itemNum = itemNum + 1;
	}
	else {
		for (int j = 0; j < DEFAULT_ITEM_NUM; j++) {
			if (item[j] == NULL) {
				item[j] = new nodeItem;
				item[j] = i;
				itemNum = itemNum + 1;
				sortNode();
				break;
			}
		}
	}
}

int treeNode::checkChildsPtrNum(nodeItem *i) {
	int result;
	int tmp = i->index;

	if (item[0]->index > tmp) {
		result = 0;
	}
	else if (item[itemNum - 1]->index <= tmp) {
		result = itemNum;
	}
	else {
		for (int j = 0; j < itemNum; j++) {
			if (item[j]->index < tmp && item[j + 1]->index > tmp) {
				result = j;
				break;
			}
		}
	}

	return result;
}


int treeNode::inputK() {
	cout << "몇번째 leaf node 찾을건가요?";
	cin >> k;
	return k;
}

void treeNode::findKthLeafNode(treeNode *tmp) {
	while (tmp->getChilds() != NULL) {
		tmp = tmp->getChilds()[0]; //leaf Node찾기
	}

	for (int i = 0; i < k; i++) {
		tmp = tmp->getNext();
	}
	cout << k << "번째 leaf node : " << tmp->getItem()[tmp->itemNum)]->index << endl;

	int bkn = findBlockNum(tmp->getItem()[tmp->ItemNum()]->index);

	cout << "block number : " << bkn;
}


class BplusTree {
	treeNode *root;
	int depth;
	friend treeNode;
	//LinkedList *leaf;
public:
	BplusTree() {
		root = new treeNode(DEFAULT_ITEM_NUM);
		depth = 0;
	}
	~BplusTree() {
		deleteTree(root);
	}
	void deleteTree(treeNode *tmp);   //childs에 대해서 수정필요
	void setTree(ifstream &fp);
	treeNode* insertItem(nodeItem *i, treeNode *k);
	//void deleteItem(nodeItem *i);
	//int findBlockNum(float i);   //index를 이용해 해당 block number 찾기
	treeNode* split(treeNode *tmp);
	//void printLeafList();
	bool IsFull(treeNode *tmp) {
		return tmp->IsFull();
	}
};

void BplusTree::deleteTree(treeNode *tmp) {
	if (tmp->getNext() != NULL) {
		deleteTree(tmp->getNext());
	}
	delete tmp;
}

treeNode* BplusTree::insertItem(nodeItem *i, treeNode *k) {
	treeNode *t = k;
	treeNode *tmp = t;

	if (depth == 0) {
		t->insertItem(i);
	}
	else {
		int ptr = t->checkChildsPtrNum(i);

		if (t->getChilds() == NULL) {
			t->setChilds();
		}

		treeNode **n = t->getChilds();

		if (n[ptr] == NULL) {
			n[ptr] = new treeNode(DEFAULT_ITEM_NUM);
		}

		t = n[ptr];
		//insertItem(i, t);
		t->insertItem(i);
	}

	if (t->IsFull()) {
		tmp = split(t);
	}

	k = tmp;

	return k;
}
/*
void BplusTree::split(treeNode *tmp) {   //수정할 것
int p = tmp->getSize() / 2;
if (p % 2 == 0) {
p = p - 1;
}

nodeItem *t = new nodeItem();
t = tmp->getItem()[p];

treeNode *newt = new treeNode(DEFAULT_ITEM_NUM);   //child 두번째

for (int i = p; i < DEFAULT_ITEM_NUM; i++) {   //쪼갬
newt[i - p] = tmp[i];
tmp[i] = NULL;
}

treeNode *newu;

if (tmp->getUpper() == NULL) {
newu = new treeNode(DEFAULT_ITEM_NUM);   //upper
}
else {
newu = tmp->getUpper();
}

newu->insertItem(t);
newu->setChilds();

treeNode **newc = newu->getChilds();

for (int i = 0; i < DEFAULT_ITEM_NUM; i++) {
if (newc[i] == NULL) {
newu->setChilds(i, tmp, newt);
break;
}
}

newt->setUpper(newu);
tmp->setUpper(newu);

tmp->setNext(newt); //linked list 연결

depth = depth + 1;
}
*/
treeNode* BplusTree::split(treeNode *tmp) {   //스플릿
	int p;   //split될 기준 item 위치
	p = tmp->getSize() / 2;

	nodeItem **t = tmp->getItem();   //tmp의 item 배열

	treeNode *newup;   //upper node
	if (tmp->getUpper() == NULL) {
		newup = new treeNode(DEFAULT_ITEM_NUM);
	}
	else {
		newup = tmp->getUpper();   //기존 upper node
	}

	newup->insertItem(t[p]);   //upper node에 기준 item insert

	tmp = newup;   //tmp를 newup으로 교체

	nodeItem **k = tmp->getItem();
	int ptrUp;   //split 기준 item이 newup에 들어간 위치
	for (int i = 0; i < tmp->getItemNum(); i++) {
		if (k[i] == t[p]) {
			ptrUp = i;
			break;
		}
	}

	//찾은 ptrUp 위치에 childs node 설정
	if (tmp->getChilds() == NULL) {
		tmp->setChilds();
	}
	if (tmp->getChilds()[ptrUp] == NULL) {
		tmp->getChilds()[ptrUp] = new treeNode(DEFAULT_ITEM_NUM);
	}
	if (tmp->getChilds()[ptrUp + 1] == NULL) {
		tmp->getChilds()[ptrUp + 1] = new treeNode(DEFAULT_ITEM_NUM);
	}

	if (ptrUp >= 1) {
		tmp->getChilds()[ptrUp] = new treeNode();
		tmp->getChilds()[ptrUp + 1] = new treeNode();
	}
	for (int j = 0; j < p; j++) {
		tmp->getChilds()[ptrUp]->insertItem(t[j]);
	}
	for (int j = p; j < tmp->getSize(); j++) {
		tmp->getChilds()[ptrUp + 1]->insertItem(t[j]);
	}

	if (tmp->getChilds()[ptrUp]->IsFull() == true) {
		split(tmp->getChilds()[ptrUp]);
	}
	if (tmp->getChilds()[ptrUp + 1]->IsFull() == true) {
		split(tmp->getChilds()[ptrUp + 1]);
	}

	tmp->getChilds()[ptrUp]->setUpper(tmp);
	tmp->getChilds()[ptrUp + 1]->setUpper(tmp);

	tmp->setIsLeaf(false);

	depth = depth + 1;

	return tmp;
}

void BplusTree::setTree(ifstream &fp) {
	if (fp.is_open() != true) {
		cout << "error\n";
		return;
	}

	/*
	ofstream fp2;
	fp2.open("Students_score.idx", ios::binary);

	if (fp2.is_open() != true) {
	cout << "error2\n";
	return;
	}
	*/

	int num;   //number of student records
	string buffer;
	getline(fp, buffer);
	stringstream str(buffer);
	str >> num;
	cout << "number of data : " << num << endl;

	Student std;

	for (int i = 0; i < num; i++) {
		/* 1. Student 데이터를 input file에서 읽어와 타입 변환 */
		string buffer2;
		getline(fp, buffer2);
		cout << buffer2 << endl;
		stringstream str2(buffer2);

		string wholename;
		string lastname;
		unsigned stdID;
		float sc;
		unsigned admID;

		str2 >> wholename >> lastname;

		char* tmp = new char[lastname.length() + 1];
		strcpy(tmp, lastname.c_str());

		char** ptr = new char*[4];
		ptr[0] = strtok(tmp, ",");
		lastname = ptr[0];

		wholename = wholename + " " + lastname;
		cout << "name : " << wholename << endl;

		int j = 0;
		while (ptr[j] != NULL) {
			//cout<<ptr[j]<<endl;
			j++;
			ptr[j] = strtok(NULL, ","); 
		}

		stdID = atoi(ptr[1]);
		cout << "studentID : " << stdID << endl;
		sc = atof(ptr[2]);
		cout << "score : " << sc << endl;
		admID = atoi(ptr[3]);
		cout << "adminID : " << admID << endl << endl;

		/* 2. 타입 변환된 데이터를 Student struct에 저장 */
		strcpy(std.name, wholename.c_str());
		std.studentID = stdID;
		std.score = sc;
		std.advisorID = admID;

		/* 3. nodeItem 설정 */

		nodeItem *ntmp = new nodeItem;
		ntmp->index = std.score;

		/* 4. hashing으로 blockNum 설정
		* 임시로 hash function 사용 -> k % 32
		*/
		int bn = std.studentID % 32;
		ntmp->blockNum = bn;

		/* 5. b+tree에 ntmp insert */
		root = insertItem(ntmp, root);
	}

	/* Students_score.idx에 쓰기
	for (int i = 0; i < num; i++) {

	}*/
}

int main() {
	ifstream fp;
	fp.open("sampleData.csv");

	BplusTree tree;
	tree.setTree(fp);

	return 0;
}
