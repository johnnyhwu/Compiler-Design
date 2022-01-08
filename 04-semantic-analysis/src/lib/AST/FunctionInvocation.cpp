#include "AST/FunctionInvocation.hpp"
#include "visitor/AstNodeVisitor.hpp"

FunctionInvocationNode::FunctionInvocationNode(const uint32_t line,
                                               const uint32_t col,
                                               const char *p_name,
                                               Exprs *p_arguments)
    : ExpressionNode{line, col}, name(p_name),
      arguments(std::move(*p_arguments)) {}

const char *FunctionInvocationNode::getNameCString() const {
    return name.c_str();
}

void FunctionInvocationNode::accept(AstNodeVisitor &p_visitor) {
    p_visitor.visit(*this);
}

void FunctionInvocationNode::visitChildNodes(AstNodeVisitor &p_visitor) {
    for (auto &argument : arguments) {
        argument->accept(p_visitor);
    }
}

int FunctionInvocationNode::getArgsSize() {
    return arguments.size();
}

void FunctionInvocationNode::visitChildNode(AstNodeVisitor &p_visitor, int idx) {
    int count = 0;
    for (auto &argument : arguments) {
        if(count == idx) {
            argument->accept(p_visitor);
            break;
        } else {
            count++;
        }
    }
}
