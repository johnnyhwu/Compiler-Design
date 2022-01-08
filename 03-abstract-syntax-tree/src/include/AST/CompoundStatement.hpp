#ifndef __AST_COMPOUND_STATEMENT_NODE_H
#define __AST_COMPOUND_STATEMENT_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"
#include <vector>

class CompoundStatementNode : public AstNode {
  public:
    CompoundStatementNode(const uint32_t line, const uint32_t col,
                          std::vector<AstNode*> * _declaration_node_list,
                          std::vector<AstNode*> * _statement_node_list
                          /* TODO: declarations, statements */);
    ~CompoundStatementNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  public:
    // TODO: declarations, statements
    std::vector<AstNode*> * declaration_node_list;
    std::vector<AstNode*> * statement_node_list;
};

#endif
