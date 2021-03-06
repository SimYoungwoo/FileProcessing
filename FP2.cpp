#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#define DEFAULT_TABLE_SIZE 8
#define MAX_BLOCK_SIZE 4096
using namespace std;

class Node {	//for hash
private:
	Node *next;	//다음 노드(아이템)
	unsigned key;	//studentID
public:
	Node() {
		next = NULL;
		key = -1;
	}
	Node(unsigned k) {
		next = NULL;
		key = k;
	}
	Node *getNext() {
		return next;
	}
	unsigned getKey() {
		return key;
	}
	void setNext(Node *n) {
		next = n;
	}
	void setKey(unsigned k) {
		key = k;
	}
};

class LinkedList {
private:
	int blockNum;	//해당 리스트가 속한 블럭넘버
	int numItem;	//해당 리스트에 들어있는 아이템(노드) 수
	int size;	//해당 리스트의 크기 : 4k를 넘으면 overflow 발생
	Node *first;
public:
	LinkedList() {
		blockNum = -1;
		numItem = 0;
		size = 0;
		first = NULL;
	}
	LinkedList(int b) {
		blockNum = b;
		numItem = 0;
		size = 0;
		first = NULL;
	}
	~LinkedList() {
		deleteList(first);
	}
	void deleteList(Node *t) {
		if (t->getNext() == NULL) {
			delete t;
		}
		else {
			deleteList(t->getNext());
			delete t;
		}
	}
	void insertItem(Node *t);
	Node *getFirst() {
		return first;
	}
	Node *getItem(unsigned k);
	int getBlockNum() {
		return blockNum;
	}
	void setBlockNum(int b) {
		blockNum = b;
	}
	int getNumItem() {
		return numItem;
	}
	int getSize() {
		return size;
	}
	void printList();
	bool IsFull();
};

void LinkedList::insertItem(Node *t) {
	if (first == NULL) {
		first = t;
		t->setNext(NULL);
		numItem = numItem + 1;
		size = size + 32;
	}
	else {
		Node *p = first;
		if (IsFull()) {
			cout << "overflow!\n";
		}
		else {
			while (p->getNext() != NULL) {
				p = p->getNext();
			}
			p->setNext(t);
			t->setNext(NULL);
			numItem = numItem + 1;
			size = size + 32;
		}
	}
}

Node* LinkedList::getItem(unsigned k) {
	Node *t = first;
	while (t->getNext() != NULL) {
		if (t->getKey() == k) {
			return t;
		}
		t = t->getNext();
	}
	cout << "no matched Item\n";
	return NULL;
}

void LinkedList::printList() {
	if (first == NULL) {
		cout << "no Item to print\n";
		return;
	}

	cout << "[block number : " << blockNum << " ]" << endl;
	Node *t = first;
	while (t->getNext() != NULL) {
		cout << t->getKey() << " ";
		t = t->getNext();
	}
	cout << endl;
}

bool LinkedList::IsFull() {
	bool result = false;

	if (MAX_BLOCK_SIZE - size < 32) {
		cout << "list is full, overflow\n";
		result = true;
	}

	return result;
}

struct Student {
	char name[20];
	unsigned studentID;	//key for hashing
	float score;	//index for indexing
	unsigned advisorID;
};

class DHash {
	LinkedList **table;	//block number and Nodes
	int tableSize;	//size of table
	int *blockTable;	//hash table for block number
public:
	DHash();
	~DHash();
	void insertItem(Node *t);
	void setHashTable(ifstream &fp);
	int hashFunc(unsigned k);	//compute block number
	void doubleTable();	//extend hash table about double
	void printTable();
	int countNum(int blockNum);	//count total item matched to blockNum in table
	int findBlockNum(unsigned k);	//find block number of key in hash table
};

DHash::DHash() {
	tableSize = DEFAULT_TABLE_SIZE;	//default table size : 8
	table = new LinkedList*[tableSize];
	blockTable = new int[tableSize];
	for (int i = 0; i < tableSize; i++) {
		table[i] = NULL;
	}
	for (int i = 0; i < tableSize; i++) {
		blockTable[i] = -1;
	}
}

DHash::~DHash() {
	delete table;
	delete blockTable;
}

void DHash::insertItem(Node *t) {
	int hash;
	hash = hashFunc(t->getKey());
	if (table[hash] == NULL) {
		table[hash] = new LinkedList();
		table[hash]->setBlockNum(hash);
		table[hash]->insertItem(t);
		blockTable[hash] = hash;
		//cout << "insert item block "<<table[hash]->getBlockNum()<<" in table[" << hash << "]\n";
	}
	else {
		if (table[hash]->IsFull()) {
			cout << "overflow, double table size\n";
			doubleTable();
			insertItem(t);
			//printTable();
			cout << endl;
		}
		else {
			table[hash]->insertItem(t);
			//cout << "insert item in table[" << hash << "]\n";
		}
	}
}

void DHash::setHashTable(ifstream &fp) {
	if (fp.is_open() != true) {
		cout << "error\n";
		return;
	}

	ofstream fp2;
	fp2.open("Students.hash", ios::binary);	//hash 파일

	ofstream fp3;
	fp3.open("Students.DB", ios::binary | ios::app);	//DB 파일 : 덮어쓰기 가능

	if (fp2.is_open() != true || fp3.is_open() != true) {
		cout << "error2\n";
		return;
	}

	int num;	//number of student records
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
		//cout << buffer2 << endl;
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

		int j = 0;
		while (ptr[j] != NULL) {
			j++;
			ptr[j] = strtok(NULL, ",");
		}

		stdID = atoi(ptr[1]);
		sc = atof(ptr[2]);
		admID = atoi(ptr[3]);

		/* 2. 타입 변환된 데이터를 Student struct에 저장 */
		strcpy(std.name, wholename.c_str());
		std.studentID = stdID;
		std.score = sc;
		std.advisorID = admID;

		/* 3. hash function으로 저장할 블럭 계산 및 해쉬테이블에 insert */
		Node *t = new Node(std.studentID);
		insertItem(t);

		/* 4. hash function으로 정해진 DB 블럭에 데이터 저장
		* 위치 옮기기 : seekp
		* 4096*blockNum + item수*sizeof(std)
		*/
		//hash table에서 key를 이용해 블럭넘버 찾아오기
		int blockNum = findBlockNum(t->getKey());
		fp3.seekp(MAX_BLOCK_SIZE*blockNum + sizeof(Student)*countNum(blockNum), ios::beg);	//시작(ios::beg)부터 4096*blockNum + item수*sizeof(std)
		fp3.write((char*)&std, sizeof(std));	//덮어쓰기로 DB 파일 수정 가능
	}

	/* Students.hash 파일에 hash table 쓰기 */
	for (int i = 0; i < tableSize; i++) {
		fp2.write((char*)&blockTable[i], sizeof(blockTable[i]));
	}

	fp2.close();
	fp3.close();
}

int DHash::findBlockNum(unsigned k) {	//k 넣어서 hash table에서 블럭넘버 찾기
	int bn = hashFunc(k);
	int result = -1;

	for (int i = 0; i < tableSize; i++) {
		if (table[i] != NULL) {
			if (table[i]->getBlockNum() == bn) {
				result = bn;
			}
		}
	}

	return result;
}

int DHash::countNum(int blockNum) {	//해당 블럭에 몇 개 record가 들어있는지
	int result = 0;

	for (int i = 0; i < tableSize; i++) {
		if (table[i] != NULL) {
			if (table[i]->getBlockNum() == blockNum) {
				result = result + table[i]->getNumItem();
			}
		}
	}
	return result;
}

int DHash::hashFunc(unsigned k) { //hash function
	int result;

	result = k % tableSize;

	return result;
}

void DHash::doubleTable() {
	/* table 크기를 2배로 확장 */
	int sizeOld = tableSize;	//기존 table size
	tableSize = tableSize * 2;

	LinkedList **tableOld;	//기존 table을 백업하기 위함
	int *blockTableOld;	//기존 hash table
	tableOld = table;	//기존 table 을 백업
	blockTableOld = blockTable;

	table = new LinkedList*[tableSize];	//기존 table의 크기를 2배로 확장
	blockTable = new int[tableSize];
	for (int i = 0; i < tableSize; i++) {
		table[i] = NULL;
		blockTable[i] = NULL;
	}

	/* 기존 table(tableOld)의 리스트와 리스트 내부 노드를 확장된 table에 insert */
	for (int i = 0; i < sizeOld; i++) {
		if (tableOld[i] != NULL && blockTableOld[i] != NULL) {
			LinkedList *list = tableOld[i];	//기존 table의 i번째 LinkedList
			int b = list->getBlockNum();	//기존 LinkedList의 블럭넘버
			Node *t = list->getFirst();
			while (t != NULL) {
				insertItem(t);	//노드 t를 다시 table에 insert
				t = t->getNext();
			}
		}
	}
	delete[] tableOld;	//기존 table 제거
	delete[] blockTableOld;
}

void DHash::printTable() {
	for (int i = 0; i < tableSize; i++) {
		if (table[i] != NULL) {
			cout << "table[" << i << "] block number : ";
			cout << blockTable[i] << endl;
		}
	}
}

int main() {
	ifstream fp;
	fp.open("sampleData.csv");

	DHash hash;
	hash.setHashTable(fp);
	hash.printTable();

	fp.close();

	return 0;
}