%{
#include <stdint.h>

#define LIST                strCat(yytext)
#define TOKEN(t)            { LIST; if (opt_tok) printf("<%s>\n", #t); }
#define TOKEN_CHAR(t)       { LIST; if (opt_tok) printf("<%c>\n", (t)); }
#define TOKEN_STRING(t, s)  { LIST; if (opt_tok) printf("<%s: %s>\n", #t, (s)); }
#define MAX_LINE_LENG       512

uint32_t line_num = 1;
char buffer[MAX_LINE_LENG];
char modify_buffer[MAX_LINE_LENG];

static int32_t opt_src = 1;
static int32_t opt_tok = 1;
static char *buffer_ptr = buffer;


static void strCat(const char *Text);
static void strModify(char *Text);
static void complementSrc(int32_t state);
static void complementTok(int32_t state);
static void debug(int32_t flag);

%}

%x COMMENT_STATE

%%
 /******************** Space & Tab ********************/

" " { LIST; }
(\t)+ { LIST; }
 
 /******************* Pseudocomment *******************/

([/]{2})(&S)([+]{1})([\40-\176]*) { complementSrc(1); LIST;  } 
([/]{2})(&S)([-]{1})([\40-\176]*) { complementSrc(0); LIST;  } 
([/]{2})(&T)([+]{1})([\40-\176]*) { complementTok(1); LIST;  } 
([/]{2})(&T)([-]{1})([\40-\176]*) { complementTok(0); LIST;  } 

 /********************* Comment *********************/

[/]{1}[*]{1} { BEGIN COMMENT_STATE; LIST; }
<COMMENT_STATE>[^*\n]* { LIST; }
<COMMENT_STATE>[*]+[^/\n] { LIST; }
<COMMENT_STATE>[*]{1}[/]{1} { BEGIN INITIAL; LIST; }

[/]{1}[/]{1}.* { LIST; }

 /******************** Delimiters ********************/

 /* Comma */
"," {TOKEN_CHAR(','); }

 /* Semicolon */
";" {TOKEN_CHAR(';'); }

 /* Colon */
":" {TOKEN_CHAR(':'); }

 /* Parantheses */
")" { TOKEN_CHAR(')'); }
"(" { TOKEN_CHAR('('); }

 /* Square Brackets */
"[" { TOKEN_CHAR('['); }
"]" { TOKEN_CHAR(']'); }

 /******************** Arithmetic ********************/

 /* Addition */
"+" { TOKEN_CHAR('+'); }

 /* Subtraction */
"-" { TOKEN_CHAR('-'); }

 /* Multiplication */
"*" { TOKEN_CHAR('*'); }

 /* Division */
"/" { TOKEN_CHAR('/'); }

 /* Division */
"mod" { TOKEN(mod); }

 /* Assignment */
":=" { TOKEN(:=); }

 /******************** Relational ********************/

 /* Less than */
"<" { TOKEN_CHAR('<'); }

 /* Less than or Equal to */
"<=" { TOKEN(<=); }

 /* Not Equal to */
"<>" { TOKEN(<>); }

 /* Greater that or Equal to */
">=" { TOKEN(>=); }

 /* Greater than */
">" { TOKEN_CHAR('>'); }

 /* Equal to */
"=" { TOKEN_CHAR('='); }

 /********************* Logical *********************/

 /* AND */
"and" { TOKEN(and); }

 /* OR */
"or" { TOKEN(or); }

 /* NOT */
"not" { TOKEN(not); }

 /********************* Keywords *********************/

 "array" { TOKEN(KWarray); }
 "begin" { TOKEN(KWbegin); }
 "boolean" { TOKEN(KWboolean); }
 "def" { TOKEN(KWdef); }
 "do" { TOKEN(KWdo); }
 "else" { TOKEN(KWelse); }
 "end" { TOKEN(KWend); }
 "false" { TOKEN(KWfalse); }
 "for" { TOKEN(KWfor); }
 "integer" { TOKEN(KWinteger); }
 "if" { TOKEN(KWif); }
 "of" { TOKEN(KWof); }
 "print" { TOKEN(KWprint); }
 "read" { TOKEN(KWread); }
 "real" { TOKEN(KWreal); }
 "string" { TOKEN(KWstring); }
 "then" { TOKEN(KWthen); }
 "to" { TOKEN(KWto); }
 "true" { TOKEN(KWtrue); }
 "return" { TOKEN(KWreturn); }
 "var" { TOKEN(KWvar); }
 "while" { TOKEN(KWwhile); }

 /******************** Identifier ********************/

[A-Za-z][a-zA-Z0-9]* { TOKEN_STRING(id, yytext); }

 /***************** Integer Constants *****************/

 /* octal */
0[0-7]+ { TOKEN_STRING(oct_integer, yytext); }

 /* decimal */
0|([1-9][0-9]*) { TOKEN_STRING(integer, yytext); }

 /*************** Float-Point Constants ***************/

"0.0" {TOKEN_STRING(float, yytext); }
(([1-9][0-9]*)|(0))\.([0-9]*[1-9]+) { TOKEN_STRING(float, yytext); }

 /**************** Scientific Notation ****************/

((0|([1-9][0-9]*))|("0.0")|((([1-9][0-9]*)|(0))\.(([0-9]*[1-9]+)|0)))(E|e)((\+|-)?(0|([1-9][0-9]*))) { TOKEN_STRING(scientific, yytext); }

 /********************** String **********************/

\"[^"]*\" { strModify(yytext); TOKEN_STRING(string, modify_buffer); }
\"[^"]+\"{2,}[^"]+\" { strModify(yytext); TOKEN_STRING(string, modify_buffer); }
\"\"+\" { strModify(yytext); TOKEN_STRING(string, modify_buffer); }
\"\"{2,}[^" ]+\"{2,}\" { strModify(yytext); TOKEN_STRING(string, modify_buffer); }
\"[^" ]+\"{2,}[^" ]+\"{2,}[^" ]+\" { strModify(yytext); TOKEN_STRING(string, modify_buffer); }

\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}

<COMMENT_STATE>\n {
    if (opt_src) {
        printf("%d: %s\n", line_num, buffer);
    }
    ++line_num;
    buffer[0] = '\0';
    buffer_ptr = buffer;
}


 /* Catch the character which is not accepted by rules above */
. {
    printf("Error at line %d: bad character \"%s\"\n", line_num, yytext);
    exit(-1);
}

%%

static void debug(int32_t flag) {
    printf("debug: %d\n", flag);
}

static void strCat(const char *Text) {
    while (*Text) {
        *buffer_ptr++ = *Text++;
    }
    *buffer_ptr = '\0';
}
 /* "ab""c" */
static void strModify(char *Text) {
    int i=1;
    int j=0;
    while(i<yyleng-1) {
        if(Text[i] != '\"') {
            modify_buffer[j] = Text[i];
            j++;
            i++;
        }
        else {
            while(Text[i+1] == '\"' && i+1<yyleng-1) {
                modify_buffer[j] = '\"';
                j++;
                i = i + 2;
            }

            if(i<yyleng-1) {
                modify_buffer[j] = Text[i];
                j++;
                i++;
            }
        }
        
    }
    modify_buffer[j] = '\0';

}

static void complementSrc(int32_t state) {
    if(state == 1) {
        opt_src = 1;
    }
    else {
        opt_src = 0;
    }
}

static void complementTok(int32_t state) {
    if(state == 1) {
        opt_tok = 1;
    }
    else {
        opt_tok = 0;
    }
}

int main(int argc, const char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./scanner [filename]\n");
        exit(-1);
    }

    yyin = fopen(argv[1], "r");
    if (yyin == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-1);
    }

    yylex();

    return 0;
}
