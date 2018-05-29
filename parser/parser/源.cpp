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
#define MAX 501
#define EMPTY "~"
using namespace std;
using Syntax = vector<Production>;

char lastNonTerminal;
map<string, int> syntaxDic;
Syntax proVector;	//一个语法，产生式集合
ProductionArray *productionArray;
set<string> nullable;
string START;
static string openFile("test2.txt");


int getIndex(string s);
void remove();
bool judgeAndGetLeftFactor(set<string> &productionRight, char &ch);
void extract();
void printSyntax();
void calculateFirst(ProductionArray *production, const int N);
void printFirst();
void calculateFollow(ProductionArray *production, const int N);
void calculateNullable(ProductionArray *production, const int N);
bool IsTerminal(char x);
bool IsNonTerminal(char x);
bool IsExist(set<string> set, string x);
bool IsExist(set<char> set, char x);
bool IsNullable(string left);
void printFollow();
set<char> calculateFirst_S(string right);


/*获取在产生式向量里的下标*/
int getIndex(string s)
{
	return syntaxDic[s] - 1;
}

//消除直接左递归
void remove()
{
	for (size_t i = 0; i < proVector.size(); i++)
	{
		char ch = proVector[i].left[0];	//取正规式左边，以字符形式
		set<string>& zgsRights = proVector[i].right;
		set<string>::iterator it = zgsRights.begin();
		string newLeft = "";
		bool flag = true;
		for (; it != zgsRights.end(); it++)	//遍历每个以|分隔的产生式
			if (it->at(0) == ch)	//如果有左递归
			{
				newLeft = string("") + ++lastNonTerminal;
				proVector.push_back(Production(newLeft));	//新建一个产生式，这里改变了vector的大小，可能导致此前定义的所有引用、指针失效
				syntaxDic[newLeft] = proVector.size();
				flag = false;
				break;
			}
		if (flag) continue;		//如果没有左递归,跳过后边的步骤
		int x = getIndex(newLeft);	//新正规式下标
		vector<string> buf;	//缓冲区，存放(非左递归的)(产生式右边)
		set<string>& newRights = proVector[x].right;
		newRights.insert(EMPTY);
		auto iter = (proVector[i].right).begin();
		while (!(proVector[i].right).empty())
		{
			if ((*iter).at(0) == ch)//每次判断第一个，如果是左递归的那条	S->Sa|b  
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (proVector[i].right).erase(iter);	//判断完去掉第一个
		}
		for (size_t j = 0; j < buf.size(); j++)
		{
			(proVector[i].right).insert(buf[j]);	//缓冲区的产生式右部放回去
		}
	}
}

/*判断是否有左因子并返回*/
bool judgeAndGetLeftFactor(set<string> &productionRight, char &ch)
{
	vector<char> res;
	for (string s : productionRight) {	//把第一个字符都放进一个数组
		res.push_back(s.at(0));
	}
	sort(res.begin(), res.end());	//排序看有没有一样的
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
	for (size_t i = 0; i < proVector.size(); i++) {
		if (judgeAndGetLeftFactor(proVector[i].right, ch)) { //有左因子ch
			string newLeft = string("") + ++lastNonTerminal;
			proVector.push_back(Production(newLeft));	//vector变化，引用失效
			set<string> &zgsRights = proVector[i].right;	//重新赋值
			syntaxDic[newLeft] = proVector.size();
			int x = getIndex(newLeft);
			auto it = zgsRights.begin();
			while (it != zgsRights.end()) {
				if (it->at(0) == ch) {	//左因子，要改的
					if (it->size() == 1) {
						proVector[x].right.insert(EMPTY);	//如果只有一个字符，那提完左因子就剩空
					}
					else {
						proVector[x].right.insert(it->substr(1));
					}
					it = zgsRights.erase(it);
				}
			}
			zgsRights.insert(string("") + ch + newLeft);
		}
	}
}

/*打印文法*/
void printSyntax()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].print();
}

/*打印first集*/
void printFirst()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].printFirst();
}

/*打印follow集*/
void printFollow()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].printFollow();
}

/*是否是终结符，包括~*/
bool IsTerminal(char x)
{
	for (Production wf : proVector) {
		if (wf.left[0] == x)
			return false;
	}
	return true;
}

/*是否是非终结符*/
bool IsNonTerminal(char x)
{
	for (Production wf : proVector) {
		if (wf.left[0] == x)
			return true;
	}
	return false;
}

/*判断集合中是否存在指定字符*/
bool IsExist(set<char> set, char x)
{
	if (set.count(x) != 0)
		return true;
	else
		return false;
}

/*判断集合中是否已经存在指定串*/
bool IsExist(set<string> set, string x)
{
	if (set.count(x) != 0)
		return true;
	else
		return false;
}

/*该非终结符是否可以推出空*/
bool IsNullable(string left)
{
	if (nullable.count(left) != 0)
		return true;
	else
		return false;
}

/*计算那些可以推出空的非终结符集合*/
void calculateNullable(ProductionArray *production, const int N)
{
	nullable.clear();
	bool CHANGING = false;
	/*这个非终结符是否能间接推出空*/
	bool flag = true;
	do {
		for (int i = 0; i < N; i++) {
			CHANGING = false;
			flag = true;
			if (production[i].right == "~") {	//可以直接推出空
				if (IsExist(nullable, production[i].left))
					continue;
				nullable.insert(production[i].left);
				CHANGING = true;
				continue;
			}
			for (char x : production[i].right) {
				/*如果在这个产生式右边的任意一个非终结符不能推出空*/
				if (count(nullable.begin(), nullable.end(), string("") + x) == 0) {
					flag = false;
				}
			}
			if (flag) {
				if (IsExist(nullable, production[i].left))
					continue;
				nullable.insert(production[i].left);
				CHANGING = true;
				continue;
			}
		}
	} while (CHANGING);
}

/*计算first集*/
void calculateFirst(ProductionArray *production, const int N)
{
	bool CHANGING = false;
	do {
		for (int i = 0; i < N; i++) {
			for (char x : production[i].right) {
				CHANGING = false;
				if (IsTerminal(x)) {	//如果是终结符
					int index = getIndex(production[i].left);
					if (IsExist(proVector[index].firstSet, x))	//如果该字符已经存在于此非终结符的First
						break;	//直接退出对剩余字符的遍历
					proVector[index].firstSet.insert(x);	//直接添加到First集
					CHANGING = true;
					break;
				}
				if (IsNonTerminal(x)) {	//非终结符
					int indexM = getIndex(string("") + x);
					int indexN = getIndex(production[i].left);
					if (!proVector[indexM].firstSet.empty())
						for (char c : proVector[indexM].firstSet) {
							if (IsExist(proVector[indexN].firstSet, c))
								continue;	//如果已经存在，那么直接进下次循环
							proVector[indexN].firstSet.insert(c);
							CHANGING = true;
						}
					if (!IsNullable(proVector[indexM].left))	//如果这个非终结符不能推出空
						break;			//退出对剩余字符的遍历
				}
			}
		}
	} while (CHANGING);
}

/*计算任意串的first集*/
set<char> calculateFirst_S(string right)
{
	if (IsTerminal(right[0])) {
		set<char> res = { right[0] };
		return res;
	}
	if (IsNonTerminal(right[0])) {
		return proVector[getIndex(string("") + right)].firstSet;
	}
}

/*打印follow集*/
void calculateFollow(ProductionArray *production, const int N)
{
	set<char> temp;
	proVector[getIndex(START)].followSet.insert('#');	//开始符号follow添加#
	bool CHANGING = false;
	do {
		CHANGING = false;
		for (int i = 0; i < N; i++) {
			int index = getIndex(production[i].left);
			temp = proVector[index].followSet;	//这里是拷贝构造
			string &right = production[i].right;
			for (int j = right.size() - 1; j > -1; --j) {	//逆序，这里不能用size_t,因为它永远不会小于0
				if (IsTerminal(right[j])) {
					temp.clear();
					temp.insert(right[j]);
				}
				if (IsNonTerminal(right[j])) {
					auto &followM = proVector[getIndex(string("") + right[j])].followSet;
					for (char x : temp) {
						if (IsExist(followM, x)) {
							continue;
						}
						followM.insert(x);
						CHANGING = true;
					}
					auto &firstM = proVector[getIndex(string("") + right[j])].firstSet;
					if (!IsNullable(string("") + right[j]))
						temp = firstM;
					else
						for (char x : firstM)
							temp.insert(x);
				}
			}
		}
	} while (CHANGING);
}


/*输入文法*/
void inputSyntax(std::ifstream &is, char  buf[501], const int &LINE_LENGTH)
{	
	int N;
	syntaxDic.clear();
	proVector.clear();
	is >> N;
	is >> START;
	is.getline(buf, LINE_LENGTH);	//读取第一行剩余空白字符
									/*读取输入的产生式*/
	for (int i = 0; i < N; i++) {
		is.getline(buf, LINE_LENGTH);	//将输入读取到buf
		int len = strlen(buf), j;
		for (j = 0; j < len; j++)
			if (buf[j] == '-')	//检测到箭头就停，分出左边和右边
			{
				buf[j] = 0;	//置这里为0就让string构造器在这停了
				break;
			}
		string proLeft = buf;
		if (i == N - 1) {
			lastNonTerminal = proLeft[0];
		}
		if (!syntaxDic[proLeft])
		{
			proVector.push_back(Production(proLeft));
			syntaxDic[proLeft] = proVector.size();	//让VN_dic[zgsLeft]这里不为0,以此判断是新建还是直接添加
		}
		int x = syntaxDic[proLeft] - 1;	//刚输的正规式的下标
		string proRight = buf + j + 2;	//让它从右边开始读
		proVector[x].insert(proRight);	//加入一个右边
	}
}



int main()
{
	char buf[MAX];
	const int LINE_LENGTH = 500;
	ifstream is(openFile);
	/*有几个语法*/
	int N;
	is >> N;	// TODO 这里，每次读入一个语法，然后把这个语法的预测分析表，下推自动机生成以后，再读下一个语法。
	is.getline(buf, LINE_LENGTH);
	/*输入文法*/
	inputSyntax(is, buf, LINE_LENGTH);
	printSyntax();
	putchar('\n');
		/*把最初输入的WF原样保存*/
	Syntax tmpSyntax(proVector);
	remove();
	extract();
//-----------------------------------------------------
	int sum = 0;	//做完消左递归和提左因子以后的正规式数
	for (Production cfg : proVector) {
		sum += cfg.right.size();
	}
	productionArray = new ProductionArray[sum];
	int count = 0;
	for (const Production &cfg : proVector) {
		for (const string &s : cfg.right) {
			productionArray[count].left = cfg.left;
			productionArray[count].right = s;
			count++;
		}
	}
	for (int i = 0; i < sum; i++) {
		cout << productionArray[i].left << "->" << productionArray[i].right << endl;
	}
//-----------------------------------------------------
	calculateNullable(productionArray, sum);
	calculateFirst(productionArray, sum);
	calculateFollow(productionArray, sum);


	printFirst();
	printFollow();

	system("pause");
}