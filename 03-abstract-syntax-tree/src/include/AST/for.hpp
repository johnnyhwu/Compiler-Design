#ifndef __AST_FOR_NODE_H
#define __AST_FOR_NODE_H

#include "AST/ast.hpp"
#include "AST/AstDumper.hpp"

class ForNode : public AstNode {
  public:
    ForNode(const uint32_t line, const uint32_t col, AstNode* _declaration,
        AstNode* _assignment, AstNode* _expression,
        AstNode* _compound_statement
            /* TODO: declaration, assignment, expression,
             *       compound statement */);
    ~ForNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  private:
    // TODO: declaration, assignment, expression, compound statement
    AstNode* declaration;
    AstNode* assignment;
    AstNode* expression;
    AstNode* compound_statement;
};

#endif
