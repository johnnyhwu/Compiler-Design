%{
#include "AST/ast.hpp"
#include "AST/program.hpp"
#include "AST/decl.hpp"
#include "AST/variable.hpp"
#include "AST/ConstantValue.hpp"
#include "AST/function.hpp"
#include "AST/CompoundStatement.hpp"
#include "AST/print.hpp"
#include "AST/expression.hpp"
#include "AST/BinaryOperator.hpp"
#include "AST/UnaryOperator.hpp"
#include "AST/FunctionInvocation.hpp"
#include "AST/VariableReference.hpp"
#include "AST/assignment.hpp"
#include "AST/read.hpp"
#include "AST/if.hpp"
#include "AST/while.hpp"
#include "AST/for.hpp"
#include "AST/return.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>



#define YYLTYPE yyltype

typedef struct YYLTYPE {
    uint32_t first_line;
    uint32_t first_column;
    uint32_t last_line;
    uint32_t last_column;
} yyltype;

/* Declared by scanner.l */
extern uint32_t line_num;
extern char buffer[512];
extern FILE *yyin;
extern char *yytext;
/* End */

extern "C" int yylex(void);
static void yyerror(const char *msg);
extern int yylex_destroy(void);

static AstNode *root;
AstNode *temp_node;
AstDumper ast_dumper;
%}


%code requires {
    class AstNode;
    #include <vector>
    #include "AST/variable.hpp"
}

    /* For yylval */
%union {
    /* basic semantic value */
    char *text;

    AstNode *node;
    std::vector<AstNode*> *vector_node;
    std::vector<VariableNode*> *vector_node_variable;
};

%type <text> ProgramName
%type <text> ID
%type <vector_node> DeclarationList
%type <vector_node> Declarations
%type <vector_node> FunctionList
%type <vector_node> Functions
%type <node> CompoundStatement
%type <vector_node_variable> IdList
%type <text> VAR
%type <text> Type
%type <text> INTEGER
%type <text> REAL
%type <text> STRING
%type <text> BOOLEAN
%type <text> ScalarType
%type <text> INT_LITERAL
%type <text> ArrDecl
%type <text> ArrType
%type <node> Declaration
%type <text> LiteralConstant
%type <text> NegOrNot
%type <text> StringAndBoolean
%type <text> REAL_LITERAL
%type <text> STRING_LITERAL
%type <text> FunctionName
%type <node> Function
%type <node> FunctionDeclaration
%type <node> FunctionDefinition
%type <vector_node> FormalArgList
%type <text> ReturnType
%type <vector_node> FormalArgs
%type <node> FormalArg
%type <node> Statement 
%type<node> Simple
%type<node> Condition
%type<node> While
%type<node> For
%type<node> Return
%type<node> FunctionCall
%type<node> Expression
%type<node> IntegerAndReal
%type<node> VariableReference
%type<node> FunctionInvocation
%type<vector_node> Statements
%type<vector_node> Expressions
%type<vector_node> ExpressionList
%type<vector_node> ArrRefs
%type<vector_node> ArrRefList
%type<node> ElseOrNot
%type<vector_node> StatementList




    /* Delimiter */
%token COMMA SEMICOLON COLON
%token L_PARENTHESIS R_PARENTHESIS
%token L_BRACKET R_BRACKET

    /* Operator */
%token ASSIGN
%left OR
%left AND
%right NOT
%left LESS LESS_OR_EQUAL EQUAL GREATER GREATER_OR_EQUAL NOT_EQUAL
%left PLUS MINUS
%left MULTIPLY DIVIDE MOD
%right UNARY_MINUS

    /* Keyword */
%token ARRAY BOOLEAN INTEGER REAL STRING
%token END BEGIN_ /* Use BEGIN_ since BEGIN is a keyword in lex */
%token DO ELSE FOR IF THEN WHILE
%token DEF OF TO RETURN VAR
%token FALSE TRUE
%token PRINT READ

    /* Identifier */
%token ID

    /* Literal */
%token INT_LITERAL
%token REAL_LITERAL
%token STRING_LITERAL

%%
    /*
       Program Units
                     */

Program:
    ProgramName SEMICOLON
    /* ProgramBody */
    DeclarationList FunctionList CompoundStatement
    /* End of ProgramBody */
    END {
        root = new ProgramNode(@1.first_line, @1.first_column, $1, "void", $3, $4, $5);

        CompoundStatementNode* ptr = (CompoundStatementNode*)$5;
        // printf("%d", ptr->statement_node_list->size());
        
    }
;

ProgramName:
    ID {
        $$ = $1;
    }
;

DeclarationList:
    Epsilon {
        $$ = NULL;
    }
    |
    Declarations {
        $$ = $1;
    }
;

Declarations:
    Declaration {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Declarations Declaration {
        $1->push_back($2);
        $$ = $1;
    }
;

Declaration:
    VAR IdList COLON Type SEMICOLON {

        
        for (auto & variable_node : *$2) {
            //printf("DEBUG: %s", $4);
            variable_node->setTypeName($4);
        }

        DeclNode* declaration_node = new DeclNode(@1.first_line, @1.first_column, $2);


        $$ = declaration_node;
    } 
    |
    VAR IdList COLON LiteralConstant SEMICOLON {


        for (auto & variable_node : *$2) {
            variable_node->setConstantValueNode(temp_node);
            variable_node->setTypeName($4);
        }

        DeclNode* declaration_node = new DeclNode(@1.first_line, @1.first_column, $2);
        
        $$ = declaration_node;
    }
;

Type:
    ScalarType {
        $$ = $1;
    }
    |
    ArrType {
        $$ = $1;
    }
;

ScalarType:
    INTEGER {
        $$ = $1;
    }
    |
    REAL {
        $$ = $1;
    }
    |
    STRING {
        $$ = $1;
    }
    |
    BOOLEAN {
        $$ = $1;
    }
;

ArrType:
    ArrDecl ScalarType {
        char *tmp = (char*)malloc(sizeof(char) * 50);
        strcat(tmp, $2);
        strcat(tmp, " ");
        strcat(tmp, $1);
        $$ = tmp;
    }
;

ArrDecl:
    ARRAY INT_LITERAL OF {
        char *tmp = (char*)malloc(sizeof(char) * 50);
        strcat(tmp, "[");
        strcat(tmp, $2);
        strcat(tmp, "]");
        $$ = tmp;
    }
    |
    ArrDecl ARRAY INT_LITERAL OF {
        char *tmp = (char*)malloc(sizeof(char) * 50);
        strcat(tmp, $1);
        strcat(tmp, "[");
        strcat(tmp, $3);
        strcat(tmp, "]");
        $$ = tmp;
    }
;

LiteralConstant:
    NegOrNot INT_LITERAL {
        $$ = "integer";
        char *tmp = (char*)malloc(sizeof(char) * 50);
        int col_local = @2.first_column;
        if(strcmp($1, "-") == 0) {
            strcat(tmp, $1);
            col_local-=1;
        }
        strcat(tmp, $2);
        temp_node = new ConstantValueNode(@2.first_line, col_local, tmp, false);
        //temp_node->print();
    }
    |
    NegOrNot REAL_LITERAL {
        $$ = "real";
        char *tmp = (char*)malloc(sizeof(char) * 50);
        int col_local = @2.first_column;
        if(strcmp($1, "-") == 0) {
            strcat(tmp, $1);
            col_local-=1;
        }
        strcat(tmp, $2);
        temp_node = new ConstantValueNode(@2.first_line, col_local, tmp, true);
        //temp_node->print();
    }
    |
    StringAndBoolean {
        $$ = $1;
    }
;

IdList:
    ID {
        VariableNode* variable_node = new VariableNode(@1.first_line, @1.first_column,
                                                       $1, "TypeName", NULL);
        $$ = new std::vector<VariableNode*>;
        $$->push_back(variable_node);
    }
    |
    IdList COMMA ID {
        VariableNode* variable_node = new VariableNode(@3.first_line, @3.first_column,
                                                      $3, "TypeName", NULL);
        $1->push_back(variable_node);
        $$ = $1;
    }
;

FunctionList:
    Epsilon {
        $$ = NULL;
    }
    |
    Functions {
        $$ = $1;
    }
;

Functions:
    Function {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Functions Function {
        $1->push_back($2);
        $$ = $1;
    }
;

Function:
    FunctionDeclaration {
        $$ = $1;
    }
    |
    FunctionDefinition {
        $$ = $1;
    }
;

FunctionDeclaration:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType SEMICOLON {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5);
    }
;

FunctionDefinition:
    FunctionName L_PARENTHESIS FormalArgList R_PARENTHESIS ReturnType
    CompoundStatement
    END {
        $$ = new FunctionNode(@1.first_line, @1.first_column, $1, $3, $5, $6);
    }
;

FunctionName:
    ID {
        $$ = $1;
    }
;

FormalArgList:
    Epsilon {
        $$ = NULL;
    }
    |
    FormalArgs {
        $$ = $1;
    }
;

FormalArgs:
    FormalArg {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    FormalArgs SEMICOLON FormalArg {
        $1->push_back($3);
        $$ = $1;
    }
;

FormalArg:
    IdList COLON Type {
        for (auto & variable_node : *$1) {
            variable_node->setTypeName($3);
        }

        DeclNode* declaration_node = new DeclNode(@1.first_line, @1.first_column, $1);

        $$ = declaration_node;
    }
;



ReturnType:
    COLON ScalarType {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = "void";
    }
;

    /*
       Data Types and Declarations
                                   */
NegOrNot:
    Epsilon {
        $$ = "";
    }
    |
    MINUS %prec UNARY_MINUS {
        $$ = "-";
    }
;

StringAndBoolean:
    STRING_LITERAL {
        $$ = "string";
        // printf("DEBUG: %d", temp.length());
        temp_node = new ConstantValueNode(@1.first_line, @1.first_column, $1, false);
    }
    |
    TRUE {
        $$ = "boolean";
        temp_node = new ConstantValueNode(@1.first_line, @1.first_column, "true", false);
    }
    |
    FALSE {
        $$ = "boolean";
        temp_node = new ConstantValueNode(@1.first_line, @1.first_column, "false", false);
    }
;

IntegerAndReal:
    INT_LITERAL {
        char *tmp = (char*)malloc(sizeof(char) * 50);
        int col_local = @1.first_column;
        strcat(tmp, $1);
        $$ = new ConstantValueNode(@1.first_line, col_local, tmp, false);
    }
    |
    REAL_LITERAL {
        char *tmp = (char*)malloc(sizeof(char) * 50);
        int col_local = @1.first_column;
        strcat(tmp, $1);
        $$ = new ConstantValueNode(@1.first_line, col_local, tmp, true);
    }
;

    /*
       Statements
                  */

Statement:
    CompoundStatement {
        $$ = $1;
    }
    |
    Simple {
        $$ = $1;
        //$$->print();
    }
    |
    Condition {
        $$ = $1;
    }
    |
    While {
        $$ = $1;
    }
    |
    For {
        $$ = $1;
    }
    |
    Return {
        $$ = $1;
    }
    |
    FunctionCall {
        $$ = $1;
    }
;

CompoundStatement:
    BEGIN_
    DeclarationList
    StatementList
    END {
        $$ = new CompoundStatementNode(@1.first_line, @1.first_column, $2, $3);
    }
;

Simple:
    VariableReference ASSIGN Expression SEMICOLON {
        $$ = new AssignmentNode(@2.first_line, @2.first_column, $1, $3);
        //$$->print();
    }
    |
    PRINT Expression SEMICOLON {
        $$ = new PrintNode(@1.first_line, @1.first_column, $2);
    }
    |
    READ VariableReference SEMICOLON {
        $$ = new ReadNode(@1.first_line, @1.first_column, $2);
        //$$->print();
    }
;

VariableReference:
    ID ArrRefList {
        $$ = new VariableReferenceNode(@1.first_line, @1.first_column, $1, $2);
        //$$->print();
    }
;

ArrRefList:
    Epsilon {
        $$ = NULL;
    }
    |
    ArrRefs {
        $$ = $1;
    }
;

ArrRefs:
    L_BRACKET Expression R_BRACKET {
        $$ = new std::vector<AstNode*>;
        $$->push_back($2);
    }
    |
    ArrRefs L_BRACKET Expression R_BRACKET {
        $1->push_back($3);
        $$ = $1;
    }
;

Condition:
    IF Expression THEN
    CompoundStatement
    ElseOrNot
    END IF {
        $$ = new IfNode(@1.first_line, @1.first_column, $2, $4, $5);
    }
;

ElseOrNot:
    ELSE
    CompoundStatement {
        $$ = $2;
    }
    |
    Epsilon {
        $$ = NULL;
    }
;

While:
    WHILE Expression DO
    CompoundStatement
    END DO {
        $$ = new WhileNode(@1.first_line, @1.first_column, $2, $4);
    }
;

For:
    FOR ID ASSIGN INT_LITERAL TO INT_LITERAL DO
    CompoundStatement
    END DO { 


        AstNode* decl_node = new DeclNode(@2.first_line, @2.first_column, $2, "integer");
        // decl_node->print();

        AstNode* var_ref = new VariableReferenceNode(@2.first_line, @2.first_column, $2, NULL);
        // var_ref->print();

        AstNode* constant_value_node = new ConstantValueNode(@4.first_line, @4.first_column, $4, false);
        // constant_value_node->print();

        AstNode* assign_node = new AssignmentNode(@3.first_line, @3.first_column, var_ref , constant_value_node);
        // assign_node->print();

        AstNode* constant_value_node2 = new ConstantValueNode(@6.first_line, @6.first_column, $6, false);

        $$ = new ForNode(@1.first_line, @1.first_column, decl_node, assign_node, constant_value_node2, $8);
        
        //$$->print();
    }
;

Return:
    RETURN Expression SEMICOLON {
        $$ = new ReturnNode(@1.first_line, @1.first_column, $2);
    }
;

FunctionCall:
    FunctionInvocation SEMICOLON {
        $$ = $1;
    }
;

FunctionInvocation:
    ID L_PARENTHESIS ExpressionList R_PARENTHESIS {
        $$ = new FunctionInvocationNode(@1.first_line, @1.first_column, $1, $3);
    }
;

ExpressionList:
    Epsilon {
        $$ = NULL;
    }
    |
    Expressions {
        $$ = $1;
    }
;

Expressions:
    Expression {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Expressions COMMA Expression {
        $1->push_back($3);
        $$ = $1;
    }
;

StatementList:
    Epsilon {
        $$ = NULL;
    }
    |
    Statements {
        $$ = $1;
    }
;

Statements:
    Statement {
        $$ = new std::vector<AstNode*>;
        $$->push_back($1);
    }
    |
    Statements Statement {
        $1->push_back($2);
        $$ = $1;
    }
;

Expression:
    L_PARENTHESIS Expression R_PARENTHESIS {
        $$ = $2;
    }
    |
    MINUS Expression %prec UNARY_MINUS {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "neg", $2);
    }
    |
    Expression MULTIPLY Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "*", $1, $3);
    }
    |
    Expression DIVIDE Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "/", $1, $3);
    }
    |
    Expression MOD Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "mod", $1, $3);
    }
    |
    Expression PLUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "+", $1, $3);
    }
    |
    Expression MINUS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "-", $1, $3);
    }
    |
    Expression LESS Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<", $1, $3);
    }
    |
    Expression LESS_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<=", $1, $3);
    }
    |
    Expression GREATER Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">", $1, $3);
    }
    |
    Expression GREATER_OR_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, ">=", $1, $3);
    }
    |
    Expression EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "=", $1, $3);
    }
    |
    Expression NOT_EQUAL Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "<>", $1, $3);
    }
    |
    NOT Expression {
        $$ = new UnaryOperatorNode(@1.first_line, @1.first_column, "not", $2);
    }
    |
    Expression AND Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "and", $1, $3);
    }
    |
    Expression OR Expression {
        $$ = new BinaryOperatorNode(@2.first_line, @2.first_column, "or", $1, $3);
    }
    |
    IntegerAndReal {
        $$ = $1;
    }
    |
    StringAndBoolean {
        $$ = temp_node;
    }
    |
    VariableReference {
        $$ = $1;
    }
    |
    FunctionInvocation {
        $$ = $1;
    }
;

    /*
       misc
            */
Epsilon:
;
%%

void yyerror(const char *msg) {
    fprintf(stderr,
            "\n"
            "|-----------------------------------------------------------------"
            "---------\n"
            "| Error found in Line #%d: %s\n"
            "|\n"
            "| Unmatched token: %s\n"
            "|-----------------------------------------------------------------"
            "---------\n",
            line_num, buffer, yytext);
    exit(-1);
}

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: ./parser <filename> [--dump-ast]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    if (argc >= 3 && strcmp(argv[2], "--dump-ast") == 0) {
        // root->print();
        root->accept(ast_dumper);
    }

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");

    delete root;
    fclose(yyin);
    yylex_destroy();
    return 0;
}
