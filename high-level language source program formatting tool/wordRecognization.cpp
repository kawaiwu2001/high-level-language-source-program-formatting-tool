#define _CRT_SECURE_NO_WARNINGS
#include "wordRecognization.h"
#include "gettoken.h"
using namespace tk;

extern char token_text[30];
extern int row_count;
extern FILE* fp;

int wordRecognization() {
	
	if ((fp = fopen("test1.txt", "r")) == NULL) {
		printf("\t   ���ļ�ʧ��!\n");
		return ERROR;
	}
	else {
		printf("\t   ���ļ��ɹ�!\n");
	}
	printf("%-15s","\t�������");
	printf( "%s","����ֵ\n");
	int gettoken_result = -1;
	do {
		gettoken_result = gettoken(fp);
		if (gettoken_result != ERROR_TOKEN) {
			switch (gettoken_result) {
			case IDENT: 
				printf("%-15s", "\t��ʶ��");
				break;
			case KEYWORD: 
				printf("%-15s", "\t�ؼ���");
				break;
			case INT_CONST: 
				printf("%-15s", "\t���ͳ���");
				break;
			case FLOAT_CONST: 
				printf("%-15s", "\t����������");
				break;
			case CHAR_CONST: 
				printf("%-15s", "\t�ַ�����");
				break;
			case INT: 
				printf("%-15s", "\t���͹ؼ���");
				break;
			case FLOAT: 
				printf("%-15s", "\t�����͹ؼ���");
				break;
			case CHAR: 
				printf("%-15s", "\t�ַ��͹ؼ���");
				break;
			case SIGNED: 
				printf("%-15s", "\t�з������͹ؼ���");
				break;
			case UNSIGNED: 
				printf("%-15s", "\t�޷������͹ؼ���");
				break;
			case VOID: 
				printf("%-15s", "\t�����͹ؼ���");
				break;
			case ARRAY: 
				printf("%-15s", "\t����");
				break;
			case DO: 
				printf("%-15s", "\t�ؼ���");
				break;
			case WHILE: 
				printf("%-15s", "\t�ؼ���");
				break;
			case BREAK: 
				printf("%-15s", "\t�ؼ���");
				break;
			case SWITCH: 
				printf("%-15s", "\t�ؼ���");
				break;
			case CASE: 
				printf("%-15s", "\t�ؼ���");
				break;
			case DEFAULT: 
				printf("%-15s", "\t�ؼ���");
				break;
			case EXTURN: 
				printf("%-15s", "\t�ؼ���");
				break;
			case STATIC: 
				printf("%-15s", "\t�ؼ���");
				break;
			case CONST: 
				printf("%-15s", "\t�ؼ���");
				break;
			case TYPEDEF: 
				printf("%-15s", "\t�ؼ���");
				break;
			case FOR: 
				printf("%-15s", "\t�ؼ���");
				break;
			case IF: 
				printf("%-15s", "\t�ؼ���");
				break;
			case ELSE: 
				printf("%-15s", "\t�ؼ���");
				break;
			case INCLUDE: 
				printf("%-15s", "\tͷ�ļ�����");
				break;
			case DEFINE: 
				printf("%-15s", "\t�궨��");
				break;
			case PLUS: 
				printf("%-15s", "\t�Ӻ�");
				break;				
			case MINUS: 
				printf("%-15s", "\t����");
				break;
			case MULTIPLE: 
				printf("%-15s", "\t�˺�");
				break;
			case DIVIDE: 
				printf("%-15s", "\t����");
				break;
			case CONTINUE: 
				printf("%-15s", "\t�ؼ���");
				break;
			case RETURN: 
				printf("%-15s", "\t�ؼ���");
				break;
			case EQ: 
				printf("%-15s", "\t��Ⱥ�");
				break;
			case NEQ: 
				printf("%-15s", "\t����Ⱥ�");
				break;
			case AND: 
				printf("%-15s", "\t�߼���");
				break;
			case OR: 
				printf("%-15s", "\t�߼���");
				break;
			case GREATER: 
				printf("%-15s", "\t���ں�");
				break;
			case GREATER_EQUAL: 
				printf("%-15s", "\t���ڵ��ں�");
				break;
			case LESS: 
				printf("%-15s", "\tС�ں�");
				break;
			case LESS_EQUAL: 
				printf("%-15s", "\tС�ڵ��ں�");
				break;
			case LINE_COMMENT: 
				printf("%-15s", "\t����ע��");
				break;
			case BLOCK_COMMENT: 
				printf("%-15s", "\t��ע��");
				break;
			case ASSIGN: 
				printf("%-15s", "\t�xֵ");
				break;
			case LP: 
				printf("%-15s", "\t������");
				break;
			case RP: 
				printf("%-15s", "\t������");
				break;
			case LB:
				printf("%-15s", "\t�������");
				break;
			case RB:
				printf("%-15s", "\t�Ҵ�����");
				break;
			case COMMA: 
				printf("%-15s", "\t����");
				break;
			case SEMI: 
				printf("%-15s", "\t�ֺ�");
				break;
			case POUND:
				printf("%-15s", "\t����");
				break;
			case AUTO_INCREASING:
				printf("%-15s", "\t��������");
				break;
			case AUTO_DECREASING:
				printf("%-15s", "\t�Լ�����");
				break;
			}
			printf("%s\n", token_text);
		}
		else {		
			printf("\t�ļ��еĵ�%d�г��ִ���\n", row_count);
			printf("\t�Ƿ��ʻ�!\n");
			break;
		}
	} while (gettoken_result != -1);
	return 0;
}



