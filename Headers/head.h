#pragma once
#define FILENAMEMAX 80  //�ļ����ַ����������ֵ
#define POS 0  //����literal��ά�����������Ϊ�϶���ʽ
#define NEG 1  //ͬ�ϱ�ʾ����ʽ
#define WORDMAX 20  //��ȡ�ļ�ʱÿ���ʶ�Ӧ�ַ�������󳤶�
#define SATISFIABLE 1
#define UNSATISFIABLE 0
#define CONFLICT 0
#define UNKNOWN -1
#define YES 1
#define NO 0
#define ASSIGNED 1
#define UNASSIGNED 0
#define INCREMENT 50  //���鶯̬������

typedef struct lit_node {  //������clause�ϱ�ʾ���ֵ�����������
	int literal;
	lit_node * next;
}lit_node;
typedef struct clause {  //clause�ڽӱ������ͣ���1��ʼ����
	int is_satisfied;
	int current_length;
	lit_node * firstlit;
}clause;

typedef struct cla_node {  //������literal�ϱ�ʾ�Ӿ������������
	int clause;  //clause��1��ʼ������0��ʾͷ�ڵ�
	cla_node * next;
}cla_node;
typedef struct literal {  //literal�ڽӱ�������
	int is_assigned;
	int n_occur;
	cla_node * firstcla;
}literal;

typedef int unitclause;  //���Ӿ������ջ�������
typedef int value;  //��ֵջ�������

typedef struct cnftype {
	int n_variable;
	int n_clause;
	literal(*literals)[2];  //ָ���ά�����ָ��
	clause * clauses;  //ָ�������ָ��

	//��Ϣ����ṹ
	unitclause * unitclauses;  //ָ��������Ӿ�ջ��ָ��
	int unitclause_index;  //ָ���������Ӿ���Ŀ
	value * values;  //ָ��ֵջ��ָ��
	int value_index;  //ָ���Ѹ�ֵ��Ԫ��Ŀ
	int * pure_literals;  //�洢�����ֵ�ջ
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

