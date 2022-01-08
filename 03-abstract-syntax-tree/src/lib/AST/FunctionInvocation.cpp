#include "AST/FunctionInvocation.hpp"
#include "AST/AstDumper.hpp"

// TODO
FunctionInvocationNode::FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char* _function_name,
                           std::vector<AstNode*> * _expression_node_list)
    : ExpressionNode{line, col},
      function_name(_function_name),
      expression_node_list(_expression_node_list)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionInvocationNode::print() {}

void FunctionInvocationNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void FunctionInvocationNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    if(expression_node_list) {
        for (auto& expression_node : *expression_node_list) {
            expression_node->accept(ast_dumper);
        }
    }
    
}
