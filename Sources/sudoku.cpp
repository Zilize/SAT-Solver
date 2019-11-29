#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<time.h>
#include"head.h"
int sudoku[9][9];
extern char cnf_filename[FILENAMEMAX];

/*
�������ܣ���ȡSudoku����ļ�
���룺Sudoku������
�������
*/
void read_sudoku(int i) {
	FILE * fp;
	char filename[5], ch;
	_itoa_s(i, filename, 10);
	fopen_s(&fp, filename, "r");
	for (int j = 0; j < 9; j++) {
		for (int k = 0; k < 9; k++) {
			ch = fgetc(fp);
			sudoku[j][k] = ch - 48;
		}
		ch = fgetc(fp);
	}
	fclose(fp);
}

/*
�������ܣ���Sudoku���ת��Ϊcnf�ļ�����
���룺��ʾSudoku��ֵ����
���������ļ����ļ���
*/
char * write_cnf(int isudoku) {
	FILE * fp;
	int n = 0;  //Sudoku���Ѹ�ֵ����
	_itoa_s(isudoku, cnf_filename, 10);
	strcat_s(cnf_filename, "sudoku.cnf");
	if (fopen_s(&fp, cnf_filename, "w") != 0) {
		printf("OPENERROR!");
		exit(0);
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (sudoku[i][j] != 0) n++;
		}
	}
	fprintf_s(fp, "p cnf 729 %d\n", 8829 + n);
	for (int i = 0; i < 9; i++) {  //single clause
		for (int j = 0; j < 9; j++) {
			if (sudoku[i][j] != 0) {
				fprintf_s(fp, "%d 0\n", i * 81 + j * 9 + sudoku[i][j]);
			}
		}
	}
	for (int i = 0; i < 9; i++) {  //entry
		for (int j = 0; j < 9; j++) {
			for (int k = 1; k < 10; k++) {
				fprintf_s(fp, "%d ", i * 81 + j * 9 + k);
			}
			fprintf_s(fp, "0\n");
		}
	}
	for (int i = 0; i < 9; i++) {  //row
		for (int k = 1; k < 10; k++) {
			for (int j = 0; j < 8; j++) {
				for (int x = j + 1; x < 9; x++) {
					fprintf_s(fp, "%d %d 0\n", 0 - (i * 81 + j * 9 + k), 0 - (i * 81 + x * 9 + k));
				}
			}
		}
	}
	for (int j = 0; j < 9; j++) {  //column
		for (int k = 1; k < 10; k++) {
			for (int i = 0; i < 8; i++) {
				for (int x = i + 1; x < 9; x++) {
					fprintf_s(fp, "%d %d 0\n", 0 - (i * 81 + j * 9 + k), 0 - (x * 81 + j * 9 + k));
				}
			}
		}
	}
	for (int z = 1; z < 10; z++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int x = 0; x < 3; x++) {
					for (int y = 0; y < 3; y++) {
						for (int k = y + 1; k < 3; k++) {
							fprintf_s(fp, "%d %d 0\n", 0 - ((3 * i + x) * 81 + (3 * j + y) * 9 + z), 0 - ((3 * i + x) * 81 + (3 * j + k) * 9 + z));
						}
					}
				}
			}
		}
	}
	for (int z = 1; z < 10; z++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int x = 0; x < 3; x++) {
					for (int y = 0; y < 3; y++) {
						for (int k = x + 1; k < 3; k++) {
							for (int l = 0; l < 3; l++) {
								fprintf_s(fp, "%d %d 0\n", 0 - ((3 * i + x) * 81 + (3 * j + y) * 9 + z), 0 - ((3 * i + k) * 81 + (3 * j + l) * 9 + z));
							}
						}
					}
				}
			}
		}
	}
	fclose(fp);
	return cnf_filename;
}


/*
�������ܣ���SAT�Ľ�ת��ΪSudoku���ո��
���룺����Sudoku���ո�ֵ�����,cnf��ʽ
�������
*/
void decoder(value * values, cnftype * cnf) {
	int v, x = 0, y = 0;
	for (int i = 0; i < cnf->n_variable; i++) {
		if (values[i] > 0) {
			v = values[i];
			while (v > 81) {
				v = v - 81;
				x++;
			}
			while (v > 9) {
				v = v - 9;
				y++;
			}
			sudoku[x][y] = v;
			x = 0;
			y = 0;
		}
	}
}