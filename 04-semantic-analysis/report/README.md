# hw4 report

|||
|-:|:-|
|Name|吳泓緯|
|ID|0716060|

## How much time did you spend on this project

大約花了一週總共 20 個小時（平均每天 2 ~ 3 小時）來完成這次的作業

## Project overview

### Project Structure
這次的作業大致可以分成以下部分：
- 修改 scanner.l
- 修改 parser.y
- 建立 symbol table 的 prototype
- 實作 semantic analyzer

### 修改 scanner.l
因為這次的作業中，必須印出 symbol table。然而，是否印出 symbol table 又可以由 source code 中特別的 comment 來控制。

因此，我透過一個 flag （全域變數）作為要不要印出 symbol table 的依據。所以，必須在 scanner.l 中進行判斷，如果偵測到特定的 token 就對這個 flag 進行相對應的設置。

```
    /* Pseudocomment */
"//&"[STD][+-].* {
    LIST;
    char option = yytext[3];
    switch (option) {
    case 'S':
        opt_src = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'T':
        opt_tok = (yytext[4] == '+') ? 1 : 0;
        break;
    case 'D':
        opt_table = (yytext[4] == '+') ? 1 : 0;
        break;
    }
}
```

### 修改 parser.y
因為這次的作業多做了 semantic analysis，所以當 source code 中沒有任何錯誤時，印出的文字與上次的作業會有所不同，因此必須針對這部份進行修改。在這次的作業中，除了建立一個 syntax tree 之外，還必須對 syntax tree 上的 node 進行 semantic analysis。

因此，在 parser.y 的 main function 中，必須讓 syntax tree 的 root node accept 一個 SemanticAnalyzer 物件，以 visitor pattern 的方式，讓 syntax tree 中的 node 可以進行 semantic analysis。

```
SemanticAnalyzer analyzer;
    root->accept(analyzer);

    if(semantic_error == false) {
        printf("\n"
           "|---------------------------------------------------|\n"
           "|  There is no syntactic error and semantic error!  |\n"
           "|---------------------------------------------------|\n");
    }
```

### 建立 Symbol Table 的 Prototype

在本次作業中，因為要對 syntax tree 中的 node 進行 semantic analysis，因此必須先建立一個 symbol table 來存放這些 node 的資訊。實現 symbol table 的資料結構有很多種，這裡透過一個 vector 來實作。

然而，並不是所有的 node 都要存入 symbol table 中，只需要存放 declaration 的資訊即可。也就是說，變數宣告與函式的宣告都會被當作一個 entry 存放在 symbol table 中。

每一個變數或函式都有其 scope，因此整個 program 可能會有很多不同 level 的 scope，因此必須透過一個 manager 來管理這些 symbol table。這裡以 stack 資料結構來實作 manager。

以下為我在本次作業中基於助教提供的 sample code 實作 symbol table manager、symbol table 與 symbol entry 的方式：

- SymbolTableManager
```cpp
class SymbolManager : public AstNodeVisitor {
    public:
        SymbolManager();
        ~SymbolManager();

        void pushTable(SymbolTable new_table);
        void popTable();

        std::stack<SymbolTable> tables;
};
```

- SymbolTable
```cpp
class SymbolTable
{
    public:
        SymbolTable(/* args */);
        ~SymbolTable();

        void addEntry(SymbolEntry & entry);

        std::vector<SymbolEntry> table;
};
```

- SymbolEntry
```cpp
class SymbolEntry {
    public:
        SymbolEntry(std::string _name, std::string _kind, int _level, std::string _type, std::string _attribute);
        SymbolEntry();
        ~SymbolEntry();
        
        std::string name;
        std::string kind;
        int level;
        std::string type;
        std::string attribute;
};
```

### 實作 Semantic Analyzer

本次作業中必須以 visitor pattern 的方式對 syntax tree 中的 node 進行 semantic analysis。因此在這個部分中主要是實作每一個 node 的 visit function。

每一個 node 的 visit function 的內容可以概括的分為以下五個步驟：
- 如果這個 node 形成一個新的 scope 的話，就必須 push 一個新的 symbol table 到 manager 中
- 如果這個 node 正在進行變數宣告或是函式宣告，就必須新增一個 entry 到 symbol table 中
- 拜訪這個節點的子結點
- 對這個結點進行 semantic analysis
- 將第一步 push 到 manager 中的 symbol table pop 出

然而，因為每個 node 有其不同的功能，因此實際在實作每一個 node 的 visit function 時，也有很多差別。因為 node 數量與程式碼數量龐大，因此以下將會針對比較特別的地方與實作方式進行解說。

- Program Node
    
    在 syntax tree 中，如果遇到某些特定的 node 就必須建立一個新的 scope，也就是 push 一個新的 symbol table 到 manager 中，Program Node 就是其中的一個 node。這裡的做法是新建立一個 symbol table，並新增一個 entry 到 table 中，並在拜訪子結點之前，先將 table push 到 manager 中。如此一來，在子結點如果遇到任何函式與變數的宣告，就能夠在這個 table 新增 entry。
```cpp
st = new SymbolTable();
    
se = new SymbolEntry();

std::string program_name = p_program.getNameCString();
program_name = program_name.substr(0, 32);

se->name = program_name;
se->kind = "program";
scope_level = 0;
se->level = scope_level;
se->type = "void";

st->addEntry(*se);

sm->pushTable(*st);
// p_program.visitChildNodes(*this);
p_program.visitChildNodesDecl(*this);
p_program.visitChildNodesFunc(*this);

from_program = true;
p_program.visitChildNodesComp(*this);
from_program = false;

dumpSymbol();
sm->popTable();
```


- Variable Node
    
    當遇到 variable node 時，就相當於是遇到了變數的宣告。因此，必須在 symbol table 中新增一個 entry。然而，變數的宣告可以是 variabl、parameter 或是 loop_var。因此，透過許多 flag（例如：enter_function_param、from_for、enter_for_decl）來區別不同的變數宣告。在 semantic analyzer 中，也透過許多這樣的 flag 來區別目前的狀態。
```cpp
se = new SymbolEntry();
se->name = variable_name;
se->kind = "variable";
if(enter_function_param) {
    se->kind = "parameter";
}

if(from_for && enter_for_decl) {
    se->kind = "loop_var";
}

...

st = &(sm->tables.top());
st->addEntry(*se);
```


- BinaryOperator Node
    
    BinaryOperator 因為可以有很深的階層關係，因此這裡透過一個 stack 來存放每一次拜訪子結點所得到的類型。當拜訪完左右子結點之後，就可以將 stack 中最上面兩個元素 pop 出，並針對這兩個類型進行比較。
```cpp
from_binary_operator_left = true;
p_bin_op.visitChildNodeLeft(*this);
from_binary_operator_left = false;

from_binary_operator_right = true;
p_bin_op.visitChildNodeRight(*this);
from_binary_operator_right = false;


right_operand_type_name = binary_op_stk.top();
binary_op_stk.pop();

left_operand_type_name = binary_op_stk.top();
binary_op_stk.pop();
```


- Assignment Node
    
    因為在 test case 中是先檢查右邊有無錯誤再檢查左邊。因此，不可以直接使用原本的 visitChildNodes method，而是將左節點與右節點的 visit 分開，並將順序對調。此外，assignment 過程中，也必須檢查左右兩邊的 type 是否吻合，因此這裡也透過 stack 來暫時存放左節點與右節點的類型，並進行比較。
```cpp
check_assignment_ref_is_consatnt = true;
p_assignment.visitChildNodesVarRef(*this);
check_assignment_ref_is_consatnt = false;

from_assignment = true;

from_assignment_right = true;
p_assignment.visitChildNodesExpr(*this);
from_assignment_right= false;

from_assignment_left = true;
p_assignment.visitChildNodesVarRef(*this);
from_assignment_left = false;

from_assignment = false;

...

std::string str1 = assignment_left_right_stack.top();
assignment_left_right_stack.pop();

...

std::string str2 = assignment_left_right_stack.top();
assignment_left_right_stack.pop();
```




## What is the hardest you think in this project

### Obstacle 1
在做本次作業時，遇到了許多挫折。剛開始將 Spec 閱讀完之後，能夠清楚的了解本次作業的方向。因此，很快就修改好了 scanner.l 與 parser.y，並建立 symbol table 的 prototype。但是要開始實作 semantic analysis 時，卻遇到許多問題。例如，同樣都是 variable node，但是卻可能出現在不同的地方，必須賦予不同的資訊到 symbol table 中。

### Solution 1
最後發現可以透過許多全域變數來當作 flag，在拜訪子節點之前，就把相對應的 flag 開啟。如此一來，在子節點中就可以依據這個 flag，進行不同的操作。

### Obstacle 2
在寫 BianryOperator Node 時，如果是只有一層的話，就沒有問題（例如：a + b）。但是，如果出現許多層級的話，就會出現問題（例如：a + (2 + 3 * (b+n))）。

### Solution 2
最後透過 stack 來存放兩邊的 type 資訊，並在拜訪完左右節點之後，才將 stack 最上面兩個元素 pop 出來，並進行比較，才得以解決這個問題。

## Feedback to T.A.s

> 很感謝助教在 spec 中清楚的說明本次作業的方向，讓我得以很快的了解這次的目的。助教提供的 symbol table 的 sample code 基本上只需要再加上一些必要的 method 就可以運作。最後，因為這次的作業我是基於助教的作業三進行改寫，因此許多 node 中都有很方便的 method 可以呼叫，讓我在將 node 資訊存入 symbol table 時，可以更為便利。
