#include "AST/program.hpp"

// TODO
ProgramNode::ProgramNode(const uint32_t line, const uint32_t col,
                const char *p_name, const char *_return_type,
                std::vector<AstNode*> *_declaration_node_lst,
                std::vector<AstNode*> *_function_node_lst,
                AstNode* _compound_statement_node) 
    : AstNode{line, col},
      name(p_name), 
      return_type(_return_type), 
      declaration_node_lst(_declaration_node_lst),
      function_node_lst(_function_node_lst),
      compound_statement_node(_compound_statement_node) 
      
      {
        
      }

// visitor pattern version: const char *ProgramNode::getNameCString() const { return name.c_str(); }
const char *ProgramNode::getNameCString() const { return name.c_str(); }

void ProgramNode::print() {
    // TODO
    // outputIndentationSpace();
    

    std::printf("program <line: %u, col: %u> %s %s\n",
                location.line, location.col,
                name.c_str(), "void");

    // TODO
    // incrementIndentation();
    

    // TODO
    // visitChildNodes();
    

    // TODO
    // decrementIndentation();
    
}

void ProgramNode::accept(AstDumper& ast_dumper) {
    ast_dumper.visit(*this);
}

void ProgramNode::visitChildNodes(AstDumper & ast_dumper) { // visitor pattern version
     /* TODO
      *
      *
      * for (auto &decl : var_decls) {
      *     decl->accept(p_visitor);
      * }
      *
      * // functions
      *
      * body->accept(p_visitor);
      */

    if(declaration_node_lst) {
         for (auto & decl : *declaration_node_lst) {
            decl->accept(ast_dumper);
        }
    }

    if(function_node_lst) {
         for (auto & func : *function_node_lst) {
         func->accept(ast_dumper);
        }
    }
     
    if(compound_statement_node) {
        compound_statement_node->accept(ast_dumper);
    }
     
}


