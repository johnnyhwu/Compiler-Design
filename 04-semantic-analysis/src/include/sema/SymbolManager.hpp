#pragma once
#include "visitor/AstNodeVisitor.hpp"
#include "sema/SymbolTable.hpp"
#include <stack>

class SymbolManager : public AstNodeVisitor {
    public:
        SymbolManager();
        ~SymbolManager();

        void pushTable(SymbolTable new_table);
        void popTable();

        std::stack<SymbolTable> tables;
};