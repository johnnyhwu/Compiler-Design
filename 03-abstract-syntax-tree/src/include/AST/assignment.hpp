#ifndef __AST_ASSIGNMENT_NODE_H
#define __AST_ASSIGNMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

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

#endif
