#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
int main() {
	int problem_type = 0;
	char ch;
	do {  //选择问题类型
		display_main();
		scanf_s("%d", &problem_type, sizeof(int));
		while ((ch = getchar()) != '\n');
		if (problem_type < 1 || problem_type>2) { printf("        INPUT ERROR!"); getchar(); }
	} while (problem_type < 1 || problem_type > 2);
	if (problem_type == 1) display_sat();
	else display_sudoku();
	getchar();
	return 0;
}