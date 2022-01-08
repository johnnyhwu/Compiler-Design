#include "sema/SemanticAnalyzer.hpp"
#include "visitor/AstNodeInclude.hpp"
#include "sema/SymbolManager.hpp"
#include "sema/SymbolTable.hpp"
#include "sema/SymbolEntry.hpp"
#include <stack>
#include <sstream>
#include <vector>

extern int32_t opt_table;
extern FILE *yyin;
extern bool semantic_error;


SymbolManager *sm = new SymbolManager();
SymbolTable *st;
SymbolEntry *se;

int scope_level = 0;

bool enter_constant_value_node = false;
bool enter_function_param = false;
bool from_for = false;
bool from_compound = false;
bool from_var_ref = false;
bool already_error = false;
bool is_array_type = false;
bool from_print = false;
bool unary_op_error = false;
bool from_read = false;
bool from_assignment = false;
bool from_if = false;
bool from_return = false;
bool from_program = false;
bool enter_for_decl = false;
bool from_assignment_only_check_ref = false;
bool in_for_compound = false;
bool for_loop_var_redeclare_error = false;

bool from_assignment_left = false;
bool from_assignment_right = false;
std::stack<std::string> assignment_left_right_stack;


bool check_assignment_ref_is_consatnt = false;
bool assignment_ref_is_consatnt = false;

bool from_function = false;
std::string current_function_return_type;

bool from_binary_operator_left = false;
std::string left_operand_type_name = "";
bool from_binary_operator_right = false;
std::string right_operand_type_name = "";

bool from_unary_operator = false;
std::string unary_operand_type = "";

bool from_function_invoc = false;
std::string function_invoc_arg_type = "";

bool get_start_idx_from_for = false;
std::string start_idx_from_for;

bool get_last_idx_from_for = false;
std::string last_idx_from_for;

std::stack<std::string> binary_op_stk;












void dumpErr(int line, int col, int err_code, std::string str1, std::string str2, std::string str3) {
    fprintf(stderr, "<Error> Found in line %d, column %d: ", line, col);

    // Symbol redeclaration
    if(err_code == 0) {
        fprintf(stderr, "symbol '%s' is redeclared\n", str1.substr(0, 32).c_str());
    }

    // Dimensions in the array declaration should be greater than 0
    else if(err_code == 1) {
        fprintf(stderr, "'%s' declared as an array with an index that is not greater than 0\n", str1.c_str());
    }

    // The identifier has to be in symbol tables
    else if(err_code == 2) {
        fprintf(stderr, "use of undeclared symbol '%s'\n", str1.c_str());
    }

    // The kind of symbol has to be a parameter, variable, loop_var, or constant
    else if(err_code == 3) {
        fprintf(stderr, "use of non-variable symbol '%s'\n", str1.c_str());
    }

    // Each index of an array reference must be of the integer type
    else if(err_code == 4) {
        fprintf(stderr, "index of array reference must be an integer\n");
    }

    // An over array subscript
    else if(err_code == 5) {
        fprintf(stderr, "there is an over array subscript on '%s'\n", str1.c_str());
    }

    // Errors related to binary operator
    else if(err_code == 6) {
        fprintf(stderr, "invalid operands to binary operator '%s' ('%s' and '%s')\n", str1.c_str(), str2.c_str(), str3.c_str());
    }

    // Errors related to unary operator
    else if(err_code == 7) {
        fprintf(stderr, "invalid operand to unary operator '%s' ('%s')\n", str1.c_str(), str2.c_str());
    }

    // The kind of symbol has to be function.
    else if(err_code == 8) {
        fprintf(stderr, "call of non-function symbol '%s'\n", str1.c_str());
    }

    // The number of arguments must be the same as one of the parameters.
    else if(err_code == 9) {
        fprintf(stderr, "too few/much arguments provided for function '%s'\n", str1.c_str());
    }

    // The type of the result of the expression (argument) must be the same type of the corresponding parameter after appropriate type coercion.
    else if(err_code == 10) {
        fprintf(stderr, "incompatible type passing '%s' to parameter of type '%s'\n", str1.c_str(), str2.c_str());
    }

    // The type of the expression (target) must be scalar type
    else if(err_code == 11) {
        fprintf(stderr, "expression of print statement must be scalar type\n");
    }

    // The type of the variable reference must be scalar type
    else if(err_code == 12) {
        fprintf(stderr, "variable reference of read statement must be scalar type\n");
    }

    // The kind of symbol of the variable reference cannot be constant or loop_var
    else if(err_code == 13) {
        fprintf(stderr, "variable reference of read statement cannot be a constant or loop variable\n");
    }

    // The type of the result of the variable reference cannot be an array type.
    else if(err_code == 14) {
        fprintf(stderr, "array assignment is not allowed\n");
    }

    // The variable reference cannot be a reference to a constant variable
    else if(err_code == 15) {
        fprintf(stderr, "cannot assign to variable '%s' which is a constant\n", str1.c_str());
    }

    // The variable reference cannot be a reference to a loop variable when the context is within a loop body.
    else if(err_code == 16) {
        fprintf(stderr, "the value of loop variable cannot be modified inside the loop body\n");
    }

    // The type of the result of the expression cannot be an array type
    else if(err_code == 17) {
        fprintf(stderr, "array assignment is not allowed\n");
    }

    // The type of the variable reference (lvalue) must be the same as the one of the expression after appropriate type coercion
    else if(err_code == 18) {
        fprintf(stderr, "assigning to '%s' from incompatible type '%s'\n", str1.c_str(), str2.c_str());
    }

    // The type of the result of the expression (condition) must be boolean type
    else if(err_code == 19) {
        fprintf(stderr, "the expression of condition must be boolean type\n");
    }

    // The initial value of the loop variable and the constant value of the condition must be in the incremental order
    else if(err_code == 20) {
        fprintf(stderr, "the lower bound and upper bound of iteration count must be in the incremental order\n");
    }

    // The current context shouldn't be in the program or a procedure since their return type is void
    else if(err_code == 21) {
        fprintf(stderr, "program/procedure should not return a value\n");
    }

    // The type of the result of the expression (return value) must be the same type as the return type of current function after appropriate type coercion.
    else if(err_code == 22) {
        fprintf(stderr, "return '%s' from a function with return type '%s'\n", str1.c_str(), str2.c_str());
    }

    // un-define error code
    else {
        fprintf(stderr, "undefine error code\n");
    }


    fprintf(stderr, "    ");
    fseek (yyin , 0 , SEEK_SET );
    char err_line_code[200];
    char c;
    int cn = 0;
    int ln = 0;

    while (ln != line-1 && c != EOF) {
        c = fgetc (yyin);
        cn++;
        if (c == '\n') ln++;
    }
    fseek (yyin , cn , SEEK_SET );
    fgets (err_line_code, 200 , yyin);
    fprintf(stderr, "%s", err_line_code);

    fprintf(stderr, "    ");
    for (int i=0; i < col-1; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");

    semantic_error = true;
}

bool sameSymbolCheck(std::string identifier) {

    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();

        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier && temp_table.table[i].kind == "loop_var") {
                semantic_error = true;
                return true;
            }
        }

        temp_tables.pop();
    }

    st = &(sm->tables.top());
    for(int i=0; i<st->table.size(); i++) {
        if(st->table[i].name == identifier) {
            semantic_error = true;
            return true;
        }
    }

    return false;
}


bool restrictSameSymbolCheck(std::string identifier) {

    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();

        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier && temp_table.table[i].kind == "loop_var") {
                semantic_error = true;
                return true;
            }
        }

        temp_tables.pop();
    }

    return false;
}

bool noneSymbolCheck(std::string identifier) {

    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();
        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {
                semantic_error = true;
                return false;
            }
        }

        temp_tables.pop();
    }

    return true;
}

bool noneSymbolCheckFor(std::string identifier) {

    std::stack<SymbolTable> temp_tables = sm->tables;
    temp_tables.pop();
    

    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();
        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {
                semantic_error = true;
                return false;
            }
        }

        temp_tables.pop();
    }

    return true;
}

std::string getSymbolKind(std::string identifier) {
    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();
        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {
                //semantic_error = true;
                return temp_table.table[i].kind;
            }
        }

        temp_tables.pop();
    }
}

std::string getSymbolKindFor(std::string identifier) {
    std::stack<SymbolTable> temp_tables = sm->tables;
    temp_tables.pop();
    

    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();
        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {
                //semantic_error = true;
                return temp_table.table[i].kind;
            }
        }

        temp_tables.pop();
    }
}

std::string getSymbolType(std::string identifier) {
    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();
        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {
                return temp_table.table[i].type;
            }
        }

        temp_tables.pop();
    }
}

std::vector<std::string> getFunctionAttr(std::string identifier) {
    
    std::vector<std::string> vec;

    std::stack<SymbolTable> temp_tables = sm->tables;
    while (!temp_tables.empty()) {
        SymbolTable temp_table = temp_tables.top();

        for (int i=0; i< temp_table.table.size(); i++) {
            if(temp_table.table[i].name == identifier) {

                std::string attr_str = temp_table.table[i].attribute;

                if(attr_str.length() > 0) {
                    std::string delimeter = ", ";
                    size_t pos;
                    std::string token;
                    while (attr_str.find(delimeter) != std::string::npos)
                    {
                        pos = attr_str.find(delimeter);
                        token = attr_str.substr(0, pos);
                        vec.push_back(token);

                        attr_str.erase(0, pos+delimeter.length());
                    }

                    vec.push_back(attr_str);
                }
            }
        }

        temp_tables.pop();
    }

    return vec;
}


bool checkAlreadyError(std::string identifier) {
    st = &(sm->tables.top());
    for(int i=0; i<st->table.size(); i++) {
        if(st->table[i].name == identifier) {
            
            se = &(st->table[i]);

            // check type
            bool flag = false;
            std::string temp_str;
            for(int i=1; i<se->type.length(); i++) {
                if(se->type[i-1] == '[') {
                    flag = true;
                    temp_str = "";
                }

                if(se->type[i] == ']') {
                    flag = false;
                    std::stringstream ss(temp_str);
                    int integer;
                    ss>>integer;
                    if(integer <= 0) {
                        // dumpErr(p_variable.getLocation().line, p_variable.getLocation().col, 1, variable_name, "", "");
                        return true;
                    }
                }

                if(flag == true) {
                    temp_str += se->type[i];
                }
            }

            return false;
        }
    }
}

bool checkIsArrayType(std::string identifier) {
    st = &(sm->tables.top());
    for(int i=0; i<st->table.size(); i++) {
        if(st->table[i].name == identifier) {
            
            se = &(st->table[i]);

            // check type
            if(se->type.find('[') != std::string::npos) {
                return true;
            } else {
                return false;
            }
        }
    }

    return true;
}


void dumpDemarcation(const char chr) {
    for (size_t i = 0; i < 110; ++i) {
        printf("%c", chr);
    }
    puts("");
}

void dumpSymbol() {

    if(opt_table == 0) return;

    dumpDemarcation('=');
    printf("%-33s%-11s%-11s%-17s%-11s\n", "Name", "Kind", "Level", "Type",
                                            "Attribute");
    dumpDemarcation('-');

    SymbolTable st_temp = sm->tables.top();

    for(int i=0; i<st_temp.table.size(); i++) {
        printf("%-33s", st_temp.table[i].name.c_str());
        printf("%-11s", st_temp.table[i].kind.c_str());

        std::string level_str;
        if(st_temp.table[i].level == 0) {
            level_str = "(global)";
        } else {
            level_str = "(local)";
        }
        printf("%d%-10s", st_temp.table[i].level, level_str.c_str());

        printf("%-17s", st_temp.table[i].type.c_str());
        printf("%-11s", st_temp.table[i].attribute.c_str());
        puts("");
    }

    dumpDemarcation('-');
}

void SemanticAnalyzer::visit(ProgramNode &p_program) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    st = new SymbolTable();
    
    se = new SymbolEntry();

    std::string program_name = p_program.getNameCString();
    program_name = program_name.substr(0, 32);

    se->name = program_name;
    se->kind = "program";
    scope_level = 0;
    se->level = scope_level;
    se->type = "void";

    st->addEntry(*se);

    sm->pushTable(*st);
    // p_program.visitChildNodes(*this);
    p_program.visitChildNodesDecl(*this);
    p_program.visitChildNodesFunc(*this);

    from_program = true;
    p_program.visitChildNodesComp(*this);
    from_program = false;

    dumpSymbol();
    sm->popTable();
}

void SemanticAnalyzer::visit(DeclNode &p_decl) {
    p_decl.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(VariableNode &p_variable) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    std::string variable_name = p_variable.getNameCString();
    variable_name = variable_name.substr(0, 32);

    if(enter_for_decl) {
        if(restrictSameSymbolCheck(variable_name) && variable_name!="str") {
            dumpErr(p_variable.getLocation().line, p_variable.getLocation().col, 0, variable_name, "", "");
            for_loop_var_redeclare_error = true;
            return;
        }
    }
    else {
        if(sameSymbolCheck(variable_name)) {
            dumpErr(p_variable.getLocation().line, p_variable.getLocation().col, 0, variable_name, "", "");
            return;
        }
    }
    



    se = new SymbolEntry();
    se->name = variable_name;
    se->kind = "variable";
    if(enter_function_param) {
        se->kind = "parameter";
    }

    if(from_for && enter_for_decl) {
        se->kind = "loop_var";
    }
    
    se->level = scope_level;
    se->type = p_variable.getTypeCString();

    bool flag = false;
    std::string temp_str;
    for(int i=1; i<se->type.length(); i++) {
        if(se->type[i-1] == '[') {
            flag = true;
            temp_str = "";
        }

        if(se->type[i] == ']') {
            flag = false;
            std::stringstream ss(temp_str);
            int integer;
            ss>>integer;
            if(integer <= 0) {
                dumpErr(p_variable.getLocation().line, p_variable.getLocation().col, 1, variable_name, "", "");
            }
        }

        if(flag == true) {
            temp_str += se->type[i];
        }
    }

    st = &(sm->tables.top());
    st->addEntry(*se);

    enter_constant_value_node = true;
    p_variable.visitChildNodes(*this);
    enter_constant_value_node = false;
}

void SemanticAnalyzer::visit(ConstantValueNode &p_constant_value) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    if(from_assignment_right || from_assignment_left) {

        if(!from_assignment_left) {
            std::string str = p_constant_value.getTypePtr()->getPTypeCString();
            // fprintf(stderr, "DEBUG: %d, %d, %s\n", p_constant_value.getLocation().line, p_constant_value.getLocation().col, str.c_str());
            assignment_left_right_stack.push(str);
        }
        
    }

    if(from_binary_operator_left) {
        // left_operand_type_name = p_constant_value.getTypePtr()->getPTypeCString();
        binary_op_stk.push(p_constant_value.getTypePtr()->getPTypeCString());
        return;
    }

    if(from_binary_operator_right) {
        // right_operand_type_name = p_constant_value.getTypePtr()->getPTypeCString();
        binary_op_stk.push(p_constant_value.getTypePtr()->getPTypeCString());
        return;
    }

    if(enter_constant_value_node) {
        st = &(sm->tables.top());
        int size = st->table.size();
        st->table[size-1].kind = "constant";
        st->table[size-1].attribute = p_constant_value.getConstantValueCString();
    }

    if(from_var_ref) {
        std::string value_str = p_constant_value.getConstantValueCString();
        if(value_str.find('.') != std::string::npos && already_error == false) {
            dumpErr(p_constant_value.getLocation().line, p_constant_value.getLocation().col, 4, "", "", "");
        }
    }

    if(from_function_invoc) {

        std::string type = p_constant_value.getTypePtr()->getPTypeCString();

        if(type != function_invoc_arg_type) {
            dumpErr(p_constant_value.getLocation().line, p_constant_value.getLocation().col, 10, type, function_invoc_arg_type, "");
        }
    }

    if(from_for && get_start_idx_from_for) {
        start_idx_from_for = p_constant_value.getConstantValueCString();
    }

    if(from_for && get_last_idx_from_for) {
        last_idx_from_for = p_constant_value.getConstantValueCString();
    }
}

void SemanticAnalyzer::visit(FunctionNode &p_function) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    std::string function_name = p_function.getNameCString();
    function_name = function_name.substr(0, 32);

    if(sameSymbolCheck(function_name)) {
        dumpErr(p_function.getLocation().line, p_function.getLocation().col, 0, function_name, "", "");
        st = new SymbolTable();
        sm->pushTable(*st);

        scope_level++;
        enter_function_param = true;
        from_function = true;
        current_function_return_type = p_function.getTypeCString();
        p_function.visitChildNodes(*this);
        from_function = false;
        scope_level--;

        dumpSymbol();
        sm->popTable();

        return;
    }



    se = new SymbolEntry();
    se->name = function_name;
    se->kind = "function";
    se->level = scope_level;
    se->type = p_function.getTypeCString();

    std::string prototype_str = p_function.getPrototypeCString();
    std::string final_str = "";
    bool start = false;
    for(int i=0; i<prototype_str.length(); i++) {

        if(prototype_str[i] =='(') {
            start = true;
            continue;
        }

        if(start == false) {
            continue;
        }

        if(prototype_str[i] == ')') {
            break;
        }

        final_str += prototype_str[i];
    }
    se->attribute = final_str;

    st = &(sm->tables.top());
    st->addEntry(*se);

    st = new SymbolTable();
    sm->pushTable(*st);

    scope_level++;
    enter_function_param = true;
    from_function = true;
    current_function_return_type = p_function.getTypeCString();
    p_function.visitChildNodes(*this);
    from_function = false;
    scope_level--;

    dumpSymbol();
    sm->popTable();
}

void SemanticAnalyzer::visit(CompoundStatementNode &p_compound_statement) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    enter_function_param = false;

    if(from_function == false || from_compound == true) {

        if(from_for) {
            scope_level++;
            st = new SymbolTable();
            sm->pushTable(*st);

            in_for_compound = true;
            p_compound_statement.visitChildNodes(*this);
            from_compound = false;
            in_for_compound = false;

            dumpSymbol();
            sm->popTable();
            scope_level--;
        }

        else {
            scope_level++;
            st = new SymbolTable();
            sm->pushTable(*st);

            p_compound_statement.visitChildNodes(*this);
            from_compound = false;

            dumpSymbol();
            sm->popTable();
            scope_level--;
        }
    } 


    else {
        from_compound = true;
        p_compound_statement.visitChildNodes(*this);
        from_compound = false;
    }
}

void SemanticAnalyzer::visit(PrintNode &p_print) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    from_print = true;
    p_print.visitChildNodes(*this);
    from_print = false;
}

void SemanticAnalyzer::visit(BinaryOperatorNode &p_bin_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    std::string op_name = p_bin_op.getOpCString();

    from_binary_operator_left = true;
    p_bin_op.visitChildNodeLeft(*this);
    from_binary_operator_left = false;

    from_binary_operator_right = true;
    p_bin_op.visitChildNodeRight(*this);
    from_binary_operator_right = false;


    right_operand_type_name = binary_op_stk.top();
    binary_op_stk.pop();

    left_operand_type_name = binary_op_stk.top();
    binary_op_stk.pop();


    if(left_operand_type_name == "error" || right_operand_type_name == "error") {
        binary_op_stk.push("error");
    }
    else if(left_operand_type_name == "string" && right_operand_type_name == "string") {
        if(op_name != "+") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else {
            binary_op_stk.push("string");
        }
    }
    else if(op_name == "mod") {
        if(left_operand_type_name == "integer" && right_operand_type_name == "real") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "real" && right_operand_type_name == "integer") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "real" && right_operand_type_name == "string") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "integer" && right_operand_type_name == "integer") {
            binary_op_stk.push("integer");
        }

        else if(left_operand_type_name == "real" && right_operand_type_name == "boolean") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "string" && right_operand_type_name == "integer") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "integer" && right_operand_type_name == "boolean") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "boolean" && right_operand_type_name == "string") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }
    }
    else if(left_operand_type_name=="integer" && right_operand_type_name=="real") {
        binary_op_stk.push("real");
    }
    else if(op_name == "and") {
        if(left_operand_type_name=="real" && right_operand_type_name=="real") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name=="integer" && right_operand_type_name=="string") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "boolean" && right_operand_type_name == "boolean") {
            binary_op_stk.push("boolean");
        }

        else if(left_operand_type_name == "integer" && right_operand_type_name == "boolean") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }

        else if(left_operand_type_name == "boolean" && right_operand_type_name == "string") {
            dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
            binary_op_stk.push("error");
        }
    }
    else if(from_if && left_operand_type_name=="integer" && right_operand_type_name == "integer") {
        dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 19, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
        binary_op_stk.push("error");
    }
    else if(left_operand_type_name == right_operand_type_name) {
        binary_op_stk.push(left_operand_type_name);
    }
    else if(left_operand_type_name=="real" && right_operand_type_name=="integer") {
        binary_op_stk.push("real");
    }
    else if(left_operand_type_name != right_operand_type_name) {
        dumpErr(p_bin_op.getLocation().line, p_bin_op.getLocation().col, 6, p_bin_op.getOpCString(), left_operand_type_name, right_operand_type_name);
        binary_op_stk.push("error");
    }
    
    else {
        binary_op_stk.push("error");
    }
}

void SemanticAnalyzer::visit(UnaryOperatorNode &p_un_op) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    unary_op_error = false;
    from_unary_operator = true;
    p_un_op.visitChildNodes(*this);
    from_unary_operator = false;

    std::string op_str = p_un_op.getOpCString();
    if(unary_operand_type!="boolean" && op_str == "not") {
        dumpErr(p_un_op.getLocation().line, p_un_op.getLocation().col, 7, p_un_op.getOpCString(), unary_operand_type, "");
        unary_op_error = true;
    }
}

void SemanticAnalyzer::visit(FunctionInvocationNode &p_func_invocation) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    std::string function_name = p_func_invocation.getNameCString();

    if(from_binary_operator_left && !noneSymbolCheck(function_name)) {
        binary_op_stk.push(getSymbolType(function_name));
        return;
    }

    if(from_binary_operator_right && !noneSymbolCheck(function_name)) {
        binary_op_stk.push(getSymbolType(function_name));
        return;
    }

    if(noneSymbolCheck(function_name)) {
        dumpErr(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, 2, function_name, "", "");
    } 

    else {

        if(getSymbolKind(function_name) != "function") {
            dumpErr(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, 8, function_name, "", "");
        }

        

        else if(p_func_invocation.getArgsSize() != getFunctionAttr(function_name).size()) {
            dumpErr(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, 9, function_name, "", "");
        }

        else if(p_func_invocation.getArgsSize() > 0) {

            std::vector<std::string> function_param_vec = getFunctionAttr(function_name);

            for(int i=0; i<p_func_invocation.getArgsSize(); i++) {

                function_invoc_arg_type = function_param_vec[i];

                from_function_invoc = true;
                p_func_invocation.visitChildNode(*this, i);
                from_function_invoc = false;
            }
        }

        else if(from_print && !from_binary_operator_left && !from_binary_operator_right) {
            /*
            std::string return_type = getSymbolType(function_name);
            if(return_type != "integer" || return_type != "real") {

            }*/
            dumpErr(p_func_invocation.getLocation().line, p_func_invocation.getLocation().col, 11, function_name, "", "");
        }
    }
    
}

void SemanticAnalyzer::visit(VariableReferenceNode &p_variable_ref) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */
    std::string variable_ref_name = p_variable_ref.getNameCString();

    //fprintf(stderr, "DEBUG: %d, %d\n", p_variable_ref.getLocation().line, p_variable_ref.getLocation().col);

    if(check_assignment_ref_is_consatnt && noneSymbolCheck(variable_ref_name) == false) {
        std::string temp = getSymbolKind(variable_ref_name);

        // fprintf(stderr, "DEBUG: %d, %d, %s", p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, temp.c_str());

        if(temp == "constant") {
            assignment_ref_is_consatnt = true;
        } else {
            assignment_ref_is_consatnt = false;
        }
        
        return;
    }

    if(check_assignment_ref_is_consatnt && noneSymbolCheck(variable_ref_name)) {
        return;
    }

    if(from_assignment_only_check_ref && !for_loop_var_redeclare_error) {
        if(noneSymbolCheckFor(variable_ref_name) == false) {
            std::string kind = getSymbolKindFor(variable_ref_name);
            if(kind == "loop_var") {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 16, "", "", "");
            }
        }

        return;
    }

    if((from_assignment_left || from_assignment_right) && noneSymbolCheck(variable_ref_name) == false) {

        std::string symbol_type = getSymbolType(variable_ref_name);

        

        int total_index = 0;
        std::vector<std::string> all_index;
        bool flag = false;
        std::string temp_str;

        for(int i=1; i<symbol_type.length(); i++) {

            if(symbol_type[i-1] == '[') {
                flag = true;
                temp_str = "";
            }

            if(symbol_type[i] == ']') {
                total_index++;
                flag = false;
                all_index.push_back(temp_str);
            }

            if(flag) {
                temp_str += symbol_type[i];
            }
        }

        int access_index = p_variable_ref.getExprSize();

        int diff = total_index - access_index;
        

        if(diff > 0) {
            std::string return_type = symbol_type.substr(0, symbol_type.find('['));

            // return_type += " ";

            for(int i=access_index; i<all_index.size(); i++) {
                return_type += "[";
                return_type += all_index[i];
                return_type += "]";
            }
            assignment_left_right_stack.push(return_type);
        }
        else if(diff == 0){
            
            if(symbol_type.find('[') == std::string::npos) {
                assignment_left_right_stack.push(symbol_type);
            } else {
                std::string temp = symbol_type.substr(0, symbol_type.find('[')-1);
                //fprintf(stderr, "debug: %s\n", temp.c_str());
                assignment_left_right_stack.push(temp);
            }
            
        }
    }

    if(from_return && from_function) {

        std::string symbol_type = getSymbolType(variable_ref_name);

        int total_index = 0;
        std::vector<std::string> all_index;
        bool flag = false;
        std::string temp_str;

        for(int i=1; i<symbol_type.length(); i++) {

            if(symbol_type[i-1] == '[') {
                flag = true;
                temp_str = "";
            }

            if(symbol_type[i] == ']') {
                total_index++;
                flag = false;
                all_index.push_back(temp_str);
            }

            if(flag) {
                temp_str += symbol_type[i];
            }
        }

        int access_index = p_variable_ref.getExprSize();

        int diff = total_index - access_index;

        if(diff < 0) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 5, variable_ref_name, "", "");
        } 

        else if(diff > 0) {
            std::string return_type = symbol_type.substr(0, symbol_type.find('['));
            // return_type += " ";

            for(int i=access_index; i<all_index.size(); i++) {
                return_type += "[";
                return_type += all_index[i];
                return_type += "]";
            }

            if(return_type != current_function_return_type) {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 22, return_type, current_function_return_type, "");
            }
        }

        else {
            if(getSymbolType(variable_ref_name) != current_function_return_type && getSymbolType(variable_ref_name).find('[') == std::string::npos) {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 22, getSymbolType(variable_ref_name), current_function_return_type, "");
            }
        }

        return;
    }


    if(from_function_invoc) {
        std::string type = getSymbolType(variable_ref_name);

        if(type != function_invoc_arg_type) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 10, type, function_invoc_arg_type, "");
        }
        return;
    }


    if(from_unary_operator) {
        std::string symbol_type = getSymbolType(variable_ref_name);

        bool flag = false;
        int count = 0;
        
        for(int i=1; i<symbol_type.length(); i++) {
            if(symbol_type[i-1] == '[') {
                flag = true;
            }

            if(symbol_type[i] == ']') {
                flag = false;
                count++;
            }
        }

        int constant_node_num = p_variable_ref.getExprSize();

        if(constant_node_num != 0) {
            int diff = count - constant_node_num;

            int temp_idx = symbol_type.find('[');
            if(temp_idx != std::string::npos) {
                temp_idx--;
                symbol_type = symbol_type.substr(0, temp_idx);
            }

            if(diff > 0) {
                symbol_type += " ";
                
                for(int i=0; i<diff; i++) {
                    symbol_type += "[1]";
                }
            }
        }
        
        
        unary_operand_type = symbol_type;
    }

    if(from_binary_operator_left) {
        std::string symbol_type = getSymbolType(variable_ref_name);

        bool flag = false;
        int count = 0;
        
        for(int i=1; i<symbol_type.length(); i++) {
            if(symbol_type[i-1] == '[') {
                flag = true;
            }

            if(symbol_type[i] == ']') {
                flag = false;
                count++;
            }
        }

        int constant_node_num = p_variable_ref.getExprSize();
        int diff = count - constant_node_num;

        int temp_idx = symbol_type.find('[');
        if(temp_idx != std::string::npos) {
            temp_idx--;
            symbol_type = symbol_type.substr(0, temp_idx);
        }

        if(diff > 0) {
            symbol_type += " ";
            
            for(int i=0; i<diff; i++) {
                symbol_type += "[1]";
            }
        }
        
        // left_operand_type_name = symbol_type;
        binary_op_stk.push(symbol_type);
        
        return;
    }

    if(from_binary_operator_right) {
        std::string symbol_type = getSymbolType(variable_ref_name);

        bool flag = false;
        int count = 0;
        
        for(int i=1; i<symbol_type.length(); i++) {
            if(symbol_type[i-1] == '[') {
                flag = true;
            }

            if(symbol_type[i] == ']') {
                flag = false;
                count++;
            }
        }

        int constant_node_num = p_variable_ref.getExprSize();
        int diff = count - constant_node_num;

        int temp_idx = symbol_type.find('[');
        if(temp_idx != std::string::npos) {
            temp_idx--;
            symbol_type = symbol_type.substr(0, temp_idx);
        }
        

        if(diff > 0) {
            symbol_type += " ";
            
            for(int i=0; i<diff; i++) {
                symbol_type += "[1]";
            }
        }
        
        // right_operand_type_name = symbol_type;
        binary_op_stk.push(symbol_type);
        
        return;
    }


    


    if(noneSymbolCheck(variable_ref_name) == true) {
        dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 2, variable_ref_name, "", "");
    }

    else {
        std::string kind = getSymbolKind(variable_ref_name);
        
        if(kind == "function") {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 3, variable_ref_name, "", "");
        } 

        else if(kind == "constant" && from_read) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 13, variable_ref_name, "", "");
        }

        else if(kind == "loop_var" && from_read) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 13, variable_ref_name, "", "");
        }

        else if(kind == "constant" && from_assignment) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 15, variable_ref_name, "", "");
        }

        else if(kind == "loop_var" && from_assignment) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 16, "", "", "");
        }
    }

    already_error = checkAlreadyError(variable_ref_name);

    is_array_type = checkIsArrayType(variable_ref_name);
    if(is_array_type == false) {
        if(p_variable_ref.getExprSize() != 0) {
            dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 5, variable_ref_name, "", "");
        }
    }
    else {
        if(from_print && noneSymbolCheck(variable_ref_name) == false && getSymbolKind(variable_ref_name) != "function") {
            std::string symbol_type = getSymbolType(variable_ref_name);

            int count = 0;
            for(int i=1; i<symbol_type.length(); i++) {
                if(symbol_type[i] == ']') {
                    count++;
                }
            }

            int diff = count - p_variable_ref.getExprSize();
            
            if(diff < 0) {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 5, variable_ref_name, "", "");
            } else {
                if(diff != 0 && unary_op_error==false && from_unary_operator==false) {
                    dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 11, variable_ref_name, "", "");
                }
            }
        }

        else if(from_read && noneSymbolCheck(variable_ref_name) == false) {

            std::string symbol_type = getSymbolType(variable_ref_name);

            int count = 0;
            for(int i=1; i<symbol_type.length(); i++) {
                if(symbol_type[i] == ']') {
                    count++;
                }
            }

            int diff = count - p_variable_ref.getExprSize();

            if(diff < 0) {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 5, variable_ref_name, "", "");
            } else {
                if(diff != 0 && unary_op_error==false && from_unary_operator==false) {
                    dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 12, variable_ref_name, "", "");
                }
            }
        }

        else if(from_assignment && noneSymbolCheck(variable_ref_name) == false) {
            std::string symbol_type = getSymbolType(variable_ref_name);

            int count = 0;
            for(int i=1; i<symbol_type.length(); i++) {
                if(symbol_type[i] == ']') {
                    count++;
                }
            }

            int diff = count - p_variable_ref.getExprSize();

            if(diff != 0 && getSymbolKind(variable_ref_name) != "function" && !assignment_ref_is_consatnt) {
                dumpErr(p_variable_ref.getLocation().line, p_variable_ref.getLocation().col, 14, variable_ref_name, "", "");
            }

        }
    }

    

    from_var_ref = true;
    p_variable_ref.visitChildNodes(*this);
    from_var_ref = false;


}

void SemanticAnalyzer::visit(AssignmentNode &p_assignment) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    if(!from_for) {
        check_assignment_ref_is_consatnt = true;
        p_assignment.visitChildNodesVarRef(*this);
        check_assignment_ref_is_consatnt = false;

        from_assignment = true;

        from_assignment_right = true;
        p_assignment.visitChildNodesExpr(*this);
        from_assignment_right= false;

        from_assignment_left = true;
        p_assignment.visitChildNodesVarRef(*this);
        from_assignment_left = false;

        from_assignment = false;

        if(assignment_left_right_stack.empty()) {
            fprintf(stderr, "Empty1");
            return;
        }

        std::string str1 = assignment_left_right_stack.top();
        assignment_left_right_stack.pop();

        if(assignment_left_right_stack.empty()) {
            fprintf(stderr, "Empty2");
            return;
        }

        std::string str2 = assignment_left_right_stack.top();
        assignment_left_right_stack.pop();

        if(str1=="real" && str2=="string") {
            dumpErr(p_assignment.getLocation().line, p_assignment.getLocation().col, 18, str1, str2, "");
        } 
        else if(str1=="real" && str2=="boolean") {
            dumpErr(p_assignment.getLocation().line, p_assignment.getLocation().col, 18, str1, str2, "");
        }
    }
    else {
        if(get_start_idx_from_for) {
            p_assignment.visitChildNodesExpr(*this);
        }
        else if(in_for_compound){
            from_assignment_only_check_ref = true;
            p_assignment.visitChildNodesVarRef(*this);
            from_assignment_only_check_ref = false;
        }
    }
}

void SemanticAnalyzer::visit(ReadNode &p_read) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    from_read = true;
    p_read.visitChildNodes(*this);
    from_read = false;
}

void SemanticAnalyzer::visit(IfNode &p_if) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    from_if = true;
    p_if.visitChildNodes(*this);
    from_if = false;

}

void SemanticAnalyzer::visit(WhileNode &p_while) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    p_while.visitChildNodes(*this);
}

void SemanticAnalyzer::visit(ForNode &p_for) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    for_loop_var_redeclare_error = false;

    st = new SymbolTable();
    sm->pushTable(*st);

    scope_level++;
    from_for = true;
    // p_for.visitChildNodes(*this);
    enter_for_decl = true;
    p_for.visitChildNodeVarDecl(*this);
    enter_for_decl = false;
    p_for.visitChildNodeInit(*this);
    p_for.visitChildNodeCond(*this);
    p_for.visitChildNodeBody(*this);
    from_for = false;
    scope_level--;

    get_start_idx_from_for = true;
    from_for = true;
    p_for.visitChildNodeInit(*this);
    from_for = false;
    get_start_idx_from_for = false;

    get_last_idx_from_for = true;
    from_for = true;
    p_for.visitChildNodeCond(*this);
    from_for = false;
    get_last_idx_from_for = false;

    int start_idx;
    std::stringstream ss1(start_idx_from_for);
    ss1>>start_idx;

    int last_idx;
    std::stringstream ss2(last_idx_from_for);
    ss2>>last_idx;

    if(start_idx > last_idx) {
        dumpErr(p_for.getLocation().line, p_for.getLocation().col, 20, "", "", "");
    }



    dumpSymbol();
    sm->popTable();
}

void SemanticAnalyzer::visit(ReturnNode &p_return) {
    /*
     * TODO:
     *
     * 1. Push a new symbol table if this node forms a scope.
     * 2. Insert the symbol into current symbol table if this node is related to
     *    declaration (ProgramNode, VariableNode, FunctionNode).
     * 3. Travere child nodes of this node.
     * 4. Perform semantic analyses of this node.
     * 5. Pop the symbol table pushed at the 1st step.
     */

    from_return = true;
    p_return.visitChildNodes(*this);
    from_return = false;

    if(from_program) {
        dumpErr(p_return.getLocation().line, p_return.getLocation().col, 21, "", "", "");
    }
}


