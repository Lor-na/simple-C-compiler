#include <string>

#define N_PROGRAM 0
#define N_EXTDEC 1
#define N_FUNC_DEC 3
#define N_DEC 4
#define N_TYPE_DEF 5
#define N_DECLARATOR 6
#define N_BLOCK 7
#define N_DEC_ITEM 8
#define N_INITIAL 9
#define N_PARA_LIST 10
#define N_PARA_ITEM 11
#define N_ID_LIST 12

// stm class from 20-39
#define N_EXP_STM 20
#define N_SELECT_STM 21
#define N_SWITCH_STM 22
#define N_CASE_STM 23
#define N_WHILE_STM 24
#define N_FOR_STM 25
#define N_JUMP_STM 26

// exp class from 40-59
#define N_ASSIGN_EXP 40
#define N_BINARY_EXP 41
#define N_UNARY_EXP 42
#define N_PRIMARY_EXP 43
#define N_VARIABLE_EXP 44
#define N_CONSTANT_EXP 45

#define N_TYPE 50
#define N_VALUE 51

// declarator
#define D_ID 0
#define D_ARRAY 1
#define D_FUNC_DEF 2
#define D_FUNC_CALL 3
#define D_FUNC_EMPTY 4

// initializer
#define I_EXP 0
#define I_ARRAY 1

// type
#define T_INTEGER 0
#define T_DOUBLE 1
#define T_CHAR 2
#define T_BOOL 3
#define T_STRING 4
#define T_VOID 5

// operator
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

// value type
#define V_INT 0
#define V_DOUBLE 1
#define V_CHAR 2
#define V_BOOL 3
#define V_STRING 4

// jump statement
#define J_CONTINUE 0
#define J_BREAK 1
#define J_RETURN 2