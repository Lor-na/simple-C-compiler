#include <string>

#define N_PROGRAM 0
#define N_EXTDEC 1
#define N_FUNC_DEC 3
#define N_DEC 4
#define N_TYPE_DEF 5
#define N_DECLARATOR 6
#define N_BLOCK 7

// stm class from 20-39
#define N_EXP_STM 20

// exp class from 40-59
#define N_ASSIGN_EXP 40
#define N_BINARY_EXP 41
#define N_UNARY_EXP 42
#define N_PRIMARY_EXP 43
#define N_VARIABLE_EXP 44
#define N_CONSTANT_EXP 45

#define N_TYPE 50
#define N_VALUE 51

#define D_ID 0

#define T_INTEGER 0
#define T_DOUBLE 1
#define T_CHAR 2
#define T_BOOL 3
#define T_STRING 4
#define T_VOID 5

#define OP_ADD 0
#define OP_MINUS 1
#define OP_MUL 2
#define OP_DIV 3
#define OP_MOD 5
#define OP_AND 6
#define OP_OR 7
#define OP_LESS 8
#define OP_GREATER 9
#define OP_LESS_EQUAL 10
#define OP_GREATER_EQUAL 11
#define OP_EQUAL 12
#define OP_NOT_EQUAL 13
#define OP_DOT 14
#define OP_INDEX 15
#define OP_NULL 16
#define OP_EXCLUSIVE_OR 17
#define OP_INC 18
#define OP_DEC 19

bool isStm(Base *b){
	if(b->node_type >= 20 && b->node_type < 40)
		return true;
	else
		return false;
}

bool isExp(Base *b) {
	if(b->node_type >= 40 && b->node_type < 60)
		return true;
	else
		return false;
}