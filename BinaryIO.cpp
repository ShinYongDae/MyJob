//Main.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#include "Student.h"
#include "StudentDAO.h"
#include "StudentView.h"
#include "StudentMgr.h"

using namespace std;

int StudentDAO::StudentCnt = 0;

int main()
{
	StudentMgr mgr;
	mgr.Process();

	cout << "���α׷� ����...." << endl;
	return 0;
}


//Student.h

#pragma once

#include <cstring>

class Student {
	int id;
	char name[50];

public:
	Student() {}
	Student(int id, char* name);

	void SetId(int id);
	void SetName(char* name);
	int GetId();
	char* GetName();
};


//Student.cpp

#include "Student.h"

Student::Student(int id, char * name){
	this->id = id;
	strcpy(this->name, name);
}

void Student::SetId(int id){
	this->id = id;
}

void Student::SetName(char * name){
	strcpy(this->name, name);
}

int Student::GetId(){
	return id;
}

char * Student::GetName(){
	return name;
}


//StudentMgr.h

#pragma once

#include "Student.h"
#include "StudentView.h"
#include "StudentDAO.h"

class StudentMgr
{
public:
	StudentMgr(){}
	StudentView view;
	StudentDAO dao;

	void Process();
	void list();
	void insert();
	void search();
	void update();
	void remove();
};


//StudentMgr.cpp

#include <iostream>

#include "StudentMgr.h"
#include "Student.h"

using namespace std;

void StudentMgr::Process()
{
	bool go = true;

	while (go)
	{
		char m = view.Menu();
		switch (m)
		{
		case 's': list(); break;
		case 'i': insert();	 break;
		case 'f': search();  break;
		case 'u': update();  break;
		case 'd': remove();  break;
		case 'x': go = false; break;
		default: view.PrintError("�Է¿���");
		}
	}
}

void StudentMgr::list()
{
	Student** arr = dao.GetList();
	view.PrintLine();
	view.PrintList(arr);
	view.PrintLine();
}

void StudentMgr::insert()
{
	Student* pSt = view.InputSt();
	bool inserted = dao.Insert(pSt);
	if (inserted) view.PrintMsg("���� ����");
	else view.PrintMsg("���� ����");
}

void StudentMgr::search()
{
	int id = view.InputSearchKey();
	Student* pSt = dao.Search(id);
	view.PrintSt(pSt);
}

void StudentMgr::update()
{
	Student* pSt = view.InputUpdateSt();
	bool updated = dao.Update(pSt);
	view.PrintUpdateResult(updated);
}

void StudentMgr::remove()
{
	int del = view.InputDeleteKey();
	bool deleted = dao.Delete(del);
	view.PrintDeleteResult(deleted);
}


//StudentDAO.h

#pragma once

#include "Student.h"

class StudentDAO {
	public:
		static int StudentCnt;

		StudentDAO() {};
		Student** GetList();
		Student* Search(int id);

		int GetCount();
		bool Insert(Student* pSt);
		bool Update(Student* pSt);
		bool Delete(int id);
};


//StudentDAO.cpp

#include "StudentDAO.h"

#include <iostream>
#include <fstream>

using namespace std;

Student ** StudentDAO::GetList() {
	ifstream in;
	in.open("D:\\test\\student.dat", ios::binary);

	if (!in.is_open()) {
		cout << "���Ͽ��� ����"<<endl;
		return NULL;
	}

	int StCnt = GetCount();

	Student** arr = new Student*[StCnt];

	for (int i = 0; i < StCnt; i++) {
		Student* pSt = new Student;
		if (in.read((char*)pSt, sizeof(*pSt))) {
			arr[i] = pSt;
		}
	}

	StudentDAO::StudentCnt = StCnt;

	in.close();

	return arr;
}

int StudentDAO::GetCount()
{
	ifstream in("D:\\test\\student.dat");

	if (!in.is_open()) {
		cout <<"���Ͽ��� ����"<<endl;
		return 0;
	}

	int cnt = 0;
	Student obj; 
	while (in.read((char*)&obj, sizeof(obj))) cnt++;

	in.close();

	return cnt;
}

bool StudentDAO::Insert(Student * pSt)
{

	bool inserted = false;
	ofstream outFile;
	outFile.open("D:\\test\\student.dat", ios::binary | ios::app);

	outFile.write((char*)pSt, sizeof(*pSt));

	if (outFile.good()) inserted = true;
	else inserted = false;

	outFile.close();

	delete pSt;

	return inserted;
}

Student * StudentDAO::Search(int id)
{
	ifstream inFile; 
	inFile.open("D:\\test\\student.dat", ios::binary); 

	Student* pObj = new Student; 

	bool found = false;
	while (inFile.read((char*)pObj, sizeof(*pObj))) {
		if(pObj->GetId() == id){
			found = true;
			break;
		}
	} 

	inFile.close();

	if (!found) pObj = NULL;
	return pObj;
}

bool StudentDAO::Update(Student * pSt)
{
	fstream file; 
	file.open("D:\\test\\student.dat", ios::binary | ios::in | ios::out); 
	Student obj;

	bool updated = false;

	while (file.read((char*)&obj, sizeof(obj))) { 
		if (obj.GetId() == pSt->GetId()) { 
			obj.SetId(pSt->GetId());
			obj.SetName(pSt->GetName());
			int pos = -1 * sizeof(obj); 
			file.seekp(pos, ios::cur); 
			file.write((char*)&obj, sizeof(obj));

			cout << "�л����� ���� ����" << endl;

			if (file.good()) {
				updated = true;
				break;
			}
		} 
	} 

	file.close();

	return updated;
}

bool StudentDAO::Delete(int id)
{

	Student obj; 
	ifstream inFile; 
	inFile.open("D:\\test\\student.dat", ios::binary); 

	ofstream outFile;
	outFile.open("D:\\test\\temp.dat", ios::out | ios::binary); 

	bool found = false;
	while (inFile.read((char*)&obj, sizeof(obj))) { 
		if (obj.GetId() != id) { 
			outFile.write((char*)&obj, sizeof(obj)); 
		}
		else {
			found = true;
		}
	}

	inFile.close(); 
	outFile.close(); 

	remove("D:\\test\\student.dat");
	rename("D:\\test\\temp.dat", "D:\\test\\student.dat");

	return found ? true : false;
}


//StudentView.h

#pragma once

#include "Student.h"
#include <string>

using namespace std;

class StudentView {
public:
	char Menu();	

	void PrintList(Student** arr);
	Student* InputSt();
	int InputSearchKey();
	int InputDeleteKey();
	Student* InputUpdateSt();
	void PrintSt(Student* pSt);
	void PrintUpdateResult(bool updated);
	void PrintDeleteResult(bool delted);
	void PrintMsg(string msg);
	void PrintError(string msg);
	void PrintLine();
};


//StudentView.cpp

#include "StudentView.h"
#include "StudentDAO.h"
#include "Student.h"

#include <iostream>

char StudentView::Menu()
{
	cout << "�޴�: ����Ʈ(s), �߰�(i), �˻�(f), ����(u), ����(d), ����(x):";
	char m;
	cin >> m;
	return m;
}

void StudentView::PrintList(Student ** arr)
{
	cout << "  ***** �л����� ����Ʈ *****" << endl;
	for (int i = 0; i < StudentDAO::StudentCnt; i++) {
		cout << arr[i]->GetId() << "\t";
		cout << arr[i]->GetName() << endl;
	}

	for (int i = 0; i < StudentDAO::StudentCnt; i++) {
		delete arr[i];
	}

	delete[] arr;
}

Student * StudentView::InputSt()
{
	int id;
	string name;

	cout << "��ȣ �̸� (�������� ����):";
	cin >> id >> name;

	Student* pSt = new Student(id, (char*)name.c_str());
	return pSt;
}

int StudentView::InputSearchKey()
{
	cout << "�˻��� �л� ��ȣ:";

	int id;
	cin >> id;

	return id;
}

int StudentView::InputDeleteKey()
{
	cout << "������ �л� ��ȣ:";

	int id;
	cin >> id;

	return id;
}

Student * StudentView::InputUpdateSt()
{
	cout << "������ �л���ȣ �̸�:";

	int id;
	string name;
	cin >> id >> name;

	Student* pSt = new Student;
	pSt->SetId(id);
	pSt->SetName((char*)name.c_str());

	return pSt;
}

void StudentView::PrintSt(Student * pSt)
{
	if (pSt) {
		cout << "  ***** �˻��� �л����� *****" << endl;
		cout << pSt->GetId() << "\t" << pSt->GetName() << endl;
		delete pSt;
	}
	else {
		cout << "  �˻��� ������ �����ϴ�" << endl;
	}
}

void StudentView::PrintUpdateResult(bool updated)
{
	string msg;
	if (updated) msg = "���������� ȸ�������� �����߽��ϴ�";
	else msg = "ȸ�������� �������� ���߽��ϴ�";
	cout << "  --> ȸ������ �������:" << msg << endl;
}

void StudentView::PrintDeleteResult(bool delted)
{
	string msg;
	if (delted) msg = "���������� ȸ�������� �����߽��ϴ�";
	else msg = "ȸ�������� �������� ���߽��ϴ�";

	cout << "  --> ȸ������ �������:" << msg << endl;
}

void StudentView::PrintMsg(string msg)
{
	cout << "  --> " << msg << endl;
}

void StudentView::PrintError(string msg)
{
	cerr << "  --> " << msg << endl;
}

void StudentView::PrintLine()
{
	cout << "------------------------------------------------------------" << endl;
}