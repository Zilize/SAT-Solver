#pragma once
#define FILENAMEMAX 80  //文件名字符串长度最大值
#define POS 0  //用于literal二维数组表明文字为肯定形式
#define NEG 1  //同上表示否定形式
#define WORDMAX 20  //读取文件时每个词对应字符串的最大长度
#define SATISFIABLE 1
#define UNSATISFIABLE 0
#define CONFLICT 0
#define UNKNOWN -1
#define YES 1
#define NO 0
#define ASSIGNED 1
#define UNASSIGNED 0
#define INCREMENT 50  //数组动态扩大量

typedef struct lit_node {  //附着于clause上表示文字的链表结点类型
	int literal;
	lit_node * next;
}lit_node;
typedef struct clause {  //clause邻接表结点类型，从1开始计数
	int is_satisfied;
	int current_length;
	lit_node * firstlit;
}clause;

typedef struct cla_node {  //附着于literal上表示子句的链表结点类型
	int clause;  //clause从1开始计数，0表示头节点
	cla_node * next;
}cla_node;
typedef struct literal {  //literal邻接表结点类型
	int is_assigned;
	int n_occur;
	cla_node * firstcla;
}literal;

typedef int unitclause;  //单子句待处理栈结点类型
typedef int value;  //赋值栈结点类型

typedef struct cnftype {
	int n_variable;
	int n_clause;
	literal(*literals)[2];  //指向二维数组的指针
	clause * clauses;  //指向数组的指针

	//信息处理结构
	unitclause * unitclauses;  //指向待处理单子句栈的指针
	int unitclause_index;  //指明待处理单子句数目
	value * values;  //指向赋值栈的指针
	int value_index;  //指明已赋值变元数目
	int * pure_literals;  //存储纯文字的栈
	int pure_literal_index;
}cnftype;

//Display
void display_main();
void display_sat();
void display_sudoku();

//CNFparser
cnftype * readfile();
void show_list(cnftype * cnf);

//DPLLsolve
int DPLL(cnftype * cnf, int branch_literal);
int UP(cnftype * cnf, int up_literal);
int pure_process(cnftype * cnf, int pure_literal);
int decide_next_branch(cnftype * cnf);
cnftype * copy(cnftype * cnf);
void freecnf(cnftype * cnf);
int verify(cnftype * cnf, value * values);
void write_res(cnftype * cnf, int status, double time);
int decide_next_branch_max(cnftype * cnf);

//Sudoku
void read_sudoku(int i);
char * write_cnf(int isudoku);
void decoder(value * values, cnftype * cnf);

