#ifndef __AST_READ_NODE_H
#define __AST_READ_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

class ReadNode : public AstNode {
  public:
    ReadNode(const uint32_t line, const uint32_t col, AstNode* _ref_node
             /* TODO: variable reference */);
    ~ReadNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: variable reference
    AstNode* ref_node;
};

#endif
