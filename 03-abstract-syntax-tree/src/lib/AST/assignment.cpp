#include "AST/assignment.hpp"
#include "AST/AstDumper.hpp"

// TODO
AssignmentNode::AssignmentNode(const uint32_t line, const uint32_t col,
                   AstNode* _variable_ref_node,
                   AstNode* _expression_node)
    : AstNode{line, col},
      variable_ref_node(_variable_ref_node),
      expression_node(_expression_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void AssignmentNode::print() {
    std::printf("assignment statement <line: %u, col: %u>\n",
                getLocation().line,
                getLocation().col);
}

void AssignmentNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void AssignmentNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    
    variable_ref_node->accept(ast_dumper); 
    if (expression_node) {
        expression_node->accept(ast_dumper);
    }
}
