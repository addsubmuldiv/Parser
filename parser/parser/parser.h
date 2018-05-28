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
class WF
{
public:
	string left;	//²úÉúÊ½×ó±ß
	set<string> right;	//ÓÒ±ß
	set<char> firstSet;
	set<char> followSet;
	WF(const string& temp)
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

	WF(const WF &wf);
};

WF::WF(const WF &wf)
{
	left = wf.left;
	for (string s : wf.right) {
		right.insert(s);
	}
}

struct Production {
	string left;
	string right;
};

void WF::printFirst() 
{
	printf("first %s:", left.c_str());
	for (const char s : this->firstSet) {
		printf("%c ", s);
	}
	putchar('\n');
}