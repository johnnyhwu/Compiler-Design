#include "AST/while.hpp"
#include "AST/AstDumper.hpp"

// TODO
WhileNode::WhileNode(const uint32_t line, const uint32_t col,
              AstNode* _expression_node,
              AstNode* _compound_node)
    : AstNode{line, col},
      expression_node(_expression_node),
      compound_node(_compound_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void WhileNode::print() {}

void WhileNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void WhileNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    expression_node->accept(ast_dumper);
    compound_node->accept(ast_dumper);
}
