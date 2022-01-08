# hw2 report

|||
|-:|:-|
|Name|吳泓緯|
|ID|0716060|

## How much time did you spend on this project

| 任務 | 時間 |
| ---- | ---- |
| 了解 Project2 | 1 hr |
| 了解如何修改 scanner.l | 0.5hr |
| 修改 scanner.l | 0.5hr |
| 了解如何撰寫 parser.y | 0.5hr |
| 撰寫 parser.y | 3hr |
| 合計 | 5.5hr |

## Project overview

Project 2 的任務可以分成下列兩項：
- 修改 scanner.l
- 撰寫 parser.y

### 修改 scanner.l
因為 parser 會透過 yyparse() 對 input stream of tokens 進行 parse，所以就必須先取得 token。因此，yyparse() 就會呼叫 scanner 的 yylex()，yylex() 就會對 input stream of characters 進行 lexical analysis 並回傳 token。

為了讓 scanner.l 與 parse.y 可以溝通相同的 token，必須先在 parse.y 的 declaration 區塊先對 token 進行宣告。並在 scanner.l 的 regular expression 的 action 中 return 相對應的 token。

在 project2 中，scanner.l 使用助教提供的 code 進行修改。以下為在本次 project 中，scanner.l 與 parser.y 互相使用的 token：
| Type | Regex | Token |
| ---- | ---- | ---- |
| Delimiter | "," | COMMA |
|  | ";" | SEMICOLON |
|  | ":" | COLON |
|  | "(" | OPENPAREN |
|  | ")" | CLOSEPAREN |
|  | "[" | OPENBRACKET |
|  | "]" | CLOSEBRACKET |
| Operator | "+" | ADDITION |
|  | "-" | SUBTRACTION |
|  | "*" | MULTIPLICATION |
|  | "/" | DIVISION |
|  | "mod" | MODULE |
|  | ":=" | ASSIGN |
|  | "<" | LESS |
|  | "<=" | LESSEQUAL |
|  | "<>" | LESSGREATER |
|  | ">=" | GREATEREQUAL |
|  | ">" | GREATER |
|  | "=" | EQUAL |
|  | "and" | AND |
|  | "or" | OR |
|  | "not" | NOT |
| Keyword | "array" | KWARRAY |
|  | "begin" | KWBEGIN |
|  | "while" | KWWHILE |
|  | "boolean" | KWBOOLEAN |
|  | "def" | KWDEF |
|  | "do" | KWDO |
|  | "else" | KWELDE |
|  | "end" | KWBEND |
|  | "false" | KWFALSE |
|  | "for" | KWFOR |
|  | "integer" | KWINTEGER |
|  | "if" | KWIF |
|  | "of" | KWOF |
|  | "print" | KWPRINT |
|  | "read" | KWREAD |
|  | "real" | KWREAL |
|  | "string" | KWSTRING |
|  | "then" | KWTHEN |
|  | "to" | KWTO |
|  | "true" | KWTRUE |
|  | "return" | KWRETURN |
|  | "var" | KWVAR |
| Identifier | [a-zA-Z][a-zA-Z0-9]* | ID |
| Decimal Integer | 0&#124;[1-9][0-9]* | INTEGER |
| Octal Integer | 0[0-7]+ | OCTINTEGER |
| Floating Point | {integer}\.(0&#124;[0-9]*[1-9]) | FLOAT |
| Scientific Notation | ({integer}&#124;{float})[Ee][+-]?({integer}) | SCIENTIFIC |
| String | \"([^"\n]&#124;\"\")*\" | STRING |

Whitespace, Comment 與 PseudoComment 的 token 則不需要回傳給 parser。

### 撰寫 parser.y
在前面的階段中已經把會用到的 token 在 parser.y 的 declaration 區塊定義完成。在這個階段，主要就是撰寫 context-free grammar rule。

根據助教提供的 spec，P language 的 syntax 主要可以分為下列：
- [Program](#program)
- [Function](#Function)
- Data Declaration
- [Statement](#Statement)
- [Expresion](#Expression)

#### Program
- program spec:
```
identifier;
<zero or more variable and constant declaration>
<zero or more function declaration and definition>
<one compound statement>
end
```

- context-free grammar:
```
program_id: identifier_semicolon program_contents program_end;
identifier_semicolon: ID SEMICOLON;
program_contents: variable_constant_decl function_decl_def compound_statement;
program_end: KWEND;
```

#### Function
- declaration spec:
```
identifier (<zero or more formal arguments>): scalar_type;
```

- definition spec:
```
identifier (<zero or more formal arguments>): scalar_type
<one compound statement>
end
```

- context-free grammar:
```
function_decl_def: | function_decl_def1;
function_decl_def1: function_decl_def2 | function_decl_def1 function_decl_def2;
function_decl_def2: function_decl | function_def;
function_decl: ID OPENPAREN function_argument CLOSEPAREN COLON scalar_type SEMICOLON;
function_def: ID OPENPAREN function_argument CLOSEPAREN COLON scalar_type compound_statement KWEND
              | ID OPENPAREN function_argument CLOSEPAREN compound_statement KWEND
              ;
```

- argument spec:
```
identifier_list: type
```

- context-free grammar:
```
function_argument: | function_argument1;
function_argument1: function_argument2 | function_argument1 SEMICOLON function_argument2;
function_argument2: identifier_list  COLON scalar_type;
```

- identifier list spec:
```
identifier, identifier, ..., identifier
```

- context-free grammar:
```
identifier_list: identifier_list1;
identifier_list1: ID | identifier_list1 COMMA ID;
```

- context-free grammar:
```
scalar_type: KWINTEGER | KWREAL | KWSTRING | KWBOOLEAN;
array_type: KWARRAY INTEGER KWOF scalar_type | KWARRAY INTEGER KWOF array_type;
constant_value: INTEGER | FLOAT | OCTINTEGER | SCIENTIFIC | STRING | KWTRUE | KWFALSE;
```

#### Data Declaration
- variable spec:
```
var identifier_list: scalar_type;
```
```
var identifier_list: array integer_constant of type;
```

- constant spec:
```
var identifier_list: literal_constant;
```

- context-free grammar:
```
variable_constant_decl: | variable_constant_decl1;
variable_constant_decl1: variable_constant_decl2 | variable_constant_decl1 variable_constant_decl2;
variable_constant_decl2: KWVAR identifier_list COLON scalar_type SEMICOLON | KWVAR identifier_list COLON array_type SEMICOLON |KWVAR identifier_list COLON constant_value SEMICOLON;
```

#### Statement
- context-free grammar:
```
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
```

- compound spec:
```
begin
<zero or more variable and constant declaration>
<zero or more statements>
end
```

- context-free grammar:
```
compound_statement: KWBEGIN variable_constant_decl statement KWEND;
```

- simple spec:
```
variable_reference := expression;
```
```
print variable_reference;
```
```
read variable_reference;
```
```
identifier [expression] [expression] [...]
```

- context-free grammar:
```
simple_statement: | simple_statement1;
simple_statement1: simple_statement2 | simple_statement1 simple_statement2;
simple_statement2: variable_ref ASSIGN expression SEMICOLON 
                   | KWPRINT variable_ref SEMICOLON 
                   | KWPRINT expression SEMICOLON 
                   | KWREAD variable_ref SEMICOLON
                   | procedure_call_statement
                   ;
```
```
variable_ref: ID | ID OPENBRACKET expression CLOSEBRACKET;
```

- conditional spec:
```
if expression then
<one compound statement>
else
<one compound statement>
end if
```
```
if exprression then
<one compound statement>
end if
```

- context-free grammar:
```
condition_statement: KWIF expression KWTHEN compound_statement KWELSE compound_statement KWEND KWIF 
                     | KWIF expression KWTHEN compound_statement KWEND KWIF
                     ;
```

- while spec:
```
while expression do
<one compound statement>
end do
```

- context-free grammar:
```
while_statement: KWWHILE expression KWDO compound_statement KWEND KWDO;
```

- for spec:
```
for identifier := integer_constant to integer_constant do
<one compound statement>
end do
```

- context-free grammar:
```
for_statement: KWFOR ID ASSIGN INTEGER KWTO INTEGER KWDO compound_statement KWEND KWDO;
```

- return spec:
```
return expression ;
```

- context-free grammar:
```
return_statement: KWRETURN expression SEMICOLON;
```

- procedure call spec:
```
identifier (<expressions separated by zero or more comma>) ;
```

- context-free grammar:
```
procedure_call_statement: ID OPENPAREN function_call_argument CLOSEPAREN SEMICOLON;
```




#### Expression
- context-free grammar:
```
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
```

- function invocation spec:
```
identifier (<expression separated by zero or more comma>)
```

- context-free grammar:
```
function_call_argument: | function_call_argument1;
function_call_argument1: expression | function_call_argument1 COMMA expression;
```

## What is the hardest you think in this project

- 起手式：
    我認為在實作 project2 中最困難的地方在於「第一步」。雖然複習投影片可以知道 parser 與 scanner 之間的邏輯與互動關係。但是要開始實作時，仍有些不確定要從何處開始修改。
- CFG Debug：
    在寫 project1 的 regex 時，可以寫一個 rule 就去 test 一遍，也就是說可以很容易地進行 unit testing。但是在寫 project2 的 cfg 時，就必須先將很多 rule 一次寫完之後，才有辦法進行 testing。當 testing 發生錯誤時，也必須花更多時間想像 parsing 過程，找到可能錯誤的 rule。 

## Feedback to T.A.s

> 感謝 TA 提供非常多的測資，這使我在寫作業的過程可以更精確的發現錯誤，提升 Debug 的效率！
> Project 在 Github 上的描述已經相當清楚。但是如果可以在介紹每種 Syntax 的 Rule 時，再加上一些範例，就可以更好理解 Syntex Spec 的意義，就可以在實際去檢查 test folder 的檔案之前，對於該 syntax 有更多理解。
