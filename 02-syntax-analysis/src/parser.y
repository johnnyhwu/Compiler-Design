%{
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

extern int32_t line_num;   /* declared in scanner.l */
extern char buffer[512];  /* declared in scanner.l */
extern FILE *yyin;        /* declared by lex */
extern char *yytext;      /* declared by lex */

extern int yylex(void); 
static void yyerror(const char *msg);
%}

%token COMMA SEMICOLON COLON OPENPAREN CLOSEPAREN OPENBRACKET CLOSEBRACKET
%token ADDITION SUBTRACTION MULTIPLICATION DIVISION MODULE ASSIGN LESS LESSEQUAL LESSGREATER GREATEREQUAL GREATER EQUAL AND OR NOT
%token KWARRAY KWBEGIN KWBOOLEAN KWDEF KWDO KWELSE KWEND KWFALSE KWFOR KWINTEGER KWIF KWOF KWPRINT KWREAD KWREAL KWSTRING KWTHEN KWTO KWTRUE KWRETURN KWVAR KWWHILE
%token ID
%token INTEGER OCTINTEGER
%token FLOAT
%token SCIENTIFIC
%token STRING

%left ADDITION SUBTRACTION;
%left MODULE MULTIPLICATION DIVISION;

%%

program_id: identifier_semicolon program_contents program_end;
identifier_semicolon: ID SEMICOLON;
program_contents: variable_constant_decl function_decl_def compound_statement;
program_end: KWEND;

variable_constant_decl: | variable_constant_decl1;
variable_constant_decl1: variable_constant_decl2 | variable_constant_decl1 variable_constant_decl2;
variable_constant_decl2: KWVAR identifier_list COLON scalar_type SEMICOLON | KWVAR identifier_list COLON array_type SEMICOLON |KWVAR identifier_list COLON constant_value SEMICOLON;

function_decl_def: | function_decl_def1;
function_decl_def1: function_decl_def2 | function_decl_def1 function_decl_def2;
function_decl_def2: function_decl | function_def;
function_decl: ID OPENPAREN function_argument CLOSEPAREN COLON scalar_type SEMICOLON;
function_def: ID OPENPAREN function_argument CLOSEPAREN COLON scalar_type compound_statement KWEND
              | ID OPENPAREN function_argument CLOSEPAREN compound_statement KWEND
              ;

function_argument: | function_argument1;
function_argument1: function_argument2 | function_argument1 SEMICOLON function_argument2;
function_argument2: identifier_list  COLON scalar_type;

identifier_list: identifier_list1;
identifier_list1: ID | identifier_list1 COMMA ID;

scalar_type: KWINTEGER | KWREAL | KWSTRING | KWBOOLEAN;
array_type: KWARRAY INTEGER KWOF scalar_type | KWARRAY INTEGER KWOF array_type;
constant_value: INTEGER | FLOAT | OCTINTEGER | SCIENTIFIC | STRING | KWTRUE | KWFALSE;

statement: | statement1;
statement1: statement2 | statement1 statement2 ;
statement2: compound_statement 
            | simple_statement 
            | return_statement 
            | condition_statement 
            | while_statement 
            | for_statement 
            | procedure_call_statement
            ;

procedure_call_statement: ID OPENPAREN function_call_argument CLOSEPAREN SEMICOLON;

return_statement: KWRETURN expression SEMICOLON;

condition_statement: KWIF expression KWTHEN compound_statement KWELSE compound_statement KWEND KWIF 
                     | KWIF expression KWTHEN compound_statement KWEND KWIF
                     ;

while_statement: KWWHILE expression KWDO compound_statement KWEND KWDO;

for_statement: KWFOR ID ASSIGN INTEGER KWTO INTEGER KWDO compound_statement KWEND KWDO;

compound_statement: KWBEGIN variable_constant_decl statement KWEND;

simple_statement: | simple_statement1;
simple_statement1: simple_statement2 | simple_statement1 simple_statement2;
simple_statement2: variable_ref ASSIGN expression SEMICOLON 
                   | KWPRINT variable_ref SEMICOLON 
                   | KWPRINT expression SEMICOLON 
                   | KWREAD variable_ref SEMICOLON
                   | procedure_call_statement
                   ;

variable_ref: ID | ID OPENBRACKET expression CLOSEBRACKET;
expression: INTEGER 
            | FLOAT
            | STRING
            | ID OPENBRACKET expression CLOSEBRACKET
            | SCIENTIFIC
            | ID 
            | expression ADDITION expression 
            | expression SUBTRACTION expression 
            | expression MULTIPLICATION expression 
            | expression DIVISION expression 
            | expression MODULE expression 
            | SUBTRACTION expression %prec MULTIPLICATION 
            | OPENPAREN expression CLOSEPAREN
            | expression LESS expression
            | expression LESSEQUAL expression
            | expression LESSGREATER expression
            | expression GREATEREQUAL expression
            | expression GREATER expression
            | expression EQUAL expression
            | expression AND expression
            | expression OR expression
            | NOT expression %prec MULTIPLICATION
            | ID OPENPAREN function_call_argument CLOSEPAREN
            ;

function_call_argument: | function_call_argument1;
function_call_argument1: expression | function_call_argument1 COMMA expression;

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
    if (argc != 2) {
        fprintf(stderr, "Usage: ./parser <filename>\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    assert(yyin != NULL && "fopen() fails.");

    yyparse();

    printf("\n"
           "|--------------------------------|\n"
           "|  There is no syntactic error!  |\n"
           "|--------------------------------|\n");
    return 0;
}
