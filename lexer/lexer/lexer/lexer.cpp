#include <iostream>
#include <vector>
#include <cctype>
#include <string>
#include <stack>
#include <set>
#include <iterator>
#include <fstream>
#include <algorithm>
using namespace std;


#define KEYWORD 0
#define IDENTIFIER 1
#define OPERATOR 3
#define CONSTANT 4
#define DIVIDE 2
#define CLOSURE '*'
#define JOIN '&'
#define OR '|'
const static bool UNACCEPT = false;
const static bool ACCEPT = true;
typedef unsigned int GroupId;
typedef int State;
GroupId STARTGROUP;
bool GROUPCHANGED = false;
vector<string> operatorChars = { "+","-","*","/","=",">","<","<=",">=","!=" };
vector<string> keywords = { "int","double","float" };
bool IsTheStringNeedToDivideAgain(string &s);
int findGroupId(int state);	//����һ��״̬�����ĸ���

struct Edge {
	int start;
	int end;
	char letter;
	int startGroupId;
	int endGroupId;
	Edge(int start, int end, char letter);
	void Print();
	void PrintGroup();
};

void Edge::Print()
{
	printf("from %d to %d with %c\n", start, end, letter);
}

void Edge::PrintGroup()
{
	printf("from %d to %d with %c\n", startGroupId, endGroupId, letter);
}

Edge::Edge(int start, int end, char letter)
{
	this->start = start;
	this->end = end;
	this->letter = letter;
	this->startGroupId = -1;
	this->endGroupId = -1;
}
int S_num = -1;

class FA {
public:
	vector<State> S;
	vector<char> chars;
	vector<Edge> edges;
	int S0, f;
	vector<int> finalStates;
	string zgs;
	int dfaEdgeCount;
	State currentState;
public:
	FA(string zgs);
	FA(char zgs);
	FA() {}
	void Print();
	void PrintEdgeGroup();
	void Transition(char nextChar);	//TODO ��һ��״̬��DFA����һ���ַ�����ת��
	bool isCurrentAccept();
	bool isAccept(string str);
	void clearTheChars();
	inline State getCurrentState()
	{
		return currentState;
	}
};


FA::FA(char zgs)
{

	int T1 = S_num + 1;//ÿ���������״̬
	int T2 = S_num + 2;

	S.push_back(T1);
	S.push_back(T2);
	this->chars.push_back(zgs);

	Edge es(T1, T2, zgs);//���һ����
	edges.push_back(es);
	S0 = T1;
	f = T2;
	S_num = S_num + 2;
	this->zgs = zgs;
}


FA::FA(string zgs)
{
	S0 = f = 0;
	this->zgs = zgs;
}
/*������FA��������ݣ�״̬���ߣ�����״̬���ļ���*/
void FA::Print()
{
	printf("״̬��\n");
	for (State s : S) {
		printf("%d ", s);
	}
	putchar('\n');

	printf("��ʼ״̬��\n");
	printf("%d\n", S0);
	putchar('\n');

	printf("�ߣ�\n");
	for (Edge e : edges) {
		e.Print();
	}

	printf("����״̬\n");
	for (State s : finalStates) {
		printf("%d ", s);
	}

}
void FA::PrintEdgeGroup()
{
	putchar('\n');
	for (Edge e : edges) {
		e.PrintGroup();
	}
}
void FA::Transition(char nextChar)
{
	if (count(chars.begin(), chars.end(), nextChar) == 0) {
		//printf("�����ܵ��ַ�");
		currentState = -1;
	}
	for (Edge e : edges) {
		if (e.start == currentState && e.letter == nextChar) {
			currentState = e.end;
			return;
		}
	}
	//printf("û�������ıߣ���̬Ϊ %d , ���� %c ������һ��״̬��", currentState, nextChar);
	currentState = -1;
}
bool FA::isCurrentAccept()
{
	if (count(finalStates.begin(), finalStates.end(), currentState) == S0)
		return false;
	else
		return true;
}
bool FA::isAccept(string str)
{
	currentState = S0;
	auto it = str.begin();
	while (it != str.end()) {
		this->Transition(*it);
		it++;
	}
	if (count(finalStates.begin(), finalStates.end(), getCurrentState()) != 0) {
		//printf("\n\n���ܸ��ַ�����\n\n");
		return true;
	}
	//printf("���Զ��������ܴ��ַ���");
	return false;
}

void FA::clearTheChars()
{
	sort(chars.begin(), chars.end());
	auto iter = unique(chars.begin(), chars.end());
	chars.erase(iter, chars.end());
}



FA bibao(FA NFA) {
	FA reNFA = NFA;
	State T1 = S_num + 1;//ÿ���������״̬
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, T2, '~');//���������
	Edge es2(reNFA.f, reNFA.S0, '~');
	Edge es3(T1, reNFA.S0, '~');
	Edge es4(reNFA.f, T2, '~');
	reNFA.edges.push_back(es);
	reNFA.edges.push_back(es2);
	reNFA.edges.push_back(es3);
	reNFA.edges.push_back(es4);
	reNFA.S0 = T1;
	reNFA.f = T2;
	S_num = S_num + 2;
	return reNFA;

}


FA huo(FA NFA2, FA NFA1) {

	FA reNFA = NFA1;//��NFA1�Ž�ȥ

					//vector<char> chars;�ַ���

	for (int i = 0; i<NFA2.S.size(); i++) {//��NFA2��S�Ž�ȥ
		State temp;
		temp = NFA2.S[i];
		reNFA.S.push_back(temp);
	}
	for (int i = 0; i<NFA2.chars.size(); i++) {//��NFA2��chars�Ž�ȥ
		char temp;
		temp = NFA2.chars[i];
		reNFA.chars.push_back(temp);
	}
	//vector<Edge> edges;
	for (int i = 0; i<NFA2.edges.size(); i++) {//��NFA2��edges�Ž�ȥ

		reNFA.edges.push_back(NFA2.edges[i]);
	}


	State T1 = S_num + 1;//ÿ���������״̬
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, NFA1.S0, '~');//���������
	Edge es2(T1, NFA2.S0, '~');
	Edge es3(NFA1.f, T2, '~');
	Edge es4(NFA2.f, T2, '~');

	reNFA.edges.push_back(es);
	reNFA.edges.push_back(es2);
	reNFA.edges.push_back(es3);
	reNFA.edges.push_back(es4);

	reNFA.S0 = T1;
	reNFA.f = T2;
	S_num = S_num + 2;
	return reNFA;
}

FA lianjie(FA NFA2, FA NFA1) {
	FA reNFA = NFA1;//��NFA1�Ž�ȥ

					//vector<char> chars;�ַ���

	for (int i = 0; i<NFA2.S.size(); i++) {//��NFA2��S�Ž�ȥ
		State temp;
		temp = NFA2.S[i];
		reNFA.S.push_back(temp);
	}
	for (int i = 0; i<NFA2.chars.size(); i++) {//��NFA2��chars�Ž�ȥ
		char temp;
		temp = NFA2.chars[i];
		reNFA.chars.push_back(temp);
	}
	//vector<Edge> edges;
	for (int i = 0; i<NFA2.edges.size(); i++) {//��NFA2��edges�Ž�ȥ

		reNFA.edges.push_back(NFA2.edges[i]);
	}
	reNFA.f = NFA2.f;

	State T1 = S_num + 1;//ÿ���������״̬
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, reNFA.S0, '~');//���������
	Edge es2(reNFA.f, T2, '~');
	Edge es3(NFA1.f, NFA2.S0, '~');

	reNFA.edges.push_back(es);
	reNFA.edges.push_back(es2);
	reNFA.edges.push_back(es3);

	reNFA.S0 = T1;
	reNFA.f = T2;
	S_num = S_num + 2;
	return reNFA;
}



void print(FA NFA) {

	cout << "��̬:" << NFA.S0 << "             ��̬��" << NFA.f << endl;
	cout << "״̬����      ";
	for (int i = 0; i<NFA.S.size(); i++) {

		cout << NFA.S[i] << "    ";

	}
	cout << endl;
	cout << "�ַ�����      ";
	for (int i = 0; i<NFA.chars.size(); i++) {

		cout << NFA.chars[i];
	}
	cout << endl;
	cout << "�߼���      " << endl;
	for (int i = 0; i<NFA.edges.size(); i++) {

		cout << "��㣺" << NFA.edges[i].start << "           �յ㣺" << NFA.edges[i].end << "           ������" << NFA.edges[i].letter << endl;

	}
}





FA TransNFA(string zgs) {
	stack <FA>stk;//����һ��NFAջ
	zgs = zgs + '#';
	FA NFA;
	stk.push(NFA);		//�ڱ�
	int i = 0;
	while (zgs[i] != '#') {
		if (zgs[i] != '|'&&zgs[i] != '&'&&zgs[i] != '*') {//���ʽѹջ
			FA NFAs(zgs[i]);
			NFA = NFAs;
			stk.push(NFA);
		}
		else if (zgs[i] == '*') {//�հ�
			NFA = stk.top();
			stk.pop();
			NFA = bibao(NFA);
			stk.push(NFA);
		}
		else if (zgs[i] == '|')//��
		{
			FA NFA1 = stk.top(); stk.pop();
			FA NFA2 = stk.top(); stk.pop();
			NFA = huo(NFA1, NFA2);
			stk.push(NFA);
		}
		else {//����
			FA NFA1 = stk.top(); stk.pop();
			FA NFA2 = stk.top(); stk.pop();
			NFA = lianjie(NFA1, NFA2);
			stk.push(NFA);
		}
		i++;
	}
	NFA.clearTheChars();
	return NFA;

}


bool contain(vector<Edge> src, Edge elem) {

	for (int i = 0; i<src.size(); i++) {
		Edge item = src[i];
		if (item.start == elem.start&&item.end == elem.end&&item.letter == elem.letter) {
			return true;
		}
	}
	return false;

}



vector<Edge> distinct(vector<Edge> src) {
	vector<Edge> dest;
	for (int i = 0; i<src.size(); i++) {
		Edge item = src[i];
		if (!contain(dest, item)) {
			dest.push_back(item);
		}
	}
	return dest;
}




FA NFA_TO_DFA(FA NFA)
{
	FA dfa;
	dfa.S0 = 0;
	//��ʼ�� DFA���ַ��� ��ȥ����
	for (int i = 0; i < NFA.chars.size(); i++)
	{
		if (NFA.chars[i] == '~') {
			continue;
		}
		else {
			dfa.chars.push_back(NFA.chars[i]);
		}
	}

	vector <vector<int> > unit; //�����ά���� ��¼���µ����ɵļ���
	vector <int> u;
	u.push_back(999);
	unit.push_back(u);
	unit[0].push_back(0);
	unit[0].push_back(NFA.S0); 
							   //--------������һ������----
							   //1.ֱ�ӿ�ת��
	for (int i = 0; i < NFA.edges.size(); i++)
	{
		if (NFA.edges[i].start == NFA.S0 && NFA.edges[i].letter == '~')
		{
			unit[0].push_back(NFA.edges[i].end);   //��ʼ״̬��ʼ������ֱ�ӿ�ת�Ƶ����״̬
		}                                          
	}
	//2.��ӿ�ת��
	for (int i = 3; i < unit[0].size(); i++)	//��ʼ״̬�ļ�ӿ�ת��
	{
		for (int j = 0; j < NFA.edges.size(); j++)         //����һ���Ļ����ϣ���ѯԭNFA���Ҷ��ο�ת�ƺ�����õ�״̬
		{
			if (unit[0][i] == NFA.edges[j].start && NFA.edges[j].letter == '~')//���磺999,0,��ʼ״̬0,1,7��2,4��ͨ��1�����ҵ� 2,4��
			{
				unit[0].push_back(NFA.edges[j].end);  //����������
			}
		}
	}
	//��һ�����Ͻ�����ϣ���������DFA
	
	dfa.dfaEdgeCount = 1; //��ʼ��DFAͼ����1
	u.pop_back();
	u.push_back(1000);//1000�����
	unit.push_back(u);
	for (int i = 0; i < unit.size(); i++)
	{                                                    
		if (unit[i][0] == 999)                         
		{                                              
			for (int j = 0; j < dfa.chars.size(); j++)    //ѭ��ÿһ����֧������ ����a��b,c����
			{                                            
				if (unit[dfa.dfaEdgeCount][0] == 1000)
				{ //��ǰλ���Ƿ�Ϊ�գ����������һ��ת�����ļ��ϲ�Ϊ�գ���Ϊ���������һ��ת����û���õ�
					unit.pop_back();

					u.pop_back();
					u.push_back(999);
					unit.push_back(u);
					unit[dfa.dfaEdgeCount].push_back(j);
				}


				for (int k = 2; k < unit[i].size(); k++)//ֱ��ת��
				{                                                      //�ҳ���ǰ���ϵ�ǰ����  ��ȥ����״̬
					for (int h = 0; h < NFA.edges.size(); h++)
					{                                                //��ѯNFAͼ�����еıߣ�ֱ�ӿ��Ե������̬ ���磺3,8
						if (unit[i][k] == NFA.edges[h].start && dfa.chars[j] == NFA.edges[h].letter)
						{                                            //�ҵ�NFAͼ�п�ʼ״̬��ͬ��ͨ����ͬ��������

							unit[dfa.dfaEdgeCount].push_back(NFA.edges[h].end);
						}
					}
				}

				if (unit[dfa.dfaEdgeCount].size() == 2)
				{
					continue;//���ת����û���õ����ռ���
				}
				


				Edge e(i, dfa.dfaEdgeCount, dfa.chars[j]);
				dfa.edges.push_back(e);   //��e������ ���� DFA��


										  //---ת�ƺ�Ĳ�ȫ�������ڲ����б���------
				for (int l = 2; l < unit[dfa.dfaEdgeCount].size(); l++)
				{
					for (int m = 0; m < NFA.edges.size(); m++)//��ѯNFAͼ
					{
						if (unit[dfa.dfaEdgeCount][l] == NFA.edges[m].start&&NFA.edges[m].letter == '~')
						{
							//�����ʼ״̬��ͬ��ת����Ϊ��  ��
							int index; //����
							for (index = 2; index< unit[dfa.edges.size()].size(); index++)
							{
								if (NFA.edges[m].end != unit[dfa.dfaEdgeCount][index])
									continue;
								else
								{
									break;
								}
							}
							if (index == unit[dfa.dfaEdgeCount].size())
							{
								unit[dfa.edges.size()].push_back(NFA.edges[m].end);
							}
						}
					}
				}



				//��ѯ����DFA�еļ����Ƿ����ظ��ĳ���
				bool issame = false;
				int flag = -1;
				int k, n;
				for (k = 0; k<unit.size() - 1; k++)
				{
					for (n = 2; n<unit[k].size() && n<unit[unit.size() - 1].size(); n++)//��ֹ�±�Խ��
					{                                    //ǰ�����ƱȽϵ��г���  �����Ǻͱ�������
						if (unit[k][n] == unit[unit.size() - 1][n])
						{
							continue;
						}
						else {
							break;
						}
					}
					if (n == unit[k].size())
					{
						issame = true;
						if (flag == -1)
						{
							flag = k;//�ѵ�ǰ��������FLAG	
						}
						break;
					}
					else
					{
						issame = false;
					}
				}
				if (issame)
				{
					unit[unit.size() - 1][0] = flag;//ÿ����λ���ִ��� �����ظ�������λ��
				}
				dfa.dfaEdgeCount++;//��������1
				u.pop_back();
				u.push_back(1000);
				unit.push_back(u);
			}
		}
	}
	


	int news = 0;//�ϲ���״̬���
	for (int i = 0; i < unit.size(); i++)
	{
		if (unit[i][0] == 999)
		{
			unit[i][0] = 999 + news;
			news++;
			dfa.S.push_back(unit[i][0] - 999);//����DFA��״̬��
			for (int j = 2; j < unit[i].size(); j++)
			{
				if (unit[i][j] == NFA.f)
				{
					
					dfa.finalStates.push_back(unit[i][0] - 999);//������̬
					break;
				}  //������̬������̬����������
			}
		}
	}


	for (int i = 0; i < dfa.dfaEdgeCount - 1; i++) {//dfaͼ���±��״̬
		if (unit[dfa.edges[i].start][0] >= 999) {//��ʼ״̬�ı�
			dfa.edges[i].start = unit[dfa.edges[i].start][0] - 999;//�������ظ����ֵģ�
		}
		else {
			dfa.edges[i].start = unit[unit[dfa.edges[i].start][0]][0] - 999;//���ظ����ֵļ��ϣ�
		}

		if (unit[dfa.edges[i].end][0] >= 999) {//��ֹ״̬�ı�
			dfa.edges[i].end = unit[dfa.edges[i].end][0] - 999;//�����ظ����ֵģ�
		}
		else {
			dfa.edges[i].end = unit[unit[dfa.edges[i].end][0]][0] - 999;//d�ظ����ֵļ��ϣ�
		}

	}

	cout << endl << "����DFA���£�" << endl;
	dfa.edges = distinct(dfa.edges);
	print(dfa);
	return dfa;
}



typedef FA DFA;
int GetPriority(char sy)//���ø��������������ȼ�
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
	oper.push('#');		//�ڱ�
	for (size_t i = 0; i < zgs.size(); i++) {
		if (!isOper(zgs[i])) {  //����'-'����ķ���
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
			//			res.push_back('��');
			oper.pop();	//��������Ҳ��ջ
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


GroupId Transition(State state, char character, vector<Edge> edges)	//����һ��״̬������һ���ַ��õ����״̬������
{
	for (Edge e : edges) {
		if (e.start == state && e.letter == character)
			return e.endGroupId;
	}
	return findGroupId(state);
}




struct Group {
	GroupId groupId;
	set<int> states;
	bool isAccept;
	Group(GroupId &id, bool isAccept)
	{
		groupId = id++;
		this->isAccept = isAccept;
	}
	bool isInThisGroup(int state);
	void Print();
	bool isEdgeFromThisGroup(Edge e);
	bool isSeparable(DFA &dfa);
	bool isEdgeInThisGroup(Edge e) {
		if (e.endGroupId == e.startGroupId && e.startGroupId == groupId)
			return true;
		else
			return false;
	}
	vector<Edge> getEdgeStartFromThisGroup(vector<Edge> edges);
};


bool Group::isInThisGroup(int state)	//�ж�һ��״̬�ǲ������������
{
	if (this->states.count(state) != 0)
		return true;
	else
		return false;
}

void Group::Print()
{
	auto it = states.begin();
	printf("��%d\n", groupId);
	for (; it != states.end(); it++) {
		printf("%d ", *it);
	}
	putchar('\n');
}

/*��һ�����Ƿ��Ǵ�����������*/
bool Group::isEdgeFromThisGroup(Edge e)
{
	if (e.startGroupId == groupId)
		return true;
	return false;
}
/*���״̬�Ƿ�ɷ�*/
bool Group::isSeparable(DFA &dfa)
{
	vector<Edge> &edges = dfa.edges;
	if (this->states.size() == 1) {	//ֻ��һ��״̬ʱ�����ɷ�
		return false;
	}
	vector<Edge> edgeOut;	//�õ����������������б�
	for (Edge e : edges) {
		if (isEdgeInThisGroup(e)) {
			continue;
		}
		if (isEdgeFromThisGroup(e)) {	//���־�������ת�Ƶı�
			edgeOut.push_back(e);
		}
	}
	for (char ch : dfa.chars) {	//�������ַ�������������״̬��Ŀ���飬�����������
		auto state = this->states.begin();
		GroupId gId = Transition(*state, ch, edgeOut);
		state++;
		for (; state != this->states.end(); state++) {
			if (Transition(*state, ch, edgeOut) != gId)
				return true;
		}
	}

	return false;
}

/*������д����������ı�*/
vector<Edge> Group::getEdgeStartFromThisGroup(vector<Edge> edges)
{
	vector<Edge> res;
	for (Edge e : edges) {
		if (e.startGroupId == this->groupId) {
			res.push_back(e);
		}
	}
	return res;
}

GroupId ID = 0;		//�������ID
					//bool GROUPCHANGED = false; //���Ƿ��ڱ仯
vector<Group> groups;

int findGroupId(int state)	//����һ��״̬�����ĸ���
{
	for (size_t i = 0; i < groups.size(); i++) {
		if (groups[i].states.count(state) != 0)
			return groups[i].groupId;
	}
	return -1;
}

void edgeToGroup(FA &dfa)	//������Ҫ֪����Щ��Ҫ�ϳ�һ����,��ÿ���߼�¼���������յ����ڵ���
{
	vector<Edge> &edges = dfa.edges;
	for (Edge &e : edges) {	//���ı�Ҫ������
		int startGroupId = findGroupId(e.start);
		int endGroupId = findGroupId(e.end);
		if (startGroupId == endGroupId)
			e.startGroupId = e.endGroupId = startGroupId;
		else {
			e.startGroupId = startGroupId;
			e.endGroupId = endGroupId;
		}
	}
}


struct ToGroup {
	GroupId desGroupId;
	char letter;
	vector<State> states;
	ToGroup(GroupId des, char letter) {
		this->desGroupId = des;
		this->letter = letter;
	}
};

vector<ToGroup> toGroups;

bool findToGroupAndPush(State state, char character, vector<Edge> edges)	//����Ѿ����������ַ�һ�����յ�һ�������Ǿ�ֱ��push
{
	for (ToGroup &tG : toGroups) {
		if (tG.desGroupId == Transition(state, character, edges) && tG.letter == character ) {
			tG.states.push_back(state);
			return true;
		}
	}
	return false;
}


bool isTheEdgeExist(Edge e, vector<Edge> &tmpEdges)
{
	for (Edge edge : tmpEdges) {
		if (e.startGroupId == edge.startGroupId
			&& e.endGroupId == edge.endGroupId
			&& e.letter == edge.letter)
			return true;
	}
	return false;
}


void Split(DFA &dfa)
{
	for (size_t i = 0; i < groups.size(); i++) { //�����飬��Ҫ��̬�ں������Ԫ�أ���for
												 //										 /*ͨ����������ȷ������ٷ���*/
		/*�п��ܳ���һ������ȫ���˵������Ҫɾȥ*/
		for (auto it = groups.begin(); it != groups.end();) {
			if ((*it).states.empty()) {
				it = groups.erase(it);
			}
			else
				it++;
		}
		if (groups[i].isSeparable(dfa)) {
			GROUPCHANGED = true;

			vector<Edge> edges = groups[i].getEdgeStartFromThisGroup(dfa.edges);//���������������б�

			for (char ch : dfa.chars) {	//��ÿһ��dfa���ܵ��ַ�
				auto it = groups[i].states.begin();
				while (it != groups[i].states.end()) {	//��ÿһ�������е�״̬
					State tmpState = *it;	//�ȱ������״̬
					GroupId targetGroup = Transition(tmpState, ch, edges);
					if (targetGroup != groups[i].groupId) {		//��������ַ��¸�״̬��Ŀ�����ǲ��Ǳ���
						/*���ǵĻ�����Ҫ�����ֳ�����������Ѿ��ֳ�����������ͬ���ģ��Ͷ���ȥ�������½�һ��*/
						printf("��ʱ�������ĸ %c ������ %d ������ %d ɾ����״̬�� %d\n", ch, targetGroup, groups[i].groupId, *it);
						it = groups[i].states.erase(it);	//���������ɾ��
						if (!findToGroupAndPush(tmpState, ch, edges)) {
							ToGroup newToGroup(Transition(tmpState, ch, edges), ch);
							newToGroup.states.push_back(tmpState);
							toGroups.push_back(newToGroup);
						}
					}
					else
						it++;
				}
			}
		}

		for (ToGroup tG : toGroups) {
			auto &states = tG.states;	//�����·ֳ������״̬����
			Group newGroup(ID, groups[i].isAccept);	//�µ����ǽ��ܻ��Ƿǽ����������ĸ���ľ���
			//copy(states.begin(), states.end(), newGroup.states.begin());	
			for (auto iter = states.begin(); iter != states.end(); iter++) {//�½�һ���飬�ѷֳ�����״̬�Ž�ȥ
				/*����·ֵ����״̬�� 0 ������ʵ״̬����ô��������ʶΪ��ʼ��*/
				if (*iter == 0) {
					STARTGROUP = newGroup.groupId;
				}
				newGroup.states.insert(*iter);
			}
			groups.push_back(newGroup);		//����ŵ���ļ�������
		}

		edgeToGroup(dfa);//���±ߵ�����Ϣ
		dfa.PrintEdgeGroup();
		toGroups.clear();
	}
}

DFA ConstructDfa(DFA dfa)
{
	//-----------������СDFA
	DFA newDfa;
	newDfa.chars.resize(dfa.chars.size());
	/*����copy�㷨��Ŀ����������ʵ��������㹻�Ŀռ�*/
	copy(dfa.chars.begin(), dfa.chars.end(), newDfa.chars.begin());	//�ַ���û�б仯
	set<Edge> newEdgeSet;
	vector<Edge> &tmpEdges = newDfa.edges;	//�������ô�����
	for (Edge e : dfa.edges) {
		if (isTheEdgeExist(e, tmpEdges))	//����������ڱ߻��߱��Ѿ�����
			continue;
		else
			tmpEdges.push_back(e);
	}	//tmpEdges���µı�
		//copy(tmpEdges.begin(), tmpEdges.end(),& newDFA.edges.begin());	//���µı߷�����DFA
	for (auto iter = groups.begin(); iter != groups.end(); iter++) {	//ȥ������
		if ((*iter).states.empty())
			iter = groups.erase(iter);
	}
	for (Group group : groups) {	//����ıߺ��±�Բ���,����״̬Ҳ��
		newDfa.S.push_back(group.groupId);
		/*����ĸ�������ʼ�飬��ô�����±����ھ��ǿ�ʼ�ĵط�*/
		if (group.groupId == STARTGROUP) {
			newDfa.S0 = newDfa.S.size() - 1;
		}
		if (group.isAccept) {
			newDfa.finalStates.push_back(group.groupId);
		}
	}
	for (size_t i = 0; i < newDfa.S.size(); i++) {	//�±�ͱ߶��Ͼ���,״̬�Լ������ɶûӰ��
		for (Edge &e : newDfa.edges) {
			if (e.startGroupId == newDfa.S[i])
				e.start = i;
			if (e.endGroupId == newDfa.S[i])
				e.end = i;
		}
		for (State &state : newDfa.finalStates) {	//�ѽ���״̬���±�Ķ�
			if (state == newDfa.S[i])
				state = i;
		}
	}
	return newDfa;
}

DFA Hopcroft(DFA &dfa)
{
	Group unAccept(ID, UNACCEPT);	//�ǽ�����0
	Group accept(ID, ACCEPT);	//������1
	for (int x : dfa.finalStates) {
		accept.states.insert(x);
	}
	printf("����״̬�黮�����\n");
	accept.Print();

	for (size_t i = 0; i < dfa.S.size(); i++) {
		if (accept.states.count(i) == 0) {  //������״̬���ǽ���״̬
			unAccept.states.insert(i);	//�ͷŵ��ǽ���״̬�ļ�����
		}
	}
	printf("�ǽ���״̬�������\n");
	unAccept.Print();
	groups.push_back(unAccept);
	groups.push_back(accept);
	edgeToGroup(dfa);

	dfa.PrintEdgeGroup();
	do {
		GROUPCHANGED = false;
		Split(dfa);
	} while (GROUPCHANGED == true);
	DFA finalDfa = ConstructDfa(dfa);
	printf("\n\n\n����Ч��\n\n\n");
	finalDfa.Print();
	return finalDfa;
}



DFA UnitTestOfMinimizeDfa(const char *str)
{
	DFA dfa;
	/*
	���ļ���ȡ���� �������� ״̬��M ����N �ַ���T ����״̬��
	������N���Ǳ� ��ʽ�� ��� �յ� �ַ�
	Ȼ��һ���� �ַ�����
	���һ���� ����״̬����
	*/
	int M, N, T, A;
	ifstream is(str);
	if (!is.bad()) {
		is >> M;
		is >> N;
		is >> T;
		is >> A;
		//cout << M << N << T << A;
	}

	for (int i = 0; i < M; i++) {	//����״̬����һЩ����������Ҫ�����±�
		dfa.S.push_back(i);
	}

	int start, end;
	char letter;
	for (int i = 0; i < N; i++) {	//�����
		is >> start;
		is >> end;
		is >> letter;
		dfa.edges.push_back(Edge(start, end, letter));
	}

	for (int i = 0; i < T; i++) {	//�����ַ���
		is >> letter;
		dfa.chars.push_back(letter);
	}

	int acceptState;
	for (int i = 0; i < A; i++) { //�������״̬
		is >> acceptState;
		dfa.finalStates.push_back(acceptState);
	}

	//���ˣ����Ǵ���С����DFA

	DFA FDfa = Hopcroft(dfa);
	return FDfa;
}





namespace stringMethod {
	using std::string;
	using std::vector;
	string& trim(string &s)
	{
		if (s.empty()) {
			return s;
		}

		s.erase(0, s.find_first_not_of(" "));
		s.erase(s.find_last_not_of(" ") + 1);
		return s;
	}

	vector<string>& split(std::string& s, const char *delim)
	{
		vector<string> *ret = new vector<string>;
		size_t last = 0;
		size_t index = s.find_first_of(delim, last);
		while (index != std::string::npos) {
			ret->push_back(s.substr(last, index - last));
			last = index + 1;
			index = s.find_first_of(delim, last);
		}
		if (index - last>0) {
			ret->push_back(s.substr(last, index - last));
		}
		return *ret;
	}
}



bool IsTwoCharOper(char first, char second)
{
	string op = "";
	op = op + first + second;
	if (count(operatorChars.begin(), operatorChars.end(), op) != 0) {
		return true;
	}
	return false;
}


/*substr�Ĳ��������±���ַ�������*/
/*���ж�ȡ����,����һ�н��зִ�*/
vector<string> DivideWord(string input)
{
	vector<string> res = stringMethod::split(input, " ");
	vector<string> tmp;
	vector<string> divideAgain;
	auto it = res.begin();
	while (it != res.end()) {
		if (IsTheStringNeedToDivideAgain(*it)) {
			size_t last = 0;
			for (size_t i = 0; i < (*it).size(); i++) {
				if (!isdigit((*it)[i]) && IsTwoCharOper((*it)[i], (*it)[i+1])) {
					if(last != i)
						divideAgain.push_back((*it).substr(last, i-last));
					string tmp = "";
					tmp = tmp + (*it)[i] + (*it)[i + 1];
					divideAgain.push_back(tmp);
					i += 2;
					if (i >= (*it).size()) {
						printf("�﷨����");
					}
					last = i;
					break;
				} 
				else {
					string op = string("");
					op = op + (*it)[i];
					if (count(operatorChars.begin(), operatorChars.end(), op) != 0) {
						if (i != last)
							divideAgain.push_back((*it).substr(last, i - last));
						divideAgain.push_back(op);
						i++;
						last = i;
					}
				}
			}
			if ((*it).size() != last)
				divideAgain.push_back((*it).substr(last, (*it).size() - last));
			it = res.erase(it);
			auto iter = divideAgain.rbegin();
			while (iter != divideAgain.rend()) {	//�������գ�divideAgain����Ķ����᲻�ϼӵ�res���棬Ȼ�󲻶ϳ�����Ҫ�ٷֵĴ�������ѭ����
				it = res.insert(it, *iter);
				iter++;
			}
			divideAgain.clear();
		}
		else
			it++;
	}
	return res;
}


bool IsNotOper(string &s)
{
	if (s != "+"&&s != "-"&&s != "*"&&s != "/"&&s != "="&&s != "!="&&s != ">"&&s != "<"&&s != "<="&&s != ">=")
		return true;
	else
		return false;
}



bool IsTheStringNeedToDivideAgain(string &s)
{
	if (IsNotOper(s)) {
		for (string oper : operatorChars) {
			if (s.find(oper) != string::npos) {
				return true;
			}
		}
	}
	return false;
}

typedef int TokenClass;
typedef string Value;
class Token {
public:
	TokenClass tokenClass;
	Value valueOrPos;
public:
	Token()
	{
		tokenClass = -1;
		valueOrPos = "";
	}
};



Token isKeyword(string str)
{
	Token res;
	for (size_t i = 0; i < keywords.size(); i++) {
		if (str == keywords[i]) {
			res.tokenClass = KEYWORD;
			res.valueOrPos = res.valueOrPos + to_string(int(i));
			return res;
		}
	}
	res.tokenClass = -1;
	return res;
}


Token isOperator(string str)
{
	Token res;
	for (size_t i = 0; i < operatorChars.size(); i++) {
		if (stringMethod::trim(str) == operatorChars[i]) {
			res.tokenClass = OPERATOR;
			res.valueOrPos = res.valueOrPos + str;
			return res;
		}
	}
	res.tokenClass = -1;
	return res;
}

vector<string> divide = { ",",";","\'","\"" };

Token isDivide(string s)
{
	Token res;
	for (size_t i = 0; i < divide.size(); i++) {
		if (stringMethod::trim(s) == divide[i]) {
			res.tokenClass = DIVIDE;
			res.valueOrPos = res.valueOrPos + s;
			return res;
		}
	}
	res.tokenClass = -1;
	return res;
}


int main(void)
{
	/*
		string zgs = "[a-zA-Z_][a-zA-Z0-9_]*";
		string zgs1 = "(a|b|c)*&a|b&(a&b|b&c)*";
		string res = Transform(zgs);
		cout << res;
	*/
	/*DFA dfa = UnitTestOfMinimizeDfa("test2.txt");
	string input = "bbbab";
	while (input != "fuck") {
		dfa.isAccept(input);
		cout << "\n\n\n" << "��һ�����룺\n";
		cin >> input;
	}*/

	////string zgs1 = "(a|b|c)*&a&b&(a&b|b&c)*";
	//string zgs2 = "(a|b)*&a&b&b";

	//string ss = Transform(zgs2);
	//FA fa = TransNFA(ss);
	//fa = NFA_TO_DFA(fa);
	//fa = Hopcroft(fa);
	//
	//putchar('\n');
	string input = "int aA=123++848";
	//�ִ�
	vector<string> words = DivideWord(input);
	for (string s : words) {
		cout << s << endl;
	}

	//string zgs = "(a|b*)*|d";
	//string res = Transform(zgs);
	//cout << res;
	//��ʶ��Dfa
	string identifier = "[a|A|_]&[a|A|0|_]*";
	string zgsback = Transform(identifier);
	cout << zgsback << endl;
	FA NFAIdentifier = TransNFA(zgsback);
	print(NFAIdentifier);
	DFA dfaIdentifier = NFA_TO_DFA(NFAIdentifier);
	dfaIdentifier = Hopcroft(dfaIdentifier);
	
	//����DFA
	string integer = "[1-9]&[0-9]*";
	string zgsbackInteger = Transform(integer);
	cout << zgsbackInteger << endl;
	FA NFAInteger = TransNFA(zgsbackInteger);
	DFA dfaInteger = NFA_TO_DFA(NFAInteger);
	dfaInteger = Hopcroft(dfaInteger);

	
	putchar('\n');
	putchar('\n');
	putchar('\n');
	putchar('\n');

	ofstream os("output.txt");

	for (string s : words) {
		if (isKeyword(s).tokenClass!=-1) {
			Token tmp = isKeyword(s);
			os << "(" << tmp.tokenClass << "," << tmp.valueOrPos.c_str() << ")" << endl;
		}
		else if (dfaIdentifier.isAccept(s)) {
			os << "(" << IDENTIFIER << "," << s << ")" << endl;
		}
		else if (dfaInteger.isAccept(s)) {
			os << "(" << CONSTANT << "," << s << ")" << endl;
		}
		else if (isOperator(s).tokenClass != -1) {
			Token tmp = isOperator(s);
			os << "(" << tmp.tokenClass << "," << tmp.valueOrPos.c_str() << ")" << endl;
		}
		else if (isDivide(s).tokenClass != -1) {
			Token tmp = isDivide(s);
			os << "(" << tmp.tokenClass << "," << tmp.valueOrPos.c_str() << ")" << endl;
		}
	}
	
	system("pause");
}

