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
#include "parser.h"
#define MAX 507
#define EMPTY "~"
using namespace std;

char lastNonTerminal;
/*文法的数据结构*/

map<string, int> VN_dic;
vector<WF> VN_vector;	//文法集合
string start;
Production *production;
bool used[MAX];
bool CHANGING = false;


//消除直接左递归
void remove()
{
	for (int i = 0; i < VN_vector.size(); i++)
	{
		char ch = VN_vector[i].left[0];	//取正规式左边，以字符形式
		set<string>& zgsRights = VN_vector[i].right;
		set<string>::iterator it = zgsRights.begin();
		//string newLeft = VN_vector[i].left.substr(0, 1) + "\'";
		string newLeft = "";
		//string newLeft(""+lastNonTerminal++);
		bool flag = true;
		for (; it != zgsRights.end(); it++)
			if (it->at(0) == ch)	//如果有左递归
			{	newLeft = string("") + ++lastNonTerminal;
				VN_vector.push_back(WF(newLeft));	//新建一个正规式，这里改变了vector的大小，可能导致此前定义的所有引用、指针失效
				VN_dic[newLeft] = VN_vector.size();
				flag = false;
				break;
			}
		if (flag) continue;		//如果没有左递归,跳过后边的步骤
		set<string>& zgsRights1 = VN_vector[i].right;
		int x = VN_dic[newLeft] - 1;	//新正规式下标
		vector<string> buf;	//缓冲区，存放(非左递归的)(产生式右边)
		set<string>& newRights = VN_vector[x].right;
		newRights.insert(EMPTY);
		auto iter = (VN_vector[i].right).begin();
		while (!(VN_vector[i].right).empty())
		{
			if ((*iter).at(0) == ch)//每次判断第一个，如果是左递归的那条	S->Sa|b  
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (VN_vector[i].right).erase(iter);	//判断完去掉第一个
		}
		for (int j = 0; j < buf.size(); j++)
		{
			(VN_vector[i].right).insert(buf[j]);	//缓冲区的产生式右部放回去
		}
	}
}


int getIndex(string s)
{
	return VN_dic[s] - 1;
}


bool judgeAndGetLeftFactor(set<string> &zgsRights,char &ch)
{
	vector<char> res;
	for (string s : zgsRights) {
		res.push_back(s.at(0));
	}
	sort(res.begin(), res.end());
	for (size_t i = 1; i < res.size(); i++) {
		if (res[i - 1] == res[i]) {
			ch = res[i];
			return true;
		}
	}
	return false;
}



/*提取左因子*/
void extract()
{
	char ch = -1;
	for (size_t i = 0; i < VN_vector.size(); i++) {
		if (judgeAndGetLeftFactor(VN_vector[i].right, ch)) { //有左因子ch
			string newLeft = string("") + ++lastNonTerminal;
			VN_vector.push_back(WF(newLeft));
			set<string> &zgsRights = VN_vector[i].right;
			VN_dic[newLeft] = VN_vector.size();
			int x = VN_dic[newLeft] - 1;
			auto it = zgsRights.begin();
			while (it != zgsRights.end()) {
				if (it->at(0) == ch) {	//左因子，要改的
					if (it->size() == 1) {
						VN_vector[x].right.insert(EMPTY);	//如果只有一个字符，那提完左因子就剩空
					} 
					else {
						VN_vector[x].right.insert(it->substr(1));
					}
					it = zgsRights.erase(it);
				}
			}
			zgsRights.insert(string("") + ch + newLeft);	
		}
	}
}



void printWF()
{
	for (size_t i = 0; i < VN_vector.size(); i++)
		VN_vector[i].print();
}

void printFirst()
{
	for (size_t i = 0; i < VN_vector.size(); i++)
		VN_vector[i].printFirst();
}


bool isTerminal(char x)
{
	for (WF wf : VN_vector) {
		if (wf.left[0] == x)
			return false;
	}
	return true;
}

bool isNonTerminal(char x)
{
	for (WF wf : VN_vector) {
		if (wf.left[0] == x)
			return true;
	}
	return false;
}

bool IsExist(set<char> set, char x)
{
	if (set.count(x) != 0)
		return true;
	else
		return false;
}


void calculateFirst(Production *production, const int N)
{
	do {
		for (int i = 0; i < N; i++) {
			for (char x : production[i].right) {
				CHANGING = false;
				if (isTerminal(x)) {	//如果是终结符
					int index = getIndex(production[i].left);
					if (IsExist(VN_vector[index].firstSet, x))	//如果该字符已经存在于此非终结符的First
						break;	//直接退出对剩余字符的遍历
					VN_vector[index].firstSet.insert(x);	//直接添加到First集
					CHANGING = true;
					break;
				}
				if (isNonTerminal(x)) {	//非终结符
					int indexM = getIndex(string("") + x);
					int indexN = getIndex(production[i].left);
					if(!VN_vector[indexM].firstSet.empty())
						for (char c : VN_vector[indexM].firstSet) {
							if (IsExist(VN_vector[indexN].firstSet, c))
								continue;	//如果已经存在，那么直接进下次循环
							VN_vector[indexN].firstSet.insert(c);
							CHANGING = true;
						}
					if (VN_vector[indexM].right.count("~") == 0)	//如果这个非终结符不推出空
						break;	//退出对剩余字符的遍历
				}
			}
		}
	} while (CHANGING);
}






int main()
{
	char buf[MAX];
	int N;
	VN_dic.clear();
	VN_vector.clear();
	start = "S";
	ifstream is("test2.txt");
	is >> N;
	const int LINE_LENGTH = 500;
	is.getline(buf,LINE_LENGTH);
	for (int i = 0; i < N; i++) {
		is.getline(buf,LINE_LENGTH);	//将输入读取到buf
		int len = strlen(buf), j;
		for (j = 0; j < len; j++)
			if (buf[j] == '-')	//检测到箭头就停，分出左边和右边
			{
				buf[j] = 0;	//置这里为0就让string构造器在这停了
				break;
			}
		string zgsLeft = buf;	
		if (i == N-1) {
			lastNonTerminal = zgsLeft[0];
		}
		if (!VN_dic[zgsLeft])
		{
			VN_vector.push_back(WF(zgsLeft));
			VN_dic[zgsLeft] = VN_vector.size();	//让VN_dic[zgsLeft]这里不为0,以此判断是新建还是直接添加
		}
		int x = VN_dic[zgsLeft] - 1;	//刚输的正规式的下标
		string zgsRight = buf + j + 2;	//让它从右边开始读
		VN_vector[x].insert(zgsRight);	//加入一个右边
	}

	vector<WF> tmpWF(VN_vector);
	remove();
	extract();
	int sum = 0;	//做完消左递归和提左因子以后的正规式数
	for (WF wf : VN_vector) {
		sum += wf.right.size();
	}

	production = new Production[sum];
	int count = 0;
	for (const WF &wf : VN_vector) {
		for (const string &s : wf.right) {
			production[count].left = wf.left;
			production[count].right = s;
			count++;
		}
	}

	for (int i = 0; i < sum; i++) {
		cout << production[i].left << "   " << production[i].right << endl;
	}

	calculateFirst(production, sum);
	printWF();

	printFirst();
	system("pause");
}