#include "AST/ConstantValue.hpp"

// TODO
ConstantValueNode::ConstantValueNode(const uint32_t line, const uint32_t col,
                      std::string _constant_value, bool _is_float)
    : ExpressionNode{line, col},
      constant_value(_constant_value),
      is_float(_is_float)
     {

     }

// TODO: You may use code snippets in AstDumper.cpp
void ConstantValueNode::print() {
  if(is_float == false) {
        std::printf("constant <line: %u, col: %u> %s\n",
                getLocation().line,
                getLocation().col,
                constant_value.c_str());
    } else {
        std::printf("constant <line: %u, col: %u> %f\n",
                getLocation().line,
                getLocation().col,
                atof(constant_value.c_str()));
        
    }
}

void ConstantValueNode::accept(AstDumper & ast_dumper) {
    ast_dumper.visit(*this);
}
