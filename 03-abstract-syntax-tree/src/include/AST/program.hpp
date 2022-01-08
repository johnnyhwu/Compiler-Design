#ifndef __AST_PROGRAM_NODE_H
#define __AST_PROGRAM_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

#include <vector>

class ProgramNode : public AstNode {
  public:
    ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *_return_type,
                std::vector<AstNode*> *_declaration_node_lst,
                std::vector<AstNode*> *_function_node_lst,
                AstNode* _compound_statement_node
                /* TODO: return type, declarations, functions,
                 *       compound statement */);

    ~ProgramNode() = default;

    // visitor pattern version: const char *getNameCString() const; 
    const char *getNameCString() const;
    
    void visitChildNodes(AstDumper & p_visitor);

    void print() override;
    void accept(AstDumper & ast_dumper) override;

    


  public:
    const std::string name;

    // TODO: return type, declarations, functions, compound statement
    const std::string return_type;
    std::vector<AstNode*> *declaration_node_lst;
    std::vector<AstNode*> *function_node_lst;
    AstNode* compound_statement_node;
};

#endif
