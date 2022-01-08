# hw3 report

|||
|-:|:-|
|Name|吳泓緯|
|ID|0716060|

## How much time did you spend on this project

8 hours: 了解本次作業以及 Visitor Pattern 概念

6 hours: 了解應該如何定義每一個 Node 以及完成每一個 Node 的 cpp 與 hpp 檔案

10 hours: 修改 scanner.l 與撰寫 parser.y 並除錯


## Project overview

> 在這個 Project 中，目的是透過撰寫 Parser 中的 Action (Semantic Rule)，建立相對應的 Node，進而組成一棵 Syntax Tree。此外，在 Print 出 Syntax Tree 的過程，並不是單純的將 Node Print 出來，而是透過 Visitor Pattern 的方式，來 Print 出整個 Syntax Tree

為了完成此 Project 可以大致分為三個步驟：
- 修改 scanner.l 才可以在 parser.y 中讀取到相對應的 Token 的 value
- 建立 Abstract Syntax Tree 中每一個 Node 的 Prototype (hpp) 與 Implementation (cpp)
- 撰寫 parser.y 中的 semantic action，使得 parse 完某個 syntax 時，可以執行相對應的 action，建立相對應的 Node

以下會針對每個部分的重點實作想法與細節進行說明

#### Part1: 修改 scanner

> 目的是為了能夠在 parser.y 中的 semantic action 中，透過 $$、$1、$2 ... 等變數，去讀取到 Token 與 Nonterminal 的 value

因此，scanner.l 中，除了辨識到某個 Token 的 Pattern 要回傳這個 Token 的代號之外，還要將此 Token 實際的內容存到 yylval 中。

例如：
```
[a-zA-Z][a-zA-Z0-9]* {
    TOKEN_STRING(id, yytext);
    yylval.text = strndup(yytext, MAX_ID_LENG);
    return ID;
}
```

yylval 能夠存取不同類型的資料，在 parser.y 中透過 %union 區塊來定義 yylval 可以存放的資料的類型。

#### Part2: 建立每一個 Node 的 Prototype 與 Implementation

> 因為最後必須要建立一棵 Abstract Syntax Tree，因此必須先定義好 AST 中每一個 Node 的 Attribute 與 Method。

在這個部分中，助教提供了很多細節，除了在 Github 上有說明每一個 Node 應該具備的 Attribute 之外，在 hpp 與 cpp files 中，也提供了大部分的程式碼，並透過 "TODO Comment" 提示我們應該改的地方。

> 因為所有的 Node 都必須紀錄 location 的資訊，所以透過讓所有的 Node 都繼承 AstNode 的方式，並在 AstNode 中定義了 location 的結構變數。如此一來，就不用在每個 Node 中都還需要重複處理 location 的問題。

舉例：ast.hpp
```cpp
struct Location {
    Location(const uint32_t line, const uint32_t col) : line(line), col(col) {}

    uint32_t line;
    uint32_t col;
};

class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;
    virtual void accept(AstDumper & ast_dumper) = 0;
    
  protected:
    const Location location;
};
```

> 透過 pointer to node 來記錄一個 node 的資訊；透過 pointer to vector 來儲存 list of node 的資訊

舉例：assignment.hpp

一個 AssignmentNode 中，必須記錄一個 VariableReferenceNode 與 ExpressionNode，因此在 AssignmentNode 中就會有兩個 pointer to node 的 attribute，並在建立 AssignmentNode 時，對這兩個 pointer 進行初始化

```cpp
class AssignmentNode : public AstNode {
  public:
    AssignmentNode(const uint32_t line, const uint32_t col,
                   AstNode* _variable_ref_node,
                   AstNode* _expression_node
                   /* TODO: variable reference, expression */);
    ~AssignmentNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: variable reference, expression
    AstNode* variable_ref_node;
    AstNode* expression_node;
};
```

舉例：decl.hpp

一個 DeclNode 中，必須記錄很多 VariableNode 的資訊。因此，在 DeclNode 中，會有一個 pointer to vector，且這個 vector 中的每一個元素都是 pointer to node。透過這樣的方式使 DeclNode 可以紀錄很多 VariableNode 的資訊

```cpp
class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             std::vector<VariableNode*> *_variable_node_list
             /* TODO: identifiers, type */);
    
    DeclNode(const uint32_t line, const uint32_t col, char* identifiers,
        char* type);

    // constant variable declaration
    // DeclNode(const uint32_t, const uint32_t col
    //         /* TODO: identifiers, constant */);

    ~DeclNode() = default;
    void visitChildNodes(AstDumper & ast_dumper);
    void accept(AstDumper & ast_dumper) override;
    std::vector<std::string> getVariableInfo();

    void print() override;

  public:
    // TODO: variables
    std::vector<VariableNode*> *variable_node_list;
    
};
```
> 透過 Visitor Pattern 的方式來印出 AST

讓每一個 Node 都可以透過 accept() 並傳入 AstDumper，再透過 AstDumper 呼叫 visit() 並傳入自己 (Node)，進而印出這個 Node 應該印出的內容。

所以，必須在 ast.hpp 中，加入 accept() virtual function。
```cpp
class AstNode {
  public:
    AstNode(const uint32_t line, const uint32_t col);
    virtual ~AstNode() = 0;

    const Location &getLocation() const;
    virtual void print() = 0;
    virtual void accept(AstDumper & ast_dumper) = 0;
    
  protected:
    const Location location;
};
```

並在每一個繼承 AstNode 的 Node 中，都必須 override accept()。並增加 visitChildNodes()，才能以 visitor pattern 的方式繼續印出其 child node。這裏以 PrintNode 為例：
```cpp
class PrintNode : public AstNode {
  public:
    PrintNode(const uint32_t line, const uint32_t col, AstNode* _expression_node
              /* TODO: expression */);
    ~PrintNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);
    void accept(AstDumper& adt_node) override;
    void print() override;

  private:
    // TODO: expression
    AstNode* expression_node;
};
```

```cpp
void PrintNode::accept(AstDumper& ast_dumper)
{ // visitor pattern version
    ast_dumper.visit(*this);
}
```

```cpp
void PrintNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    expression_node->accept(ast_dumper);
}
```

最後，則是修改 AstDumper.cpp 中的 visit() method。這裡助教已經提示了每一個 Node 應該印出什麼資訊。這裡以 visit(VariableNode & p_variable) 為例：
```cpp
oid AstDumper::visit(VariableNode &p_variable) {
    outputIndentationSpace(m_indentation);

    // TODO: name, type
    std::printf("variable <line: %u, col: %u> %s %s\n",
                p_variable.getLocation().line, p_variable.getLocation().col,
                p_variable.getVariableName().c_str(), p_variable.getTypeName().c_str());

    incrementIndentation();
    p_variable.visitChildNodes(*this);
    decrementIndentation();
}
```

#### Part3: 撰寫 parser.y 中的 semantic action

> 為了能夠在 semantic action 中，透過 $$、$1 ... 等變數來讀取 Token 與 Nonterminal 的 value。必須先在 %union 中定義 yylval 可以接受的 type，並透過 %type 定義每一個 Terminal 與 Nonterminal 的 type。

```
%union {
    /* basic semantic value */
    char *text;

    AstNode *node;
    std::vector<AstNode*> *vector_node;
    std::vector<VariableNode*> *vector_node_variable;
};
```

```
%type <text> ProgramName
%type <text> ID
%type <vector_node> DeclarationList
%type <vector_node> Declarations
%type <vector_node> FunctionList
%type <vector_node> Functions
%type <node> CompoundStatement
%type <vector_node_variable> IdList
...
```

> 撰寫 parse 到每一個 rule 之後，應該執行的 semantic action

舉例：IdList 

當 parse 到 IdList 中的一個 ID 時，透過 new 的方式在 heap memory 中新增一個 VariableNode。並創立一個 vector 來存放 pointer to node。最後，再將此 pointer to vector assign 到 Nonterminal IdList。

```
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
```


## What is the hardest you think in this project

> 最難的部分在於剛開始看完整份 Spec 後，還是不知道應該如何下手。因為之前也沒有聽過 visitor pattern，除了對這個 design pattern 很陌生之外，也不知道該如何應用在此 Project 中。最後是把整個 Spec 再多看幾次，並釐清出應該進行的步驟（三步驟），並研究助教提供的 visitor pattern 的 example code，才知道應該如何開始做這份 Project。

## Feedback to T.A.s

> 很感謝助教在這份 Project 中清楚定義了每一個 Node 應該是什麼樣子，應該存放哪些資料，也在 hpp 與 cpp 檔案中，提供了很多範例與提示。更重要的，是在 AstDumper.cpp 中清楚提供 visitor pattern 實作的樣子。如果沒有這些充足的資訊，我一定會卡非常非常久。
