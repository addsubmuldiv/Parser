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
	void Transition(char nextChar);	//TODO 对一个状态，DFA接受一个字符进行转移
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

	int T1 = S_num + 1;//每次添加俩种状态
	int T2 = S_num + 2;

	S.push_back(T1);
	S.push_back(T2);
	this->chars.push_back(zgs);

	Edge es(T1, T2, zgs);//添加一条边
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
		//printf("不接受的字符");
		currentState = -1;
	}
	for (Edge e : edges) {
		if (e.start == currentState && e.letter == nextChar) {
			currentState = e.end;
			return;
		}
	}
	//printf("没有这样的边，初态为 %d , 接受 %c 到达另一个状态！", currentState, nextChar);
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
		//printf("\n\n接受该字符串！\n\n");
		return true;
	}
	//printf("该自动机不接受此字符串");
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
	State T1 = S_num + 1;//每次添加俩种状态
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, T2, '~');//添加四条边
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

	FA reNFA = NFA1;//把NFA1放进去

					//vector<char> chars;字符集

	for (int i = 0; i<NFA2.S.size(); i++) {//把NFA2的S放进去
		State temp;
		temp = NFA2.S[i];
		reNFA.S.push_back(temp);
	}
	for (int i = 0; i<NFA2.chars.size(); i++) {//把NFA2的chars放进去
		char temp;
		temp = NFA2.chars[i];
		reNFA.chars.push_back(temp);
	}
	//vector<Edge> edges;
	for (int i = 0; i<NFA2.edges.size(); i++) {//把NFA2的edges放进去

		reNFA.edges.push_back(NFA2.edges[i]);
	}


	State T1 = S_num + 1;//每次添加俩种状态
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, NFA1.S0, '~');//添加四条边
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
	FA reNFA = NFA1;//把NFA1放进去

					//vector<char> chars;字符集

	for (int i = 0; i<NFA2.S.size(); i++) {//把NFA2的S放进去
		State temp;
		temp = NFA2.S[i];
		reNFA.S.push_back(temp);
	}
	for (int i = 0; i<NFA2.chars.size(); i++) {//把NFA2的chars放进去
		char temp;
		temp = NFA2.chars[i];
		reNFA.chars.push_back(temp);
	}
	//vector<Edge> edges;
	for (int i = 0; i<NFA2.edges.size(); i++) {//把NFA2的edges放进去

		reNFA.edges.push_back(NFA2.edges[i]);
	}
	reNFA.f = NFA2.f;

	State T1 = S_num + 1;//每次添加俩种状态
	State T2 = S_num + 2;
	reNFA.S.push_back(T1);
	reNFA.S.push_back(T2);
	Edge es(T1, reNFA.S0, '~');//添加三条边
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

	cout << "初态:" << NFA.S0 << "             终态：" << NFA.f << endl;
	cout << "状态集：      ";
	for (int i = 0; i<NFA.S.size(); i++) {

		cout << NFA.S[i] << "    ";

	}
	cout << endl;
	cout << "字符集：      ";
	for (int i = 0; i<NFA.chars.size(); i++) {

		cout << NFA.chars[i];
	}
	cout << endl;
	cout << "边集：      " << endl;
	for (int i = 0; i<NFA.edges.size(); i++) {

		cout << "起点：" << NFA.edges[i].start << "           终点：" << NFA.edges[i].end << "           条件：" << NFA.edges[i].letter << endl;

	}
}





FA TransNFA(string zgs) {
	stack <FA>stk;//定义一个NFA栈
	zgs = zgs + '#';
	FA NFA;
	stk.push(NFA);		//哨兵
	int i = 0;
	while (zgs[i] != '#') {
		if (zgs[i] != '|'&&zgs[i] != '&'&&zgs[i] != '*') {//表达式压栈
			FA NFAs(zgs[i]);
			NFA = NFAs;
			stk.push(NFA);
		}
		else if (zgs[i] == '*') {//闭包
			NFA = stk.top();
			stk.pop();
			NFA = bibao(NFA);
			stk.push(NFA);
		}
		else if (zgs[i] == '|')//或
		{
			FA NFA1 = stk.top(); stk.pop();
			FA NFA2 = stk.top(); stk.pop();
			NFA = huo(NFA1, NFA2);
			stk.push(NFA);
		}
		else {//连接
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
	//初始化 DFA的字符集 ，去掉空
	for (int i = 0; i < NFA.chars.size(); i++)
	{
		if (NFA.chars[i] == '~') {
			continue;
		}
		else {
			dfa.chars.push_back(NFA.chars[i]);
		}
	}

	vector <vector<int> > unit; //定义二维向量 记录接下的生成的集合
	vector <int> u;
	u.push_back(999);
	unit.push_back(u);
	unit[0].push_back(0);
	unit[0].push_back(NFA.S0); 
							   //--------建立第一个集合----
							   //1.直接空转移
	for (int i = 0; i < NFA.edges.size(); i++)
	{
		if (NFA.edges[i].start == NFA.S0 && NFA.edges[i].letter == '~')
		{
			unit[0].push_back(NFA.edges[i].end);   //初始状态开始的所有直接空转移到达的状态
		}                                          
	}
	//2.间接空转移
	for (int i = 3; i < unit[0].size(); i++)	//初始状态的间接空转移
	{
		for (int j = 0; j < NFA.edges.size(); j++)         //在上一步的基础上，查询原NFA，找二次空转移后所获得的状态
		{
			if (unit[0][i] == NFA.edges[j].start && NFA.edges[j].letter == '~')//例如：999,0,开始状态0,1,7，2,4（通过1可以找到 2,4）
			{
				unit[0].push_back(NFA.edges[j].end);  //存入向量中
			}
		}
	}
	//第一个集合建立完毕，接下扩建DFA
	
	dfa.dfaEdgeCount = 1; //初始化DFA图边数1
	u.pop_back();
	u.push_back(1000);//1000代表空
	unit.push_back(u);
	for (int i = 0; i < unit.size(); i++)
	{                                                    
		if (unit[i][0] == 999)                         
		{                                              
			for (int j = 0; j < dfa.chars.size(); j++)    //循环每一个分支条件， 例如a，b,c条件
			{                                            
				if (unit[dfa.dfaEdgeCount][0] == 1000)
				{ //当前位置是否为空，空则代表上一个转换符的集合不为空，不为空则代表上一个转换符没有用到
					unit.pop_back();

					u.pop_back();
					u.push_back(999);
					unit.push_back(u);
					unit[dfa.dfaEdgeCount].push_back(j);
				}


				for (int k = 2; k < unit[i].size(); k++)//直接转移
				{                                                      //找出当前集合当前条件  会去到的状态
					for (int h = 0; h < NFA.edges.size(); h++)
					{                                                //查询NFA图中所有的边，直接可以到达的终态 例如：3,8
						if (unit[i][k] == NFA.edges[h].start && dfa.chars[j] == NFA.edges[h].letter)
						{                                            //找到NFA图中开始状态相同，通过相同的条件后

							unit[dfa.dfaEdgeCount].push_back(NFA.edges[h].end);
						}
					}
				}

				if (unit[dfa.dfaEdgeCount].size() == 2)
				{
					continue;//这个转换符没有用到，空集合
				}
				


				Edge e(i, dfa.dfaEdgeCount, dfa.chars[j]);
				dfa.edges.push_back(e);   //将e这条边 放入 DFA中


										  //---转移后的补全，集合内部进行遍历------
				for (int l = 2; l < unit[dfa.dfaEdgeCount].size(); l++)
				{
					for (int m = 0; m < NFA.edges.size(); m++)//查询NFA图
					{
						if (unit[dfa.dfaEdgeCount][l] == NFA.edges[m].start&&NFA.edges[m].letter == '~')
						{
							//如果初始状态相同，转换符为空  且
							int index; //查重
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



				//查询整个DFA中的集合是否有重复的出现
				bool issame = false;
				int flag = -1;
				int k, n;
				for (k = 0; k<unit.size() - 1; k++)
				{
					for (n = 2; n<unit[k].size() && n<unit[unit.size() - 1].size(); n++)//防止下标越界
					{                                    //前者限制比较的行长度  后者是和遍历过的
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
							flag = k;//把当前行数赋给FLAG	
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
					unit[unit.size() - 1][0] = flag;//每行首位数字代表 和他重复的行数位置
				}
				dfa.dfaEdgeCount++;//最后边数多1
				u.pop_back();
				u.push_back(1000);
				unit.push_back(u);
			}
		}
	}
	


	int news = 0;//合并新状态标号
	for (int i = 0; i < unit.size(); i++)
	{
		if (unit[i][0] == 999)
		{
			unit[i][0] = 999 + news;
			news++;
			dfa.S.push_back(unit[i][0] - 999);//整理DFA的状态集
			for (int j = 2; j < unit[i].size(); j++)
			{
				if (unit[i][j] == NFA.f)
				{
					
					dfa.finalStates.push_back(unit[i][0] - 999);//所有终态
					break;
				}  //整理终态，把终态集合重命名
			}
		}
	}


	for (int i = 0; i < dfa.dfaEdgeCount - 1; i++) {//dfa图中下标变状态
		if (unit[dfa.edges[i].start][0] >= 999) {//初始状态改变
			dfa.edges[i].start = unit[dfa.edges[i].start][0] - 999;//（不是重复出现的）
		}
		else {
			dfa.edges[i].start = unit[unit[dfa.edges[i].start][0]][0] - 999;//（重复出现的集合）
		}

		if (unit[dfa.edges[i].end][0] >= 999) {//终止状态改变
			dfa.edges[i].end = unit[dfa.edges[i].end][0] - 999;//不是重复出现的）
		}
		else {
			dfa.edges[i].end = unit[unit[dfa.edges[i].end][0]][0] - 999;//d重复出现的集合）
		}

	}

	cout << endl << "最终DFA如下：" << endl;
	dfa.edges = distinct(dfa.edges);
	print(dfa);
	return dfa;
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


/*substr的参数是首下标和字符数……*/
/*按行读取输入,对这一行进行分词*/
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
						printf("语法错误");
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
			while (iter != divideAgain.rend()) {	//如果不清空，divideAgain里面的东西会不断加到res里面，然后不断出现需要再分的串，就死循环了
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
		cout << "\n\n\n" << "下一次输入：\n";
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
	//分词
	vector<string> words = DivideWord(input);
	for (string s : words) {
		cout << s << endl;
	}

	//string zgs = "(a|b*)*|d";
	//string res = Transform(zgs);
	//cout << res;
	//标识符Dfa
	string identifier = "[a|A|_]&[a|A|0|_]*";
	string zgsback = Transform(identifier);
	cout << zgsback << endl;
	FA NFAIdentifier = TransNFA(zgsback);
	print(NFAIdentifier);
	DFA dfaIdentifier = NFA_TO_DFA(NFAIdentifier);
	dfaIdentifier = Hopcroft(dfaIdentifier);
	
	//数字DFA
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

