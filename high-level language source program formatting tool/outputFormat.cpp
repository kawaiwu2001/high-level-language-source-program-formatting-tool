#define _CRT_SECURE_NO_WARNINGS
#include "outputFormat.h"
#include "syntaxAnalyse.h"
#include "gettoken.h"

extern FILE* fp;
extern char token_text[30];
extern int row_count; //����������
extern Vars* varRoot;
extern char copy[100];
int countVar = 0;

int outputFormat() {
	if ((fp = fopen("test3.txt", "r")) == NULL) {
		printf("\t   ���ļ�ʧ��!\n");
		return -1;
	}
	else {
		row_count = 0;
		printf("\t   ���ļ��ɹ�!\n");
	}
	ASTtree*root = program();
	if (!root) {
		printf("\t�﷨���д�\n");
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
		case extern_define:							//�ⲿ����
		case extern_define_sequence:			//�ⲿ��������
			break;
		case extern_variable_define:			//�ⲿ��������
		case local_variable_define:				//�ֲ���������
			idepth = depth;
		case formal_parameter_array:
		case formal_parameter:					//�β�
		case function_define:						//��������
			strcat(content, " ");
			break;
		case variable_sequence:					//��������	
			if(root->sibling) strcat(content, ", ");
			else strcat(content, ";\n");
			break;
		case return_type:								//����ֵ����
			break;
		case formal_parameter_sequence:	//�β�����
			if (root->sibling->type != NodeType::no_formal_parameter) {
				strcat(content, ", ");
			}
			break;
		case formal_parameter_type_array:
		case formal_parameter_type:			//��������
		
		case formal_parameter_name:
		case complex_statement:				//�������
			break;
		case IF_content:
			strcat(content, ";\n");
			for (int i = 0; i < depth - 1; i++) {
				strcat(content, "\t");
			}
			strcat(content, "}\n");
			idepth = depth;
			break;
		case statement:								//�Z��
			if (root->children) {
				if (root->children->type != IF_ELSE_node && root->children->type != IF_node && root->children->type != WHILE_node && root->children->type != FOR_node) {
					strcat(content, ";\n");
				}
			}
			idepth = depth;
			break;
		case statement_sequence:				//�������
		case expression:								//���ʽ
		case operation:								//������
		case expression_ident:						//���ʽ�еĲ�����
		case operator_number:					//������
			break;
		case IF_ELSE_node:							//IF - ELSE���
		case IF_node:									//IF���
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
		case WHILE_node:							//WHILE���
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
		case SWITCH_node:							//SWITCH���
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
		case array_name:								//������
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
		case array_type:								//��������
			break;
		case no_actual_parameter_sequence:
			strcat(content, "()");
			break;
		case actual_parameter_sequence:	//��������
			if (root->data.content) {
				printf("%s", root->data.content);
				printf("(");
			}
			content[0] = '\0';
			if (root->sibling) strcat(content, ", ");
			else strcat(content, ")");
			break;
		case variable:									//����
			break;
		case function_name:						//��������
			printf("%s", root->data.content);
			printf("(");
			break;
		case no_formal_parameter: //�����Յ����r
			printf("){\n");
			break;
		case variable_type:							//�������
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
		case RETURN_node:						//RETURN���
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
			//��������
			PreorderTranverse2(root->children, depth);
		}
		printf("%s", content);
		content[0] = '\0';
		idepth = 0;
		if (root->sibling) {
			switch (root->sibling->type) {
			case Ignore:
			case extern_define:							//�ⲿ����
			case extern_define_sequence:			//�ⲿ��������
			case statement_sequence:				//�������
			case extern_variable_define:			//�ⲿ��������
			case variable_sequence:					//��������							
			case return_type:								//����ֵ����
			case formal_parameter_sequence:	//�β�����
			case formal_parameter_type_array:
			case formal_parameter_type:			//��������
			case formal_parameter:					//�β�
			case formal_parameter_array:
			case formal_parameter_name_array:
			case formal_parameter_name:
				break;
			case no_complex_statement:
			case IF_ELSE_node:							//IF - ELSE���
			case IF_node:									//IF���
			case IF_content:
			case WHILE_node:							//WHILE���
			case complex_statement:				//�������
				depth++;
				break;
			case ELSE_node:
			case local_variable_define:				//�ֲ���������
			case statement:								//�Z��
			case expression:								//���ʽ
			case operation:								//������
			case expression_ident:						//���ʽ�еĲ�����
			case operator_number:					//������
			case WHILE_condition:
			case RETURN_no_node:
			case SWITCH_node:							//SWITCH���
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
			case array_name:								//������
			case array_type:								//��������
			case actual_parameter_sequence:	//��������
			case function_define:						//��������
			case variable:									//����
			case function_name:						//��������
			case variable_type:							//�������
			case RETURN_node:						//RETURN���
				break;
			}
			//�����Һ���
			PreorderTranverse2(root->sibling, depth);
		}
		if (root) {
			switch (root->type) {
			case Ignore:
			case extern_define:							//�ⲿ����
			case extern_define_sequence:			//�ⲿ��������
			case statement_sequence:				//�������
			case extern_variable_define:			//�ⲿ��������
			case variable_sequence:					//��������							
			case return_type:								//����ֵ����
			case formal_parameter_sequence:	//�β�����
			case formal_parameter_type_array:
			case formal_parameter_type:			//��������
			case formal_parameter_array:
			case formal_parameter:					//�β�
			case formal_parameter_name_array:
			case formal_parameter_name:
			case IF_ELSE_node:							//IF - ELSE���
				break;
			case IF_node:									//IF���
			case IF_content:
			case complex_statement:				//�������
				strcat(content, "}\n");
				idepth = depth;
				break;
			case no_complex_statement:
				printf("\n");
				strcat(content, "}\n");
				idepth = depth;
				break;
			case WHILE_node:							//WHILE���
			case ELSE_node:
			case local_variable_define:				//�ֲ���������
			case statement:								//�Z��
			case expression:								//���ʽ
			case operation:								//������
			case expression_ident:						//���ʽ�еĲ�����
			case operator_number:					//������
			case WHILE_condition:
			case RETURN_no_node:
			case SWITCH_node:							//SWITCH���
			case FOR_node:
			case FOR_node3:
			case FOR_condition1:
			case FOR_condition2:
			case FOR_condition3:
			case CONTINUE_node:
			case BREAK_node:
			case array_name:								//������
			case array_type:								//��������
			case actual_parameter_sequence:	//��������
			case function_define:						//��������
			case variable:									//����
			case function_name:						//��������
			case variable_type:							//�������
			case RETURN_node:						//RETURN���
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