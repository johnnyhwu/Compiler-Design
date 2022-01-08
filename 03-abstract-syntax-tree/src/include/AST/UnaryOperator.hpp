#ifndef __AST_UNARY_OPERATOR_NODE_H
#define __AST_UNARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"

class UnaryOperatorNode : public ExpressionNode {
  public:
    UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      const char* _operator_name,
                      AstNode* _right_expression_node
                      /* TODO: operator, expression */);
    ~UnaryOperatorNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  public:
    // TODO: operator, expression
    std::string operator_name;
    AstNode* right_expression_node;
};

#endif
