#ifndef __AST_DECL_NODE_H
#define __AST_DECL_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include "AST/variable.hpp"
#include <vector>
#include <string>

class DeclNode : public AstNode {
  public:
    // variable declaration
    DeclNode(const uint32_t line, const uint32_t col,
             std::vector<VariableNode*> *_variable_node_list
             /* TODO: identifiers, type */);
    
    DeclNode(const uint32_t line, const uint32_t col, char* identifiers,
        char* type);

    // constant variable declaration
    // DeclNode(const uint32_t, const uint32_t col
    //         /* TODO: identifiers, constant */);

    ~DeclNode() = default;
    void visitChildNodes(AstDumper & ast_dumper);
    void accept(AstDumper & ast_dumper) override;
    std::vector<std::string> getVariableInfo();

    void print() override;

  public:
    // TODO: variables
    std::vector<VariableNode*> *variable_node_list;
    
};

#endif
