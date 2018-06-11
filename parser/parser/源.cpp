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
#include <unordered_map> // TODO 用来做预测分析表
#include "parser.h"
#define MAX 501
#define EMPTY "~"
#define EMPTYCHAR '~'
#define HASH '#'


#define DIVIDE 'c'
#define IDENTIFIER 'b'
#define KEYCHAR 'a'
#define NUMBER 'd'
#define OPER 'e'


using namespace std;
using Syntax = vector<Production>;
vector<Syntax> syntaxBuffer;
vector<PredictTable> predictTables;
char lastNonTerminal;
map<string, int> syntaxDic;
Syntax proVector;	//一个语法，产生式集合
ProductionArray *proArray;
set<string> nullable;
string START;
static string openFile("declear.txt");

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
PredictTable& buildPredictive(int sum);

void readAndAnalysis(std::ifstream &is1);

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
			if ((*iter).at(0) == ch)//每次判断第一个，如果是左递归的那条	S->Sa|b|c|d|e ==> S->bS'|cS'|dS'|eS'
				newRights.insert((*iter).substr(1) + newLeft); // S'->aS'	
			else
			{
				buf.push_back(*iter + newLeft);
			}
			iter = (proVector[i].right).erase(iter);	//判断完去掉
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
                else
                    it++;
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
    printf("\n这是First集:\n");
	for (size_t i = 0; i < proVector.size(); i++)
		proVector[i].printFirst();
}

/*打印follow集*/
void printFollow()
{
    printf("\n这是Follow集:\n");
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
{	/*
		挨个读取产生式的字符，如果是终结符，就直接添加到first集
		如果是非终结符，去掉空添加到first集，然后判断它是否可以推出空，如果可以，继续遍历字符
		否则就换下一个产生式
	*/
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
							if (c != EMPTYCHAR) {
								proVector[indexN].firstSet.insert(c);
								CHANGING = true;
							}
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
		return proVector[getIndex(string("") + right[0])].firstSet;
	}
	return set<char>();
}

/*计算follow集*/
void calculateFollow(ProductionArray *production, const int N)
{
	/*
		复制一份当前产生式左边非终结符的follow集为temp，
		然后逆序遍历该产生式右边，
		如果是终结符，就把temp清空,并把这个终结符放进temp
		如果是非终结符，把temp添加到这个非终结符的follow,
		然后如果这个非终结符推不出空，就把这个非终结符的first复制给temp
		否则把该非终结符的first除去空放进temp
	*/
	set<char> temp;
	proVector[getIndex(START)].followSet.insert(HASH);	//开始符号follow添加#
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
							if (x != EMPTYCHAR)
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


//构造预测分析表
PredictTable& buildPredictive(int sum)
{
	PredictTable *predictTable = new PredictTable;
	PredictTable &pT = *predictTable;
	pT.col = new unordered_map<char, int>;
	pT.row = new unordered_map<char, int>;
	pT.START = START;
	pT.proArray = proArray;
	pT.proArraySize = sum;
	auto &col = *pT.col;
	auto &row = *pT.row;
	int countNonTerminal = 0;
	int countTerminal = 0;
	for (Production &s : proVector) {	//非终结符插入完成
		row[s.left[0]] = countNonTerminal++;
	}
	for (int i = 0; i < sum; i++) {
		for (char c : proArray[i].right) {	//是终结符且尚未添加
			if (c != EMPTYCHAR && row.count(c) == 0 && col.count(c) == 0) {
				col[c] = countTerminal++;
			}
		}
	}
	col[HASH] = countTerminal++;	//添加井号


	pT.table = new char*[row.size()];
	for (size_t i = 0; i < row.size(); i++) {
		pT.table[i] = new char[row.size()];
	}
	auto &table = pT.table;
	for (size_t i = 0; i < row.size(); i++) {
		for (size_t j = 0; j < col.size(); j++) {
			table[i][j] = -1;	//初始化预测分析表
		}
	}
	// 预测分析表赋值
	for (int i = 0; i < sum; i++) {
		set<char> firstSet = calculateFirst_S(proArray[i].right);
		for (char c : firstSet) {
			if (c != EMPTYCHAR)	//不是空
				table[row.at(proArray[i].left[0])][col.at(c)] = char(i);
		}
		if (firstSet.count(EMPTYCHAR) != 0) {
			set<char> &followSet = proVector[getIndex(proArray[i].left)].followSet;
			for (char c : followSet) {
				if (c != EMPTYCHAR)	//不是空
					table[row.at(proArray[i].left[0])][col.at(c)] = char(i);
			}
		}
	}
	return pT;
}



vector<Token> tokenTransform(string tokens)
{
    vector<Token> tokenVector;
    string res("");
    Token a;
    for (int i = 0; i < tokens.length(); i++) {
        if (tokens[i] == '(') {
            int j = i + 1;
            for (; j < tokens.length(); j++) {
                if (tokens[j] == ',')
                    break;
            }
            //res += tokens.substr(i + 1, j - i + 1);
            string typeStr = tokens.substr(i + 1, j - i - 1);
            int typeId = stoi(typeStr);
            a.type = typeId;
        }
        else if (tokens[i] == ',') {
            int j = i + 1;
            for (; j < tokens.length(); j++) {
                if (tokens[j] == ')')
                    break;
            }
            //res += tokens.substr(i + 1, j - i + 1);
            string valueStr = tokens.substr(i + 1, j - i - 1);
            a.value = valueStr;
            tokenVector.push_back(a);
        }
    }
    return tokenVector;
}



char getTerminal(int n)
{
    switch (n) {
    case 1: return KEYCHAR; //int,folat,double...
    case 2: return IDENTIFIER; //abc
    case 3: return DIVIDE;  //,;
    case 4: return NUMBER;
    case 5: return 'e';
    case 6: return 'f';
    default:return '\0';
    }
}



string getStrToAnalysis(vector<Token> &tokenVector)
{
    string res = "";
    char ch;
    for (Token token : tokenVector) {
        ch = getTerminal(token.type);
        if (ch == DIVIDE) {
            ch = divide[stoi(token.value)];
        }
        else if (ch == OPER) {
            ch = oper[stoi(token.value)];
        }
        res += ch;
    }
    return res;
}


string getFinalStr(string tokens)
{
    auto tokenVector = tokenTransform(tokens);
    return getStrToAnalysis(tokenVector);
}





void analysis(PushDownAuto &pd, string statement)
{
    printf("这是分析过程\n\n");
    bool flag = false;
    for (int i = 0; i <= predictTables.size(); i++) {
        pd.changeTable(&predictTables[i]);
        flag = pd.analysis(statement);
        if (flag)
            return;
    }
}


void readAndAnalysis(std::ifstream &is1)
{
    string tokens;
    PushDownAuto pdAuto;    //构造下推自动机,默认自动打开一个文件作为输出
    while (!is1.eof()) {
        is1 >> tokens;
        string statement = getFinalStr(tokens);

        analysis(pdAuto, statement);
    }
    pdAuto.os.close();  //关闭文件
}

int main()
{
	char buf[MAX];
	const int LINE_LENGTH = 500;
	ifstream is(openFile);
	/*有几个语法*/
	int N;
	is >> N;	// TODO 这里，每次读入一个语法，然后把这个语法的预测分析表，下推自动机生成以后，再读下一个语法。
	is.getline(buf, LINE_LENGTH);	//读取空行
	/*输入文法,构造N张预测分析表*/
	while (N-- != 0) {
		inputSyntax(is, buf, LINE_LENGTH);
		/*打印输入的文法*/
		//printSyntax();
		putchar('\n');
			/*把最初输入的WF原样保存*/
		Syntax tmpSyntax(proVector);
		syntaxBuffer.push_back(tmpSyntax);

        printf("\n原产生式:\n");
        printSyntax();

		/*消除左递归*/
		remove();
        printf("\n消除左递归以后的产生式:\n");
        printSyntax();

		/*提取左因子*/
		extract();
        printf("\n提取左因子以后的产生式:\n");
        printSyntax();

	//-----------------------------------------------------
		int sum = 0;	//做完消左递归和提左因子以后的正规式数
		for (Production cfg : proVector) {
			sum += cfg.right.size();
		}
		proArray = new ProductionArray[sum];
		int count = 0;
		for (const Production &cfg : proVector) {
			for (const string &s : cfg.right) {
				proArray[count].left = cfg.left;
				proArray[count].right = s;
				count++;
			}
		}
		
	//-----------------------------------------------------
		calculateNullable(proArray, sum);

		calculateFirst(proArray, sum);  //计算First
        printFirst();

		calculateFollow(proArray, sum); //计算Follow
        printFollow();
    //------------------------------------------------------
		PredictTable pt = buildPredictive(sum); //构造预测分析表
		predictTables.push_back(pt);    //构造一张就放到vector里面
	}
	is.close();
		//pt.print();
	//printf("以上是临时用\n\n");
    //打印每张预测分析表
	for (PredictTable tb : predictTables) {
		tb.print();
	}
	putchar('\n');
	ifstream is1("input.txt");  //读取要输入的记号流
    readAndAnalysis(is1);
	is1.close();
	//char tmpR=' ', tmpC=' ';
	//while (tmpR != '9') {
	//	printf("输入:");
	//	scanf(" %c", &tmpR);
	//	scanf(" %c", &tmpC);
	//	printf("\n\n\n\n%d", pt.getProIndex(tmpR, tmpC));
	//}
	system("pause");
}