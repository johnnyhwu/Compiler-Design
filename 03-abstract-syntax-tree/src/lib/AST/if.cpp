#include "AST/if.hpp"
#include "AST/AstDumper.hpp"

// TODO
IfNode::IfNode(const uint32_t line, const uint32_t col,
           AstNode* _exp_node,
           AstNode* _compound_node1,
           AstNode* _compound_node2)
    : AstNode{line, col},
      exp_node(_exp_node),
      compound_node1(_compound_node1),
      compound_node2(_compound_node2)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void IfNode::print() {}

void IfNode::visitChildNodes(AstDumper & p_visitor) {
    // TODO
    exp_node->accept(p_visitor);
    compound_node1->accept(p_visitor);
    if (compound_node2) {
        compound_node2->accept(p_visitor);
    }
}

void IfNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}
