#ifndef __AST_BINARY_OPERATOR_NODE_H
#define __AST_BINARY_OPERATOR_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"

#include <memory>

class BinaryOperatorNode : public ExpressionNode {
  public:
    BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       const char* _operator_name, AstNode* _left_expression_node,
                       AstNode* _right_expression_node
                       /* TODO: operator, expressions */);
    ~BinaryOperatorNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  public:
    // TODO: operator, expressions
    std::string operator_name;
    AstNode* left_expresseion_node;
    AstNode* right_expression_node;
};

#endif
