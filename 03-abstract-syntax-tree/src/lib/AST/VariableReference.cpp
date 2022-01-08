#include "AST/VariableReference.hpp"
#include "AST/AstDumper.hpp"

// TODO
VariableReferenceNode::VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char* _name, std::vector<AstNode*> * _expression_node_list)
    : ExpressionNode{line, col},
    name(_name),
    expression_node_list(_expression_node_list)
    {}

// TODO
// VariableReferenceNode::VariableReferenceNode(const uint32_t line,
//                                              const uint32_t col)
//     : ExpressionNode{line, col} {}

// TODO: You may use code snippets in AstDumper.cpp
void VariableReferenceNode::print() {
    std::printf("variable reference <line: %u, col: %u> %s\n",
                getLocation().line,
                getLocation().col,
                "TODO");
}

void VariableReferenceNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void VariableReferenceNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    if(expression_node_list) {
        for (auto& decl : *expression_node_list) {
            decl->accept(ast_dumper);
        }
    }
    
}
