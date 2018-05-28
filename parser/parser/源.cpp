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
/*�ķ������ݽṹ*/

map<string, int> VN_dic;
vector<WF> VN_vector;	//�ķ�����
string start;
Production *production;
bool used[MAX];
bool CHANGING = false;


//����ֱ����ݹ�
void remove()
{
	for (int i = 0; i < VN_vector.size(); i++)
	{
		char ch = VN_vector[i].left[0];	//ȡ����ʽ��ߣ����ַ���ʽ
		set<string>& zgsRights = VN_vector[i].right;
		set<string>::iterator it = zgsRights.begin();
		//string newLeft = VN_vector[i].left.substr(0, 1) + "\'";
		string newLeft = "";
		//string newLeft(""+lastNonTerminal++);
		bool flag = true;
		for (; it != zgsRights.end(); it++)
			if (it->at(0) == ch)	//�������ݹ�
			{	newLeft = string("") + ++lastNonTerminal;
				VN_vector.push_back(WF(newLeft));	//�½�һ������ʽ������ı���vector�Ĵ�С�����ܵ��´�ǰ������������á�ָ��ʧЧ
				VN_dic[newLeft] = VN_vector.size();
				flag = false;
				break;
			}
		if (flag) continue;		//���û����ݹ�,������ߵĲ���
		set<string>& zgsRights1 = VN_vector[i].right;
		int x = VN_dic[newLeft] - 1;	//������ʽ�±�
		vector<string> buf;	//�����������(����ݹ��)(����ʽ�ұ�)
		set<string>& newRights = VN_vector[x].right;
		newRights.insert(EMPTY);
		auto iter = (VN_vector[i].right).begin();
		while (!(VN_vector[i].right).empty())
		{
			if ((*iter).at(0) == ch)//ÿ���жϵ�һ�����������ݹ������	S->Sa|b  
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (VN_vector[i].right).erase(iter);	//�ж���ȥ����һ��
		}
		for (int j = 0; j < buf.size(); j++)
		{
			(VN_vector[i].right).insert(buf[j]);	//�������Ĳ���ʽ�Ҳ��Ż�ȥ
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



/*��ȡ������*/
void extract()
{
	char ch = -1;
	for (size_t i = 0; i < VN_vector.size(); i++) {
		if (judgeAndGetLeftFactor(VN_vector[i].right, ch)) { //��������ch
			string newLeft = string("") + ++lastNonTerminal;
			VN_vector.push_back(WF(newLeft));
			set<string> &zgsRights = VN_vector[i].right;
			VN_dic[newLeft] = VN_vector.size();
			int x = VN_dic[newLeft] - 1;
			auto it = zgsRights.begin();
			while (it != zgsRights.end()) {
				if (it->at(0) == ch) {	//�����ӣ�Ҫ�ĵ�
					if (it->size() == 1) {
						VN_vector[x].right.insert(EMPTY);	//���ֻ��һ���ַ��������������Ӿ�ʣ��
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
				if (isTerminal(x)) {	//������ս��
					int index = getIndex(production[i].left);
					if (IsExist(VN_vector[index].firstSet, x))	//������ַ��Ѿ������ڴ˷��ս����First
						break;	//ֱ���˳���ʣ���ַ��ı���
					VN_vector[index].firstSet.insert(x);	//ֱ����ӵ�First��
					CHANGING = true;
					break;
				}
				if (isNonTerminal(x)) {	//���ս��
					int indexM = getIndex(string("") + x);
					int indexN = getIndex(production[i].left);
					if(!VN_vector[indexM].firstSet.empty())
						for (char c : VN_vector[indexM].firstSet) {
							if (IsExist(VN_vector[indexN].firstSet, c))
								continue;	//����Ѿ����ڣ���ôֱ�ӽ��´�ѭ��
							VN_vector[indexN].firstSet.insert(c);
							CHANGING = true;
						}
					if (VN_vector[indexM].right.count("~") == 0)	//���������ս�����Ƴ���
						break;	//�˳���ʣ���ַ��ı���
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
		is.getline(buf,LINE_LENGTH);	//�������ȡ��buf
		int len = strlen(buf), j;
		for (j = 0; j < len; j++)
			if (buf[j] == '-')	//��⵽��ͷ��ͣ���ֳ���ߺ��ұ�
			{
				buf[j] = 0;	//������Ϊ0����string����������ͣ��
				break;
			}
		string zgsLeft = buf;	
		if (i == N-1) {
			lastNonTerminal = zgsLeft[0];
		}
		if (!VN_dic[zgsLeft])
		{
			VN_vector.push_back(WF(zgsLeft));
			VN_dic[zgsLeft] = VN_vector.size();	//��VN_dic[zgsLeft]���ﲻΪ0,�Դ��ж����½�����ֱ�����
		}
		int x = VN_dic[zgsLeft] - 1;	//���������ʽ���±�
		string zgsRight = buf + j + 2;	//�������ұ߿�ʼ��
		VN_vector[x].insert(zgsRight);	//����һ���ұ�
	}

	vector<WF> tmpWF(VN_vector);
	remove();
	extract();
	int sum = 0;	//��������ݹ�����������Ժ������ʽ��
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