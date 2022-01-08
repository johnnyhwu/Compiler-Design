#include "AST/UnaryOperator.hpp"
#include "AST/AstDumper.hpp"

// TODO
UnaryOperatorNode::UnaryOperatorNode(const uint32_t line, const uint32_t col,
                      const char* _operator_name,
                      AstNode* _right_expression_node)
    : ExpressionNode{line, col},
      operator_name(_operator_name),
      right_expression_node(_right_expression_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void UnaryOperatorNode::print() {}

void UnaryOperatorNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void UnaryOperatorNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    right_expression_node->accept(ast_dumper);
}
