#ifndef __AST_VARIABLE_REFERENCE_NODE_H
#define __AST_VARIABLE_REFERENCE_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"
#include <vector>

class VariableReferenceNode : public ExpressionNode {
  public:
    // normal reference
    VariableReferenceNode(const uint32_t line, const uint32_t col,
                          const char* _name, std::vector<AstNode*> * _expression_node_list
                          /* TODO: name */);
    // array reference
    // VariableReferenceNode(const uint32_t line, const uint32_t col
    //                       /* TODO: name, expressions */);
    ~VariableReferenceNode() = default;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  public:
    // TODO: variable name, expressions
    std::string name;
    std::vector<AstNode*> * expression_node_list;
};

#endif
