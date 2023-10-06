#pragma once
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#include <stack>

typedef int DataType;
typedef struct NodeData {
	DataType datatype;
	char* content;
}NodeData;

typedef struct VarList {
	char* var;
	int size;
	DataType datatype;
	struct VarList* next;
}Vars;

enum class Operator {
	MULTIPLE,
	DIVIDE,
	PLUS,
	MINUS,
	LESS_EQUAL,
	GREATER_EQUAL, //��춵��>=
	LESS,
	GREATER, // ���
	EQ,
	NEQ,
	AND,
	OR,
	ASSIGN, //�xֵ
	LP, //������
	RP, //������
	POUND, //��ֹ�� 
	AUTO_INCREASING,//����
	AUTO_DECREASING,
};

typedef enum NodeType { 
	ERROR_TYPE,
	extern_define_sequence,		//�ⲿ��������
	extern_define,				//�ⲿ����
	extern_variable_define,		//�ⲿ��������
	variable,					//����
	variable_type,				//�������
	variable_sequence,			//��������
	function_define,			//��������
	function_name,				//��������
	return_type,				//����ֵ����
	formal_parameter_sequence,	//�β�����
	formal_parameter_type,		//��������
	formal_parameter,			//�β�
	formal_parameter_name,
	complex_statement,			//�������
	statement_sequence,			//�������
	local_variable_define_sequence,	//�ֲ�������������
	local_variable_define,			//�ֲ���������
	statement,						//�Z��
	expression,						//���ʽ
	operation,						//������
	expression_ident,				//���ʽ�еĲ�����
	operator_number,				//������
	IF_ELSE_node,					//IF - ELSE���
	IF_node,						//IF���
	IF_content,
	ELSE_node,
	WHILE_node,						//WHILE���
	WHILE_condition,
	RETURN_node,					//RETURN���
	RETURN_no_node,
	SWITCH_node,					//SWITCH���
	FOR_node,
	FOR_node1,
	FOR_node2,
	FOR_node3,
	FOR_condition1,
	FOR_condition2,
	FOR_condition3,
	FOR_no_condition_node,
	CONTINUE_node,
	BREAK_node,
	array_name,						//������
	array_type,						//��������
	Ignore,
	actual_parameter_sequence,		//��������
	no_formal_parameter,
	no_complex_statement,
	no_actual_parameter_sequence,
	formal_parameter_type_array,
	formal_parameter_name_array,
	formal_parameter_array,
}NodeType;

typedef struct ASTtree {
	struct ASTtree* sibling;
	struct ASTtree* children;
	NodeType type;
	NodeData data;
}ASTtree;

ASTtree* program();
ASTtree* ExtDefList();
ASTtree* ExtDef();
ASTtree* ExtVarDef(char* varname, char* kindName);
ASTtree* VarList(char* varname);
ASTtree* VarDef(char* varname);
ASTtree* FuncDef(char* varname, char* kindName);
ASTtree* ForParaList();
ASTtree* ForParaDef();
ASTtree* ActParaList(int x);
ASTtree* CompSent();
ASTtree* ScentSeq();
ASTtree* LocalVarDef();
ASTtree* LocalVarDefSeq();
void freeAST(ASTtree* root);
ASTtree* Statement();
ASTtree* Expression(int endsym);
void printType(NodeType type);
void PreorderTranverse(ASTtree* root, int depth);
int syntaxAnalyse();
int GetTokenWithOutANNO();
int add_vars( char* varname, int cap, DataType kind);
int check_vars(char* varname);
int had_return(ASTtree* root,DataType kind);
