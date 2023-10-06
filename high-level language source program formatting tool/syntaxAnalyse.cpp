#define _CRT_SECURE_NO_WARNINGS
#include "gettoken.h"
#include "syntaxAnalyse.h"
using namespace tk;
extern FILE* fp;
extern char token_text[30];
extern char array_capacity[10]; //存放數組容量
extern int array_capacity_int;
extern int row_count; //行数计数器
int w, inRecycle = -1, countCondition = 0, countvarname = 0, isarray = 0, countScentSeq = 0, isreturn = 0, countreturn = 0, countvarreturn = 0
, hadreturn = 0, returnkind = 0, inFor = -1, inReturn = 0;
extern Vars* varRoot, * varTail;
char copy[100];

int syntaxAnalyse() { //程序语法正确，返回的语法树根结点指针，可遍历显示
    if ((fp = fopen("test2.txt", "r")) == NULL) {
        printf("\t   打开文件失败!\n");
        return -1;
    }
    else {
        row_count = 0;
        printf("\t   打开文件成功!\n");
    }
    ASTtree *root = program();
    PreorderTranverse(root, 0);
    return 0;
}

void freeAST(ASTtree* root) {
    if (root->data.content) {
        free(root->data.content);
        root->data.content = NULL;
    }
    if (root->children) {
        freeAST(root->children);
        root->children = NULL;
    }
    if (root->sibling) {
        freeAST(root->sibling);
        root->sibling = NULL;
    }
    free(root);
    root = NULL;
}

ASTtree* program() {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
    root = ExtDefList();
    if (root) {   
        return root;
    }         
    else{
          printf("\t文件中的第%d行出现错误\n", row_count);
          printf("\t错误:指针为空\n");
          return NULL;
    }
}

int GetTokenWithOutANNO(){
    w = gettoken(fp);
    while (w == (int)token_kind::LINE_COMMENT || w == (int)token_kind::BLOCK_COMMENT){
        w = gettoken(fp);
    }
    return w;
}

//<外部定义序列>：：=<外部定义> <外部定义序列>  |  <外部定义>
ASTtree* ExtDefList() {           //处理外部定义序列，正确时，返回子树根结点指针，否则错误处理
    w = GetTokenWithOutANNO();
    if (w == EOF) return NULL;
    while (w == (int)token_kind::INCLUDE || w == (int)token_kind::DEFINE) {
        strcat(copy, token_text);
        strcat(copy, "\n");
        w = GetTokenWithOutANNO();
    }
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));                      //生成一个外部定义序列结点root
    if (root) {
        root->children = ExtDef();                                              //处理一个外部定义，得到一棵子树，作为root的第一棵子树
        root->type = NodeType::extern_define_sequence;
        root->data.content = NULL;
        root->sibling = ExtDefList();                                           //得到的子树，作为root的第二棵子树
        return root;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
}

//<外部定义>：： = <外部变量定义> | <函数定义>
int kind = -1;
ASTtree* ExtDef() {  //处理外部定义，正确时，返回子树根结点指针，否则返回NULL 
    char* kindName = (char*)malloc(sizeof(char) * 30);
    if (w == (int)token_kind::INT || w == (int)token_kind::FLOAT || w == (int)token_kind::CHAR || w == (int)token_kind::VOID) {
        kind = w; //保存类型说明符
        if (kindName) {
            *(kindName) = '\0';
            strcpy(kindName, token_text);
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
    }
    else if (w == (int)token_kind::BREAK) {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:break应出现在循环中\n");
        return NULL;
    }
    else if (w == (int)token_kind::CONTINUE) {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:continue应出现在循环中\n");
        return NULL;
    }
    else {//报错并返回NULL
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t外部定义出现错误:类型关键字错误\n");
        return NULL;
    }
    w = GetTokenWithOutANNO();
    if (w != (int)token_kind::IDENT && w!= (int)token_kind::ARRAY) { //报错并返回NULL  //数组
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t外部定义出现错误:标识符错误\n");
        return NULL;
    }
    else {
        char* varname = (char*)malloc(sizeof(char) * 30);
        if (varname) {
            *(varname) = '\0';
            if (w == (int)token_kind::IDENT) {
                strcpy(varname, token_text);  // 保存第一个变量名或函数名到tokenText0
                if (check_vars(varname) == -1) {
                    return NULL;
                }
                else {
                    if (add_vars(varname, 0,kind) == 0) {
                        countvarname++;
                    }  
                }
            }
            else {
                isarray = 1;
                for (int i = 0; i < strlen(token_text); i++) {
                    char f = token_text[i];
                    if (f == '[') {
                        strncpy(varname, token_text, i);  // 保存第一个变量名或函数名到tokenText0 a[10]
                        varname[i] = '\0';
                        break;
                    }
                }
                if (check_vars(varname) == -1) {
                    return NULL;
                }
                else {
                    if (add_vars(varname, array_capacity_int,kind) == 0) {
                        countvarname++;
                    }
                }  
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        w = GetTokenWithOutANNO();
        ASTtree* p;
        if (w != (int)token_kind::LP) p = ExtVarDef(varname,kindName);      //调用外部变量定义子程序
        else {
            p = FuncDef(varname,kindName);             //调用函数定义子程序
        }
        free(varname);
        free(kindName);
        return p; // 如果返回值p非空，表示成功完成一个外部定义的处理，返回p
    }
}

// <外部变量定义>：：=<类型说明符> <变量序列> ；
ASTtree* ExtVarDef(char *varname,char *kindName) {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));//生成外部变量定义结点
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
        root->type = NodeType::extern_variable_define;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
 
    if (kind != -1) {
        ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));//根据已读入的外部变量的类型，生成外部变量类型结点
        if (p) {
            p->children = NULL;
            p->sibling = NULL;
            char* name = (char*)malloc(sizeof(char) * 30);
            if (name) {
                *(name) = '\0';
                strcpy(name, kindName);
            }
            p->data.content = name;
            if (isarray == 1) {
                p->type = NodeType::array_type;
            }
            else {
                p->type = NodeType::variable_type;
            }
            root->children = p; //作为root的第一个孩子
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:外部变量类型节点的指针为空\n");
            return NULL;
        }
        root->sibling = VarList(varname);
        return root;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:错误定义或未指定外部变量的类型\n");
        return NULL;
    }
}

//<变量序列>：： = <变量>,<变量序列> | <变量>;
ASTtree* VarList(char* varname) {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));//生成`变量序列结点`
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
        root->type = NodeType::variable_sequence;
        root->children = VarDef(varname);
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
    if (w == (int)token_kind::IDENT) {
        w = GetTokenWithOutANNO();
    }
    if (w != (int)token_kind::COMMA && w != (int)token_kind::SEMI) {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:变量定义错误,可能在逗号或分号附近发生语句错误\n");
        freeAST(root); //报错，释放root为根的全部结点，返回空指针
        return NULL;     
    }
    if (w == (int)token_kind::SEMI) {
        //w = GetTokenWithOutANNO();
        return root;
        //返回根结点root；
    }
    else if (w == (int)token_kind::COMMA) {
        w = GetTokenWithOutANNO(); //w == ,     eg. int i,j;
        if (w != (int)token_kind::IDENT) {
            //报错，释放root为根的全部结点，返回空指针
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t外部定义出现错误:变量定义错误\n");
            freeAST(root);
            return NULL;
        }
        root->sibling = VarList(token_text);
    }
    return root;
}
//<变量> 
ASTtree* VarDef(char* varname) {
    ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));//生成`变量结点`
    if (p) {
        p->children = NULL;
        p->sibling = NULL;
        char* name = (char*)malloc(sizeof(char) * 30);
        if (name) {
            name[0] = '\0';
            strcpy(name, varname); //由保存在varname的第一个变量名生成一个`变量名结点`
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        p->data.datatype = kind;
        p->data.content = name;
        if (isarray == 1) {
            p->type = NodeType::array_name;
            isarray = 0;
        }
        else {
            p->type = NodeType::variable;
        }
        return p;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
}

//<形式参数序列>：：=<形式参数> ， <形式参数序列>  |  <形式参数> 
ASTtree* ForParaList() { //处理参数部分到反小括号结束，调用形参子程序，得到参数部分的子树根指针，
    w = GetTokenWithOutANNO();
    if (w == (int)token_kind::RP) {
        ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
        if (root) {
            root->children = NULL;
            root->sibling = NULL;
            root->data.content = NULL;
            root->type = NodeType::no_formal_parameter;
            return root;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        return NULL;
    }
    else if (w == (int)token_kind::COMMA) {
        w = GetTokenWithOutANNO();
    }
    // 如果后面跟着的是<形式参数> 
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));//生成`形式参数序列结点`
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
        root->type = NodeType::formal_parameter_sequence;
        root->children = ForParaDef();
        root->sibling = ForParaList();
        return root;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
}

//<形式参数>：： = <类型说明符> 标识符
ASTtree* ForParaDef() {
    if (w != (int)token_kind::INT && w != (int)token_kind::FLOAT && w != (int)token_kind::CHAR && w != (int)token_kind::VOID) {//无参函数得到NULL，该子树作为root的第二棵子树；
        printf("\t参数错误\n");
        return NULL;
    }
    kind = w;
    ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));
    if (p) {
        p->data.content = NULL;
        p->children = NULL;
        p->sibling = NULL;
        char* name = (char*)malloc(sizeof(char) * 30);
        if (name) {
            *(name) = '\0';
            strcpy(name, token_text);
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        p->data.content = name;
        p->type = NodeType::formal_parameter_type;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t申请内存失败\n");
        return NULL;
    }
    w = GetTokenWithOutANNO();
    if(w == (int)token_kind::IDENT){
        char* name = (char*)malloc(sizeof(char)*30);
        if (name) {
            *(name) = '\0';
            strcpy(name, token_text);
            add_vars(name, 0,kind );
            }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
            
        ASTtree* q = (ASTtree*)malloc(sizeof(ASTtree));
        if (q) {
            q->data.content = name;
            q->type = NodeType::formal_parameter_name;
            q->data.datatype = kind;
            q->children = NULL;
            q->sibling = NULL;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
        if (root) {
            root->data.content = NULL;
            root->children = p;
            root->sibling = q;
            root->type = NodeType::formal_parameter;
            return root;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
    }
    else if (w == (int)token_kind::ARRAY) {
        char* varname = (char*)malloc(sizeof(char) * 30);
        if (varname) {
            *(varname) = '\0';
            for (int i = 0; i < strlen(token_text); i++) {
                char f = token_text[i];
                if (f == '[') {
                    strncpy(varname, token_text, i);  // 保存第一个变量名或函数名到varname 
                    varname[i] = '\0';
                    break;
                }
            }
            if (check_vars(varname) == -1) {
                return NULL;
            }
            else {
                if (add_vars(varname, array_capacity_int, kind) == 0) {
                    countvarname++;
                }
            }
            p->type = NodeType::formal_parameter_type_array;
            ASTtree* q = (ASTtree*)malloc(sizeof(ASTtree));
            if (q) {
                q->data.content = varname;
                q->type = NodeType::formal_parameter_name_array;
                q->data.datatype = kind;
                q->children = NULL;
                q->sibling = NULL;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
            ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
            if (root) {
                root->data.content = NULL;
                root->children = p;
                root->sibling = q;
                root->type = NodeType::formal_parameter_array;
                return root;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }

        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:识别符和关键字相同\n");
        return NULL;
    }
}

//<实参序列>：：=<表达式> <实参序列>  |  <空>
ASTtree* ActParaList(int x) {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
        root->type = actual_parameter_sequence;
        if (w == (int)token_kind::RP) {
            root->type = no_actual_parameter_sequence;
            return root;
        }
        ASTtree* t = Expression((int)token_kind::COMMA);
        if (t) {
            x = ftell(fp);
            w = GetTokenWithOutANNO();
            root->children = t;
            root->sibling = ActParaList(x);
            if (root->sibling) {
                if (root->sibling->type == Ignore) {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:实参不完整\n");
                    exit(-1);
                }
            }
        }
        else {
            fseek(fp, x, SEEK_SET);
            w = GetTokenWithOutANNO();
            ASTtree* t = Expression((int)token_kind::RP);
            root->children = t;
        }
        return root;
    }
    return NULL;
}
//<复合语句>：：={ <局部变量定义序列>  <语句序列> } 
//（复合语句）子程序
ASTtree* CompSent() { //正大括号，则调用函数体（复合语句）子程序，得到函数体子树根指针，其它符号表示有错。
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
    if (root) {
        root->children = NULL; //root = 生成复合语句结点；注意其中局部变量说明和语句序列都可以为空
        root->sibling = NULL;
        root->data.content = NULL;
        root->type = NodeType::Ignore;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
    root->children = LocalVarDefSeq();//调用处理局部变量说明序列子程序
    root->sibling = ScentSeq();//调用处理'语句序列子程序'，返回子树根结点指针，作为root的第2个孩子
    if (w != (int)token_kind::RB ){
        freeAST(root);                    //返回空指针，报错并释放结点
        return NULL;
    } 
    if (root->children || root->sibling) {
        root->type = NodeType::complex_statement;
    }
    else {
        root->type = NodeType::no_complex_statement;
    }
    return root;            //  返回复合语句的子树根指针。

}

//<函数定义>：： = <类型说明符> <函数名>（<形式参数序列>）<复合语句> 
ASTtree* FuncDef(char* varname,char *kindName) {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));  //生成函数定义结点root；
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
        root->data.content = NULL;
        root->type = NodeType::function_define;     
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
              
    ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree)); //生成返回值类型结点，作为root的第一个孩子
    if (p) {
        p->children = NULL;
        p->sibling = NULL;
        char* name = (char*)malloc(sizeof(char) * 30);
        if (name) {
            *(name) = '\0';
            strcpy(name, kindName);
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        p->data.content = name;
        p->type = NodeType::return_type; 
        if (kind != (int)token_kind::VOID) {
            isreturn = 1;
        }
        else {
            isreturn = 2;
        }
        p->data.datatype = kind;
        returnkind = kind;
        root->children = p;
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
    ASTtree* q = (ASTtree*)malloc(sizeof(ASTtree)); //生成函數名类型结点，作为root的第二个孩子
    if (q) {
        q->children = NULL;
        q->sibling = NULL;
        char* name = (char*)malloc(sizeof(char) * 30);
        if (name) {
            name[0] = '\0';
            strcpy(name, varname);
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        q->data.content = name;
        q->type = NodeType::function_name;
        q->data.datatype = kind;

        q->children = ForParaList();
    }
    root->sibling = q;
    if (!root->sibling) {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t函数定义出现错误\n");
        return NULL;
    }
    else {
        w = GetTokenWithOutANNO();
        if (w == (int)token_kind::SEMI) {        //读入符号，如果分号，就是函数原型声明，函数体子树为空；
            root->sibling->sibling = NULL;

        }
        else if (w == (int)token_kind::LB) {     //得到的函数体子树作为root的第三棵子树
            ASTtree* t = (ASTtree*)malloc(sizeof(ASTtree));
            t = CompSent();
            root->sibling->sibling = t;
            //如果函数要返回值，但没有返回值
            countreturn = 0;
            countvarreturn = 0;
            hadreturn = 0;
            if (isreturn == 1 || isreturn == 2 || isreturn == 3) {
                switch (had_return(t, returnkind)) {
                case -1:
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t函数缺少返回值\n");
                    return NULL;
                    break;
                case -2:
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t返回值类型错误，应为%s类型\n", kindName);
                    return NULL;
                    break;
                case -3:
                    if (isreturn != 2) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t没有此返回值\n");
                        return NULL;
                        break;
                    }
                }
                isreturn = 0;
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t函数定义出现错误\n");
            return NULL;
        }
        return root;  // 返回root；
    }
}

//<语句序列>：：=<语句><语句序列> | <空> 
ASTtree* ScentSeq() {
    //要考虑语句序列为空的情况 
    ASTtree* r1 = (ASTtree*)malloc(sizeof(ASTtree));
    r1 = Statement();   //调用处理一条语句的子程序；返回其子树根指针r1；
    if (r1 == NULL) {     
        //否则表示语句序列已结束
        return NULL;                      //返回NULL
    }
    else {                    //初始化子树，根指针root = NULL；
         ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));   // 生成语句序列的结点root
        if (root) {
            root->children = NULL;
            root->sibling = NULL;
            root->data.content = NULL;
            root->type = NodeType::statement_sequence;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }                       
        root->children = r1;        //root->第1孩子 = r1;
        
        w = GetTokenWithOutANNO();
        root->sibling = ScentSeq(); //root->第2孩子 = 递归调用处理语句序列子程序后的返回值；
        return root;                //返回root；
    }
}

//<局部变量定义序列>：： = <局部变量定义>  <局部变量定义序列> | <空>
ASTtree* LocalVarDefSeq() {
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));                     
    if (root) {
        w = GetTokenWithOutANNO();
        if (w == (int)token_kind::INT || w == (int)token_kind::FLOAT || w == (int)token_kind::CHAR || w == (int)token_kind::VOID) {
            root->children = LocalVarDef();
            root->type = NodeType::local_variable_define_sequence;
            root->data.content = NULL;
            root->sibling = LocalVarDefSeq();
            return root;
        }
        else {
            return NULL;   //局部变量定义为空
        }
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
}
//<局部变量定义>：： = <类型说明符> <变量序列>
ASTtree *LocalVarDef(){         
    char* _type = (char*)malloc(sizeof(char) * 30);
    if(_type) *(_type) = '\0';
    else {
        return NULL;
    }
    kind = w;
    ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));  // 得到返回的子树根结点作为root的第一个孩子 
    if (p) {
        p->children = NULL;
        p->sibling = NULL;
        p->data.content = NULL;
        p->type = NodeType::local_variable_define;
        p->data.datatype = w;
        strcpy(_type,token_text);
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
    w = GetTokenWithOutANNO();
    if (w != (int)token_kind::IDENT && w != (int)token_kind::ARRAY) { //报错并返回NULL
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t局部变量定义出现错误:标识符错误\n");
        return NULL;
    }
    else {
        char* varname = (char*)malloc(sizeof(char) * 30);
        if (varname) {
            *(varname) = '\0';
            if (w == (int)token_kind::IDENT) {
                strcpy(varname, token_text);  // 保存第一个变量名或函数名到tokenText0
                if (check_vars(varname) == -1) {
                    return NULL;
                }
                else {
                    if (add_vars(varname, 0, kind) == 0) {
                        countvarname++;
                    }
                }
            }
            else {
                isarray = 1;
                for (int i = 0; i < strlen(token_text); i++) {
                    char f = token_text[i];
                    if (f == '[') {
                        strncpy(varname, token_text, i);  // 保存第一个变量名或函数名到tokenText0 a[10]
                        varname[i] = '\0';
                        break;
                    }
                }
                if (check_vars(varname) == -1) {
                    return NULL;
                }
                else {
                    if (add_vars(varname, array_capacity_int, kind) == 0) {
                        countvarname++;
                    }
                }
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t申请内存失败\n");
            return NULL;
        }
        // int i=0;
        ASTtree* q = (ASTtree*)malloc(sizeof(ASTtree));
        if (q) {
            q->children = NULL;
            q->sibling = NULL;
            q->data.content = _type;
            if (isarray == 1) {
                q->type = NodeType::array_type;
            }
            else {
                q->type = NodeType::variable_type;
            }
            q->data.datatype = w;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        p->children = q;
        p->sibling = VarList(token_text);
        return p;
    }
}

/*<语句>::=  <表达式>； | return  <表达式>
| if （<表达式>）<语句>
| if （<表达式>）<语句> else <语句>*/
ASTtree *Statement(){
    ASTtree* root = (ASTtree*)malloc(sizeof(ASTtree));
    if (root) {
        root->children = NULL;
        root->sibling = NULL;
        root->data.content = NULL;
    }
    ASTtree *p = NULL,*q = NULL,*t = NULL,*p2 = NULL,*q2 = NULL,*t2 = NULL,*f = NULL,*root2 = NULL;
    int seek_index = 0;
    switch (w){
    case  (int)token_kind::IF://分析条件语句
        w = GetTokenWithOutANNO();
        if (w != (int)token_kind::LP){
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\tif语句出现错误\n");
            return NULL;//报错并返回空
        }
        w = GetTokenWithOutANNO();
        p = Expression((int)token_kind::RP);    //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
        w = GetTokenWithOutANNO();
        q = Statement();                        //调用处理一条语句的子程序，得到IF子句的子树根指针。
        seek_index = ftell(fp);
        w = GetTokenWithOutANNO();
        if (w == (int)token_kind::ELSE){
            w = GetTokenWithOutANNO();
            t = Statement();                               //调用处理一条语句的子程序，得到ELSE子句的子树根指针。     
            // 生成IF - ELSE结点，下挂条件、IF子句、ELSE子句3棵子树
            if (root) {
                root->children = p;        //IF条件
                root->sibling = q;          //IF子句
                root->sibling->type = IF_content;
                root->sibling->sibling = t;
                root->sibling->sibling->type = ELSE_node;
                root->data.content = NULL;
                root->type = NodeType::IF_ELSE_node;
                return root;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
        }
        else {//生成IF结点，下挂条件、IF子句2棵子树           
            fseek(fp, seek_index, SEEK_SET);
            if (root) {
                root->children = p;
                root->sibling = q;
                root->data.content = NULL;
                root->type = NodeType::IF_node;
                return root;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
        } 
        break;
    case  (int)token_kind::LB:
        if (root) {
           root =  CompSent();//调用处理复合语句子程序,返回得到的子树根指针
           return root;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        break;
    case (int)token_kind::WHILE:
        inRecycle = 1;
        inFor = 0;
        w = GetTokenWithOutANNO();
        if (w != (int)token_kind::LP) {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\tif语句出现错误\n");
            return NULL;//报错并返回空
        }
        w = GetTokenWithOutANNO();
        p = Expression((int)token_kind::RP); 
        if (p) {
            p2 = (ASTtree*)malloc(sizeof(ASTtree));
            if (p2) {
                p2->children = p;
                p2->sibling = NULL;
                p2->data.content = NULL;//调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
                p2->type = NodeType::WHILE_condition;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
            w = GetTokenWithOutANNO();
            q = Statement();                      //调用处理一条语句的子程序，得到WHILE子句的子树根指针。處理{ }中的東西
            inRecycle = -1;
            inFor = -1;
            if (root) {// 生成WHILE结点，下挂条件、WHILE子句 2棵子树
                root->children = p2;
                root->sibling = q;
                root->data.content = NULL;
                root->type = NodeType::WHILE_node;
                return root;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
        }
        else {
            return NULL;
        }  
        break;
    case (int)token_kind::IDENT:
    case (int)token_kind::INT_CONST:
    case (int)token_kind::FLOAT_CONST:
    case (int)token_kind::CHAR_CONST:
        p = Expression((int)token_kind::SEMI);                  // 调用表达式处理子程序（结束符号为分号）；
        if (root) {
            root->data.content = NULL;
            root->children = p;
            root->sibling = NULL;
            root->type = NodeType::statement;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        return root;                                                 
        break;
    case  (int)token_kind::LP:   //各种表达式语句，含赋值等，形式为表达式以分号结束 ????
        return Expression((int)token_kind::SEMI);
    case (int)token_kind::RB: //语句序列结束符号，
        return NULL;
        break;
    case (int)token_kind::FOR:
        inRecycle = 1;
        inFor = 1;
        w = GetTokenWithOutANNO();
        if (w != (int)token_kind::LP) {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\tif语句出现错误\n");
            return NULL;//报错并返回空
        }
        w = GetTokenWithOutANNO();
        p = Expression((int)token_kind::SEMI);
        w = GetTokenWithOutANNO();
        q = Expression((int)token_kind::SEMI);
        w = GetTokenWithOutANNO();
        t = Expression((int)token_kind::RP);
        countCondition = 0;
        root2 = (ASTtree*)malloc(sizeof(ASTtree)); //FOR的條件結點
        if (root2) {
            root2->data.content = NULL;
            p2 = (ASTtree*)malloc(sizeof(ASTtree));
            if (p2 && p) {
                p2->children = p;
                p2->sibling = NULL;
                p2->data.content = NULL;
                if (p->type == NodeType::Ignore) {
                    p2->type = NodeType::Ignore;
                }
                else {
                    p2->type = NodeType::FOR_condition1;
                    countCondition++;
                }
                root2->children = p2;
            }
            else {
                free(p2);
                p2 = NULL;
                exit(-1);
            }
            q2 = (ASTtree*)malloc(sizeof(ASTtree));
            if (q2 && q) {
                q2->children = q;
                q2->sibling = NULL;
                q2->data.content = NULL;
                if (q->type == NodeType::Ignore) {
                    q2->type = NodeType::Ignore;
                }
                else {
                    q2->type = NodeType::FOR_condition2;
                    countCondition++;
                }
                root2->sibling = q2;  
            }
            else {
                free(q2);
                q2 = NULL;
                exit(-1);
            }
            t2 = (ASTtree*)malloc(sizeof(ASTtree));
            if (t2 && t) {
                t2->children = t;
                t2->sibling = NULL;
                t2->data.content = NULL;
                if (t->type == NodeType::Ignore) {
                    t2->type = NodeType::Ignore;
                }
                else {
                    t2->type = NodeType::FOR_condition3;
                    countCondition++;
                }
                root2->sibling->sibling = t2;
                
            }
            else{
                free(t2);
                t2 = NULL;
                exit(-1);
            }
            if (countCondition == 1) {
                root2->type = NodeType::FOR_node1;
            }
            else if (countCondition == 2) {
                root2->type = NodeType::FOR_node2;
            }
            else if(countCondition == 3) {
                root2->type = NodeType::FOR_node3;
            }
            else if (countCondition == 0) {
                root2->type = NodeType::FOR_no_condition_node;
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        w = GetTokenWithOutANNO();
        f = Statement();
        if (f) {
            if (root) {
                root->children = root2;
                root->sibling = f;
                root->data.content = NULL;
                root->type = FOR_node;
                inRecycle = 0;
                inFor = -1;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
        }
        return root;
        break;
    case (int)token_kind::CONTINUE:
        if (inRecycle == 1) {
            w = GetTokenWithOutANNO();
            if (w != (int)token_kind::SEMI) {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:语句错误\n");
                return NULL;
            }
            else {
                if (root) {
                    root->data.content = NULL;
                    root->children = NULL;
                    root->sibling = NULL;
                    root->type = CONTINUE_node; 
                    return root;
                }
                else {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:指针为空\n");
                    return NULL;
                }
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:continue应出现在循环中\n");
            return NULL;
        }
        break;
    case (int)token_kind::BREAK:
        if (inRecycle == 1) {
            w = GetTokenWithOutANNO();
            if (w != (int)token_kind::SEMI) {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:语句错误\n");
                return NULL;
            }
            else {
                if (root) {
                    root->type = BREAK_node;
                    root->data.content = NULL;
                    root->children = NULL;
                    root->sibling = NULL;
                    return root;
                }
                else {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:指针为空\n");
                    return NULL;
                }
            }
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:break应出现在循环中\n");
            return NULL;
        }
        break;
    case (int)token_kind::RETURN:
        if (isreturn == 0) {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:函数没有返回值\n");
            return NULL;
        }
        inReturn = 1;
        w = GetTokenWithOutANNO();
        p = Statement();
        inReturn = 0;
        if (root) {
            root->children = p;
            root->data.content = NULL;
            root->sibling = NULL;
            root->type = NodeType::RETURN_node;
            return root;
        }
        else {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:指针为空\n");
            return NULL;
        }
        break;
    default:
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:表达式错误\n");
            return NULL;
            break;
    }
}

//运算符优先级
char precede[18][18] = {
    {1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	1,	-1,	-1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	0,	-1,	-1,	-1},
    {1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1},
    {-1,-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	-1,	1,	0,	1,	1},
    {1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1},
    {1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	1,	-1,	1,	1,	-1,	-1}
};


/*<表达式>：： = <表达式> +<表达式> | <表达式> -<表达式> | <表达式> *<表达式>
| <表达式> / <表达式> | INT_CONST | IDENT | IDENT(<实参序列>)
| <表达式> == <表达式> | <表达式> != <表达式> | <表达式> > <表达式>
| <表达式> > <表达式> | <表达式> >= <表达式> | <表达式> < <表达式>
    | <表达式> <= <表达式> | 标识符 = <表达式> */
int parentheses = 0; //纪录左括号是否有对应的右括号
ASTtree* Expression(int endsym) {  //调用处理表达式的子程序（结束符号为反小括号），正确时得到条件表达式子树结点指针
    //调用该算法时，在调用者已经读入了第一个单词在w中
    //表达式结束符号endsym可以是分号，如表达式语句，可以是反小括号，作为条件时使用                                         
    // 定义运算符栈 op; 并初始化，将起止符#入栈
    std::stack<ASTtree*> op_stack;
    ASTtree* t = (ASTtree*)malloc(sizeof(ASTtree));
    if (t) {
        t->type = NodeType::operation;
        t->children = NULL;
        t->sibling = NULL;
        t->data.datatype = (int)Operator::POUND;
        op_stack.push(t);
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:指针为空\n");
        return NULL;
    }
    // 定义操作数栈opn，元素是结点的指针
    std::stack<ASTtree*> opn_stack;
    std::stack<ASTtree*> deal_stack;
    int error = 0, count = 0, c = 0;
    ASTtree *operator_node, *operator_num1, *operator_num2,*deal_root = NULL;
    if (inRecycle == 1) {
        if (inFor == 1 && w == endsym) {
            ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));
            if (p) {
                p->type = Ignore;
                p->children = NULL;
                p->sibling = NULL;
                p->data.content = NULL;
                p->data.datatype = w;
                return p;
            }
        }
        else if (inFor == 0 && w == endsym) {
            printf("\t文件中的第%d行出现错误\n", row_count);
            printf("\t错误:while循环缺少条件\n");
            return NULL;
        }
    }
    while ((w != (int)Operator::POUND || op_stack.top()->data.datatype != (int)Operator::POUND) && !error)  //当运算符栈栈顶不是起止符号，并没有错误时
    {
        if (count >= 1 && w != (int)Operator::POUND) {
            w = GetTokenWithOutANNO();
        }
        count++;
        if (w == (int)token_kind::IDENT || w == (int)token_kind::INT_CONST || w == (int)token_kind::CHAR_CONST || w == (int)token_kind::FLOAT_CONST)//w是标识符或常数等操作数时
        {
            if (w == (int)token_kind::IDENT) {
                Vars* p = varRoot->next;
                bool flag = false;
                while (p) {
                    if (!strcmp(p->var,token_text)) {
                        flag = true;
                        break;
                    }
                    p = p->next;
                }
                if (!flag) {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:没有%s这个标识符\n",token_text);
                    return NULL;
                }
                char* identName = (char*)malloc(sizeof(char) * 30);
                if (identName) {
                    identName[0] = '\0';
                    strcpy(identName, token_text);
                }
                else {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:指针为空\n");
                    return NULL;
                }
                c = fgetc(fp);
                if (c == '(') {
                    int seek_index = ftell(fp);
                    w = GetTokenWithOutANNO();
                    ASTtree* node = ActParaList(seek_index);
                    if (node) {
                        node->data.content = identName;
                        opn_stack.push(node);
                        w = (int)token_kind::RP;
                        continue;
                    }
                    else {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:实参序列 %s 表达式出现错误！\n", token_text);
                        exit(-1);
                    }
                }
                else {
                    ungetc(c, fp);
                }
            }
            ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree)); // 根据w生成一个结点，结点指针进栈opn
            if (p) {
                p->type = NodeType::operator_number;
                p->children = NULL;
                p->sibling = NULL;
                p->data.content = NULL;
                if (isreturn == 1) {
                    isreturn = 3;
                }
                p->data.datatype = w;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t错误:指针为空\n");
                return NULL;
            }
            if (inReturn == 1) {
                if (isreturn == 2 && w == endsym) {
                    ASTtree* p = (ASTtree*)malloc(sizeof(ASTtree));
                    if (p) {
                        p->type = RETURN_no_node;
                        p->children = NULL;
                        p->sibling = NULL;
                        p->data.content = NULL;
                        return p;
                    }
                }
                else if (isreturn == 1 && w == endsym) {
                    return NULL;
                }
                else if (isreturn == 1 && w != endsym) {

                }
                else if (isreturn == 3) {

                }
                else {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:void函数不应该有返回值\n");
                    return NULL;
                }
            }
            char* number = (char*)malloc(sizeof(char) * 30);
            if (number) {
                number[0] = '\0';
                strcpy(number, token_text);
                p->data.content = number;
            }
            else {
                printf("\t文件中的第%d行出现错误\n", row_count);
                printf("\t內存分配失败\n");
                return NULL;
            }
            opn_stack.push(p);
        }                                    
        else if (w == endsym) {
            w = (int)token_kind::POUND;//遇到结束标记分号，w被替换成#
        }
        else if (w >= (int)token_kind::MULTIPLE && w <= (int)token_kind::AUTO_DECREASING)          //w是运算符,按照excel排
        {
            Operator cal_op = Operator::POUND;
            char* buf = NULL;
            switch (precede[op_stack.top()->data.datatype][w]) {
            case -1:
                //解決括號
                if (w == LP) { 
                    w = GetTokenWithOutANNO();
                    parentheses++;
                    deal_root = Expression(RP);
                    parentheses--;
                    if (deal_root) {
                        opn_stack.push(deal_root);
                        w = (int)token_kind::IDENT;
                    }
                    else {
                        return NULL;
                    }
                }
                else {
                    operator_node = (ASTtree*)malloc(sizeof(ASTtree));
                    if (!operator_node) return NULL;
                    operator_node->type = NodeType::operation;
                    operator_node->children = NULL;
                    operator_node->sibling = NULL;
                    operator_node->data.datatype = w;
                    op_stack.push(operator_node);
                }
                break;
            case 0:
                if (op_stack.size() == 0) {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:表达式错误\n");
                    return NULL;
                }
                operator_node = op_stack.top();  
                op_stack.pop();
                if (!operator_node) {
                    error++;
                }
                w = GetTokenWithOutANNO();
                break;   //去括号
            case 1:
                //Wendy Do
                if (w == (int)token_kind::AUTO_INCREASING || w == (int)token_kind::AUTO_DECREASING) {
                    if (opn_stack.size() == 0) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:表达式错误\n");
                        return NULL;
                    }
                    operator_num2 = opn_stack.top();
                    opn_stack.pop();
                    if (!operator_num2) {
                        error++;
                    }
                    operator_num1 = NULL;
                    operator_node = (ASTtree*)malloc(sizeof(ASTtree));
                    if (!operator_node) return NULL;
                    operator_node->type = NodeType::operation;
                    operator_node->children = NULL;
                    operator_node->sibling = NULL;
                    operator_node->data.datatype = w;
                }
                else {
                    if (w == (int)token_kind::RP && parentheses == 0) {
                        if (endsym != (int)token_kind::COMMA) {
                            printf("\t文件中的第%d行出现错误\n", row_count);
                            printf("\t错误:表达式括号匹配错误\n");
                        }
                        return NULL;
                    }
                    if (opn_stack.size() == 0) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:表达式错误\n");
                        return NULL;
                    }
                    operator_num2 = opn_stack.top();
                    opn_stack.pop();
                    if (!operator_num2) {
                        error++;
                    }
                    if (opn_stack.size() == 0) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:表达式错误\n");
                        return NULL;
                    }
                    operator_num1 = opn_stack.top();
                    opn_stack.pop();
                    if (!operator_num1) {
                        error++;
                    }
                    if (op_stack.size() == 0) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:表达式错误\n");
                        return NULL;
                    }
                    operator_node = op_stack.top();
                    op_stack.pop();
                    if (!operator_node) {
                        error++;
                    }
                }  
                //根据运算符栈退栈得到的运算符t和操作数的结点指针t1和t2，
                //完成建立生成一个运算符的结点，结点指针进栈opn
                if (!operator_node) return NULL;
                /*Max Do*/
                if (!operator_num1) {
                    operator_node->children = operator_num2;
                }
                else {
                    operator_node->children = operator_num1;
                    operator_node->sibling = operator_num2;
                }
                /*Max End*/
                cal_op = (Operator)operator_node->data.datatype;
                buf = (char*)malloc(sizeof(char) * 30);
                *(buf) = '\0';
                if (!buf) {
                    printf("\tmalloc分配空间失败\n");
                    return NULL;
                }
                switch (cal_op)
                {
                case Operator::MULTIPLE:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) * atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "*");
                    break;
                case Operator::DIVIDE:
                    if (!operator_num2) return NULL;
                    if (atoi(operator_num2->data.content) == 0) {
                        printf("\t文件中的第%d行出现错误\n", row_count);
                        printf("\t错误:分母不能是零\n");
                        error++;
                        free(buf);
                        //fun(a + b);
                    }
                    else {
                        if (!operator_num1) return NULL;
                        //operator_node->data.content = _itoa(atoi(operator_num1->data.content) / atoi(operator_num2->data.content), buf, 10);
                        operator_node->data.content = strcpy(buf, "/");
                    }
                    break;
                case Operator::PLUS:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) + atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "+");
                    break;
                case Operator::MINUS:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) - atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "-");
                    break;
                case Operator::LESS_EQUAL:
                    if (!operator_num1 || !operator_num2) return NULL;
                   // operator_node->data.content = _itoa(atoi(operator_num1->data.content) <= atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "<=");
                    break;
                case Operator::GREATER_EQUAL:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) >= atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, ">=");
                    break;
                case Operator::LESS:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) < atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "<");
                    break;
                case Operator::GREATER:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) > atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, ">");
                    break;
                case Operator::EQ:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) == atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "==");
                    break;
                case Operator::NEQ:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) != atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "!=");
                    break;
                case Operator::AND:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) && atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "&&");
                    break;
                case Operator::OR:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = _itoa(atoi(operator_num1->data.content) || atoi(operator_num2->data.content), buf, 10);
                    operator_node->data.content = strcpy(buf, "||");
                    break;
                case Operator::ASSIGN:
                    if (!operator_num1 || !operator_num2) return NULL;
                    //operator_node->data.content = operator_num2->data.content;
                    operator_node->data.content = strcpy(buf, "=");
                    break;
                case Operator::AUTO_INCREASING:
                    if (!operator_num2) return NULL;
                    operator_node->data.content = strcpy(buf, "++");
                    break;
                case Operator::AUTO_DECREASING:
                    if (!operator_num2) return NULL;
                    operator_node->data.content = strcpy(buf, "--");
                    break;
                default:
                    free(buf);
                    break;
                }
                opn_stack.push(operator_node);
                if (w == endsym) { //遇到结束标记）
                    w = (int)token_kind::POUND; //，w被替换成#
                }
                break;
            default:
                if (w == endsym) { //遇到结束标记）
                    w = (int)token_kind::POUND; //，w被替换成#
                }
                else {
                    error = 1;
                }
                break;
            }
        }
        else { 
            error = 1; 
        }
    }
    if (opn_stack.size() == 1 && op_stack.top()->data.datatype == (int)Operator::POUND && !error) {
        free(t);
        t = NULL;
        return opn_stack.top();  //return操作数栈唯一的这个结点指针;    //成功返回表达式语法树的根结点指针
    }
    else {
        printf("\t文件中的第%d行出现错误\n", row_count);
        printf("\t错误:表达式错误\n");
        free(t);
        t = NULL;
        return NULL;                  //表达式分析有错
    }
    return NULL;
}

//打印出类型
void printType(NodeType type) {
    switch (type) {
    case ERROR_TYPE:
        printf("错误类型\n");
        break;
    case extern_define:
        printf("外部定义:\n");
        break;
    case extern_variable_define:
        printf("外部变量定义:\n");
        break;
    case variable:
        printf("变量:");
        break;
    case variable_type:
        printf("变量类型: ");
        break;
    case function_define:
        printf("函数定义:\n");
        break;
    case return_type:
        printf("返回值类型: ");
        break;
    case formal_parameter:
        printf("函数形参:\n");
        break;
    case formal_parameter_array:
        printf("函数形参:\n");
        break;
    case formal_parameter_name:
        printf("参数名: ");
        break;
    case formal_parameter_name_array:
        printf("参数名: ");
        break;
    case formal_parameter_type:
        printf("参数类型: ");
        break;
    case formal_parameter_type_array:
        printf("参数类型: ");
        break;
    case complex_statement:
        printf("复合语句:\n");
        break;
    case statement:
        printf("语句:\n");
        break;
    case expression:
        printf("表达式:\n");
        break;
    case operation:
        printf("操作符: ");
        break;
    case operator_number:
        printf("操作数: ");
        break;
    case IF_ELSE_node:
        printf("IF - ELSE子句:\n");
        break;
    case IF_node:
        printf("IF子句:\n");
        break;
    case WHILE_node:
        printf("WHILE子句:\n");
        break;
    case FOR_node:
        printf("FOR子句:\n");
        break;
    case FOR_node1:
        printf("一个条件的FOR子句:\n");
        break;
    case FOR_node2:
        printf("两个条件的FOR子句:\n");
        break;
    case FOR_node3:
        printf("三个条件的FOR子句:\n");
        break;
    case FOR_condition1:
        printf("FOR的条件一:\n");
        break;
    case FOR_condition2:
        printf("FOR的条件二:\n");
        break;
    case FOR_condition3:
        printf("FOR的条件三:\n");
        break;
    case FOR_no_condition_node:
        printf("FOR子句沒有条件\n");
        break;
    case CONTINUE_node:
        printf("CONTINUE语句:\n");
        break;
    case BREAK_node:
        printf("BREAK语句:\n");
        break;
    case function_name:
        printf("函数名: ");
        break;
    case RETURN_node:
        printf("RETURN语句:\n");
        break;
    case RETURN_no_node:
        printf("没有RETURN值\n");
        break;
    case array_name:
        printf("数组名: ");
        break;
    case array_type:
        printf("数组类型: ");
        break;
    case WHILE_condition:
        printf("WHILE条件:\n");
        break;
    case local_variable_define:
        printf("局部变量定义:\n");
        break;
    case actual_parameter_sequence:
        printf("实参序列: ");
        break;
    case no_actual_parameter_sequence:
        printf("实参序列: ");
        break;
    }
}

//前序遍历
void PreorderTranverse(ASTtree* root, int depth) {
    if (root == NULL) {
        printf("");
    }
    else {
        int iDepth = depth;
        if (root->type) {
            switch (root->type) {
            case NodeType::no_complex_statement:
            case NodeType::no_formal_parameter:
            case NodeType::extern_define_sequence:
            case NodeType::local_variable_define_sequence:
            case NodeType::statement_sequence:
            case NodeType::variable_sequence:
            case NodeType::formal_parameter_sequence:
            case NodeType::Ignore:
                break;
            case NodeType::formal_parameter:
            case NodeType::complex_statement:
                for (int i = 0; i < iDepth - 1; i++) {
                    printf("   ");
                }
                break;
            default:
                for (int i = 0; i < iDepth; i++) {
                    printf("   ");
                }
                iDepth++;
                break;
            }
            if (root->type == NodeType::actual_parameter_sequence) {
                if (root->data.content) {
                    printType(root->type);
                }
                else {
                    printf("实参序列:\n");
                }
            }
            else {
                printType(root->type);
            }
        }  
        //打印出数组容量
        if (root->data.content != NULL) {
            printf("%s\n", root->data.content);
            Vars* p = varRoot->next;
            switch (root->type) {
            case  NodeType::variable:
            case  NodeType::extern_variable_define:
            case  NodeType::array_name:
                while (p) {
                    if (p->var){
                        if (strcmp(root->data.content, p->var) == 0) {
                            if (p->size != 0) {
                                for (int i = 0; i < iDepth - 1; i++) {
                                    printf("   ");
                                }
                                printf("数组容量: %d\n", p->size);
                            }
                            break;
                        }
                    }
                    p = p->next;
                }
                break;
            }
            
        }
        
        PreorderTranverse(root->children, iDepth);
        PreorderTranverse(root->sibling, iDepth);
    }
}

int add_vars(char *varname, int cap, DataType kind) {
    char* cpyVarname = (char *)malloc(sizeof(char)*30);
    if (cpyVarname) {
        *(cpyVarname) = '\0';
        strcpy(cpyVarname, varname);
        Vars* p = (Vars*)malloc(sizeof(Vars));
        if (p) {
            p->var = cpyVarname;
            p->next = NULL;
            p->size = cap;
            p->datatype = kind;
            varTail->next = p;
            varTail = p;
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return -1;
    }
    
}

//检查是否重定义变量名
int check_vars(char *varname) {
    if (countvarname >= 1) {
        if (varRoot->next) {
            Vars* p = varRoot->next;
            while (p) {
                if (strcmp(p->var, varname) == 0) {
                    printf("\t文件中的第%d行出现错误\n", row_count);
                    printf("\t错误:重定义变量名\n");
                    return -1;
                }
                p = p->next;
            }
            return 0;
        }
        else {
            return -1;
        }
    }
    else {
        return 0;
    } 
}

//如果函数要返回值，但没有返回值
int had_return(ASTtree *root,DataType kind) {
    if (root) {
        if (root->type == NodeType::RETURN_node) {
            //检查是有返回值
            countreturn++;
            Vars* p = varRoot->next;
            while (p) {
                if(root->children){
                    if(root->children->children->data.content)
                    if (strcmp(root->children->children->data.content, p->var) == 0) {
                        //检查是否有该返回的标识符
                        countvarreturn++;
                        if (p->datatype != kind) {
                            //检查返回值是否与函数一致
                            hadreturn++;
                        }
                    }
                }
                p = p->next;
            }
        }
        if (root->children) {
            had_return(root->children, kind);
        }
        if (root->sibling) {
            had_return(root->sibling, kind);
        }
    }  
    if (hadreturn > 0) {
        return -2;
    }
    else if ((countvarreturn == 0 && isreturn == 2)) {
        return 0;
    }
    else if ((countvarreturn == 0 && isreturn == 3)||(countvarreturn >0 && isreturn == 1 && hadreturn > 0)) {
        return -1;
    }
    else if (countvarreturn == 0) {
        return -3;
    }
    else {
        return 0;
    }
}

