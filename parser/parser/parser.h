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
class Production
{
public:
	string left;	//����ʽ���
	set<string> right;	//�ұ�
	set<char> firstSet;
	set<char> followSet;
	Production(const string& temp)
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
	Production(const Production &wf);
};

Production::Production(const Production &wf)
{
	left = wf.left;
	/*for (string s : wf.right) {
		right.insert(s);
	}*/
	right = wf.right;	//STL�������
}

/*Ϊ���㷽��Ĳ���ʽ�����ݽṹ*/
struct ProductionArray {
	string left;
	string right;
};

/*���first��*/
void Production::printFirst() 
{
	printf("first %s:", left.c_str());
	for (const char s : this->firstSet) {
		printf("%c ", s);
	}
	putchar('\n');
}

void Production::printFollow()
{
	printf("follow %s:", left.c_str());
	for (const char s : this->followSet) {
		printf("%c ", s);
	}
	putchar('\n');
}