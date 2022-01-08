#include "AST/BinaryOperator.hpp"
#include "AST/AstDumper.hpp"

// TODO
BinaryOperatorNode::BinaryOperatorNode(const uint32_t line, const uint32_t col,
                       const char* _operator_name, AstNode* _left_expression_node,
                       AstNode* _right_expression_node)
    : ExpressionNode{line, col},
      operator_name(_operator_name),
      left_expresseion_node(_left_expression_node),
      right_expression_node(_right_expression_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void BinaryOperatorNode::print() {}

void BinaryOperatorNode::accept(AstDumper& ast_dumper)
{ // visitor pattern version
    ast_dumper.visit(*this);
}

void BinaryOperatorNode::visitChildNodes(AstDumper & ast_dumper) {
    left_expresseion_node->accept(ast_dumper);
    right_expression_node->accept(ast_dumper);
    // TODO
}
