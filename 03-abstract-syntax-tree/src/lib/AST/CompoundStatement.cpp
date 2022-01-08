#include "AST/CompoundStatement.hpp"
#include "AST/AstDumper.hpp"

// TODO
CompoundStatementNode::CompoundStatementNode(const uint32_t line, const uint32_t col,
                          std::vector<AstNode*> * _declaration_node_list,
                          std::vector<AstNode*> * _statement_node_list)
    : AstNode{line, col},
      declaration_node_list(_declaration_node_list),
      statement_node_list(_statement_node_list)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void CompoundStatementNode::print() {}

void CompoundStatementNode::accept(AstDumper & ast_dumper)
{
    ast_dumper.visit(*this);
}

void CompoundStatementNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO

    if(declaration_node_list) {
        for (auto& decl : *declaration_node_list) {
            decl->accept(ast_dumper);
        }
    }

    if(statement_node_list) {
        for (auto& decl : *statement_node_list) {
            decl->accept(ast_dumper);
        }
    }
    
}
