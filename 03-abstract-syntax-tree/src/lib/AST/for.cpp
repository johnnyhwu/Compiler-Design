#include "AST/for.hpp"
#include "AST/AstDumper.hpp"

// TODO
ForNode::ForNode(const uint32_t line, const uint32_t col,
    AstNode* _declaration,
    AstNode* _assignment,
    AstNode* _expression,
    AstNode* _compound_statement)
    : AstNode{ line, col }
    , declaration(_declaration)
    , assignment(_assignment)
    , expression(_expression)
    , compound_statement(_compound_statement)
{
}

// TODO: You may use code snippets in AstDumper.cpp
void ForNode::print() {
    std::printf("for statement <line: %u, col: %u>\n", getLocation().line,
                getLocation().col);
}

void ForNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void ForNode::visitChildNodes(AstDumper& p_visitor)
{ // visitor pattern version

    if(declaration) {
        declaration->accept(p_visitor);
    }

    if(assignment) {
        assignment->accept(p_visitor);
    }
    
    if(expression) {
        expression->accept(p_visitor);
    }

    if(compound_statement) {
        compound_statement->accept(p_visitor);
    }
}
