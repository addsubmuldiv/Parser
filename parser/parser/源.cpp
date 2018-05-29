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
map<string, int> cfgDic;
vector<CFG> cfgVector;	//�ķ�����
string start;
Production *production;
bool used[MAX];
set<string> nullable;
string START;
static string openFile("test2.txt");


int getIndex(string s);
void remove();
bool judgeAndGetLeftFactor(set<string> &productionRight, char &ch);
void extract();
void printCFG();
void calculateFirst(Production *production, const int N);
void printFirst();
void calculateFollow(Production *production, const int N);
void calculateNullable(Production *production, const int N);
bool IsTerminal(char x);
bool IsNonTerminal(char x);
bool IsExist(set<string> set, string x);
bool IsExist(set<char> set, char x);
bool IsNullable(string left);
void printFollow();



/*��ȡ�ڲ���ʽ��������±�*/
int getIndex(string s)
{
	return cfgDic[s] - 1;
}

//����ֱ����ݹ�
void remove()
{
	for (size_t i = 0; i < cfgVector.size(); i++)
	{
		char ch = cfgVector[i].left[0];	//ȡ����ʽ��ߣ����ַ���ʽ
		set<string>& zgsRights = cfgVector[i].right;
		set<string>::iterator it = zgsRights.begin();
		string newLeft = "";
		bool flag = true;
		for (; it != zgsRights.end(); it++)	//����ÿ����|�ָ��Ĳ���ʽ
			if (it->at(0) == ch)	//�������ݹ�
			{
				newLeft = string("") + ++lastNonTerminal;
				cfgVector.push_back(CFG(newLeft));	//�½�һ������ʽ������ı���vector�Ĵ�С�����ܵ��´�ǰ������������á�ָ��ʧЧ
				cfgDic[newLeft] = cfgVector.size();
				flag = false;
				break;
			}
		if (flag) continue;		//���û����ݹ�,������ߵĲ���
		int x = getIndex(newLeft);	//������ʽ�±�
		vector<string> buf;	//�����������(����ݹ��)(����ʽ�ұ�)
		set<string>& newRights = cfgVector[x].right;
		newRights.insert(EMPTY);
		auto iter = (cfgVector[i].right).begin();
		while (!(cfgVector[i].right).empty())
		{
			if ((*iter).at(0) == ch)//ÿ���жϵ�һ�����������ݹ������	S->Sa|b  
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (cfgVector[i].right).erase(iter);	//�ж���ȥ����һ��
		}
		for (size_t j = 0; j < buf.size(); j++)
		{
			(cfgVector[i].right).insert(buf[j]);	//�������Ĳ���ʽ�Ҳ��Ż�ȥ
		}
	}
}

/*�ж��Ƿ��������Ӳ�����*/
bool judgeAndGetLeftFactor(set<string> &productionRight, char &ch)
{
	vector<char> res;
	for (string s : productionRight) {	//�ѵ�һ���ַ����Ž�һ������
		res.push_back(s.at(0));
	}
	sort(res.begin(), res.end());	//������û��һ����
	for (size_t i = 1; i < res.size(); i++) {
		if (res[i - 1] == res[i]) {
			ch = res[i];
			return true;
		}
	}
	return false;
}

/*��ȡ������*/
void extract()
{
	char ch = -1;
	for (size_t i = 0; i < cfgVector.size(); i++) {
		if (judgeAndGetLeftFactor(cfgVector[i].right, ch)) { //��������ch
			string newLeft = string("") + ++lastNonTerminal;
			cfgVector.push_back(CFG(newLeft));	//vector�仯������ʧЧ
			set<string> &zgsRights = cfgVector[i].right;	//���¸�ֵ
			cfgDic[newLeft] = cfgVector.size();
			int x = getIndex(newLeft);
			auto it = zgsRights.begin();
			while (it != zgsRights.end()) {
				if (it->at(0) == ch) {	//�����ӣ�Ҫ�ĵ�
					if (it->size() == 1) {
						cfgVector[x].right.insert(EMPTY);	//���ֻ��һ���ַ��������������Ӿ�ʣ��
					}
					else {
						cfgVector[x].right.insert(it->substr(1));
					}
					it = zgsRights.erase(it);
				}
			}
			zgsRights.insert(string("") + ch + newLeft);
		}
	}
}

/*��ӡ�ķ�*/
void printCFG()
{
	for (size_t i = 0; i < cfgVector.size(); i++)
		cfgVector[i].print();
}

/*��ӡfirst��*/
void printFirst()
{
	for (size_t i = 0; i < cfgVector.size(); i++)
		cfgVector[i].printFirst();
}

/*��ӡfollow��*/
void printFollow()
{
	for (size_t i = 0; i < cfgVector.size(); i++)
		cfgVector[i].printFollow();
}

/*�Ƿ����ս��������~*/
bool IsTerminal(char x)
{
	for (CFG wf : cfgVector) {
		if (wf.left[0] == x)
			return false;
	}
	return true;
}

/*�Ƿ��Ƿ��ս��*/
bool IsNonTerminal(char x)
{
	for (CFG wf : cfgVector) {
		if (wf.left[0] == x)
			return true;
	}
	return false;
}

/*�жϼ������Ƿ����ָ���ַ�*/
bool IsExist(set<char> set, char x)
{
	if (set.count(x) != 0)
		return true;
	else
		return false;
}

/*�жϼ������Ƿ��Ѿ�����ָ����*/
bool IsExist(set<string> set, string x)
{
	if (set.count(x) != 0)
		return true;
	else
		return false;
}

/*�÷��ս���Ƿ�����Ƴ���*/
bool IsNullable(string left)
{
	if (nullable.count(left) != 0)
		return true;
	else
		return false;
}

/*������Щ�����Ƴ��յķ��ս������*/
void calculateNullable(Production *production, const int N)
{
	bool CHANGING = false;
	/*������ս���Ƿ��ܼ���Ƴ���*/
	bool flag = true;
	do {
		for (int i = 0; i < N; i++) {
			CHANGING = false;
			flag = true;
			if (production[i].right == "~") {	//����ֱ���Ƴ���
				if (IsExist(nullable, production[i].left))
					continue;
				nullable.insert(production[i].left);
				CHANGING = true;
				continue;
			}
			for (char x : production[i].right) {
				/*������������ʽ�ұߵ�����һ�����ս�������Ƴ���*/
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

/*����first��*/
void calculateFirst(Production *production, const int N)
{
	bool CHANGING = false;
	do {
		for (int i = 0; i < N; i++) {
			for (char x : production[i].right) {
				CHANGING = false;
				if (IsTerminal(x)) {	//������ս��
					int index = getIndex(production[i].left);
					if (IsExist(cfgVector[index].firstSet, x))	//������ַ��Ѿ������ڴ˷��ս����First
						break;	//ֱ���˳���ʣ���ַ��ı���
					cfgVector[index].firstSet.insert(x);	//ֱ����ӵ�First��
					CHANGING = true;
					break;
				}
				if (IsNonTerminal(x)) {	//���ս��
					int indexM = getIndex(string("") + x);
					int indexN = getIndex(production[i].left);
					if (!cfgVector[indexM].firstSet.empty())
						for (char c : cfgVector[indexM].firstSet) {
							if (IsExist(cfgVector[indexN].firstSet, c))
								continue;	//����Ѿ����ڣ���ôֱ�ӽ��´�ѭ��
							cfgVector[indexN].firstSet.insert(c);
							CHANGING = true;
						}
					if (!IsNullable(cfgVector[indexM].left))	//���������ս�������Ƴ���
						break;			//�˳���ʣ���ַ��ı���
				}
			}
		}
	} while (CHANGING);
}

/*��ӡfollow��*/
void calculateFollow(Production *production, const int N)
{
	set<char> temp;
	cfgVector[getIndex(START)].followSet.insert('#');	//��ʼ����follow���#
	bool CHANGING = false;
	do {
		CHANGING = false;
		for (int i = 0; i < N; i++) {
			int index = getIndex(production[i].left);
			temp = cfgVector[index].followSet;	//�����ǿ�������
			// TODO
			string &right = production[i].right;
			for (int j = right.size() - 1; j > -1; --j) {
				if (IsTerminal(right[j])) {
					temp.clear();
					temp.insert(right[j]);
				}
				if (IsNonTerminal(right[j])) {
					auto &followM = cfgVector[getIndex(string("") + right[j])].followSet;
					for (char x : temp) {
						if (IsExist(followM, x)) {
							continue;
						}
						followM.insert(x);
						CHANGING = true;	// TODO Ҫ���������CHANGING��false
					}
					auto &firstM = cfgVector[getIndex(string("") + right[j])].firstSet;
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




int main()
{
	char buf[MAX];
	int N;
	cfgDic.clear();
	cfgVector.clear();
	start = "S";
	ifstream is(openFile);
	is >> N;
	is >> START;
	const int LINE_LENGTH = 500;
	is.getline(buf, LINE_LENGTH);	//��ȡ��һ��ʣ��հ��ַ�
	for (int i = 0; i < N; i++) {
		is.getline(buf, LINE_LENGTH);	//�������ȡ��buf
		int len = strlen(buf), j;
		for (j = 0; j < len; j++)
			if (buf[j] == '-')	//��⵽��ͷ��ͣ���ֳ���ߺ��ұ�
			{
				buf[j] = 0;	//������Ϊ0����string����������ͣ��
				break;
			}
		string zgsLeft = buf;
		if (i == N - 1) {
			lastNonTerminal = zgsLeft[0];
		}
		if (!cfgDic[zgsLeft])
		{
			cfgVector.push_back(CFG(zgsLeft));
			cfgDic[zgsLeft] = cfgVector.size();	//��VN_dic[zgsLeft]���ﲻΪ0,�Դ��ж����½�����ֱ�����
		}
		int x = cfgDic[zgsLeft] - 1;	//���������ʽ���±�
		string zgsRight = buf + j + 2;	//�������ұ߿�ʼ��
		cfgVector[x].insert(zgsRight);	//����һ���ұ�
	}

	vector<CFG> tmpCFG(cfgVector);
	remove();
	extract();
	int sum = 0;	//��������ݹ�����������Ժ������ʽ��
	for (CFG cfg : cfgVector) {
		sum += cfg.right.size();
	}

	production = new Production[sum];
	int count = 0;
	for (const CFG &cfg : cfgVector) {
		for (const string &s : cfg.right) {
			production[count].left = cfg.left;
			production[count].right = s;
			count++;
		}
	}

	for (int i = 0; i < sum; i++) {
		cout << production[i].left << "   " << production[i].right << endl;
	}

	calculateNullable(production, sum);

	calculateFirst(production, sum);
	calculateFollow(production, sum);
	printCFG();

	printFirst();

	printFollow();
	for (CFG &cfg : tmpCFG) {
		cfg.print();
	}
	
	system("pause");
}