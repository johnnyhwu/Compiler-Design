#include "AST/return.hpp"
#include "AST/AstDumper.hpp"

// TODO
ReturnNode::ReturnNode(const uint32_t line, const uint32_t col, AstNode* _expression)
    : AstNode{line, col} 
    , expression(_expression)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void ReturnNode::print() {}

void ReturnNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void ReturnNode::visitChildNodes(AstDumper& p_visitor)
{ // visitor pattern version
    expression->accept(p_visitor);
}