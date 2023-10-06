#define _CRT_SECURE_NO_WARNINGS
#include "wordRecognization.h"
#include "gettoken.h"
using namespace tk;

extern char token_text[30];
extern int row_count;
extern FILE* fp;

int wordRecognization() {
	
	if ((fp = fopen("test1.txt", "r")) == NULL) {
		printf("\t   打开文件失败!\n");
		return ERROR;
	}
	else {
		printf("\t   打开文件成功!\n");
	}
	printf("%-15s","\t单词类别");
	printf( "%s","单词值\n");
	int gettoken_result = -1;
	do {
		gettoken_result = gettoken(fp);
		if (gettoken_result != ERROR_TOKEN) {
			switch (gettoken_result) {
			case IDENT: 
				printf("%-15s", "\t标识符");
				break;
			case KEYWORD: 
				printf("%-15s", "\t关键字");
				break;
			case INT_CONST: 
				printf("%-15s", "\t整型常量");
				break;
			case FLOAT_CONST: 
				printf("%-15s", "\t浮点数常量");
				break;
			case CHAR_CONST: 
				printf("%-15s", "\t字符常量");
				break;
			case INT: 
				printf("%-15s", "\t整型关键字");
				break;
			case FLOAT: 
				printf("%-15s", "\t浮点型关键字");
				break;
			case CHAR: 
				printf("%-15s", "\t字符型关键字");
				break;
			case SIGNED: 
				printf("%-15s", "\t有符号类型关键字");
				break;
			case UNSIGNED: 
				printf("%-15s", "\t无符号类型关键字");
				break;
			case VOID: 
				printf("%-15s", "\t无类型关键字");
				break;
			case ARRAY: 
				printf("%-15s", "\t数组");
				break;
			case DO: 
				printf("%-15s", "\t关键字");
				break;
			case WHILE: 
				printf("%-15s", "\t关键字");
				break;
			case BREAK: 
				printf("%-15s", "\t关键字");
				break;
			case SWITCH: 
				printf("%-15s", "\t关键字");
				break;
			case CASE: 
				printf("%-15s", "\t关键字");
				break;
			case DEFAULT: 
				printf("%-15s", "\t关键字");
				break;
			case EXTURN: 
				printf("%-15s", "\t关键字");
				break;
			case STATIC: 
				printf("%-15s", "\t关键字");
				break;
			case CONST: 
				printf("%-15s", "\t关键字");
				break;
			case TYPEDEF: 
				printf("%-15s", "\t关键字");
				break;
			case FOR: 
				printf("%-15s", "\t关键字");
				break;
			case IF: 
				printf("%-15s", "\t关键字");
				break;
			case ELSE: 
				printf("%-15s", "\t关键字");
				break;
			case INCLUDE: 
				printf("%-15s", "\t头文件引用");
				break;
			case DEFINE: 
				printf("%-15s", "\t宏定义");
				break;
			case PLUS: 
				printf("%-15s", "\t加号");
				break;				
			case MINUS: 
				printf("%-15s", "\t减号");
				break;
			case MULTIPLE: 
				printf("%-15s", "\t乘号");
				break;
			case DIVIDE: 
				printf("%-15s", "\t除号");
				break;
			case CONTINUE: 
				printf("%-15s", "\t关键字");
				break;
			case RETURN: 
				printf("%-15s", "\t关键字");
				break;
			case EQ: 
				printf("%-15s", "\t相等号");
				break;
			case NEQ: 
				printf("%-15s", "\t不相等号");
				break;
			case AND: 
				printf("%-15s", "\t逻辑与");
				break;
			case OR: 
				printf("%-15s", "\t逻辑或");
				break;
			case GREATER: 
				printf("%-15s", "\t大于号");
				break;
			case GREATER_EQUAL: 
				printf("%-15s", "\t大于等于号");
				break;
			case LESS: 
				printf("%-15s", "\t小于号");
				break;
			case LESS_EQUAL: 
				printf("%-15s", "\t小于等于号");
				break;
			case LINE_COMMENT: 
				printf("%-15s", "\t单行注释");
				break;
			case BLOCK_COMMENT: 
				printf("%-15s", "\t块注释");
				break;
			case ASSIGN: 
				printf("%-15s", "\t賦值");
				break;
			case LP: 
				printf("%-15s", "\t左括号");
				break;
			case RP: 
				printf("%-15s", "\t右括号");
				break;
			case LB:
				printf("%-15s", "\t左大括号");
				break;
			case RB:
				printf("%-15s", "\t右大括号");
				break;
			case COMMA: 
				printf("%-15s", "\t逗号");
				break;
			case SEMI: 
				printf("%-15s", "\t分号");
				break;
			case POUND:
				printf("%-15s", "\t井号");
				break;
			case AUTO_INCREASING:
				printf("%-15s", "\t自增运算");
				break;
			case AUTO_DECREASING:
				printf("%-15s", "\t自减运算");
				break;
			}
			printf("%s\n", token_text);
		}
		else {		
			printf("\t文件中的第%d行出现错误\n", row_count);
			printf("\t非法词汇!\n");
			break;
		}
	} while (gettoken_result != -1);
	return 0;
}



