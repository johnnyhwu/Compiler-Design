#pragma once
#include <vector>
#include "sema/SymbolEntry.hpp"

class SymbolTable
{
    public:
        SymbolTable(/* args */);
        ~SymbolTable();

        void addEntry(SymbolEntry & entry);

        std::vector<SymbolEntry> table;
};




