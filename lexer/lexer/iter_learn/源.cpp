#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <stack>
using namespace std;

#define CLOSURE '*'
#define JOIN '&'
#define OR '|'

int GetPriority(char sy)//设置各个操作符的优先级
{
	switch (sy)
	{
	case '#':
		return 0;
	case '(':
	case '[':
		return 1;
	case '-':
		return 2;
	case OR:
		return 3;
	case JOIN:
		return 4;
	case ')':
	case ']':
		return 5;
	case CLOSURE:
		return 6;
	default:
		return -1;
	}
}

bool isOper(char ch)
{
	if (ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == OR || ch == CLOSURE || ch == JOIN)
		return true;
	else
		return false;
}








string Transform(string &zgs)
{
	stack<char> oper;
	vector<char> res;
	oper.push('#');		//哨兵
	for (size_t i = 0; i < zgs.size(); i++) {
		if (!isOper(zgs[i])) {  //不是'-'以外的符号
			if (zgs[i] == '-') {
				oper.push(zgs[i]);
				continue;
			}
			res.push_back(zgs[i]);
			if (oper.top() == '-') {
				//res.push_back(oper.top());
				char start = res[res.size() - 2], end = res[res.size() - 1];
				res.pop_back();
				res.pop_back();
				for (char ch = start; ch <= end; ch++) {
					res.push_back(ch);
				}
				for (char ch = start; ch < end; ch++) {
					res.push_back(OR);
				}
				oper.pop();
			}
		}
		else if (zgs[i] == '(' || zgs[i] == '[') {
			oper.push(zgs[i]);
		}
		else if (zgs[i] == ')' || zgs[i] == ']') {
			while ((zgs[i] == ']'&&oper.top() != '[') || zgs[i] == ')'&&oper.top() != '(') {
				char top = oper.top();
				res.push_back(top);
				oper.pop();
			}
			//			if (zgs[i] == ']')
			//			res.push_back('·');
			oper.pop();	//把左括号也出栈
		}
		else if (zgs[i] == OR || zgs[i] == JOIN) {
			while (GetPriority(oper.top()) > GetPriority(zgs[i])) {
				char top = oper.top();
				res.push_back(top);
				oper.pop();
			}
			oper.push(zgs[i]);
		}
		else if (zgs[i] == CLOSURE) {
			res.push_back(zgs[i]);
		}
	}
	while (oper.top() != '#') {
		char ch = oper.top();
		res.push_back(ch);
		oper.pop();
	}
	char *str = new char[res.size() + 1];
	for (size_t i = 0; i < res.size(); i++) {
		str[i] = res[i];
	}
	str[res.size()] = '\0';
	string s(str);
	return s;
}
int main(void)
{
	//vector<int> a = { 1,2,3,4,5 };
	//for (int i = 0; i < a.size(); i++) {
	//	if (i == 3) {
	//		a.push_back(4);
	//	}
	//	cout << a[i] << endl;
	//}
	//vector<int> b(10);
	//copy(a.begin(), a.end(), b.begin());
	//auto iter = a.begin();
	//while (iter != a.end())
	//{
	//	cout << *iter << endl;
	//	if (*iter == 3) {
	//		iter = a.insert(a.end(), 6);
	//	}
	//	iter++;
	//}
	//for (int x : b) {
	//	cout << x;
	//}
	//system("pause");
	//if (2 == 2 && 2 == 2)
	//	printf("fuck");


	string zgs = "[0-9|a-z|A-Z|_]*";
	zgs = Transform(zgs);
	cout << zgs.c_str() << endl;









	system("pause");
	return 0;
}