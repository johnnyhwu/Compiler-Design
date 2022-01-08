#include "AST/decl.hpp"
#include "AST/AstDumper.hpp"
#include "AST/variable.hpp"

// TODO
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
             std::vector<VariableNode*> *_variable_node_list)
    : AstNode{line, col},
      variable_node_list(_variable_node_list)

    {

    }

// TODO
//DeclNode::DeclNode(const uint32_t line, const uint32_t col)
//    : AstNode{line, col} {}
DeclNode::DeclNode(const uint32_t line, const uint32_t col,
    char* identifiers,
    char* type)
    : AstNode{ line, col }
{
    variable_node_list = new std::vector<VariableNode*>;

    VariableNode* node = new VariableNode(line, col, identifiers, type, NULL);
    variable_node_list->push_back(node);
}

std::vector<std::string> DeclNode::getVariableInfo() {
    std::vector<std::string> output_vec;
    for(auto & var_node: *variable_node_list) {
        output_vec.push_back(var_node->getTypeName());
    }
    return output_vec;
}

// TODO: You may use code snippets in AstDumper.cpp
void DeclNode::print() {
    std::printf("declaration <line: %u, col: %u>\n", getLocation().line,
                getLocation().col);
}

void DeclNode::accept(AstDumper & ast_dumper) {
    ast_dumper.visit(*this);
}

void DeclNode::visitChildNodes(AstDumper & ast_dumper) {
     // TODO
     for (auto & variable_node : *variable_node_list) {
         variable_node->accept(ast_dumper);
     }
}
