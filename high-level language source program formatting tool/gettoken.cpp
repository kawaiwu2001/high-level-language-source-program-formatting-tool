#include "gettoken.h"
using namespace std;
using namespace tk;

char token_text[30];
char array_capacity[10]; //存放數組容量
int row_count = 1; //行数计数器
int array_capacity_int = 0;
int is16(char c) {
	if ((isdigit(c)) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
		return 1;
	}
	else {
		return 0;
	}
};

int gettoken(FILE* fp) {
	char c = '0';
	int i = 0;
	int k = 0;
	token_text[i] = '\0';

	c = fgetc(fp);
	while (c == ' ' || c == '\t' || c == '\n') {
		if (c == '\n') {
			row_count++;
		}
		c = fgetc(fp);
	}


	const int ident_max_len = 200;
	// 數組
	// 是關鍵字?是標識符?
	//       1                0 - 无限（min(string.len-1, 255）)
	// （字母|下划线）（字母|数字|下划线）*
	// string == 關鍵字? => string 比较相等时返回keyword ，否则返回识别符。
	if (c == '@' || c == '&') {
		return ERROR_TOKEN;
	}
	if (isalpha(c) || c == '_') {
		i = 0;
		do {
			token_text[i++] = c;
			c = fgetc(fp);
		} while (isalnum(c) || c == '_');
		token_text[i] = '\0';
		for (int j = 0; j < 21; j++) {
			if (strcmp(token_text, keyword[j]) == 0) {	
				ungetc(c, fp);
				return keywordMap[token_text];
			}
		}
		int limit = 0;
		if (c == '[') {
			do {
				token_text[i++] = c;
				if (limit > 0) {
					array_capacity[k++] = c;
				}
				c = fgetc(fp);
				limit++;
			} while (isdigit(c) && limit <= 10);
			array_capacity[k] = '\0';
			array_capacity_int = atoi(array_capacity);
			if (c == ']') {
				token_text[i++] = c;
				token_text[i] = '\0';
				return ARRAY;
			}
			else {
				
				return ERROR_TOKEN;
			}
		}
		else {
			
			ungetc(c, fp);
			token_text[i] = '\0';
			return IDENT;
		}
	}
	//float a = 1U.75;
	//整數常量、浮點數常量
	if (isdigit(c)) {
		i = 0;
		do {
			token_text[i] = c;
			i++;
			c = fgetc(fp);
		} while (isdigit(c));
		// 跟着不是. u l 就是有可能是整数
		if (c != '.' && c != 'u' && c != 'l') {
			// + - * / % ( )      5(5*4)
			if (c != ' ' && c != ';' && c != '%' && c != '(' && c != ')' && c != '+' && c != '-' && c != '*' && c != '/') {
				return ERROR_TOKEN;
			}
			ungetc(c, fp);
			token_text[i] = '\0';
			return INT_CONST;
		}
		else if (c == '.') {//10.0f
			do {
				token_text[i++] = c;
				c = fgetc(fp);
			} while (isdigit(c));
			if (c == 'f') {
				token_text[i] = c;
				i++;
				token_text[i] = '\0';
				return  FLOAT_CONST;
			}else if (c == ' ' || c == ';' || c == '%' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || check_suffix(c)) {
				return ERROR_TOKEN;
			}
			token_text[i] = '\0';
			return FLOAT_CONST;
		}
		else if (c == 'u') {
			token_text[i] = c;
			i++;
			c = fgetc(fp);
			if (c == 'l' || c == 'L') {
				token_text[i] = c;
				i++;
				c = fgetc(fp);
				if (c == 'l' || c == 'L') {
					token_text[i] = c;
					i++;
					token_text[i] = '\0';
					return INT_CONST;
				}
				else {
					ungetc(c, fp);
					token_text[i] = '\0';
					return INT_CONST;
				}
			}else if (c == ' ' || c == ';' || c == '%' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || check_suffix(c)) {
				return ERROR_TOKEN;
			}
			else {
				ungetc(c, fp);
				token_text[i] = '\0';
				return INT_CONST;
			}
		}
		else if (c == 'l' || c == 'L') {
			token_text[i] = c;
			i++;
			c = fgetc(fp);
			if (c == 'l' || c == 'L') {
				token_text[i] = c;
				i++;
				token_text[i] = '\0';
				return INT_CONST;
			}
			else if (c == 'u') {
				token_text[i++] = c;
				token_text[i] = '\0';
				return INT_CONST;
			}
			else if (c == ' ' || c == ';' || c == '%' || c == '(' || c == ')' || c == '+' || c == '-' || c == '*' || c == '/' || check_suffix(c)) {
				return ERROR_TOKEN;
			}
			else {
				ungetc(c, fp);
				token_text[i] = '\0';
				return INT_CONST;
			}
		}
		else {	
			return ERROR_TOKEN;
		}
	}
	//0123、0x123
	if (c == '0') {
		c = fgetc(fp);
		if (c >= '0' && c <= '7') {
			token_text[i++] = c;
			do {
				token_text[i++] = c;
				c = fgetc(fp);
			} while (c >= '0' && c <= '7');
			ungetc(c, fp);
			token_text[i] = '\0';
			return INT_CONST;
		}
		else if (c == 'x') {
			token_text[i++] = c;
			do {
				token_text[i++] = c;
				c = fgetc(fp);
			} while (is16(c));
			ungetc(c, fp);
			token_text[i] = '\0';
			return INT_CONST;
		}
		else {
			return ERROR_TOKEN;
		}
	}
	//.開頭的浮點數
	if (c == '.') {
		i = 0;
		do {
			token_text[i++] = c;
			c = fgetc(fp);
		} while (isdigit(c));
		if (c == 'f') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return FLOAT_CONST;
		}
		else {
			ungetc(c, fp);
			token_text[i] = '\0';
			return FLOAT_CONST;
		}


	}

	//宏定義、頭文件
	if (c == '#') {
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (isalpha(c)) {
			do {
				token_text[i++] = c;
				c = fgetc(fp);
			} while (isalpha(c));
			ungetc(c, fp);
			token_text[i] = '\0';
			// #include <xxx> / #include "" 
			if (strcmp(token_text, "#include") == 0) {
				int limit = 0;
				while ((c = fgetc(fp)) == ' ');
				if (c == '<') {
					// 判断是否等于 ‘<’ ‘"’
					token_text[i++] = c;
					if (isalpha(c = fgetc(fp))) {
						do {
							token_text[i++] = c;
							c = fgetc(fp);
							limit++;
						} while (c != '>' && limit <= 30);
						if (c == '>') {
							token_text[i++] = c;

							c = fgetc(fp);
							if (c == '\n') {
								token_text[i] = '\0';
								return INCLUDE;
							}
							else {
								return ERROR_TOKEN;
							}
						}
						else {
							ungetc(c, fp);
							return ERROR_TOKEN;
						}
					}
					else {
						return ERROR_TOKEN;
					}
				}
				else if (c == '"') {
					token_text[i++] = c;
					if (isalpha(c = fgetc(fp))) {
						do {
							token_text[i++] = c;
							c = fgetc(fp);
							limit++;
						} while (c != '"' && limit <= 30);
						if (c == '"')
						{
							token_text[i++] = c;
							c = fgetc(fp);
							if (c == '\n') {
								token_text[i] = '\0';
								return INCLUDE;
							}
							else {
								return ERROR_TOKEN;
							}
						}
						else {
							ungetc(c, fp);
							return ERROR_TOKEN;
						}
					}
					else {
						return ERROR_TOKEN;
					}
				}
				else {
					return ERROR_TOKEN;
				}
			}
			else if (strcmp(token_text, "#define") == 0) {
				if ((c = fgetc(fp)) == ' ') {
					token_text[i++] = c;
					if (isalpha(c = fgetc(fp))) {
						do {
							token_text[i] = c;
							i++;
							c = fgetc(fp);
						} while (c != '\n');
						ungetc(c, fp);
						token_text[i] = '\0';
						return DEFINE;
					}
					else {
						return ERROR_TOKEN;
					}
				}
				else {
					return ERROR_TOKEN;
				}
			}
		}
		else {
			token_text[i] = '\0';
			return POUND;
		}
	}

	//注釋 
	if (c == '/') {
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '/') {
			do {
				token_text[i++] = c;
				c = fgetc(fp);
			} while (c != '\n');
			ungetc(c, fp);
			token_text[i] = '\0';
			return LINE_COMMENT;
		}
		else if (c == '*') {
			token_text[i++] = c;
			c = fgetc(fp);
			while (c != '*') {
				token_text[i++] = c;
				c = fgetc(fp);
				if (c == '*') {
					token_text[i++] = c;
					c = fgetc(fp);
					if (c == '/') {
						token_text[i++] = c;
						token_text[i] = '\0';
						return BLOCK_COMMENT;
					}
					else continue;
				}
			}
		}
		else {
			return ERROR_TOKEN;
		}
	}

	//字符常量、轉義字符
	if (c == '\'') {
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c != '\'' && c != '\\' && c != 'x') {  // c 不等于 \   /    '
			token_text[i++] = c;
			c = fgetc(fp);
			if (c == '\'') {
				token_text[i++] = c;
				token_text[i] = '\0';
				return CHAR_CONST;
			}
			else {
				return ERROR_TOKEN;
			}
		}
		// '\t' "\t" '\\' 't' '\t' "\t"
		else if (c == '\\') {
			token_text[i++] = c;
			c = fgetc(fp);
			if (c == 'n' || c == 't') {
				token_text[i++] = c;
				c = fgetc(fp);
				if (c == '\'') {
					token_text[i++] = c;
					token_text[i] = '\0';
					return CHAR_CONST;
				}
				else {
					return ERROR_TOKEN;
				}
			}
			else if (c >= '0' && c <= '7') {	//8進制轉義字符
				int limit = 1;
				do {
					token_text[i++] = c;
					c = fgetc(fp);
					limit++;
				} while (c >= '0' && c <= '7' && limit <= 3);
				/*if (c >= '8' && c <= '9') printf("\t大哥这是8進制啊\n");
				else */
				if (c == '\'') {
					token_text[i++] = c;
					token_text[i] = '\0';
					return CHAR_CONST;
				}
				else {
					return ERROR_TOKEN;
				}
			}
			else if (c == 'x') {		//16進制轉義字符
				int limit = 0;
				do {
					token_text[i++] = c;
					c = fgetc(fp);
					limit++;
				} while (is16(c) && limit <= 2);
				if (c == '\'') {
					token_text[i++] = c;
					token_text[i] = '\0';
					return CHAR_CONST;
				}
				else {
					return ERROR_TOKEN;
				}
			}
			else {
				return ERROR_TOKEN;
			}
		}
		else {
			return ERROR_TOKEN;
		}

	}


	//字符串常量
	if (c == '"') {
		i = 0;
		do {
			token_text[i++] = c;
			c = fgetc(fp);
		} while (c != '"');
		if (c == '"') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return CHAR_CONST;
		}
		else {
			return ERROR_TOKEN;
		}

	}
	char str[10] = "abc";


	// ==, != ,+,++,-,*,/,&&,||,(,),>,>=,<,<=, , ,;
	switch (c) {
	case'=':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '=') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return EQ;
		}
		else {
			ungetc(c, fp);
			token_text[i] = '\0';
			return ASSIGN;
		}
	case'!':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '=') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return NEQ;
		}
		else {
			return ERROR_TOKEN;
		}
	case'+':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '+') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return AUTO_INCREASING;
		}
		ungetc(c, fp);
		token_text[i] = '\0';
		return PLUS;
	case'-':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '-') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return AUTO_DECREASING;
		}
		ungetc(c, fp);
		token_text[i] = '\0';
		return MINUS;
		
	case'*':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return MULTIPLE;
	case'/':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return DIVIDE;
	case'&':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '&') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return AND;
		}
		else return ERROR_TOKEN;  //沒有位運算

	case'|':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '|') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return OR;
		}
		else return ERROR_TOKEN;  //沒有位運算
	case'(':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return LP;
	case')':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return RP;
	case'>':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '=') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return GREATER_EQUAL;
		}
		else {
			ungetc(c, fp);
			token_text[i] = '\0';
			return GREATER;
		}
	case'<':
		i = 0;
		token_text[i++] = c;
		c = fgetc(fp);
		if (c == '=') {
			token_text[i++] = c;
			token_text[i] = '\0';
			return LESS_EQUAL;
		}
		else {
			ungetc(c, fp);
			token_text[i] = '\0';
			return LESS;
		}
	case',':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return COMMA;
	case';':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return SEMI;
	case'{':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return LB;
	case'}':
		i = 0;
		token_text[i++] = c;
		token_text[i] = '\0';
		return RB;
	default:
		if (feof(fp)) {
			return EOF;
		}
		else {
			return ERROR_TOKEN;
		}
	}
}

int check_suffix(char c) {
	if (c == 'f' || c == 'F' || c == 'u' || c == 'l' || c == 'L') {
		return 0;
	}
	return 1;
}