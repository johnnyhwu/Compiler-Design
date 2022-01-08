#include "sema/SymbolTable.hpp"
#include "sema/SymbolEntry.hpp"

SymbolTable::SymbolTable(/* args */)
{

}


SymbolTable::~SymbolTable()
{
}

void SymbolTable::addEntry(SymbolEntry & entry) {
    table.push_back(entry);
}