#ifndef __AST_PRINT_NODE_H
#define __AST_PRINT_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

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

#endif
