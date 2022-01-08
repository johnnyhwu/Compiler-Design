#ifndef __AST_WHILE_NODE_H
#define __AST_WHILE_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

class WhileNode : public AstNode {
  public:
    WhileNode(const uint32_t line, const uint32_t col,
              AstNode* _expression_node,
              AstNode* _compound_node
              /* TODO: expression, compound statement */);
    ~WhileNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: expression, compound statement
    AstNode* expression_node;
    AstNode* compound_node;
};

#endif
