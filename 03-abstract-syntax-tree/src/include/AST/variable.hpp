#ifndef __AST_VARIABLE_NODE_H
#define __AST_VARIABLE_NODE_H

#include "AST/ast.hpp"
#include <string>

class VariableNode : public AstNode {
  public:
    VariableNode(const uint32_t line, const uint32_t col,
                 const char* _variable_name,
                 const char* _type_name,
                 AstNode* _constant_value_node
                 /* TODO: variable name, type, constant value */);
    ~VariableNode() = default;

    void visitChildNodes(AstDumper & p_visitor);

    void print() override;
    void accept(AstDumper & ast_dumper) override;
    std::string getVariableName();
    std::string getTypeName();
    void setTypeName(std::string _type_name);
    void setConstantValueNode(AstNode* node);

  private:
    // TODO: variable name, type, constant value
    std::string variable_name;
    std::string type_name;
    AstNode* constant_value_node;
};

#endif
