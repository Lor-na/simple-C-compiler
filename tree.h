#ifndef TREE_H
#define TREE_H
// #include <llvm/IR/Value.h>
#include <string>
#include <vector>
#include <iostream>
#include "macro.h"

using namespace std;

class CodeGenContext;
namespace tree{
class Base;
class Stm;
class Exp;

class Program;

// declaration
class FuncDec;
class Dec;
class Declarator;
class DecItem;
class Initializer;

// compound statement
class Block;

// statement
class ExpStm;
class SelectStm;
class SwitchStm;
class CaseStm;

// expression
class AssignExp;
class UnaryExp;
class BinaryExp;
class ConstantExp;
class VariableExp;

// basic class
class Type;
class Value;


class Base {
public:
    int node_type;
    Base *father = nullptr;
    bool is_legal = true;
    Base(int type = 0) : node_type(type) {}
    virtual void print(int temp_height) {}

    // virtual llvm::Value *codeGen(CodeGenContext *context) = 0;
    // virtual bool checkSemantics() = 0;
};

class Stm : public Base {
public:
    Stm(int type = 0) : Base(type) {}
    // virtual void print(int temp_height);
    // virtual llvm::Value *codeGen(CodeGenContext *context) = 0;
};

class Exp : public Base {
public:
    Value *return_value = nullptr;
    Type *return_type = nullptr;
    Exp(int type = 0) : Base(type) {}
    // virtual void print(int temp_height);
    // virtual llvm::Value *codeGen(CodeGenContext *context) = 0;
};

class Program : public Base {
public:
    string name;
    vector<FuncDec*> func_dec;
    vector<Dec*> dec;
    Program(const string &_name) : Base(N_PROGRAM), name(_name) {}
    void addFunc(FuncDec *);
    void addDec(Dec *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class FuncDec : public Base {
public:
    string name;
    // vector<Type *> args_type;
    // vector <string> args_name;
    // vector<bool> args_is_formal_parameters; //true:&, false:local
    Type *return_type; // procedure == nullptr
    Block* block;
    
    FuncDec(const string &_name, Type *_type, Block *_block) : Base(N_FUNC_DEC), name(_name), return_type(_type), block(_block) {}
    // void addArgs(const string &, Type *, bool);
    // void setReturnType(Type *);
    // void addBlock(Block *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class Dec : public Base {
public:
    Type *type;
    vector<DecItem*> dec_list;

    Dec() : Base(N_DEC), type() {}

    void setType(Type *);
    void addDecItem(DecItem *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class DecItem : public Base{
public:
    Declarator * declarator;
    Initializer * initializer;

    DecItem(Declarator *_declarator, Initializer *_initializer) : Base(N_DEC_ITEM), declarator(_declarator), initializer(_initializer) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class Initializer : public Base {
public:
    int init_type;  // I_EXP:assignment expression; I_ARRAY:such as {1,2,3}
    // A union should be used here, but i can't give it a right construction function
    // assign_exp and assign_array, only one of them will be used 
    Exp* assign_exp;
    vector<Initializer *> assign_array;

    Initializer(int _init_type) : Base(N_INITIAL), init_type(_init_type) {}

    void addArrayItem(Initializer *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class Declarator : public Base{
public:
    int d_type;
    string name;

    Declarator(int _d_type, const string &_name) : Base(N_DECLARATOR), d_type(_d_type), name(_name){}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class Block : public Stm {
public:
    vector<Dec*> dec;
    vector<Stm*> stm;

    Block() : Stm(N_BLOCK) {}
    
    void addStm(Stm *);
    void addDec(Dec *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class ExpStm : public Stm{
public:
    vector <Exp *> exps;

    ExpStm() : Stm(N_EXP_STM) {}

    void addExp(Exp *);

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class SelectStm : public Stm{
public:
    Exp* condition;
    Stm* if_do;     // maybe normal statement, maybe a block. can be distinguished by if_do->node_type == N_BLOCK
    Stm* else_do;   // the same as if_do and else_do maybe nullptr

    SelectStm(Exp* _cond, Stm* _if_do, Stm* _else_do) : Stm(N_SELECT_STM), condition(_cond), if_do(_if_do), else_do(_else_do) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class SwitchStm : public Stm{
public:
    Exp* condition;
    Stm* body;  // must be a block

    SwitchStm(Exp* _cond, Stm* _body) : Stm(N_SWITCH_STM), condition(_cond), body(_body) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class CaseStm : public Stm {
public:
    Exp* value;
    Stm* if_do;

    CaseStm(Exp* _value, Stm* _if_do) : Stm(N_CASE_STM), value(_value), if_do(_if_do) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class AssignExp : public Exp {
public:
    Exp* left_exp;
    Exp* right_exp;

    AssignExp(Exp* left, Exp* right) : Exp(N_ASSIGN_EXP), left_exp(left), right_exp(right) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class UnaryExp : public Exp {
public:
    int op_code;
    Exp *operand;
    UnaryExp(int _op_code, Exp * _operand) : Exp(N_UNARY_EXP), op_code(_op_code), 
                                             operand(_operand) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class BinaryExp : public Exp {
public:
    int op_code;
    Exp *operand1, *operand2;
    BinaryExp(int _op_code, Exp *_operand1, Exp *_operand2) : Exp(N_BINARY_EXP), op_code(_op_code),
                            operand1(_operand1), operand2(_operand2) { }

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class ConstantExp : public Exp {
public:
    Value *value;
    ConstantExp(Value *_value) : Exp(N_CONSTANT_EXP), value(_value) { return_value = _value; }

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};

class VariableExp : public Exp {
public:
    string name;
    VariableExp(const string &_name) : Exp(N_VARIABLE_EXP), name(_name) {}

    virtual void print(int temp_height);

    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override;
};


class Type : public Base {
public:
    string name; // use what name to find this value, may be empty
    int base_type; // 0: int 1: real 2: char 3: bool 5: array 6: record
    // int array_start = 0, array_end = 0; // the index for array. useless if the type is not an array
    // vector<Type *> child_type; // a list of the type of children, there is only one child if the type is array
    Type() : Base(N_TYPE) {}
    Type(int _base_type) : Base(N_TYPE), base_type(_base_type) {}

    virtual void print(int temp_height);

    // llvm::Type *toLLVMType(CodeGenContext& context);
    // virtual llvm::Value *codeGen(CodeGenContext *context) override;
    // bool checkSemantics() override { return false; }
};

class Value {
public:
    int base_type; // 0: int 1: double 2: char 3: bool 5: array 6: record
    union return_value {
        int integer_value;
        double double_value;
        char char_value;
        bool bool_value;
        string *string_value;
        // vector<Value *> *children_value; // a list of the value of children
    } val;

    void print(int temp_height);

    // llvm::Value *codeGen(CodeGenContext *context);
};

bool isStm(Base *);
bool isExp(Base *);

}

#endif