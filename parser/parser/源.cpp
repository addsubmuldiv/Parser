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
Syntax proVector;	//һ���﷨������ʽ����
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


/*��ȡ�ڲ���ʽ��������±�*/
int getIndex(string s)
{
	return syntaxDic[s] - 1;
}

//����ֱ����ݹ�
void remove()
{
	for (size_t i = 0; i < proVector.size(); i++)
	{
		char ch = proVector[i].left[0];	//ȡ����ʽ��ߣ����ַ���ʽ
		set<string>& zgsRights = proVector[i].right;
		set<string>::iterator it = zgsRights.begin();
		string newLeft = "";
		bool flag = true;
		for (; it != zgsRights.end(); it++)	//����ÿ����|�ָ��Ĳ���ʽ
			if (it->at(0) == ch)	//�������ݹ�
			{
				newLeft = string("") + ++lastNonTerminal;
				proVector.push_back(Production(newLeft));	//�½�һ������ʽ������ı���vector�Ĵ�С�����ܵ��´�ǰ������������á�ָ��ʧЧ
				syntaxDic[newLeft] = proVector.size();
				flag = false;
				break;
			}
		if (flag) continue;		//���û����ݹ�,������ߵĲ���
		int x = getIndex(newLeft);	//������ʽ�±�
		vector<string> buf;	//�����������(����ݹ��)(����ʽ�ұ�)
		set<string>& newRights = proVector[x].right;
		newRights.insert(EMPTY);
		auto iter = (proVector[i].right).begin();
		while (!(proVector[i].right).empty())
		{
			if ((*iter).at(0) == ch)//ÿ���жϵ�һ�����������ݹ������	S->Sa|b  
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (proVector[i].right).erase(iter);	//�ж���ȥ����һ��
		}
		for (size_t j = 0; j < buf.size(); j++)
		{
			(proVector[i].right).insert(buf[j]);	//�������Ĳ���ʽ�Ҳ��Ż�ȥ
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
	for (size_t i = 0; i < proVector.size(); i++) {
		if (judgeAndGetLeftFactor(proVector[i].right, ch)) { //��������ch
			string newLeft = string("") + ++lastNonTerminal;
			proVector.push_back(Production(newLeft));	//vector�仯������ʧЧ
			set<string> &zgsRights = proVector[i].right;	//���¸�ֵ
			syntaxDic[newLeft] = proVector.size();
			int x = getIndex(newLeft);
			auto it = zgsRights.begin();
			while (it != zgsRights.end()) {
				if (it->at(0) == ch) {	//�����ӣ�Ҫ�ĵ�
					if (it->size() == 1) {
						proVector[x].right.insert(EMPTY);	//���ֻ��һ���ַ��������������Ӿ�ʣ��
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

/*��ӡ�ķ�*/
void printSyntax()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].print();
}

/*��ӡfirst��*/
void printFirst()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].printFirst();
}

/*��ӡfollow��*/
void printFollow()
{
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].printFollow();
}

/*�Ƿ����ս��������~*/
bool IsTerminal(char x)
{
	for (Production wf : proVector) {
		if (wf.left[0] == x)
			return false;
	}
	return true;
}

/*�Ƿ��Ƿ��ս��*/
bool IsNonTerminal(char x)
{
	for (Production wf : proVector) {
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
void calculateNullable(ProductionArray *production, const int N)
{
	nullable.clear();
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
void calculateFirst(ProductionArray *production, const int N)
{
	bool CHANGING = false;
	do {
		for (int i = 0; i < N; i++) {
			for (char x : production[i].right) {
				CHANGING = false;
				if (IsTerminal(x)) {	//������ս��
					int index = getIndex(production[i].left);
					if (IsExist(proVector[index].firstSet, x))	//������ַ��Ѿ������ڴ˷��ս����First
						break;	//ֱ���˳���ʣ���ַ��ı���
					proVector[index].firstSet.insert(x);	//ֱ����ӵ�First��
					CHANGING = true;
					break;
				}
				if (IsNonTerminal(x)) {	//���ս��
					int indexM = getIndex(string("") + x);
					int indexN = getIndex(production[i].left);
					if (!proVector[indexM].firstSet.empty())
						for (char c : proVector[indexM].firstSet) {
							if (IsExist(proVector[indexN].firstSet, c))
								continue;	//����Ѿ����ڣ���ôֱ�ӽ��´�ѭ��
							proVector[indexN].firstSet.insert(c);
							CHANGING = true;
						}
					if (!IsNullable(proVector[indexM].left))	//���������ս�������Ƴ���
						break;			//�˳���ʣ���ַ��ı���
				}
			}
		}
	} while (CHANGING);
}

/*�������⴮��first��*/
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

/*��ӡfollow��*/
void calculateFollow(ProductionArray *production, const int N)
{
	set<char> temp;
	proVector[getIndex(START)].followSet.insert('#');	//��ʼ����follow���#
	bool CHANGING = false;
	do {
		CHANGING = false;
		for (int i = 0; i < N; i++) {
			int index = getIndex(production[i].left);
			temp = proVector[index].followSet;	//�����ǿ�������
			string &right = production[i].right;
			for (int j = right.size() - 1; j > -1; --j) {	//�������ﲻ����size_t,��Ϊ����Զ����С��0
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


/*�����ķ�*/
void inputSyntax(std::ifstream &is, char  buf[501], const int &LINE_LENGTH)
{	
	int N;
	syntaxDic.clear();
	proVector.clear();
	is >> N;
	is >> START;
	is.getline(buf, LINE_LENGTH);	//��ȡ��һ��ʣ��հ��ַ�
									/*��ȡ����Ĳ���ʽ*/
	for (int i = 0; i < N; i++) {
		is.getline(buf, LINE_LENGTH);	//�������ȡ��buf
		int len = strlen(buf), j;
		for (j = 0; j < len; j++)
			if (buf[j] == '-')	//��⵽��ͷ��ͣ���ֳ���ߺ��ұ�
			{
				buf[j] = 0;	//������Ϊ0����string����������ͣ��
				break;
			}
		string proLeft = buf;
		if (i == N - 1) {
			lastNonTerminal = proLeft[0];
		}
		if (!syntaxDic[proLeft])
		{
			proVector.push_back(Production(proLeft));
			syntaxDic[proLeft] = proVector.size();	//��VN_dic[zgsLeft]���ﲻΪ0,�Դ��ж����½�����ֱ�����
		}
		int x = syntaxDic[proLeft] - 1;	//���������ʽ���±�
		string proRight = buf + j + 2;	//�������ұ߿�ʼ��
		proVector[x].insert(proRight);	//����һ���ұ�
	}
}



int main()
{
	char buf[MAX];
	const int LINE_LENGTH = 500;
	ifstream is(openFile);
	/*�м����﷨*/
	int N;
	is >> N;	// TODO ���ÿ�ζ���һ���﷨��Ȼ�������﷨��Ԥ������������Զ��������Ժ��ٶ���һ���﷨��
	is.getline(buf, LINE_LENGTH);
	/*�����ķ�*/
	inputSyntax(is, buf, LINE_LENGTH);
	printSyntax();
	putchar('\n');
		/*����������WFԭ������*/
	Syntax tmpSyntax(proVector);
	remove();
	extract();
//-----------------------------------------------------
	int sum = 0;	//��������ݹ�����������Ժ������ʽ��
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