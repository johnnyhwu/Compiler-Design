#ifndef __AST_CONSTANT_VALUE_NODE_H
#define __AST_CONSTANT_VALUE_NODE_H

#include "AST/expression.hpp"
#include <string>

class ConstantValueNode : public ExpressionNode {
  public:
    ConstantValueNode(const uint32_t line, const uint32_t col,
                      std::string _constant_value,
                      bool _is_float
                      /* TODO: constant value */);
    ~ConstantValueNode() = default;

    void print() override;

    void accept(AstDumper & ast_dumper) override;

  public:
    // TODO: constant value
    std::string constant_value;
    bool is_float;
};

#endif
