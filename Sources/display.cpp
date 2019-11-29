#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
char cnf_filename[FILENAMEMAX];
extern int sudoku[9][9];
extern value * finalvalue;
int branch_n;

/*
函数功能：显示交互主界面
输入：无
输出：无
*/
void display_main() {
	system("cls");
	printf("\n                  A DPLL-based SAT and Sudoku sovler\n");
	printf("_____________________________________________________________________\n\n\n\n");
	printf("                  1.Boolean Satisfiability Problem;\n\n");
	printf("                   2.Sudoku Creating and Sovling;\n\n\n");
	printf("_____________________________________________________________________\n\n");
	printf("               Please enter the Problem type ( 1-2 ): ");
}

/*
函数功能：显示SAT问题交互界面
输入：无
输出：无
*/
void display_sat() {
	double time1, time2, rate;
	clock_t t1, t2;
	int i = 0, status;
	cnftype * cnf;
	system("cls");
	printf("\n                  A DPLL-based SAT and Sudoku sovler\n");
	printf("_____________________________________________________________________\n\n");
	printf("     You've chosen Problem 1. Please enter the name of cnf file:\n     ");
	fgets(cnf_filename, FILENAMEMAX, stdin);
	while (cnf_filename[i] != '\n') i++;  //fgets读入了换行符，进行搜索并清除
	cnf_filename[i] = '\0';
	
	branch_n = 2;
	cnf = readfile();
	t1 = clock();
	status = DPLL(cnf, 0);
	t2 = clock();
	time1 = (double)(t2 - t1) / CLOCKS_PER_SEC;

	printf("\n     Status: ");
	if (status == SATISFIABLE) printf("Satisfiable!\n");
	else if (status == UNSATISFIABLE) printf("Unsatisfiable!\n");
	else printf("Unknown!\n");
	printf("     Using Time: %.0lf ms\n", time1 * 1000);
	if (status == 1) {
		printf("     Verifying the answer now...\n");
		cnf = readfile();
		if (verify(cnf, finalvalue) == 1)printf("     The answer has been verified!");
		else printf("     The answer is wrong!");
	}
	
	branch_n = 1;
	cnf = readfile();

	//show_list(cnf);

	t1 = clock();
	status = DPLL(cnf, 0);
	t2 = clock();
	time2 = (double)(t2 - t1) / CLOCKS_PER_SEC;

	printf("\n\n     After being optimized:\n     Status: ");
	if (status == SATISFIABLE) printf("Satisfiable!\n");
	else if (status == UNSATISFIABLE) printf("Unsatisfiable!\n");
	else printf("Unknown!\n");
	printf("     Using Time: %.0lf ms\n", time2 * 1000);
	if (status == 1) {
		printf("     Verifying the answer now...\n");
		cnf = readfile();
		if (verify(cnf, finalvalue) == 1)printf("     The answer has been verified!");
		else printf("     The answer is wrong!");
	}

	rate = (time1 - time2) / time1;
	printf("\n\n     Optimizing rate: %.2lf%%", rate*100);

	write_res(cnf, status, time2);
}

/*
函数功能：显示Sudoku问题交互界面
输入：无
输出：无
*/
void display_sudoku() {
	double time;
	clock_t t1, t2;
	cnftype * cnf;
	int isudoku;
	system("cls");
	printf("\n                  A DPLL-based SAT and Sudoku sovler\n");
	printf("___________________________________________________________________________\n\n");
	printf("     You've chosen Problem 2. Please enter the number of sudoku (1~10) :");
	scanf_s("%d", &isudoku);
	getchar();
	printf("\n");
	if (isudoku > 10 || isudoku < 1) {
		printf("INPUT ERROR!");
		exit(0);
	}
	read_sudoku(isudoku);
	for (int i = 0; i < 9; i++) {
		printf("                      ");
		for (int j = 0; j < 9; j++) {
			printf("%d ", sudoku[i][j]);
			if (j == 2 || j == 5) printf("| ");
		}
		printf("\n");
		if (i == 2 || i == 5)printf("                      ----------------------\n");
	}
	printf("\n                Enter any key to show the answer. ");
	getchar();
	printf("\n");
	write_cnf(isudoku);
	cnf = readfile();
	branch_n = 2;
	t1 = clock();
	DPLL(cnf, 0);
	t2 = clock();
	time = (double)(t2 - t1) / CLOCKS_PER_SEC;
	decoder(finalvalue, cnf);
	for (int i = 0; i < 9; i++) {
		printf("                      ");
		for (int j = 0; j < 9; j++) {
			printf("%d ", sudoku[i][j]);
			if (j == 2 || j == 5) printf("| ");
		}
		printf("\n");
		if (i == 2 || i == 5)printf("                      ----------------------\n");
	}
	printf("\n                      Using Time: %.1lf ms\n", time * 1000);
}