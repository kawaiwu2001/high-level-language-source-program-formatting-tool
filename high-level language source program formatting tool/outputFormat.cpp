#define _CRT_SECURE_NO_WARNINGS
#include "outputFormat.h"
#include "syntaxAnalyse.h"
#include "gettoken.h"

extern FILE* fp;
extern char token_text[30];
extern int row_count; //行数计数器
extern Vars* varRoot;
extern char copy[100];
int countVar = 0;

int outputFormat() {
	if ((fp = fopen("test3.txt", "r")) == NULL) {
		printf("\t   打开文件失败!\n");
		return -1;
	}
	else {
		row_count = 0;
		printf("\t   打开文件成功!\n");
	}
	ASTtree*root = program();
	if (!root) {
		printf("\t语法树有错\n");
		exit(-1);
	}
	printf("%s", copy);
	PreorderTranverse2(root, 0);
	return 0;
}


void PreorderTranverse2(ASTtree* root, int depth) {
	if (root) {
		char* content = (char*)malloc(sizeof(char) * 50);
		*content = '\0';
		if (root->data.content && root->type != function_name) strcat(content, root->data.content);
		Vars* p = varRoot->next;
		int idepth = 0;
		switch (root->type) {
		case Ignore:
		case extern_define:							//外部定义
		case extern_define_sequence:			//外部定义序列
			break;
		case extern_variable_define:			//外部变量定义
		case local_variable_define:				//局部变量定义
			idepth = depth;
		case formal_parameter_array:
		case formal_parameter:					//形参
		case function_define:						//函数定义
			strcat(content, " ");
			break;
		case variable_sequence:					//变量序列	
			if(root->sibling) strcat(content, ", ");
			else strcat(content, ";\n");
			break;
		case return_type:								//返回值类型
			break;
		case formal_parameter_sequence:	//形参序列
			if (root->sibling->type != NodeType::no_formal_parameter) {
				strcat(content, ", ");
			}
			break;
		case formal_parameter_type_array:
		case formal_parameter_type:			//参数类型
		
		case formal_parameter_name:
		case complex_statement:				//复合语句
			break;
		case IF_content:
			strcat(content, ";\n");
			for (int i = 0; i < depth - 1; i++) {
				strcat(content, "\t");
			}
			strcat(content, "}\n");
			idepth = depth;
			break;
		case statement:								//語句
			if (root->children) {
				if (root->children->type != IF_ELSE_node && root->children->type != IF_node && root->children->type != WHILE_node && root->children->type != FOR_node) {
					strcat(content, ";\n");
				}
			}
			idepth = depth;
			break;
		case statement_sequence:				//语句序列
		case expression:								//表达式
		case operation:								//操作符
		case expression_ident:						//表达式中的操作符
		case operator_number:					//操作数
			break;
		case IF_ELSE_node:							//IF - ELSE结点
		case IF_node:									//IF结点
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("if (");
			strcat(content, "){\n");
			break;
		case ELSE_node:
			for (int i = 0; i < depth-1; i++) {
				printf("\t");
			}
			printf("else {\n");
			strcat(content, ";\n");
			idepth = depth;
			break;
		case WHILE_node:							//WHILE结点
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("while ");
			break;
		case WHILE_condition:
			printf("(");
			strcat(content, ") {\n");
			break;
		case RETURN_no_node:
		case SWITCH_node:							//SWITCH结点
			break;
		case FOR_node:
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("for (");
			break;
		
		case FOR_condition1:
			strcat(content, ";");
			break;
		case FOR_condition2:
			strcat(content, ";");
			break;
		case FOR_condition3:
			strcat(content, ") {\n");
			break;
		case FOR_node1:
		case FOR_node2:
		case FOR_node3:
		case FOR_no_condition_node:
			break;
		case formal_parameter_name_array:
		case array_name:								//数组名
			while (p) {
				if (p->var) {
					if (!strcmp(root->data.content, p->var)) {
						if (p->size != 0) {
							sprintf(content, "[%d]", p->size);
						}
						break;
					}
				}
				p = p->next;
			}
			break;
		case array_type:								//数组类型
			break;
		case no_actual_parameter_sequence:
			strcat(content, "()");
			break;
		case actual_parameter_sequence:	//實參序列
			if (root->data.content) {
				printf("%s", root->data.content);
				printf("(");
			}
			content[0] = '\0';
			if (root->sibling) strcat(content, ", ");
			else strcat(content, ")");
			break;
		case variable:									//变量
			break;
		case function_name:						//函数名称
			printf("%s", root->data.content);
			printf("(");
			break;
		case no_formal_parameter: //函數空參數時
			printf("){\n");
			break;
		case variable_type:							//变量類型
			break;
		case CONTINUE_node:
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("continue;\n");
			break;
		case BREAK_node:
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("break;\n");
			break;
		case RETURN_node:						//RETURN结点
			for (int i = 0; i < depth; i++) {
				printf("\t");
			}
			printf("return ");
			strcat(content, ";\n");
			break;
		}
		if (root->children) {
			for (int i = 0; i < idepth; i++) {
				printf("\t");
			}
			//遍历左孩子
			PreorderTranverse2(root->children, depth);
		}
		printf("%s", content);
		content[0] = '\0';
		idepth = 0;
		if (root->sibling) {
			switch (root->sibling->type) {
			case Ignore:
			case extern_define:							//外部定义
			case extern_define_sequence:			//外部定义序列
			case statement_sequence:				//语句序列
			case extern_variable_define:			//外部变量定义
			case variable_sequence:					//变量序列							
			case return_type:								//返回值类型
			case formal_parameter_sequence:	//形参序列
			case formal_parameter_type_array:
			case formal_parameter_type:			//参数类型
			case formal_parameter:					//形参
			case formal_parameter_array:
			case formal_parameter_name_array:
			case formal_parameter_name:
				break;
			case no_complex_statement:
			case IF_ELSE_node:							//IF - ELSE结点
			case IF_node:									//IF结点
			case IF_content:
			case WHILE_node:							//WHILE结点
			case complex_statement:				//复合语句
				depth++;
				break;
			case ELSE_node:
			case local_variable_define:				//局部变量定义
			case statement:								//語句
			case expression:								//表达式
			case operation:								//操作符
			case expression_ident:						//表达式中的操作符
			case operator_number:					//操作数
			case WHILE_condition:
			case RETURN_no_node:
			case SWITCH_node:							//SWITCH结点
			case FOR_node:
			case FOR_node1:
			case FOR_node2:
			case FOR_node3:
			case FOR_condition1:
			case FOR_condition2:
			case FOR_condition3:
			case FOR_no_condition_node:
			case CONTINUE_node:
			case BREAK_node:
			case array_name:								//数组名
			case array_type:								//数组类型
			case actual_parameter_sequence:	//實參序列
			case function_define:						//函数定义
			case variable:									//变量
			case function_name:						//函数名称
			case variable_type:							//变量類型
			case RETURN_node:						//RETURN结点
				break;
			}
			//遍历右孩子
			PreorderTranverse2(root->sibling, depth);
		}
		if (root) {
			switch (root->type) {
			case Ignore:
			case extern_define:							//外部定义
			case extern_define_sequence:			//外部定义序列
			case statement_sequence:				//语句序列
			case extern_variable_define:			//外部变量定义
			case variable_sequence:					//变量序列							
			case return_type:								//返回值类型
			case formal_parameter_sequence:	//形参序列
			case formal_parameter_type_array:
			case formal_parameter_type:			//参数类型
			case formal_parameter_array:
			case formal_parameter:					//形参
			case formal_parameter_name_array:
			case formal_parameter_name:
			case IF_ELSE_node:							//IF - ELSE结点
				break;
			case IF_node:									//IF结点
			case IF_content:
			case complex_statement:				//复合语句
				strcat(content, "}\n");
				idepth = depth;
				break;
			case no_complex_statement:
				printf("\n");
				strcat(content, "}\n");
				idepth = depth;
				break;
			case WHILE_node:							//WHILE结点
			case ELSE_node:
			case local_variable_define:				//局部变量定义
			case statement:								//語句
			case expression:								//表达式
			case operation:								//操作符
			case expression_ident:						//表达式中的操作符
			case operator_number:					//操作数
			case WHILE_condition:
			case RETURN_no_node:
			case SWITCH_node:							//SWITCH结点
			case FOR_node:
			case FOR_node3:
			case FOR_condition1:
			case FOR_condition2:
			case FOR_condition3:
			case CONTINUE_node:
			case BREAK_node:
			case array_name:								//数组名
			case array_type:								//数组类型
			case actual_parameter_sequence:	//實參序列
			case function_define:						//函数定义
			case variable:									//变量
			case function_name:						//函数名称
			case variable_type:							//变量類型
			case RETURN_node:						//RETURN结点
				break;
			case FOR_no_condition_node:
				strcat(content, ";;) {\n");
				break;
			case FOR_node1:
				strcat(content, ";) {\n");
				break;
			case FOR_node2:
				strcat(content, ") {\n");
				break;
			}
			for (int i = 0; i < idepth-1; i++) {
				printf("\t");
			}
			printf("%s", content);
		}
		free(content);
	}
}