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
	GREATER_EQUAL, //大於等於>=
	LESS,
	GREATER, // 大於
	EQ,
	NEQ,
	AND,
	OR,
	ASSIGN, //賦值
	LP, //左括号
	RP, //右括号
	POUND, //起止号 
	AUTO_INCREASING,//自增
	AUTO_DECREASING,
};

typedef enum NodeType { 
	ERROR_TYPE,
	extern_define_sequence,		//外部定义序列
	extern_define,				//外部定义
	extern_variable_define,		//外部变量定义
	variable,					//变量
	variable_type,				//变量類型
	variable_sequence,			//变量序列
	function_define,			//函数定义
	function_name,				//函数名称
	return_type,				//返回值类型
	formal_parameter_sequence,	//形参序列
	formal_parameter_type,		//参数类型
	formal_parameter,			//形参
	formal_parameter_name,
	complex_statement,			//复合语句
	statement_sequence,			//语句序列
	local_variable_define_sequence,	//局部变量定义序列
	local_variable_define,			//局部变量定义
	statement,						//語句
	expression,						//表达式
	operation,						//操作符
	expression_ident,				//表达式中的操作符
	operator_number,				//操作数
	IF_ELSE_node,					//IF - ELSE结点
	IF_node,						//IF结点
	IF_content,
	ELSE_node,
	WHILE_node,						//WHILE结点
	WHILE_condition,
	RETURN_node,					//RETURN结点
	RETURN_no_node,
	SWITCH_node,					//SWITCH结点
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
	array_name,						//数组名
	array_type,						//数组类型
	Ignore,
	actual_parameter_sequence,		//實參序列
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
