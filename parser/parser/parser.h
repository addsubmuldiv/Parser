#pragma once
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <cctype>
#include <map>
#include <set>
#include <fstream>
#include <unordered_map>
#define HASH '#'
#define EMPTYCHAR '~'
using namespace std;

/*文法的数据结构*/
class Production
{
public:
	string left;	//产生式左边
	set<string> right;	//右边
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
	right = wf.right;	//STL都是深拷贝
}

/*为计算方便的产生式的数据结构*/
struct ProductionArray {
	string left;
	string right;
};

/*输出first集*/
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

/*预测分析表*/
struct PredictTable {
	unordered_map<char, int> *col;
	unordered_map<char, int> *row;
	string START;
	/*产生式数组*/
	ProductionArray * proArray;
	/*产生式数*/
	int proArraySize;
	char **table;
	void print();
	int tmpRow;
	PredictTable();
	int getProIndex(const char r, const char c);
};

PredictTable::PredictTable() : col(nullptr), row(nullptr), table(nullptr), tmpRow(-1)
{

}

/*根据非终结符和终结符获取预测分析表中的产生式下标*/
inline int PredictTable::getProIndex(const char r, const char c)
{
    int rowT = row->at(r);
    int colT = col->at(c);
	return table[rowT][colT];
}


void PredictTable::print()
{
	int n = col->size();
	int m = row->size();

	printf("终结符\n");
	for (auto c : *col) {
		cout << c.first << " " << c.second << endl;
	}

	printf("非终结符\n");
	for (auto c : *row) {
		cout << c.first << " " << c.second << endl;
	}

	printf("产生式及其编号如下\n");
	for (int i = 0; i < proArraySize; i++) {
		cout << i << "  " << proArray[i].left << "->" << proArray[i].right << endl;
	}
	putchar('\n');
	printf("预测分析表如下\n");
	for (int i = 0; i < m; i++) {
		for (int j = 0; j < n; j++) {
			printf("%d   ", table[i][j]);
		}
		putchar('\n');
	}
}

/*下推自动机*/
struct PushDownAuto {
	/*预测分析表*/
	PredictTable *predictTable;
	/*符号栈*/
	stack<char> symbol;
	ofstream os;
	bool analysis(string s);
	void changeTable(PredictTable *table);
	PushDownAuto();
};


PushDownAuto::PushDownAuto() : os("output.txt")
{

}

bool isTerminal(char a)
{
    if (a<'A' || a>'Z')
        return true;
    else
        return false;
}


bool PushDownAuto::analysis(string s)
{
    while (!symbol.empty()) {
        symbol.pop();
    }
	symbol.push(HASH);
	symbol.push(this->predictTable->START[0]);

	for (int i = 0; i < s.size();) {
		//匹配
		if (symbol.top() == s[i]) {
			symbol.pop();
			cout << "动作：匹配"<< s[i] << endl;
			i = i + 1;
		}
		else {
			if (isTerminal(symbol.top())) {
				cout << "error";
				return false;
			}
			int index = predictTable->getProIndex(symbol.top(), s[i]);
			//返回的产生式右部
            if (index == -1) {
				cout << "error\n";
				return false;
			}
			string &sn = predictTable->proArray[index].right;
			cout << "动作：" << predictTable->proArray[index].left << "->" << predictTable->proArray[index].right << "\n";
			os << predictTable->proArray[index].left << "->" << predictTable->proArray[index].right << "\n";
			
			
			symbol.pop();
            if (EMPTYCHAR != sn[0])
			{
			  for (int j = sn.size() - 1; j >= 0; j--) {
				symbol.push(sn[j]);
			  }
			}
		}
	}
	cout << s << "合法" << endl;
	os << "\n\n\n";
    return true;
}

inline void PushDownAuto::changeTable(PredictTable * table)
{
	predictTable = table;
    
}



using Type = int;
struct Token {
    Type type;
    string value;
};

vector<char> divide = { ',',';' };
vector<char> oper = { '=','+' };