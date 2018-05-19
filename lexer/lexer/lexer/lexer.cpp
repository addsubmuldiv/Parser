#include <iostream>
#include <vector>
#include <string>
#include <stack>
#include <set>
#include <iterator>
#include <fstream>
#include <algorithm>
using namespace std;

#define CLOSURE '*'
#define JOIN '&'
#define OR '|'
const static bool UNACCEPT = false;
const static bool ACCEPT = true;
typedef unsigned int GroupId;
typedef int State;
GroupId STARTGROUP;
bool GROUPCHANGED = false;

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


class FA {
public:
	vector<State> S;
	vector<char> chars;
	vector<Edge> edges;
	int S0, f;
	vector<int> finalStates;
	string zgs;
	State currentState;
public:
	FA(string zgs);
	FA() {}
	void Print();
	void PrintEdgeGroup();
	void Transition(char nextChar);	//TODO ��һ��״̬��DFA����һ���ַ�����ת��
	bool isCurrentAccept();
	bool isAccept(string str);
	inline State getCurrentState()
	{
		return currentState;
	}
};

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
		printf("�����ܵ��ַ�");
		currentState = -1;
	}
	for (Edge e : edges) {
		if (e.start == currentState && e.letter == nextChar) {
			currentState = e.end;
			return;
		}
	}
	printf("û�������ıߣ���̬Ϊ %d , ���� %c ������һ��״̬��", currentState, nextChar);
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
		printf("\n\n���ܸ��ַ�����\n\n");
		return true;
	}
	printf("���Զ��������ܴ��ַ���");
	return false;
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
					/*�����ð�����ʵ״̬ʼ���ǵ�0����ĵ�0��Ԫ�أ������ڷֿ���ʱ��
					�ֳ�ȥ��һ���������������������ķ���IDʼ�վ���0*/
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





int main(void)
{
	/*
		string zgs = "[a-zA-Z_][a-zA-Z0-9_]*";
		string zgs1 = "(a|b|c)*&a|b&(a&b|b&c)*";
		string res = Transform(zgs);
		cout << res;
	*/
	DFA dfa = UnitTestOfMinimizeDfa("test2.txt");
	string input = "bbbab";
	while (input != "fuck") {
		dfa.isAccept(input);
		cout << "\n\n\n" << "��һ�����룺\n";
		cin >> input;
	}
	system("pause");
}

