#pragma once
#include <string>

class SymbolEntry {
    public:
        SymbolEntry(std::string _name, std::string _kind, int _level, std::string _type, std::string _attribute);
        SymbolEntry();
        ~SymbolEntry();
        
        std::string name;
        std::string kind;
        int level;
        std::string type;
        std::string attribute;
};