#pragma once
#include <map>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string>
#define ERROR -1;

namespace tk {
	enum token_kind {
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

		ERROR_TOKEN,
		KEYWORD,//�ؼ���
		IDENT, //��ʶ��
		INT_CONST, // ��������
		FLOAT_CONST,// ����������
		CHAR_CONST,// �ַ�����
		INT,
		FLOAT,
		CHAR,
		SIGNED,
		UNSIGNED,
		VOID,

		COMMA,//���� 
		SEMI,//�ֺ� 
		LB,//�������
		RB,//�Ҵ�����

		ARRAY,
		LINE_COMMENT,//ע��
		BLOCK_COMMENT,/**/

		DO,
		WHILE,
		BREAK,
		SWITCH,
		CASE,
		DEFAULT,
		EXTURN,
		STATIC,
		CONST,
		TYPEDEF,
		FOR,
		IF,
		ELSE,
		INCLUDE,
		DEFINE,
		CONTINUE,
		RETURN,
	};

	
	//double d = 60;//double_const
	static std::map<std::string, token_kind> keywordMap = { 
		{ "char", token_kind::CHAR}, {"float",token_kind::FLOAT},{"int", token_kind::INT},
		{"signed",token_kind::SIGNED},{"unsigned",token_kind::UNSIGNED}, {"void",token_kind::VOID},{"for",token_kind::FOR},{"do",token_kind::DO},{"while",token_kind::WHILE},
		{"break", token_kind::BREAK},{"continue",token_kind::CONTINUE},{"if",token_kind::IF},{"else",token_kind::ELSE},{"switch",token_kind::SWITCH},{"case",token_kind::CASE},
		{"default",token_kind::DEFAULT},{"return",token_kind::RETURN},{"extern",token_kind::EXTURN},{"static",token_kind::STATIC},{"const",token_kind::CONST},{"typedef",token_kind::TYPEDEF}
	};
	static char keyword[21][10] = { "char","float","int","signed","unsigned","void",
						"for","do","while","break","continue","if","else","switch","case","default","return",
						"extern","static","const","typedef"
	};
};
int gettoken(FILE* fp);
int is16(char c);
int check_suffix(char c);