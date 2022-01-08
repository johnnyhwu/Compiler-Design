#ifndef __AST_IF_NODE_H
#define __AST_IF_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

class IfNode : public AstNode {
  public:
    IfNode(const uint32_t line, const uint32_t col,
           AstNode* _exp_node,
           AstNode* _compound_node1,
           AstNode* _compound_node2
           /* TODO: expression, compound statement, compound statement */);
    ~IfNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: expression, compound statement, compound statement
    AstNode* exp_node;
    AstNode* compound_node1;
    AstNode* compound_node2;
};

#endif
