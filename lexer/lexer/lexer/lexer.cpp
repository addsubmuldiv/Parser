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

int findGroupId(int state);	//给定一个状态找在哪个组

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
	void Transition(char nextChar);	//TODO 对一个状态，DFA接受一个字符进行转移
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
/*输出这个FA里面的内容，状态，边，接受状态是哪几个*/
void FA::Print()
{
	printf("状态：\n");
	for (State s : S) {
		printf("%d ", s);
	}
	putchar('\n');

	printf("起始状态：\n");
	printf("%d\n", S0);
	putchar('\n');

	printf("边：\n");
	for (Edge e : edges) {
		e.Print();
	}

	printf("接受状态\n");
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
		printf("不接受的字符");
		currentState = -1;
	}
	for (Edge e : edges) {
		if (e.start == currentState && e.letter == nextChar) {
			currentState = e.end;
			return;
		}
	}
	printf("没有这样的边，初态为 %d , 接受 %c 到达另一个状态！", currentState, nextChar);
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
		printf("\n\n接受该字符串！\n\n");
		return true;
	}
	printf("该自动机不接受此字符串");
	return false;
}
typedef FA DFA;
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


GroupId Transition(State state, char character, vector<Edge> edges)	//对于一个状态，输入一个字符得到结果状态所在组
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


bool Group::isInThisGroup(int state)	//判断一个状态是不是属于这个组
{
	if (this->states.count(state) != 0)
		return true;
	else
		return false;
}

void Group::Print()
{
	auto it = states.begin();
	printf("组%d\n", groupId);
	for (; it != states.end(); it++) {
		printf("%d ", *it);
	}
	putchar('\n');
}

/*看一条边是否是从这个组出发的*/
bool Group::isEdgeFromThisGroup(Edge e)
{
	if (e.startGroupId == groupId)
		return true;
	return false;
}
/*这个状态是否可分*/
bool Group::isSeparable(DFA &dfa)
{
	vector<Edge> &edges = dfa.edges;
	if (this->states.size() == 1) {	//只有一个状态时，不可分
		return false;
	}
	vector<Edge> edgeOut;	//得到从这个组出发的所有边
	for (Edge e : edges) {
		if (isEdgeInThisGroup(e)) {
			continue;
		}
		if (isEdgeFromThisGroup(e)) {	//这种就是向外转移的边
			edgeOut.push_back(e);
		}
	}
	for (char ch : dfa.chars) {	//对所有字符集，审视所有状态的目标组，包括所有情况
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

/*获得所有从这个组出发的边*/
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

GroupId ID = 0;		//区别组的ID
					//bool GROUPCHANGED = false; //组是否在变化
vector<Group> groups;

int findGroupId(int state)	//给定一个状态找在哪个组
{
	for (size_t i = 0; i < groups.size(); i++) {
		if (groups[i].states.count(state) != 0)
			return groups[i].groupId;
	}
	return -1;
}

void edgeToGroup(FA &dfa)	//我们需要知道哪些边要合成一个边,对每个边记录它的起点和终点所在的组
{
	vector<Edge> &edges = dfa.edges;
	for (Edge &e : edges) {	//做改变要用引用
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

bool findToGroupAndPush(State state, char character, vector<Edge> edges)	//如果已经有了起点和字符一样且终点一样的组那就直接push
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
	for (size_t i = 0; i < groups.size(); i++) { //对于组，需要动态在后面添加元素，用for
												 //										 /*通过遍历边来确定如何再分组*/
		/*有可能出现一个组完全空了的情况，要删去*/
		for (auto it = groups.begin(); it != groups.end();) {
			if ((*it).states.empty()) {
				it = groups.erase(it);
			}
			else
				it++;
		}
		if (groups[i].isSeparable(dfa)) {
			GROUPCHANGED = true;

			vector<Edge> edges = groups[i].getEdgeStartFromThisGroup(dfa.edges);//从这个组出发的所有边

			for (char ch : dfa.chars) {	//对每一个dfa接受的字符
				auto it = groups[i].states.begin();
				while (it != groups[i].states.end()) {	//对每一个该组中的状态
					/*运气好啊，其实状态始终是第0个组的第0个元素，所以在分开的时候
					分出去的一定不会是它……所以它的分组ID始终就是0*/
					State tmpState = *it;	//先保存这个状态
					GroupId targetGroup = Transition(tmpState, ch, edges);
					if (targetGroup != groups[i].groupId) {		//看在这个字符下该状态的目标组是不是本组
						/*不是的话，就要给他分出来，如果在已经分出来的里面有同样的，就丢进去，否则新建一个*/
						printf("此时因接受字母 %c 到达组 %d 被从组 %d 删掉的状态是 %d\n", ch, targetGroup, groups[i].groupId, *it);
						it = groups[i].states.erase(it);	//从这个组中删除
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
			auto &states = tG.states;	//这是新分出的组的状态集合
			Group newGroup(ID, groups[i].isAccept);	//新的组是接受还是非接受组由它的父组的决定
			//copy(states.begin(), states.end(), newGroup.states.begin());	
			for (auto iter = states.begin(); iter != states.end(); iter++) {//新建一个组，把分出来的状态放进去
				/*如果新分的组的状态有 0 ，即其实状态，那么把这个组标识为起始组*/
				if (*iter == 0) {
					STARTGROUP = newGroup.groupId;
				}
				newGroup.states.insert(*iter);
			}
			groups.push_back(newGroup);		//新组放到组的集合里面
		}

		edgeToGroup(dfa);//更新边的组信息
		dfa.PrintEdgeGroup();
		toGroups.clear();
	}
}

DFA ConstructDfa(DFA dfa)
{
	//-----------构造最小DFA
	DFA newDfa;
	newDfa.chars.resize(dfa.chars.size());
	/*对于copy算法，目标容器必须实现申请好足够的空间*/
	copy(dfa.chars.begin(), dfa.chars.end(), newDfa.chars.begin());	//字符集没有变化
	set<Edge> newEdgeSet;
	vector<Edge> &tmpEdges = newDfa.edges;	//用引用让代码简洁
	for (Edge e : dfa.edges) {
		if (isTheEdgeExist(e, tmpEdges))	//如果边是组内边或者边已经存在
			continue;
		else
			tmpEdges.push_back(e);
	}	//tmpEdges即新的边
		//copy(tmpEdges.begin(), tmpEdges.end(),& newDFA.edges.begin());	//把新的边放入新DFA
	for (auto iter = groups.begin(); iter != groups.end(); iter++) {	//去掉空组
		if ((*iter).states.empty())
			iter = groups.erase(iter);
	}
	for (Group group : groups) {	//这里的边和下标对不上,接受状态也是
		newDfa.S.push_back(group.groupId);
		/*如果哪个组是起始组，那么它的下标所在就是开始的地方*/
		if (group.groupId == STARTGROUP) {
			newDfa.S0 = newDfa.S.size() - 1;
		}
		if (group.isAccept) {
			newDfa.finalStates.push_back(group.groupId);
		}
	}
	for (size_t i = 0; i < newDfa.S.size(); i++) {	//下标和边对上就行,状态自己保存的啥没影响
		for (Edge &e : newDfa.edges) {
			if (e.startGroupId == newDfa.S[i])
				e.start = i;
			if (e.endGroupId == newDfa.S[i])
				e.end = i;
		}
		for (State &state : newDfa.finalStates) {	//把接受状态的下标改对
			if (state == newDfa.S[i])
				state = i;
		}
	}
	return newDfa;
}

DFA Hopcroft(DFA &dfa)
{
	Group unAccept(ID, UNACCEPT);	//非接受是0
	Group accept(ID, ACCEPT);	//接受是1
	for (int x : dfa.finalStates) {
		accept.states.insert(x);
	}
	printf("接受状态组划分完成\n");
	accept.Print();

	for (size_t i = 0; i < dfa.S.size(); i++) {
		if (accept.states.count(i) == 0) {  //如果这个状态不是接受状态
			unAccept.states.insert(i);	//就放到非接受状态的集合里
		}
	}
	printf("非接受状态划分完成\n");
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
	printf("\n\n\n最终效果\n\n\n");
	finalDfa.Print();
	return finalDfa;
}



DFA UnitTestOfMinimizeDfa(const char *str)
{
	DFA dfa;
	/*
	从文件读取输入 首行数字 状态数M 边数N 字符数T 接受状态数
	接下来N行是边 格式是 起点 终点 字符
	然后一行是 字符集合
	最后一行是 接受状态集合
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

	for (int i = 0; i < M; i++) {	//随便给状态填入一些数，反正重要的是下标
		dfa.S.push_back(i);
	}

	int start, end;
	char letter;
	for (int i = 0; i < N; i++) {	//输入边
		is >> start;
		is >> end;
		is >> letter;
		dfa.edges.push_back(Edge(start, end, letter));
	}

	for (int i = 0; i < T; i++) {	//输入字符集
		is >> letter;
		dfa.chars.push_back(letter);
	}

	int acceptState;
	for (int i = 0; i < A; i++) { //输入接受状态
		is >> acceptState;
		dfa.finalStates.push_back(acceptState);
	}

	//到此，就是待最小化的DFA

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
		cout << "\n\n\n" << "下一次输入：\n";
		cin >> input;
	}
	system("pause");
}

