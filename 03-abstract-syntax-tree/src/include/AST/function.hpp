#ifndef __AST_FUNCTION_NODE_H
#define __AST_FUNCTION_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include <string>
#include <vector>

class FunctionNode : public AstNode {
  public:
    FunctionNode(const uint32_t line, const uint32_t col,
                 const char* _function_name, std::vector<AstNode*> *_declaration_node_list,
                 const char* _return_type_name
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);

    FunctionNode(const uint32_t line, const uint32_t col,
                 const char* _function_name, std::vector<AstNode*> *_declaration_node_list,
                 const char* _return_type_name, AstNode* _compound_statement_node
                 /* TODO: name, declarations, return type,
                  *       compound statement (optional) */);

    ~FunctionNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper & ast_dumper) override;
    std::string getParameterType();
    std::string getFunctionName();

  private:
    // TODO: name, declarations, return type, compound statement
    std::string function_name;
    std::vector<AstNode*> *declaration_node_list;
    std::string return_type_name;
    AstNode* compound_statement_node;
};

#endif
