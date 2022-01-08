#include "sema/SymbolEntry.hpp"
#include <string>

SymbolEntry::SymbolEntry(std::string _name, std::string _kind, int _level, std::string _type, std::string _attribute) {
    name = _name;
    kind = _kind;
    type = _type;
    attribute = _attribute;
    level = _level;
}

SymbolEntry::SymbolEntry() {
    
}

SymbolEntry::~SymbolEntry() {

}