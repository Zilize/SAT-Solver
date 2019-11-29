#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
extern char cnf_filename[FILENAMEMAX];

/*
函数功能：将cnf文件内容读取到内存中
输入：无
输出：指向cnftype类型的指针
*/
cnftype * readfile() {
	FILE * fp;
	cnftype * cnf;
	errno_t err;
	char ch, word[WORDMAX];

	//printf("%s", cnf_filename);
	//getchar();


	cnf = (cnftype *)malloc(sizeof(cnftype));
	if ((err = fopen_s(&fp, cnf_filename, "r")) != 0) {
		printf("OPEN FILE ERROR!");
		exit(0);
	}
	ch = getc(fp);
	while (ch == 'c') {  //忽略c开头的注释行
		while ((ch = getc(fp)) != '\n');
		ch = getc(fp);
	}
	for (int i = 0; i < 2; i++) { fscanf_s(fp, "%s", word, WORDMAX * sizeof(char)); }
	cnf->n_variable = atoi(word);
	fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
	cnf->n_clause = atoi(word);

	//开辟空间并初始化
	cnf->literals = (literal(*)[2])malloc(2 * cnf->n_variable * sizeof(literal));
	cnf->clauses = (clause *)malloc(cnf->n_clause * sizeof(clause));
	for (int i = 0; i < cnf->n_variable; i++) {  //给结点赋初值
		cnf->literals[i][POS].is_assigned = NO;
		cnf->literals[i][POS].n_occur = 0;
		cnf->literals[i][POS].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf->literals[i][POS].firstcla->clause = 0;  //0表示头节点
		cnf->literals[i][POS].firstcla->next = NULL;
		cnf->literals[i][NEG].is_assigned = NO;
		cnf->literals[i][NEG].n_occur = 0;
		cnf->literals[i][NEG].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf->literals[i][NEG].firstcla->clause = 0;  //0表示头节点
		cnf->literals[i][NEG].firstcla->next = NULL;
	}
	for (int i = 0; i < cnf->n_clause; i++) {  //给结点赋初值
		cnf->clauses[i].is_satisfied = NO;
		cnf->clauses[i].current_length = 0;
		cnf->clauses[i].firstlit = (lit_node *)malloc(sizeof(lit_node));
		cnf->clauses[i].firstlit->literal = 0;  //0表示头节点
		cnf->clauses[i].firstlit->next = NULL;
	}

	cla_node * cnp;  //指针用于辅助开辟结点
	lit_node * lnp;  //同上
	for (int i = 0; i < cnf->n_clause; i++) {  //for循环录入文件信息
		int status = 0, num;  //status:判断文字正负号的标记
		lnp = cnf->clauses[i].firstlit;  //使lnp指向头节点
		fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
		while (strcmp(word, "0")) {
			num = atoi(word);
			//处理clause邻接表
			lnp->next = (lit_node *)malloc(sizeof(lit_node));
			lnp = lnp->next;
			lnp->literal = num;
			lnp->next = NULL;
			cnf->clauses[i].current_length++;
			//处理literal邻接表
			if (num > 0) status = 0;
			else status = 1;
			cnp = cnf->literals[abs(num) - 1][status].firstcla;  //注意abs(num)需减一
			while (cnp->next != NULL) {  //将cnp指针移至链表末位
				cnp = cnp->next;
			}
			cnp->next = (cla_node *)malloc(sizeof(cla_node));
			cnp = cnp->next;
			cnp->clause = i + 1;  //注意i需加一
			cnp->next = NULL;
			cnf->literals[abs(num) - 1][status].n_occur++;
			//读取下一个数字
			fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
		}
	}

	//初始化信息处理结构
	cnf->unitclauses = (unitclause *)malloc(cnf->n_variable * sizeof(unitclause));
	for (int i = 0; i < cnf->n_variable; i++) {
		cnf->unitclauses[i] = 0;
	}
	cnf->unitclause_index = 0;
	cnf->values = (value *)malloc(cnf->n_variable * sizeof(value));
	for (int i = 0; i < cnf->n_variable; i++) {
		cnf->values[i] = 0;
	}
	cnf->value_index = 0;
	cnf->pure_literals = (int *)malloc(cnf->n_variable * sizeof(int));
	cnf->pure_literal_index = 0;
	for (int i = 0; i < cnf->n_variable; i++) {
		cnf->pure_literals[i] = 0;
	}
	for (int i = 0; i < cnf->n_variable; i++) {  //初始化不必考虑有没有被赋值
		if (cnf->literals[i][POS].n_occur == 0 || cnf->literals[i][NEG].n_occur == 0) {
			cnf->pure_literals[cnf->pure_literal_index++] = i + 1;  //注意存的不带符号
		}
	}

	fclose(fp);
	return cnf;
}

/*
函数功能：输出Clause和Literal邻接表的信息
输入：指向cnftype类型的指针
输出：无
*/
void show_list(cnftype * cnf) {
	cla_node * cnp;
	lit_node * lnp;
	printf("CLAUSE TEST\n");
	for (int i = 0; i < cnf->n_clause; i++) {
		printf("Clause %4d:  Is_satisfied(%d)  CurrentLength(%d)\n              Literals:  ", i + 1, cnf->clauses[i].is_satisfied, cnf->clauses[i].current_length);
		lnp = cnf->clauses[i].firstlit;  //指向头节点
		if (lnp->next == NULL) {
			printf("NULL\n");
		}
		while (lnp->next != NULL) {
			lnp = lnp->next;
			printf("%4d  ", lnp->literal);
		}
		printf("\n");
	}
	printf("\nLITERAL TEST\n");
	for (int i = 0; i < cnf->n_variable; i++) {
		printf("Variable %3d: POS: Is_assigned(%d)  N_occur(%d)\n", i + 1, cnf->literals[i][POS].is_assigned, cnf->literals[i][POS].n_occur);
		printf("              NEG: Is_assigned(%d)  N_occur(%d)\n", cnf->literals[i][NEG].is_assigned, cnf->literals[i][NEG].n_occur);
		printf("              POS clauses:  ");
		cnp = cnf->literals[i][POS].firstcla;  //指向头节点
		if (cnp->next == NULL) {
			printf("NULL");
		}
		while (cnp->next != NULL) {
			cnp = cnp->next;
			printf("%4d  ", cnp->clause);
		}
		printf("\n              NEG clauses:  ");
		cnp = cnf->literals[i][NEG].firstcla;  //指向头节点
		if (cnp->next == NULL) {
			printf("NULL");
		}
		while (cnp->next != NULL) {
			cnp = cnp->next;
			printf("%4d  ", cnp->clause);
		}
		printf("\n");
	}
	printf("\nUNITCLAUSE:@%d@\t", cnf->unitclause_index);
	for (int i = 0; i < cnf->unitclause_index; i++) {
		printf("%d  ", cnf->unitclauses[i]);
	}
	printf("\nVALUE:@%d@\t", cnf->value_index);
	for (int i = 0; i < cnf->value_index; i++) {
		printf("%d  ", cnf->values[i]);
	}
	printf("\n");
}