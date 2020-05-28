#include "tree.h"

using namespace tree;
using namespace std;

void Program::addFunc(FuncDec * func) {
	this->func_dec.push_back(func);
} 

void Program::addDec(Dec * dec) {
	this->dec.push_back(dec);
} 

void Dec::setType(Type *type) {
	this->type = type;
}

void Dec::addDecItem(DecItem *dec_item) {
	this->dec_list.push_back(dec_item);
}

void Initializer::addArrayItem(Initializer *init_item) {
    this->assign_array.push_back(init_item);
}

void Block::addDec(Dec *dec) {
	this->dec.push_back(dec);
}

void Block::addStm(Stm *stm){
	this->stm.push_back(stm);
}

void ExpStm::addExp(Exp * exp) {
	this->exps.push_back(exp);
}

void Program::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << this->name << endl;
    for(int i = 0; i < this->dec.size(); i++){
        this->dec[i]->print(temp_height + 1);
    }
    for(int i = 0; i < this->func_dec.size(); i++){
        this->func_dec[i]->print(temp_height + 1);
    }
}

void FuncDec::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << this->name << endl;
    this->return_type->print(temp_height + 1);
    this->block->print(temp_height + 1);
}

void Dec::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Variable Declaration" << endl;
    this->type->print(temp_height + 1);
    for(int i = 0; i < this->dec_list.size(); i++){
        this->dec_list[i]->print(temp_height + 1);
    }
}

void DecItem::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Declaration Item" << endl;
    this->declarator->print(temp_height + 1);
    if(this->initializer != nullptr)
        this->initializer->print(temp_height + 1);
}

void Initializer::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Initializer" << endl;
    if(this->init_type == I_EXP){
        this->assign_exp->print(temp_height + 1);
    }else{
        for(int i = 0; i < this->assign_array.size(); i++){
            this->assign_array[i]->print(temp_height + 1);
        }
    }
}

void Declarator::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << this->name << "  declarator type:  " << this->d_type << endl;
}

void Block::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Block" << endl;
    for(int i = 0; i < this->dec.size(); i++){
        this->dec[i]->print(temp_height + 1);
    }
    for(int i = 0; i < this->stm.size(); i++){
        this->stm[i]->print(temp_height + 1);
    }
}

void ExpStm::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Expression Statement" << endl;
    for(int i = 0; i < this->exps.size(); i++){
        this->exps[i]->print(temp_height + 1);
    }
}

void SelectStm::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Select Statement" << endl;
    // print condition
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|\t" << "|---" << "Condition:" << endl;
    this->condition->print(temp_height + 1);
    // print blocks
    this->if_do->print(temp_height + 1);
    if(this->else_do != nullptr)
        this->else_do->print(temp_height + 1);
}

void AssignExp::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "AssignExp" << endl;
    this->left_exp->print(temp_height + 1);
    this->right_exp->print(temp_height + 1);
}

void UnaryExp::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "UnaryExp: operator--" << this->op_code << endl;
    this->operand->print(temp_height + 1);
}

void BinaryExp::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "BinaryExp:operator--" << this->op_code << endl;
    this->operand1->print(temp_height + 1);
    this->operand2->print(temp_height + 1);
}

void ConstantExp::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Constant:  " << endl;
    this->value->print(temp_height + 1);
}

void VariableExp::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Variable  " << this->name << endl;
}

void Type::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Type:  " << this->name << endl;
}

void Value::print(int temp_height) {
    for(int i = 0; i < temp_height; i++)
        cout << "|\t";
    cout << "|---" << "Value:  ";
    switch(base_type){
        case V_INT:
            cout << this->val.integer_value << endl; break;
        case V_DOUBLE:
            cout << this->val.double_value << endl; break;
        case V_CHAR:
            cout << this->val.bool_value << endl; break;
        case V_BOOL:
            cout << this->val.string_value << endl; break;
        default:
            cout << "Error index of union." << endl;
    }
}

bool tree::isStm(Base *b){
    if(b->node_type >= 20 && b->node_type < 40)
        return true;
    else
        return false;
}

bool tree::isExp(Base *b) {
    if(b->node_type >= 40 && b->node_type < 60)
        return true;
    else
        return false;
}
