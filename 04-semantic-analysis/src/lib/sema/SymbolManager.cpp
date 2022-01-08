#include "sema/SymbolManager.hpp"
#include "sema/SymbolTable.hpp"
#include "sema/SymbolEntry.hpp"
#include <string>
#include <cstdint>
#include <cstdio>

extern int32_t opt_table;

SymbolManager::SymbolManager() {

}


SymbolManager::~SymbolManager() {

}

void SymbolManager::pushTable(SymbolTable new_table){
    tables.push(new_table);
}
        
void SymbolManager::popTable(){
    tables.pop();
}
