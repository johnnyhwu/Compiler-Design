#ifndef __AST_RETURN_NODE_H
#define __AST_RETURN_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

class ReturnNode : public AstNode {
  public:
    ReturnNode(const uint32_t line, const uint32_t col, AstNode* _expression
               /* TODO: expression */);
    ~ReturnNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: expression
    AstNode* expression;
};

#endif
