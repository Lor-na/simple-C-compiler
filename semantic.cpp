#include "tree.h"
#include "macro.h"
#include <map>
#include <stack>

using namespace tree;

class VariableMap{
public:
    map<string, Type*> myMap;
};

stack<VariableMap *> map_stack;
VariableMap* global;

map<string, vector<Type*> > func_map;

bool isTypeEqual(Type *t1, Type* t2){
    return t1->base_type == t2->base_type;
}

bool isTypeBoolean(Type *type) {
    return type->base_type == T_BOOL;
}

bool isTypeInt(Type *type) {
    return type->base_type == T_INTEGER;
}

bool isTypeReal(Type *type) {
    return type->base_type == T_DOUBLE;
}

bool isTypeChar(Type *type) {
    return type->base_type == T_CHAR;
}

void errorBroadcast(string info, bool full_text, int line) {
    if(full_text)
        cout << "In Line " << line << ", " << info << endl;
    else
        cout << "In Line " << line << ", " << "Error occurs in " << info << endl;
}

void infoBroadcast(string info, int line){
    // cout << "In Line " << line << ", " << info << endl;
    return;
}

bool expStmCheck(Stm* s){
    if(s->node_type != N_EXP_STM){
        return false;
    }
    ExpStm* e = (ExpStm *)s;
    
    // if(e->exps.size() == 0){
    //     return false;
    // }
    return true;
}

bool Base::checkSemantics(){
    is_legal=true;
    return is_legal;
}

bool Program::checkSemantics() {
    infoBroadcast("Checking Program..", this->line);

    global = new VariableMap();
    map_stack.push(global);

    for (auto dec: this->dec){
        is_legal &=dec->checkSemantics();
        if (!is_legal) return is_legal;
    }
    
    for (auto fuc: this->func_dec){
        is_legal &=fuc->checkSemantics();
        if (! is_legal)return is_legal;
    }

    map_stack.pop();
    
    infoBroadcast("Success！", this->line);
    return is_legal;
}   

bool FuncDec::checkSemantics(){
    infoBroadcast("Checking FuncDec..", this->line);
    // node type
    is_legal &=  (para->d_type == D_FUNC_DEF) | (para->d_type == D_FUNC_EMPTY);

    // register function
    vector<Type*> para_vec;
    DeclaratorFunc *d = (DeclaratorFunc *)(this->para);
    if(d->d_type != D_FUNC_EMPTY){
        for(auto item : d->para_def->para){
            para_vec.push_back(item->type);
        }
        func_map[this->name] = para_vec;
    }
    
    // recursive checking
    VariableMap* m = new VariableMap();
    map_stack.push(m);
    
    is_legal &=block->checkSemantics();

    map_stack.pop();

    // name checking
    if(d->d_type != D_FUNC_EMPTY){
        ParaList *p = d->para_def;
        for(auto item: p->para) {
            if(this->name == item->var->name)
                is_legal = false;
        }
    }
    if(!is_legal)
        errorBroadcast("FuncDec: " + this->name, false, this->line);
    return is_legal;
}

bool Block::checkSemantics(){
    infoBroadcast("Checking Block..", this->line);
    for (auto d: dec){
        is_legal &= d->checkSemantics();
    }
    if(!is_legal) return is_legal;
    for (auto s: stm){
        is_legal &= s->checkSemantics();
    }
    // infoBroadcast("Bet not here");
    return is_legal;
}

bool ExpStm::checkSemantics(){
    infoBroadcast("Checking ExpStm..", this->line);
    for (auto exp : exps){
        is_legal &= exp->checkSemantics();
    }
    if(!is_legal)
        errorBroadcast("ExpStm", false, this->line);
    return is_legal;
}

bool SelectStm::checkSemantics(){
    infoBroadcast("Checking SelectStm..", this->line);
    // recursive checking
    is_legal&=condition->checkSemantics();

    is_legal&=if_do->checkSemantics();
    if(is_legal)
        is_legal&=else_do->checkSemantics();

    errorBroadcast("SelectStm", false, this->line);
    return is_legal;
}


bool WhileStm::checkSemantics(){
    infoBroadcast("Checking WhileStm..", this->line);

    // node type check
    is_legal &= expStmCheck(this->cond);
    if(!is_legal){
        errorBroadcast("Error: wrong condition in while loop.", true, this->line);
        return is_legal;
    }
    // recursive checking
    is_legal&=body->checkSemantics();
    return is_legal;
}

bool ForStm::checkSemantics(){
    infoBroadcast("Checking ForStm..", this->line);
    if(!(is_legal &= expStmCheck(this->cond))){
        errorBroadcast("Error: wrong condition in for loop.", true, this->line);
        return is_legal;
    }
    if(iter != nullptr && iter->node_type!=N_EXP_STM){
        is_legal=false;
        errorBroadcast("Error: wrong iteration in for loop.", true, this->line);
        return is_legal;
    }
    is_legal &= init->checkSemantics();
    is_legal &= body->checkSemantics();
    
    // infoBroadcast("I bet not here");
    return is_legal;
}

bool JumpStm::checkSemantics(){
    infoBroadcast("Checking JumpStm..", this->line);
    if(jump_type==J_RETURN && return_exp->node_type!=N_EXP_STM){
        errorBroadcast("Error: wrong condition in for loop.", true, this->line);
        return is_legal;
    }
    return is_legal;
}


bool AssignExp::checkSemantics() {
    infoBroadcast("Checking AssignExp..", this->line);
    // check children
    is_legal = left_exp->checkSemantics() && right_exp->checkSemantics();
    // check between children
    // if (is_legal) is_legal =(left_exp->return_type==right_exp->return_type);
    if(is_legal)
        is_legal = isTypeEqual(left_exp->return_type, right_exp->return_type);
    // check itself
    if (is_legal){
        if (left_exp->node_type == N_CONSTANT_EXP) {
            is_legal = false;
            errorBroadcast("Semantics Error: Constant obeject cannot be the left value in an assignment statement.", true, this->line);
        }
    } else {
        errorBroadcast("Semantics Error: Left value and right value must be the same type in an assignment statement.", true, this->line);
    }
    this->return_type = left_exp->return_type;
    return is_legal;
}

bool BinaryExp::checkSemantics() {
    infoBroadcast("Checking BinaryExp..", this->line);
    is_legal = operand1->checkSemantics();
    if (is_legal)
        switch (op_code) {
            case OP_ADD:
            case OP_MINUS:
            case OP_MUL:
            case OP_DIV:
                is_legal &= operand2->checkSemantics();
                if (!is_legal) return is_legal;
                if ((!isTypeInt(operand1->return_type) && !isTypeReal(operand1->return_type)) ||
                    (!isTypeInt(operand2->return_type) && !isTypeReal(operand2->return_type))) {
                    errorBroadcast("Error: The type of operands with calculation binary operator must be integer or real.", true, this->line);
                    is_legal = false;
                } else {
                    if (isTypeReal(operand1->return_type) || isTypeReal(operand2->return_type))
                        return_type = new Type(T_DOUBLE);
                    else return_type = new Type(T_INTEGER);
                }
                break;
            case OP_MOD: {
                is_legal &= operand2->checkSemantics();
                if (!is_legal) return is_legal;
                if (!isTypeInt(operand1->return_type) || !isTypeInt(operand2->return_type)) {
                    errorBroadcast("Semantics Error: The type of operands with a binary operator \'%%\' must be integer.", true, this->line);
                    is_legal = false;
                } else {
                    return_type = new Type(T_INTEGER);
                }
            }
                break;
            case OP_AND: 
            case OP_EXCLUSIVE_OR: 
            case OP_OR:  {
                is_legal &= operand2->checkSemantics();
                if (!is_legal) return is_legal;
                if (!isTypeBoolean(operand1->return_type) || !isTypeBoolean(operand2->return_type)) {
                    errorBroadcast("Semantics Error: The type of operands with boolean binary operator must be boolean.", true, this->line);
                    is_legal = false;
                } else {
                    return_type = new Type(T_BOOL);
                }
            }
                break;
            case OP_LESS: 
            case OP_GREATER: 
            case OP_LESS_EQUAL:
            case OP_GREATER_EQUAL:
            case OP_EQUAL:
            case OP_NOT_EQUAL: {
                is_legal &= operand2->checkSemantics();
                if (!is_legal) return is_legal;
                if ((!isTypeInt(operand1->return_type) && !isTypeReal(operand1->return_type) &&
                     !isTypeChar(operand1->return_type)) ||
                    (!isTypeInt(operand2->return_type) && !isTypeReal(operand2->return_type) &&
                     !isTypeChar(operand1->return_type))) {
                    errorBroadcast("Semantics Error: The type of operands with comparation binary operator must be integer, real or char.", true, this->line);
                    is_legal = false;
                } else {
                    return_type = new Type(T_BOOL);
                }
            }
                break;
            
            default: {
                errorBroadcast("Semantics Error: There is something wrong. This operator type is unrecognized.", true, this->line);
                is_legal = false;
            }
        }
    return is_legal;
}

Type* copyType(Type * t) {
    return new Type(t->base_type);
}

bool UnaryExp::checkSemantics() {
    infoBroadcast("Checking UnaryExp..", this->line);
    is_legal = operand->checkSemantics();
    if (is_legal)
        switch (op_code) {
            case OP_DEC:
            case OP_MINUS:
            case OP_INC: {
                if (!isTypeInt(operand->return_type) && !isTypeReal(operand->return_type)) {
                    errorBroadcast("Semantics Error: The type of an operand with an unary operator \'++(--)\' must be integer or real.", true, this->line);
                    is_legal = false;
                } else {
                    return_type = copyType(operand->return_type);
                }
            }
                break;
            case OP_NOT: {
                if (!isTypeBoolean(operand->return_type)) {
                    errorBroadcast("Semantics Error: The type of an operand with an unary operator \'!\' must be boolean.", true, this->line);
                    is_legal = false;
                } else {
                    return_type = new Type(T_BOOL);
                }
            }
            break;

            default: {
                errorBroadcast("Semantics Error: There is something wrong. This operator type is unrecognised.", true, this->line);
                is_legal = false;
            }
        }
    return is_legal;
}

/*not finish yet just return*/

bool ConstantExp::checkSemantics(){
    this->return_type = new Type(this->value->base_type);
    return is_legal;
}

bool VariableExp::checkSemantics(){
    VariableMap* m = map_stack.top();
    if(m->myMap.count(this->name))
        this->return_type = m->myMap[this->name];
    else if(global->myMap.count(this->name)){
            this->return_type = global->myMap[this->name];
    } else {
        this->is_legal = false;
        errorBroadcast("VariableExp: " + this->name + " , undefined variable.", true, this->line);
    }
    return is_legal;
}

bool ArrayExp::checkSemantics(){

    // check index
    if(this->index.size() == 0 || this->index.size() > 1){
        is_legal = false;
        errorBroadcast("Error: ArrayExp: " + this->name + " only support one dimension.", true, this->line);
        return is_legal;
    }
    // whether index is expression statement 
    this->index[0]->checkSemantics();
    if(this->index[0]->node_type != N_EXP_STM) {
        is_legal =false;
        errorBroadcast("ArrayExp: " + this->name, false, this->line);
        return is_legal;
    }
    // whether index is constant expression
    if(((ExpStm*)(this->index[0]))->exps[0]->node_type != N_CONSTANT_EXP) {
        is_legal =false;
        errorBroadcast("ArrayExp: " + this->name, false, this->line);
        return is_legal;
    }
    // whether index is int or not
    ConstantExp *c = (ConstantExp *)(((ExpStm*)(this->index[0]))->exps[0]);
    if(c->value->base_type != V_INT){
        is_legal =false;
        errorBroadcast("ArrayExp: " + this->name, false, this->line);
        return is_legal;
    }

    // check variable
    VariableMap* m = map_stack.top();
    if(m->myMap.count(this->name))
        this->return_type = m->myMap[this->name];
    else if(global->myMap.count(this->name)){
            this->return_type = global->myMap[this->name];
    } else {
        this->is_legal = false;
        errorBroadcast("ArrayExp: " + this->name, false, this->line);
    }

    return is_legal;
}

bool FuncExp::checkSemantics(){
    infoBroadcast("Checking Function call: " + this->name, this->line);
    // check name
    if(func_map.count(this->name) == 0){
        is_legal = false;
        errorBroadcast("Error: No matching function defined.", true, this->line);
        return is_legal;
    }
    
    // check para list length
    int para_len = this->argu.size();
    if(func_map[this->name].size() != para_len){
        is_legal = false;
        errorBroadcast("Error: parameter mismatch in function: " + this->name, true, this->line);
    }

    // check para type
    for(auto item : this->argu){
        item->checkSemantics();
    }

    for(int i  = 0; i < para_len; i++){
        if(!isTypeEqual(func_map[this->name][i], this->argu[i]->return_type)){
            this->is_legal = false;
            errorBroadcast("Error: Function: " + this->name + " is given a parameter with wrong type.", true, this->line);
            break;
        }
    }

    // set return type
    this->return_type = global->myMap[this->name];
    
    return is_legal;
}

bool DecItem::checkSemantics(){

    return is_legal;
}

bool ParaItem::checkSemantics(){
    return is_legal;
}

bool DeclaratorArray::checkSemantics(){
    return is_legal;
}

bool DeclaratorFunc::checkSemantics(){
    return is_legal;
}

bool Declarator::checkSemantics(){
    return is_legal;
}

bool IDList::checkSemantics(){
    return is_legal;
}

bool ParaList::checkSemantics(){
    return is_legal;
}

bool Initializer::checkSemantics(){
    return is_legal;
}

bool Dec::checkSemantics(){
    infoBroadcast("Checking Declaration...", this->line);

    Type *t = this->type;
    VariableMap* m = map_stack.top();
    for(auto item : this->dec_list){
        // add key-value to map
        if(m->myMap.count(item->declarator->name) > 0){
            is_legal = false;
            errorBroadcast("Error: Variable: " + item->declarator->name + " is already defined.", true, this->line);
            break;
        }
        m->myMap[item->declarator->name] = t;

        // check variable and initializer
        if(item->initializer == nullptr)
            continue;

        // check array
        if(item->declarator->d_type == D_ARRAY && item->initializer->init_type == I_EXP){
            is_legal = false;
        }
        if(item->declarator->d_type == D_ID && item->initializer->init_type == I_ARRAY) {
            is_legal = false;
        }
        if(!is_legal){
            errorBroadcast("Error: variable and initializer mismatch.", true, this->line);
            break;
        }

        // check variable type
        if(item->initializer->init_type == I_EXP){
            item->initializer->assign_exp->checkSemantics();
            if(!isTypeEqual(t, item->initializer->assign_exp->return_type)){
                cout << t->base_type << item->initializer->assign_exp->return_type->base_type << endl;
                is_legal = false;
                errorBroadcast("Error: initializer type mismatch.", true, this->line);
            }
        } else {
            for(auto v : item->initializer->assign_array){
                v->assign_exp->checkSemantics();
                if(!isTypeEqual(t, v->assign_exp->return_type)){
                    is_legal = false;
                    errorBroadcast("Error: initializer type mismatch.", true, this->line);
                }
            }
        }
        
    }
    return is_legal;
}


bool SwitchStm::checkSemantics(){

    is_legal &= this->body->checkSemantics();

    return is_legal;
}

bool CaseStm::checkSemantics(){

    infoBroadcast("Check Case statement...", this->line);
    
    this->value->checkSemantics();
    
    if(this->value->node_type != N_CONSTANT_EXP){
        is_legal = false;
    }

    if(!is_legal)
        errorBroadcast("Error: case must be constant value.", true, this->line);

    return is_legal;
}