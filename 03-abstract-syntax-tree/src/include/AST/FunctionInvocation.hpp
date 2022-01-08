#ifndef __AST_FUNCTION_INVOCATION_NODE_H
#define __AST_FUNCTION_INVOCATION_NODE_H

#include "AST/expression.hpp"
#include "AST/AstDumper.hpp"
#include <vector>

class FunctionInvocationNode : public ExpressionNode {
  public:
    FunctionInvocationNode(const uint32_t line, const uint32_t col,
                           const char* _function_name,
                           std::vector<AstNode*> * _expression_node_list
                           /* TODO: function name, expressions */);
    ~FunctionInvocationNode() = default;

    const char *getNameCString() const;

    void visitChildNodes(AstDumper & ast_dumper);

    void print() override;
    void accept(AstDumper& adt_node) override;

  public:
    // TODO: function name, expressions
    std::string function_name;
    std::vector<AstNode*> * expression_node_list;

};

#endif
