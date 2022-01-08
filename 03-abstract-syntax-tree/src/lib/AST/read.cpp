#include "AST/read.hpp"
#include "AST/AstDumper.hpp"

// TODO
ReadNode::ReadNode(const uint32_t line, const uint32_t col, AstNode* _ref_node)
    : AstNode{line, col},
    ref_node(_ref_node)
    {}

// TODO: You may use code snippets in AstDumper.cpp
void ReadNode::print() {
    std::printf("read statement <line: %u, col: %u>\n",
                getLocation().line, getLocation().col);
}

void ReadNode::accept(AstDumper& p_visitor)
{ // visitor pattern version
    p_visitor.visit(*this);
}

void ReadNode::visitChildNodes(AstDumper & ast_dumper) {
    // TODO
    ref_node->accept(ast_dumper);
}
