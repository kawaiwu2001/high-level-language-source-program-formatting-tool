#define _CRT_SECURE_NO_WARNINGS
#include "gettoken.h"
#include "wordRecognization.h"
#include "syntaxAnalyse.h"
#include "outputFormat.h"

FILE* fp;
Vars* varRoot = NULL, * varTail = NULL;
int op;
int main(int argc, char** argv) {
	varRoot = (Vars*)malloc(sizeof(Vars));
	varTail = varRoot;
	printf("%s", "------------------start------------------\n\n");
	for (int i = 0; i < 41; i++) {
		printf("x");
	}
	printf("\n");
	printf("x\t\t1. �ʷ�����\t\tx\n");
	printf("x\t\t2. �﷨����\t\tx\n");
	printf("x\t\t3. ��ʽ������\t\tx\n");
	for (int i = 0; i < 41; i++) {
		printf("x");
	}
	printf("\n");
	printf("��������Ҫִ�еĲ���: ");
	scanf("%d", &op);
	if (op == 1) {
		wordRecognization();
	}
	else if (op == 2) {
		syntaxAnalyse();
	}
	else if (op == 3) {
		outputFormat();
	}
	else {
		printf("�]�д˹���\n");
	}
	printf("%s", "\n------------------over-------------------\n");
	system("pause");
	free(varRoot);
	varRoot = NULL;
	return 0;
}