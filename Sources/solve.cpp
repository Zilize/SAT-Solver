#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
value * finalvalue;
extern char cnf_filename[FILENAMEMAX];
extern int branch_n;

/*
�������ܣ�DPLL�㷨ʵ�ֺ���
���룺cnf��ʽ
�������ʽ״̬
*/
//������Ĳ���SATISFIABLE��������ȫ�ֱ�����¼����CNF��ʽ״̬
int DPLL(cnftype * cnf, int branch_literal) {
	int status = UNKNOWN, pure_literal;
	if (branch_literal == 0) {  //��1�ε���DPLL
		while (cnf->pure_literal_index != 0) {
			pure_literal = cnf->pure_literals[--cnf->pure_literal_index];

			if (pure_literal < 0) printf("PURE ERROR!");

			if (cnf->literals[pure_literal - 1][POS].is_assigned == NO) {
				if (cnf->literals[pure_literal - 1][POS].n_occur == 0) {
					pure_literal = -pure_literal;
				}
			}
			status = pure_process(cnf, pure_literal);
			if (status != UNKNOWN) {
				return status;
			}
		}

		int flag = 0;
		for (int i = 0; i < cnf->n_clause; i++) {  //�����Ӿ���������ջ
			if (cnf->clauses[i].current_length == 1) {
				for (int j = 0; j < cnf->unitclause_index; j++) {  //����������ջ
					if (cnf->unitclauses[j] == -cnf->clauses[i].firstlit->next->literal) {  //�෴�ĵ��Ӿ�
						return UNSATISFIABLE;
					}
					else if (cnf->unitclauses[j] == cnf->clauses[i].firstlit->next->literal) {  //��ͬ�ĵ��Ӿ�
						flag = 1;
					}
				}
				if (flag == 0) {  //ջ������ͬ�ĵ��Ӿ�
					cnf->unitclauses[cnf->unitclause_index++] = cnf->clauses[i].firstlit->next->literal;  //�洢������
				}
				flag = 0;  //�ָ���ֵ
			}
		}
		if (cnf->unitclause_index != 0) {  //���ڵ��Ӿ�
			do {
				status = UP(cnf, cnf->unitclauses[--cnf->unitclause_index]);
			} while (status == UNKNOWN && cnf->unitclause_index != 0);
			if (status != UNKNOWN) {
				return status;
			}
		}
	}
	else {  //�ǵ�1�ε���DPLL
		//�ȴ���branch_literal�����������ջ�ǿ����������
		status = UP(cnf, branch_literal);
		if (status != UNKNOWN) {
			return status;
		}
		if (cnf->unitclause_index != 0) {  //���ڵ��Ӿ�
			do {
				status = UP(cnf, cnf->unitclauses[--cnf->unitclause_index]);
			} while (status == UNKNOWN && cnf->unitclause_index != 0);
			if (status != UNKNOWN) {
				return status;
			}
		}
	}
	//״̬�ж�λ���Ϸ��������

	//���������д����ֹ�����
	for (int i = 0; i < cnf->n_variable; i++) {  //����������
		if (cnf->literals[i][POS].is_assigned == NO && (cnf->literals[i][POS].n_occur == 0 || cnf->literals[i][NEG].n_occur == 0)) {
			if (cnf->literals[i][POS].n_occur == 0 && cnf->literals[i][NEG].n_occur == 0) {  //��Ӱ�������
				cnf->literals[i][POS].is_assigned = YES;
				cnf->literals[i][NEG].is_assigned = YES;
				cnf->values[cnf->value_index++] = i + 1;
			}
			else {  //ע�ⲻ������
				cnf->pure_literals[cnf->pure_literal_index++] = i + 1;
			}
		}
	}
	while (cnf->pure_literal_index != 0) {
		pure_literal = cnf->pure_literals[--cnf->pure_literal_index];

		if (pure_literal < 0) printf("PURE ERROR!");

		if (cnf->literals[pure_literal - 1][POS].is_assigned == NO) {
			if (cnf->literals[pure_literal - 1][POS].n_occur == 0) {
				pure_literal = -pure_literal;
			}
		}
		status = pure_process(cnf, pure_literal);
		if (status != UNKNOWN) {
			return status;
		}
	}



	int branch_variable;
	cnftype * cnf_copy;
	if (branch_n == 1) {
		branch_variable = decide_next_branch(cnf);
	}
	else if (branch_n == 2) {
		branch_variable = decide_next_branch_max(cnf);
	}
	cnf_copy = copy(cnf);

	status = DPLL(cnf, branch_variable);
	if (status == SATISFIABLE) return SATISFIABLE;
	else {
		if (cnf->unitclauses != NULL) {
			freecnf(cnf);
		}
		status = DPLL(cnf_copy, -branch_variable);
		if (status == SATISFIABLE) return SATISFIABLE;
		else {
			if (cnf_copy->unitclauses != NULL) {
				freecnf(cnf_copy);
			}
			return CONFLICT;
		}
	}

}

/*
�������ܣ����Ӿ�ѭ��ʵ�ֺ���
���룺cnf��ʽ�Ͷ�Ӧ����
�������ʽ״̬
*/
int UP(cnftype * cnf, int up_literal) {
	int variable, sign, sign2, flag = 0;
	cla_node * cla_p;
	lit_node * lit_p;
	variable = abs(up_literal);
	if (up_literal > 0) sign = 0;
	else if (up_literal < 0) sign = 1;
	else {
		printf("SIGNERROR!");
		exit(0);
	}
	cnf->values[cnf->value_index++] = up_literal;  //��ֵ
	cnf->literals[variable - 1][POS].is_assigned = YES;
	cnf->literals[variable - 1][NEG].is_assigned = YES;

	cla_p = cnf->literals[variable - 1][sign].firstcla;
	while (cla_p->next != NULL) {  //����ͬ���Ӿ�
		cla_p = cla_p->next;
		if (cnf->clauses[cla_p->clause - 1].is_satisfied == NO) {
			cnf->clauses[cla_p->clause - 1].is_satisfied = YES;
			lit_p = cnf->clauses[cla_p->clause - 1].firstlit;
			while (lit_p->next != NULL) {  //�����Ӿ����ӵ�����
				lit_p = lit_p->next;
				if (lit_p->literal > 0) sign2 = 0;
				else sign2 = 1;
				if (cnf->literals[abs(lit_p->literal) - 1][sign2].is_assigned == NO) {  //δ����ֵ
					cnf->literals[abs(lit_p->literal) - 1][sign2].n_occur--;
				}
			}
		}
		//�Ѿ�������Ӿ�ʲô������
	}

	if (sign == 0) sign = 1;
	else sign = 0;
	cla_p = cnf->literals[variable - 1][sign].firstcla;
	while (cla_p->next != NULL) {  //��������Ӿ�
		cla_p = cla_p->next;
		if (cnf->clauses[cla_p->clause - 1].is_satisfied == NO) {
			cnf->clauses[cla_p->clause - 1].current_length--;
			if (cnf->clauses[cla_p->clause - 1].current_length == 0) {  //���ֿ��Ӿ�
				return CONFLICT;
			}
			else if (cnf->clauses[cla_p->clause - 1].current_length == 1) {  //�����µĵ��Ӿ�
				//�ҵ����Ӿ��Ԫȡ�������ջ��Ԫ�ؽ��бȽ�
				lit_p = cnf->clauses[cla_p->clause - 1].firstlit;
				while (lit_p->next != NULL) {
					lit_p = lit_p->next;
					if (cnf->literals[abs(lit_p->literal) - 1][POS].is_assigned == NO) {  //Ψһδ��ֵ�ı���
						break;
					}
				}
				//��ʱlit_p��ָliteral��Ϊ���Ӿ��Ԫ
				for (int j = 0; j < cnf->unitclause_index; j++) {
					if (cnf->unitclauses[j] == -lit_p->literal) {  //�෴�ĵ��Ӿ�
						return CONFLICT;
					}
					else if (cnf->unitclauses[j] == lit_p->literal) {  //��ͬ�ĵ��Ӿ�
						flag = 1;
					}
				}
				if (flag == 0) {  //ջ������ͬ���Ӿ�
					cnf->unitclauses[cnf->unitclause_index++] = lit_p->literal;
				}
				flag = 0;
			}
		}
		//��������Ӿ�ʲô������
	}

	if (cnf->value_index == cnf->n_variable) {
		finalvalue = cnf->values;
		return SATISFIABLE;
	}
	else return UNKNOWN;
}

/*
�������ܣ������ִ���
���룺cnf��ʽ�Ͷ�Ӧ����
�������ʽ״̬
*/
int pure_process(cnftype * cnf, int pure_literal) {
	int sign, sign2;
	cla_node * cla_p;
	lit_node * lit_p;
	if (pure_literal > 0) sign = POS;
	else sign = NEG;

	cnf->values[cnf->value_index++] = pure_literal;
	cnf->literals[abs(pure_literal) - 1][POS].is_assigned = YES;
	cnf->literals[abs(pure_literal) - 1][NEG].is_assigned = YES;

	cla_p = cnf->literals[abs(pure_literal) - 1][sign].firstcla;
	while (cla_p->next != NULL) {
		cla_p = cla_p->next;
		if (cnf->clauses[cla_p->clause - 1].is_satisfied == NO) {
			cnf->clauses[cla_p->clause - 1].is_satisfied = YES;
			lit_p = cnf->clauses[cla_p->clause - 1].firstlit;
			while (lit_p->next != NULL) {
				lit_p = lit_p->next;
				if (lit_p->literal > 0)sign2 = 0;
				else sign2 = 1;
				if (cnf->literals[abs(lit_p->literal) - 1][sign2].is_assigned == NO) {
					cnf->literals[abs(lit_p->literal) - 1][sign2].n_occur--;
				}
			}
		}
	}

	if (cnf->value_index == cnf->n_variable) {
		finalvalue = cnf->values;
		return SATISFIABLE;
	}
	else return UNKNOWN;
}



/*
�������ܣ���֧����ѡȡ����
���룺cnf��ʽ
�������Ӧ����
*/
int decide_next_branch(cnftype * cnf) {
	int i = 0;
	while (cnf->literals[i][POS].is_assigned != NO) {
		i++;
	}
	return i + 1;
}

/*
�������ܣ��Ż���֧����ѡȡ����
���룺cnf��ʽ
�������Ӧ����
*/
int decide_next_branch_max(cnftype * cnf) {
	int max_index = 0, max_occur = 0;
	for (int i = 0; i < cnf->n_variable; i++) {
		if (cnf->literals[i][POS].is_assigned == NO) {
			if (cnf->literals[i][POS].n_occur + cnf->literals[i][NEG].n_occur > max_occur) {
				max_occur = cnf->literals[i][POS].n_occur + cnf->literals[i][NEG].n_occur;
				max_index = i;
			}
		}
	}
	return max_index + 1;
}


/*
�������ܣ�cnf��ʽ���ƺ���
���룺cnf��ʽ
�����ָ���µ�cnf��ʽ��ָ��
*/
cnftype * copy(cnftype * cnf) {
	cla_node *cla_p1, *cla_p2;
	lit_node *lit_p1, *lit_p2;
	cnftype * cnf_copy;
	cnf_copy = (cnftype *)malloc(sizeof(cnftype));
	cnf_copy->n_variable = cnf->n_variable;
	cnf_copy->n_clause = cnf->n_clause;
	cnf_copy->literals = (literal(*)[2])malloc(2 * cnf_copy->n_variable * sizeof(literal));
	cnf_copy->clauses = (clause *)malloc(cnf_copy->n_clause * sizeof(clause));
	for (int i = 0; i < cnf->n_variable; i++) {
		cnf_copy->literals[i][POS].is_assigned = cnf->literals[i][POS].is_assigned;
		cnf_copy->literals[i][POS].n_occur = cnf->literals[i][POS].n_occur;
		cnf_copy->literals[i][POS].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf_copy->literals[i][POS].firstcla->clause = 0;
		cla_p1 = cnf->literals[i][POS].firstcla;
		cla_p2 = cnf_copy->literals[i][POS].firstcla;
		while (cla_p1->next != NULL) {
			cla_p1 = cla_p1->next;
			cla_p2->next = (cla_node *)malloc(sizeof(cla_node));
			cla_p2 = cla_p2->next;
			cla_p2->clause = cla_p1->clause;
		}
		cla_p2->next = NULL;
		cnf_copy->literals[i][NEG].is_assigned = cnf->literals[i][NEG].is_assigned;
		cnf_copy->literals[i][NEG].n_occur = cnf->literals[i][NEG].n_occur;
		cnf_copy->literals[i][NEG].firstcla = (cla_node *)malloc(sizeof(cla_node));
		cnf_copy->literals[i][NEG].firstcla->clause = 0;
		cla_p1 = cnf->literals[i][NEG].firstcla;
		cla_p2 = cnf_copy->literals[i][NEG].firstcla;
		while (cla_p1->next != NULL) {
			cla_p1 = cla_p1->next;
			cla_p2->next = (cla_node *)malloc(sizeof(cla_node));
			cla_p2 = cla_p2->next;
			cla_p2->clause = cla_p1->clause;
		}
		cla_p2->next = NULL;
	}
	for (int i = 0; i < cnf->n_clause; i++) {
		cnf_copy->clauses[i].is_satisfied = cnf->clauses[i].is_satisfied;
		cnf_copy->clauses[i].current_length = cnf->clauses[i].current_length;
		cnf_copy->clauses[i].firstlit = (lit_node *)malloc(sizeof(lit_node));
		cnf_copy->clauses[i].firstlit->literal = 0;
		lit_p1 = cnf->clauses[i].firstlit;
		lit_p2 = cnf_copy->clauses[i].firstlit;
		while (lit_p1->next != NULL) {
			lit_p1 = lit_p1->next;
			lit_p2->next = (lit_node *)malloc(sizeof(lit_node));
			lit_p2 = lit_p2->next;
			lit_p2->literal = lit_p1->literal;
		}
		lit_p2->next = NULL;
	}
	cnf_copy->unitclauses = (unitclause *)malloc(cnf_copy->n_variable * sizeof(unitclause));
	for (cnf_copy->unitclause_index = 0; cnf_copy->unitclause_index < cnf->unitclause_index; cnf_copy->unitclause_index++) {
		cnf_copy->unitclauses[cnf_copy->unitclause_index] = cnf->unitclauses[cnf_copy->unitclause_index];
	}
	cnf_copy->values = (value *)malloc(cnf_copy->n_variable * sizeof(value));
	for (cnf_copy->value_index = 0; cnf_copy->value_index < cnf->value_index; cnf_copy->value_index++) {
		cnf_copy->values[cnf_copy->value_index] = cnf->values[cnf_copy->value_index];
	}
	cnf_copy->pure_literals = (int *)malloc(cnf_copy->n_variable * sizeof(int));
	for (cnf_copy->pure_literal_index = 0; cnf_copy->pure_literal_index < cnf->pure_literal_index; cnf_copy->pure_literal_index++) {
		cnf_copy->pure_literals[cnf_copy->pure_literal_index] = cnf->pure_literals[cnf_copy->pure_literal_index];
	}
	return cnf_copy;
}

/*
�������ܣ��ͷŹ�ʽ�ڴ�ʵ�ֺ���
���룺cnf��ʽ
�������
*/
void freecnf(cnftype * cnf) {
	cla_node *cla_p1, *cla_p2;
	lit_node *lit_p1, *lit_p2;
	free(cnf->unitclauses);
	cnf->unitclauses = NULL;
	free(cnf->values);
	cnf->values = NULL;
	free(cnf->pure_literals);
	cnf->pure_literals = NULL;
	for (int i = 0; i < cnf->n_variable; i++) {
		cla_p1 = cnf->literals[i][POS].firstcla;
		cla_p2 = cla_p1->next;
		free(cla_p1);
		while (cla_p2 != NULL) {
			cla_p1 = cla_p2;
			cla_p2 = cla_p2->next;
			free(cla_p1);
		}
		cla_p1 = cnf->literals[i][NEG].firstcla;
		cla_p2 = cla_p1->next;
		free(cla_p1);
		while (cla_p2 != NULL) {
			cla_p1 = cla_p2;
			cla_p2 = cla_p2->next;
			free(cla_p1);
		}
	}
	free(cnf->literals);
	cnf->literals = NULL;
	for (int i = 0; i < cnf->n_clause; i++) {
		lit_p1 = cnf->clauses[i].firstlit;
		lit_p2 = lit_p1->next;
		free(lit_p1);
		while (lit_p2 != NULL) {
			lit_p1 = lit_p2;
			lit_p2 = lit_p2->next;
			free(lit_p1);
		}
	}
	free(cnf->clauses);
	cnf->clauses = NULL;
}

/*
�������ܣ���֤���ø�ֵ����Ƿ���ȷ
���룺cnf��ʽ��DPLL�������ø�ֵ
�������ֵ��ȷ��״̬
*/
int verify(cnftype * cnf, value * values) {
	int sign, status = 1;
	cla_node * cla_p;
	for (int i = 0; i < cnf->n_variable; i++) {
		if (values[i] > 0) sign = 0;
		else sign = 1;
		cla_p = cnf->literals[abs(values[i]) - 1][sign].firstcla;
		while (cla_p->next != NULL) {
			cla_p = cla_p->next;
			cnf->clauses[cla_p->clause - 1].is_satisfied = 1;
		}
	}
	for (int i = 0; i < cnf->n_clause; i++) {
		if (cnf->clauses[i].is_satisfied == 0) status = 0;
	}
	return status;
}

/*
�������ܣ��������д���ļ���
���룺cnf��ʽ�������Ժ�����ʱ��
�������
*/
void write_res(cnftype * cnf, int status, double time) {
	FILE * fp;
	int i, time_int;
	time_int = (int)(time * 1000.0);
	int * write_finalvalue;
	char res_name[FILENAMEMAX];
	write_finalvalue = (int *)malloc(cnf->n_variable * sizeof(int));
	for (i = 0; i < cnf->n_variable; i++) {
		write_finalvalue[abs(finalvalue[i]) - 1] = finalvalue[i];
	}
	i = 0;
	while (cnf_filename[i] != '.') {
		res_name[i] = cnf_filename[i];
		i++;
	}
	res_name[i] = '\0';
	strcat_s(res_name, ".res");
	fopen_s(&fp, res_name, "w");
	fprintf_s(fp, "s %d", status);
	if (status == SATISFIABLE) {
		fprintf_s(fp, "\nv");
		for (i = 0; i < cnf->n_variable; i++) {
			fprintf(fp, " %d", write_finalvalue[i]);
		}
	}
	fprintf_s(fp, "\nt %d", time_int);
	fclose(fp);
}