#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
extern char cnf_filename[FILENAMEMAX];

/*
�������ܣ���cnf�ļ����ݶ�ȡ���ڴ���
���룺��
�����ָ��cnftype���͵�ָ��
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
	while (ch == 'c') {  //����c��ͷ��ע����
		while ((ch = getc(fp)) != '\n');
		ch = getc(fp);
	}
	for (int i = 0; i < 2; i++) { fscanf_s(fp, "%s", word, WORDMAX * sizeof(char)); }
	cnf->n_variable = atoi(word);
	fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
	cnf->n_clause = atoi(word);

	//���ٿռ䲢��ʼ��
	cnf->literals = (literal(*)[2])malloc(2 * cnf->n_variable * sizeof(literal));
	cnf->clauses = (clause *)malloc(cnf->n_clause * sizeof(clause));
	for (int i = 0; i < cnf->n_variable; i++) {  //����㸳��ֵ
		cnf->literals[i][POS].is_assigned = NO;
		cnf->literals[i][POS].n_occur = 0;
		cnf->literals[i][POS].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf->literals[i][POS].firstcla->clause = 0;  //0��ʾͷ�ڵ�
		cnf->literals[i][POS].firstcla->next = NULL;
		cnf->literals[i][NEG].is_assigned = NO;
		cnf->literals[i][NEG].n_occur = 0;
		cnf->literals[i][NEG].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf->literals[i][NEG].firstcla->clause = 0;  //0��ʾͷ�ڵ�
		cnf->literals[i][NEG].firstcla->next = NULL;
	}
	for (int i = 0; i < cnf->n_clause; i++) {  //����㸳��ֵ
		cnf->clauses[i].is_satisfied = NO;
		cnf->clauses[i].current_length = 0;
		cnf->clauses[i].firstlit = (lit_node *)malloc(sizeof(lit_node));
		cnf->clauses[i].firstlit->literal = 0;  //0��ʾͷ�ڵ�
		cnf->clauses[i].firstlit->next = NULL;
	}

	cla_node * cnp;  //ָ�����ڸ������ٽ��
	lit_node * lnp;  //ͬ��
	for (int i = 0; i < cnf->n_clause; i++) {  //forѭ��¼���ļ���Ϣ
		int status = 0, num;  //status:�ж����������ŵı��
		lnp = cnf->clauses[i].firstlit;  //ʹlnpָ��ͷ�ڵ�
		fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
		while (strcmp(word, "0")) {
			num = atoi(word);
			//����clause�ڽӱ�
			lnp->next = (lit_node *)malloc(sizeof(lit_node));
			lnp = lnp->next;
			lnp->literal = num;
			lnp->next = NULL;
			cnf->clauses[i].current_length++;
			//����literal�ڽӱ�
			if (num > 0) status = 0;
			else status = 1;
			cnp = cnf->literals[abs(num) - 1][status].firstcla;  //ע��abs(num)���һ
			while (cnp->next != NULL) {  //��cnpָ����������ĩλ
				cnp = cnp->next;
			}
			cnp->next = (cla_node *)malloc(sizeof(cla_node));
			cnp = cnp->next;
			cnp->clause = i + 1;  //ע��i���һ
			cnp->next = NULL;
			cnf->literals[abs(num) - 1][status].n_occur++;
			//��ȡ��һ������
			fscanf_s(fp, "%s", word, WORDMAX * sizeof(char));
		}
	}

	//��ʼ����Ϣ����ṹ
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
	for (int i = 0; i < cnf->n_variable; i++) {  //��ʼ�����ؿ�����û�б���ֵ
		if (cnf->literals[i][POS].n_occur == 0 || cnf->literals[i][NEG].n_occur == 0) {
			cnf->pure_literals[cnf->pure_literal_index++] = i + 1;  //ע���Ĳ�������
		}
	}

	fclose(fp);
	return cnf;
}

/*
�������ܣ����Clause��Literal�ڽӱ����Ϣ
���룺ָ��cnftype���͵�ָ��
�������
*/
void show_list(cnftype * cnf) {
	cla_node * cnp;
	lit_node * lnp;
	printf("CLAUSE TEST\n");
	for (int i = 0; i < cnf->n_clause; i++) {
		printf("Clause %4d:  Is_satisfied(%d)  CurrentLength(%d)\n              Literals:  ", i + 1, cnf->clauses[i].is_satisfied, cnf->clauses[i].current_length);
		lnp = cnf->clauses[i].firstlit;  //ָ��ͷ�ڵ�
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
		cnp = cnf->literals[i][POS].firstcla;  //ָ��ͷ�ڵ�
		if (cnp->next == NULL) {
			printf("NULL");
		}
		while (cnp->next != NULL) {
			cnp = cnp->next;
			printf("%4d  ", cnp->clause);
		}
		printf("\n              NEG clauses:  ");
		cnp = cnf->literals[i][NEG].firstcla;  //ָ��ͷ�ڵ�
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