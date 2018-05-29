#pragma once
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <queue>
#include <cctype>
#include <map>
#include <set>
#include <fstream>
using namespace std;

/*�ķ������ݽṹ*/
class CFG
{
public:
	string left;	//����ʽ���
	set<string> right;	//�ұ�
	set<char> firstSet;
	set<char> followSet;
	CFG(const string& temp)
	{
		left = temp;
		right.clear();
	}
	void print()
	{
		printf("%s->", left.c_str());
		set<string>::iterator it = right.begin();
		printf("%s", it->c_str());
		it++;
		for (; it != right.end(); it++)
			printf("|%s", it->c_str());
		puts("");
	}
	void insert(const string& temp)
	{
		right.insert(temp);
	}
	void printFirst();
	void printFollow();
	CFG(const CFG &wf);
};

CFG::CFG(const CFG &wf)
{
	left = wf.left;
	/*for (string s : wf.right) {
		right.insert(s);
	}*/
	right = wf.right;	//STL�������
}

/*Ϊ���㷽��Ĳ���ʽ�����ݽṹ*/
struct Production {
	string left;
	string right;
};

/*���first��*/
void CFG::printFirst() 
{
	printf("first %s:", left.c_str());
	for (const char s : this->firstSet) {
		printf("%c ", s);
	}
	putchar('\n');
}

void CFG::printFollow()
{
	printf("follow %s:", left.c_str());
	for (const char s : this->followSet) {
		printf("%c ", s);
	}
	putchar('\n');
}