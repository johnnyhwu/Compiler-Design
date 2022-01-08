#include "AST/variable.hpp"
#include "AST/AstDumper.hpp"
#include <string>

// TODO
VariableNode::VariableNode(const uint32_t line, const uint32_t col,
                 const char* _variable_name,
                 const char* _type_name,
                 AstNode* _constant_value_node)
    : AstNode{line, col},
      variable_name(_variable_name),
      type_name(_type_name),
      constant_value_node(_constant_value_node)
    {

    }

std::string VariableNode::getVariableName() {
    return variable_name;
}

std::string VariableNode::getTypeName() {
    return type_name;
}

void VariableNode::setTypeName(std::string _type_name) {
    type_name = _type_name;
}

void VariableNode::setConstantValueNode(AstNode* node) {
    constant_value_node = node;
}

// TODO: You may use code snippets in AstDumper.cpp
void VariableNode::print() {
    
    std::printf("variable <line: %u, col: %u> %s %s\n",
                getLocation().line, getLocation().col,
                getVariableName().c_str(), getTypeName().c_str());
}

void VariableNode::accept(AstDumper & ast_dumper) {
    ast_dumper.visit(*this);
}

void VariableNode::visitChildNodes(AstDumper & p_visitor) {
    // TODO
    if (constant_value_node)
    {
        constant_value_node->accept(p_visitor);
    }
    
}
