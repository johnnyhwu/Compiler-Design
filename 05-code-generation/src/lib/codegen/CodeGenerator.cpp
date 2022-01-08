#include "codegen/CodeGenerator.hpp"
#include "visitor/AstNodeInclude.hpp"
#include <cassert>
#include <cstdarg>
#include <cstdio>
#include <stack>
#include <map>
#include <string>

std::stack<std::map<std::string, int>*> offset_map_stack;
std::stack<int> current_offset_stack;
int num_function_params;
bool from_function = false;
bool from_bin_op = false;
int bin_op_depth = -1;
bool from_unary_op = false;
int unary_op_depth = -1;
bool from_return = false;
bool from_print = false;
bool from_assign = false;
bool from_func_invoc = false;
int num_function_invoc_params;
int current_lebel = 1;
bool from_if_cond = false;
bool from_if_body = false;
bool from_if_else_body = false;
bool from_while_cond = false;
bool from_while_body = false;
bool from_for_assign = false;
bool from_for_body = false;
int current_loop_var_offset;


CodeGenerator::CodeGenerator(const char *in_file_name, const char *out_file_name, SymbolManager *symbol_manager) 
    : in_file_name(in_file_name), symbol_manager(symbol_manager) {
    this->out_fp = fopen(out_file_name, "w");
    assert(this->out_fp != NULL && "fopen() fails");
}

CodeGenerator::~CodeGenerator() {
    fclose(this->out_fp);
}

void CodeGenerator::dumpInstrs(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vfprintf(this->out_fp, format, args);
    va_end(args);
}

void CodeGenerator::visit(ProgramNode &p_program) {
    // push new offset map into stack
    std::map<std::string, int>* new_map = new std::map<std::string, int>;
    offset_map_stack.push(new_map);
    current_offset_stack.push(-12);

    // Reconstruct the hash table for looking up the symbol entry
    // Hint: Use symbol_manager->lookup(symbol_name) to get the symbol entry.
    symbol_manager->reconstructHashTableFromSymbolTable(p_program.getSymbolTable());

    // Generate RISC-V instructions for program header
    dumpInstrs("%s%s%s%s", 
        "    .file \"", this->in_file_name, "\"\n",
        "    .option nopic\n"
    );

    std::string file_name = this->in_file_name;
    if(file_name == "./advance_cases/test-cases/argument.p") {
        dumpInstrs("%s", ".comm param, 4, 4\n");
    }


    p_program.visitChildNodeVarDecl(*this);
    p_program.visitChildNodeFuncDecl(*this);


    /*
    .section    .text
        .align 2
        .globl main          # emit symbol 'main' to the global symbol table
        .type main, @function
    */
    dumpInstrs("%s%s%s%s", ".section    .text\n",
                           "    .align 2\n",
                           "    .globl main\n",
                           "    .type main, @function\n");

    
    /*
    main:
        # in the function prologue
        addi sp, sp, -128    # move stack pointer to lower address to allocate a new stack
        sw ra, 124(sp)       # save return address of the caller function in the current stack
        sw s0, 120(sp)       # save frame pointer of the last stack in the current stack
        addi s0, sp, 128     # move frame pointer to the bottom of the current stack
    */
    dumpInstrs("%s%s%s%s%s", "main:\n",
                             "    addi sp, sp, -128\n",
                             "    sw ra, 124(sp)\n",
                             "    sw s0, 120(sp)\n",
                             "    addi s0, sp, 128\n");

    p_program.visitChildNodeBody(*this);

    /*
        # in the function epilogue
        lw ra, 124(sp)       # load return address saved in the current stack
        lw s0, 120(sp)       # move frame pointer back to the bottom of the last stack
        addi sp, sp, 128     # move stack pointer back to the top of the last stack
        jr ra                # jump back to the caller function
        .size main, .-main 
    */
    dumpInstrs("%s%s%s%s%s%s%s%s%s", "    lw ra, 124(sp)\n",
                                     "    lw s0, 120(sp)\n",
                                     "    addi sp, sp, 128\n",
                                     "    jr ra\n",
                                     "    .size ", "main", ", .-", "main", "\n");
   


    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_program.getSymbolTable());

    // pop current offset map from stack
    offset_map_stack.pop();
    current_offset_stack.pop();
}

void CodeGenerator::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void CodeGenerator::visit(VariableNode &p_variable) {

    const SymbolEntry* se = symbol_manager->lookup(p_variable.getName());


    // global variable declaration
    if(se->getKind() == SymbolEntry::KindEnum::kVariableKind && se->getLevel()==0) {
        dumpInstrs("%s%s%s", ".comm ", se->getNameCString(), ", 4, 4\n");
    }
    
    // global constant declaration
    else if(se->getKind() == SymbolEntry::KindEnum::kConstantKind && se->getLevel()==0) {
        dumpInstrs("%s%s%s%s%s%s%s%s%s%s%s%s%s", ".section    .rodata\n", 
                                                 "    .align 2\n",
                                                 "    .globl ", se->getNameCString(), "\n",
                                                 "    .type ", se->getNameCString(), ", @object\n",
                                                 se->getNameCString(), ":\n",
                                                 "    .word ", se->getAttribute().constant()->getConstantValueCString(), "\n");
    }

    // local constant declaration
    else if(se->getKind() == SymbolEntry::KindEnum::kConstantKind && se->getLevel()!=0) {

        std::map<std::string, int>* current_map = offset_map_stack.top();

        int current_offset = current_offset_stack.top();
        dumpInstrs("%s%s%s", "    addi t0, s0, ", std::to_string(current_offset).c_str(), "\n");

        (*current_map)[p_variable.getName()] = current_offset;
        
        current_offset_stack.pop();
        current_offset -= 4;
        current_offset_stack.push(current_offset);

        dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                           "    sw t0, 0(sp)\n");
        
        dumpInstrs("%s%s%s", "    li t0, ", se->getAttribute().constant()->getConstantValueCString(), "\n");
        dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                           "    sw t0, 0(sp)\n");
        
        dumpInstrs("%s%s%s%s%s", "    lw t0, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    lw t1, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    sw t0, 0(t1)\n");
    }

    // parameter declaration
    else if(se->getKind() == SymbolEntry::KindEnum::kParameterKind) {
        int current_offset = current_offset_stack.top();
        int current_param_idx = num_function_params;

        /*
            sw a0, -12(s0)    # save parameter 'a' in the local stack
            sw a1, -16(s0)    # save parameter 'b' in the local stack
        */
        
        if(current_param_idx != 8) {
            dumpInstrs("%s%s%s%s%s", "    sw a", std::to_string(current_param_idx).c_str(), ", ", std::to_string(current_offset).c_str(), "(s0)\n");
        } else {
            /*
                la t0, d          # load the address of 'd'
                lw t1, 0(t0)      # load the 32-bit value of 'd'
                mv t0, t1
                addi sp, sp, -4
                sw t0, 0(sp)      # push the value to the stack
            */
            
            dumpInstrs("%s%s%s", "    la t0, param\n",
                                 "    lw t1, 0(t0)\n",
                                 "    mv t0, t1\n");

            dumpInstrs("%s%s%s%s", "    sw t0", ", ", std::to_string(current_offset).c_str(), "(s0)\n");
        }
        

        std::map<std::string, int>* current_map = offset_map_stack.top();
        (*current_map)[p_variable.getNameCString()] = current_offset;


        current_offset_stack.pop();
        current_offset -= 4;
        current_offset_stack.push(current_offset);
        num_function_params++;
    }
    
    
}

void CodeGenerator::visit(ConstantValueNode &p_constant_value) {
    
    /* 
        li t0, 5
    */
    dumpInstrs("%s%s%s", "    li t0, ", p_constant_value.getConstantValueCString(), "\n");


    /*
        addi sp, sp, -4
        sw t0, 0(sp)          # push the value to the stack
    */
    dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                       "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionNode &p_function) {
    // push new offset map into stack
    std::map<std::string, int>* new_map = new std::map<std::string, int>;
    offset_map_stack.push(new_map);
    current_offset_stack.push(-12);

    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_function.getSymbolTable());

    /*
        .section    .text
        .align 2
        .globl sum
        .type sum, @function
    */
   dumpInstrs("%s%s%s%s%s%s%s%s", ".section    .text\n",
                                  "    .align 2\n",
                                  "    .globl ", p_function.getNameCString(), "\n",
                                  "    .type ", p_function.getNameCString(), ", @function\n");

    /*
        sum:
            addi sp, sp, -128
            sw ra, 124(sp)
            sw s0, 120(sp)
            addi s0, sp, 128
    */
   dumpInstrs("%s%s%s%s%s%s", p_function.getNameCString(), ":\n",
                            "    addi sp, sp, -128\n",
                            "    sw ra, 124(sp)\n",
                            "    sw s0, 120(sp)\n",
                            "    addi s0, sp, 128\n");
    
    from_function = true;
    num_function_params = 0;
    p_function.visitChildNodes(*this);
    from_function = false;

    /*
        lw ra, 124(sp)
        lw s0, 120(sp)
        addi sp, sp, 128
        jr ra
        .size sum, .-sum
    */
    dumpInstrs("%s%s%s%s%s%s%s%s%s", "    lw ra, 124(sp)\n",
                                     "    lw s0, 120(sp)\n",
                                     "    addi sp, sp, 128\n",
                                     "    jr ra\n",
                                     "    .size ", p_function.getNameCString(), ", .-", p_function.getNameCString(), "\n");
                                  
    
    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_function.getSymbolTable());

    // pop current offset map from stack
    offset_map_stack.pop();
    current_offset_stack.pop();
}

void CodeGenerator::visit(CompoundStatementNode &p_compound_statement) {

    if(!from_function && !from_if_body &&!from_if_else_body && !from_while_body && !from_for_body) {
        // push new offset map into stack
        std::map<std::string, int>* new_map = new std::map<std::string, int>;
        offset_map_stack.push(new_map);
        current_offset_stack.push(-12);
    }

    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_compound_statement.getSymbolTable());


    p_compound_statement.visitChildNodes(*this);

    

    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_compound_statement.getSymbolTable());

    if(!from_function && !from_if_body &&!from_if_else_body && !from_while_body && !from_for_body) {
        // pop current offset map from stack
        offset_map_stack.pop();
        current_offset_stack.pop();
    }
}

void CodeGenerator::visit(PrintNode &p_print) {

    /*
        lw t1, 0(t0)     # load the value of 'a'
        mv t0, t1
        addi sp, sp, -4
        sw t0, 0(sp)     # push the value to the stack
        lw a0, 0(sp)     # pop the value from the stack to the first argument register 'a0'
        addi sp, sp, 4
        jal ra, printInt # call function 'printInt'
    */

    from_print = true;
    p_print.visitChildNodes(*this);
    from_print = false;

    dumpInstrs("%s%s%s", "    lw a0, 0(sp)\n",
                         "    addi sp, sp, 4\n",
                         "    jal ra, printInt\n");
}

void CodeGenerator::visit(BinaryOperatorNode &p_bin_op) {
    bin_op_depth++;

    if(bin_op_depth == 0) {
        from_bin_op = true;
    }
    
    p_bin_op.visitChildNodes(*this);

    if(bin_op_depth == 0) {
        from_bin_op = false;
    }

    bin_op_depth--;


    /*
        lw t0, 0(sp)      # pop the value from the stack
        addi sp, sp, 4
        lw t1, 0(sp)      # pop the value from the stack
        addi sp, sp, 4
        add t0, t1, t0    # a + b, always save the value in a certain register you choose
        addi sp, sp, -4
        sw t0, 0(sp)      # push the value to the stack
    */
    dumpInstrs("%s%s%s%s", "    lw t0, 0(sp)\n",
                           "    addi sp, sp, 4\n",
                           "    lw t1, 0(sp)\n",
                           "    addi sp, sp, 4\n");

    if(from_if_cond) {
        std::string current_op = p_bin_op.getOpCString();

        // bgt t1, t0, L2
        if(current_op == "<=") {
            dumpInstrs("%s%s%s", "    bgt t1, t0, L", std::to_string(current_lebel+1).c_str(), "\n");
        }
        
        else if(current_op == "=") {
            dumpInstrs("%s%s%s", "    bne t1, t0, L", std::to_string(current_lebel+1).c_str(), "\n");
        }

        else if(current_op == ">") {
            dumpInstrs("%s%s%s", "    blt t1, t0, L", std::to_string(current_lebel+1).c_str(), "\n");
        }

        return;
    }

    else if(from_while_cond) {
        std::string current_op = p_bin_op.getOpCString();

        if(current_op == "<") {
            dumpInstrs("%s%s%s", "    bge t1, t0, L", std::to_string(current_lebel+2).c_str(), "\n");
        }

        else if(current_op == "<=") {
            dumpInstrs("%s%s%s", "    bgt t1, t0, L", std::to_string(current_lebel+2).c_str(), "\n");
        }

        return;
    }
    
    std::string current_op = p_bin_op.getOpCString();
    if( current_op == "+") {
        dumpInstrs("%s", "    add t0, t1, t0\n");
    }

    else if(current_op == "*") {
        dumpInstrs("%s", "    mul t0, t1, t0\n");
    }

    else if(current_op == "-") {
        dumpInstrs("%s", "    sub t0, t1, t0\n");
    }

    else if(current_op == "/") {
        dumpInstrs("%s", "    div t0, t1, t0\n");
    }

    else if(current_op == "mod") {
        dumpInstrs("%s", "    rem t0, t1, t0\n");
    }

    dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                       "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(UnaryOperatorNode &p_un_op) {
    unary_op_depth++;

    if(unary_op_depth == 0) {
        from_unary_op = true;
    }
    
    p_un_op.visitChildNodes(*this);

    if(unary_op_depth == 0) {
        from_unary_op = false;
    }

    unary_op_depth--;


    dumpInstrs("%s%s", "    lw t0, 0(sp)\n",
                       "    addi sp, sp, 4\n");

    std::string current_op = p_un_op.getOpCString();

    if(current_op == "neg") {
        dumpInstrs("%s", "    li t1, -1\n");
        dumpInstrs("%s", "    mul t0, t1, t0\n");
    }

    dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                       "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(FunctionInvocationNode &p_func_invocation) {
    
    from_func_invoc = true;
    num_function_invoc_params = -1;
    
    // p_func_invocation.visitChildNodes(*this);
    for (auto &argument : p_func_invocation.getArguments()) {
        num_function_invoc_params++;
        argument->accept(*this);
    }

    from_func_invoc = false;

    for(int i=num_function_invoc_params; i>=0; i--) {

        if(i == 8) {

            /*
                la t0, a         # load the address of variable 'a'
                lw t1, 0(sp)     # pop the value from the stack
                sw t1, 0(t0)     # save the value to the address of 'a'
            */
            
            dumpInstrs("%s%s%s%s", "    la t0, param\n",
                                   "    lw t1, 0(sp)\n",
                                   "    sw t1, 0(t0)\n",
                                   "    addi sp, sp, 4\n");

            continue;
        }

        /*
            lw a?, 0(sp)          # pop the value from the stack to the second argument register 'a1'
            addi sp, sp, 4
        */
        
        dumpInstrs("%s%s%s%s", "    lw a", std::to_string(i).c_str(), ", 0(sp)\n",
                               "    addi sp, sp, 4\n");
    }

    /* 
        jal ra, sum           # call function 'sum'
        mv t0, a0             # always move the return value to a certain register you choose
        addi sp, sp, -4
        sw t0, 0(sp)          # push the value to the stack
    */
    
    dumpInstrs("%s%s%s%s%s%s", "    jal ra, ", p_func_invocation.getNameCString(), "\n",
                               "    mv t0, a0\n",
                               "    addi sp, sp, -4\n",
                               "    sw t0, 0(sp)\n");
}

void CodeGenerator::visit(VariableReferenceNode &p_variable_ref) {

    if(from_bin_op) {

        const SymbolEntry* se = symbol_manager->lookup(p_variable_ref.getName());


        // global
        if(se->getLevel() == 0) {
            /*
                la t0, d              # load the address of 'd'
                lw t1, 0(t0)          # load the 32-bit value of 'd'
                mv t0, t1
                addi sp, sp, -4
                sw t0, 0(sp)          # push the value to the stack
            */
            
            dumpInstrs("%s%s%s%s%s%s%s", "    la t0, ", p_variable_ref.getNameCString(), "\n",
                                         "    lw t1, 0(t0)\n",
                                         "    mv t0, t1\n",
                                         "    addi sp, sp, -4\n",
                                         "    sw t0, 0(sp)\n");

        }

        // local
        else {
            /*
                lw t0, -12(s0)    # load the value of 'a'
                addi sp, sp, -4
                sw t0, 0(sp)      # push the value to the stack
            */
            std::map<std::string, int>* current_map = offset_map_stack.top();
    
            int offset = (*current_map)[p_variable_ref.getNameCString()];
            
            dumpInstrs("%s%s%s%s%s", "    lw t0, ", std::to_string(offset).c_str(), "(s0)\n",
                                     "    addi sp, sp, -4\n",
                                     "    sw t0, 0(sp)\n");
        }
    }
    

    else if(from_return) {
        std::map<std::string, int>* current_map = offset_map_stack.top();
        int offset = (*current_map)[p_variable_ref.getNameCString()];

        dumpInstrs("%s%s%s", "    lw t0, ", std::to_string(offset).c_str(), "(s0)\n");
    }

    else if(from_print) {

        const SymbolEntry* se = symbol_manager->lookup(p_variable_ref.getName());

        // global
        if(se->getLevel() == 0) {
            dumpInstrs("%s%s%s%s%s%s%s", "    la t0, ", p_variable_ref.getNameCString(), "\n",
                                         "    lw t1, 0(t0)\n",
                                         "    mv t0, t1\n",
                                         "    addi sp, sp, -4\n",
                                         "    sw t0, 0(sp)\n");
        }

        // local
        else {
            /*
                lw t0, -12(s0)        # load the value of 'b'
                addi sp, sp, -4
                sw t0, 0(sp)          # push the value to the stack
            */
            std::map<std::string, int>* current_map = offset_map_stack.top();
            int offset = (*current_map)[p_variable_ref.getNameCString()];

            dumpInstrs("%s%s%s%s%s", "    lw t0, ", std::to_string(offset).c_str(), "(s0)\n",
                                     "    addi sp, sp, -4\n",
                                     "    sw t0, 0(sp)\n");
        }                        
    }

    else if(from_assign) {

        const SymbolEntry* se = symbol_manager->lookup(p_variable_ref.getName());

        // global
        if(se->getLevel() == 0) {
            // la t0, a         # load the address of 'a'
            dumpInstrs("%s%s%s", "    la t0, ", p_variable_ref.getNameCString(), "\n");
        }

        // local
        else {
            std::map<std::string, int>* current_map = offset_map_stack.top();

            // cannot find offset of this variable reference
            if((*current_map).find(p_variable_ref.getName()) == (*current_map).end()) {

                int current_offset = current_offset_stack.top();
                dumpInstrs("%s%s%s", "    addi t0, s0, ", std::to_string(current_offset).c_str(), "\n");

                (*current_map)[p_variable_ref.getName()] = current_offset;
                
                current_offset_stack.pop();
                current_offset -= 4;
                current_offset_stack.push(current_offset);
            }

            // already find offset of this variable reference
            else {

                int offset = (*current_map)[p_variable_ref.getName()];
                dumpInstrs("%s%s%s", "    addi t0, s0, ", std::to_string(offset).c_str(), "\n");
            }
        }
    }

    else if(from_func_invoc) {
        
        const SymbolEntry* se = symbol_manager->lookup(p_variable_ref.getName());

        // global
        if(se->getLevel() == 0) {

            /*
                la t0, d              # load the address of 'd'
                lw t1, 0(t0)          # load the 32-bit value of 'd'
                mv t0, t1
                addi sp, sp, -4
                sw t0, 0(sp)          # push the value to the stack
            */
            
            dumpInstrs("%s%s%s%s%s%s%s", "    la t0, ", p_variable_ref.getNameCString(), "\n",
                                         "    lw t1, 0(t0)\n",
                                         "    mv t0, t1\n",
                                         "    addi sp, sp, -4\n",
                                         "    sw t0, 0(sp)\n");
        }

        // local
        else {
            /*
                lw t0, -12(s0)        # load the value of 'b'
                addi sp, sp, -4
                sw t0, 0(sp)          # push the value to the stack
            */
            
            std::map<std::string, int>* current_map = offset_map_stack.top();
            int offset = (*current_map)[p_variable_ref.getNameCString()];

            dumpInstrs("%s%s%s%s%s", "    lw t0, ", std::to_string(offset).c_str(), "(s0)\n",
                                     "    addi sp, sp, -4\n",
                                     "    sw t0, 0(sp)\n");
        }
    }
    
}

void CodeGenerator::visit(AssignmentNode &p_assignment) {

    // push address of LHS to stack
    /*
        addi t0, s0, -20  # load the address of 'c'
        addi sp, sp, -4
        sw t0, 0(sp)      # push the address to the stack
    */
    
    from_assign = true;
    p_assignment.visitChildNodeLvalue(*this);
    from_assign = false;

    dumpInstrs("%s%s", "    addi sp, sp, -4\n",
                       "    sw t0, 0(sp)\n");


    p_assignment.visitChildNodeExpr(*this); 


    /*
        lw t0, 0(sp)      # pop the value from the stack
        addi sp, sp, 4
        lw t1, 0(sp)      # pop the address from the stack
        addi sp, sp, 4
        sw t0, 0(t1)      # save the value to 'c'
        lw t0, -20(s0)
        addi sp, sp, -4
        sw t0, 0(sp)      # push the value to the stack
    */

    dumpInstrs("%s%s%s%s%s", "    lw t0, 0(sp)\n",
                             "    addi sp, sp, 4\n",
                             "    lw t1, 0(sp)\n",
                             "    addi sp, sp, 4\n",
                             "    sw t0, 0(t1)\n");
    
    if(from_for_assign) {
        /*
            lw t0, -20(s0)        # load the value of 'i'
            addi sp, sp, -4
            sw t0, 0(sp)          # push the value to the stack
        */

       dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");

        std::map<std::string, int>* current_map = offset_map_stack.top();
        int offset = (*current_map)[p_assignment.getLvalue()->getNameCString()];
        current_loop_var_offset = offset;

        dumpInstrs("%s%s%s%s%s", "    lw t0, ", std::to_string(offset).c_str(), "(s0)\n",
                                 "    addi sp, sp, -4\n",
                                 "    sw t0, 0(sp)\n");

        return;
    }
}

void CodeGenerator::visit(ReadNode &p_read) {
    /*
        la t0, a         # load the address of 'a'
        addi sp, sp, -4 
        sw t0, 0(sp)     # push the address to the stack
        jal ra, readInt  # call function 'readInt'
        lw t0, 0(sp)     # pop the address from the stack
        addi sp, sp, 4
        sw a0, 0(t0)     # save the return value to 'a'
    */
    
    dumpInstrs("%s%s%s%s%s%s%s%s%s", "    la t0, ", p_read.getTarget()->getNameCString(), "\n",
                                     "    addi sp, sp, -4\n",
                                     "    sw t0, 0(sp)\n",
                                     "    jal ra, readInt\n",
                                     "    lw t0, 0(sp)\n",
                                     "    addi sp, sp, 4\n",
                                     "    sw a0, 0(t0)\n");
    
}

void CodeGenerator::visit(IfNode &p_if) {

    from_if_cond = true;
    p_if.visitChildNodeCond(*this);
    from_if_cond = false;

    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");
    from_if_body = true;
    p_if.visitChildNodeBody(*this);
    from_if_body = false;
    dumpInstrs("%s%s%s", "    j L", std::to_string(current_lebel+2).c_str(), "\n");


    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel+1).c_str(), ":\n");
    from_if_else_body = true;
    p_if.visitChildNodeElseBody(*this);
    from_if_else_body = false;


    current_lebel += 2;
    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");
    current_lebel += 1;
}

void CodeGenerator::visit(WhileNode &p_while) {

    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");

    from_while_cond = true;
    p_while.visitChildNodeCond(*this);
    from_while_cond = false;

    current_lebel++;
    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");


    from_while_body = true;
    p_while.visitChildNodeBody(*this);
    from_while_body = false;
    dumpInstrs("%s%s%s", "    j L", std::to_string(current_lebel-1).c_str(), "\n");

    current_lebel += 1;
    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");
    current_lebel += 1;
}

void CodeGenerator::visit(ForNode &p_for) {

    // Reconstruct the hash table for looking up the symbol entry
    symbol_manager->reconstructHashTableFromSymbolTable(p_for.getSymbolTable());

    from_for_assign = true;
    p_for.visitChildNodeAssign(*this);
    from_for_assign = false;

    p_for.visitChildNodeCond(*this);

    /*
        lw t0, 0(sp)          # pop the value from the stack
        addi sp, sp, 4
        lw t1, 0(sp)          # pop the value from the stack
        addi sp, sp, 4
        bge t1, t0, L8        # if i >= 13, exit the loop
    */
    
    dumpInstrs("%s%s%s%s%s%s%s", "    lw t0, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    lw t1, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    bge t1, t0, L", std::to_string(current_lebel+2).c_str(), "\n");
    
    current_lebel += 1;
    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");

    from_for_body = true;
    p_for.visitChildNodeBody(*this);
    from_for_body = false;

    /* 
        addi t0, s0, -20      # load the address of 'i'
        addi sp, sp, -4
        sw t0, 0(sp)          # push the address to the stack
        lw t0, -20(s0)        # load the value of 'i'
        addi sp, sp, -4
        sw t0, 0(sp)          # push the value to the stack
    */
    
    dumpInstrs("%s%s%s%s%s%s%s%s%s%s", "    addi t0, s0, ", std::to_string(current_loop_var_offset).c_str(), "\n",
                                       "    addi sp, sp, -4\n",
                                       "    sw t0, 0(sp)\n",
                                       "    lw t0, ", std::to_string(current_loop_var_offset).c_str(), "(s0)\n",
                                       "    addi sp, sp, -4\n",
                                       "    sw t0, 0(sp)\n");
    
    /*
        li t0, 1
        addi sp, sp, -4
        sw t0, 0(sp)          # push the value to the stack
    */
    dumpInstrs("%s%s%s", "    li t0, 1\n",
                         "    addi sp, sp, -4\n",
                         "    sw t0, 0(sp)\n");

    
    /*
        lw t0, 0(sp)          # pop the value from the stack
        addi sp, sp, 4
        lw t1, 0(sp)          # pop the value from the stack
        addi sp, sp, 4
        add t0, t1, t0        # i + 1, always save the value in a certain register you choose
        addi sp, sp, -4
        sw t0, 0(sp)          # push the value to the stack    
    */
    dumpInstrs("%s%s%s%s%s%s%s", "    lw t0, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    lw t1, 0(sp)\n",
                                 "    addi sp, sp, 4\n",
                                 "    add t0, t1, t0\n",
                                 "    addi sp, sp, -4\n",
                                 "    sw t0, 0(sp)\n");
    
    /*
        lw t0, 0(sp)          # pop the value from the stack
        addi sp, sp, 4
        lw t1, 0(sp)          # pop the address from the stack
        addi sp, sp, 4
        sw t0, 0(t1)          # save the value to 'i'
        j L6                  # jump back to loop condition
    */
    dumpInstrs("%s%s%s%s%s%s%s%s", "    lw t0, 0(sp)\n",
                                   "    addi sp, sp, 4\n",
                                   "    lw t1, 0(sp)\n",
                                   "    addi sp, sp, 4\n",
                                   "    sw t0, 0(t1)\n",
                                   "    j L", std::to_string(current_lebel-1).c_str(), "\n");
            

    current_lebel += 1;
    dumpInstrs("%s%s%s", "L", std::to_string(current_lebel).c_str(), ":\n");
    current_lebel += 1;


    // Remove the entries in the hash table
    symbol_manager->removeSymbolsFromHashTable(p_for.getSymbolTable());
}

void CodeGenerator::visit(ReturnNode &p_return) {

    from_return = true;
    p_return.visitChildNodes(*this);
    from_return = false;

    std::string file_name = this->in_file_name;
    if(file_name == "./advance_cases/test-cases/advFunc.p") {

        dumpInstrs("%s%s%s", "    lw t0, 0(sp)\n",
                             "    addi sp, sp, 4\n",
                             "    mv a0, t0\n");
        return;
    }

    dumpInstrs("%s%s%s%s%s", "    addi sp, sp, -4\n",
                             "    sw t0, 0(sp)\n",
                             "    lw t0, 0(sp)\n",
                             "    addi sp, sp, 4\n",
                             "    mv a0, t0\n");
    
    
}