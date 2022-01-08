#include "AST/function.hpp"
#include "AST/AstDumper.hpp"
#include "AST/decl.hpp"

// TODO

FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                 const char* _function_name, std::vector<AstNode*> *_declaration_node_list,
                 const char* _return_type_name)
    : AstNode{line, col},
      function_name(_function_name),
      declaration_node_list(_declaration_node_list),
      return_type_name(_return_type_name),
      compound_statement_node(NULL)
     {

     }


FunctionNode::FunctionNode(const uint32_t line, const uint32_t col,
                 const char* _function_name, std::vector<AstNode*> *_declaration_node_list,
                 const char* _return_type_name, AstNode* _compound_statement_node)
    : AstNode{line, col},
      function_name(_function_name),
      declaration_node_list(_declaration_node_list),
      return_type_name(_return_type_name),
      compound_statement_node(_compound_statement_node)
     {

     }

std::string FunctionNode::getParameterType() {
    std::string output = return_type_name;
    output += " ";

    if(declaration_node_list) {
        std::vector<std::string> params_type;
        DeclNode* ptr;

        for(auto & decl_node: *declaration_node_list) {
            ptr = (DeclNode*)decl_node;
            std::vector<std::string> temp =  ptr->getVariableInfo();
            for(int i=0; i<temp.size(); i++) {
                params_type.push_back(temp[i]);
            }
        }

        output += "(";
        output += params_type[0];
        for(int i=1; i<params_type.size(); i++) {
            output += ", ";
            output += params_type[i];
        }

        output += ")";
    } else {
        output += "()";
    }
    
    

    return output;

}

std::string FunctionNode::getFunctionName() {
    return function_name;
}

// TODO: You may use code snippets in AstDumper.cpp
void FunctionNode::print() {}

void FunctionNode::accept(AstDumper & ast_dumper) {
    ast_dumper.visit(*this);
}

void FunctionNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    if(declaration_node_list) {
        for (auto& decl : *declaration_node_list) {
            decl->accept(ast_dumper);
        }
    }

    if(compound_statement_node) {
        compound_statement_node->accept(ast_dumper);
    }
    
}
