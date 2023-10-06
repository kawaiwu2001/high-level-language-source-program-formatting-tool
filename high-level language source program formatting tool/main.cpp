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
	printf("x\t\t1. 词法分析\t\tx\n");
	printf("x\t\t2. 语法分析\t\tx\n");
	printf("x\t\t3. 格式化处理\t\tx\n");
	for (int i = 0; i < 41; i++) {
		printf("x");
	}
	printf("\n");
	printf("请输入你要执行的操作: ");
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
		printf("沒有此功能\n");
	}
	printf("%s", "\n------------------over-------------------\n");
	system("pause");
	free(varRoot);
	varRoot = NULL;
	return 0;
}