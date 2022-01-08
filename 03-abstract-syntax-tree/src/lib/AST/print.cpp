#include "AST/print.hpp"
#include "AST/AstDumper.hpp"

// TODO
PrintNode::PrintNode(const uint32_t line, const uint32_t col, AstNode* _expression_node)
    : AstNode{line, col},
      expression_node(_expression_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void PrintNode::print() {}

void PrintNode::accept(AstDumper& ast_dumper)
{ // visitor pattern version
    ast_dumper.visit(*this);
}

void PrintNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    expression_node->accept(ast_dumper);
}
