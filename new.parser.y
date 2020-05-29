%{
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <string>
#include <iostream>
#include "tree.h"

using namespace std;
using namespace tree;

extern char yytext[];
extern int column;
extern FILE * yyin;
extern FILE * yyout;
extern string text;

int yylex(void);
void yyerror(const char*); 

tree::Program* ast_root;

%}

%code requires{
	#include "tree.h"
	using namespace tree;
}

%start program

%union{
	Program* root;
	Base *base;
	Stm* stm;
	Exp* exp;
	FuncDec* funcDec;
	Dec* dec;
	Declarator* declarator;
	DecItem* decItem;
	ParaList* paraList;
	ParaItem* paraItem;
	IDList* idList;
	Initializer* init;
	Block *block;
	ExpStm *expStm;
	CaseStm *caseStm;
	JumpStm *jumpStm;
	AssignExp *assignExp;
	UnaryExp *unaryExp;
	BinaryExp *binaryExp;
	Type* type;
	Value* value;
}

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF CONSTANT_INT CONSTANT_DOUBLE
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token CHAR INT DOUBLE VOID BOOL 

%token CASE IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%token TRUE FALSE

%token ';' ',' ':' '=' '[' ']' '.' '&' '!' '~' '-' '+' '*' '/' '%' '<' '>' '^' '|' '?' '{' '}' '(' ')'

%type <exp> primary_expression postfix_expression argument_expression_list unary_expression unary_operator
%type <exp> multiplicative_expression additive_expression shift_expression relational_expression equality_expression
%type <exp> and_expression exclusive_or_expression inclusive_or_expression logical_and_expression logical_or_expression
%type <exp> assignment_expression

%type <dec> declaration init_declarator_list
%type <decItem> init_declarator
%type <paraList> parameter_list
%type <paraItem> parameter_declaration
%type <idList> identifier_list

%type <init> initializer initializer_list

%type <type> type_specifier

%type <stm> statement selection_statement iteration_statement

%type <expStm> expression_statement expression
%type <caseStm> labeled_statement
%type <jumpStm> jump_statement

%type <declarator> declarator

%type <block> compound_statement block_item_list

%type <root> translation_unit

%type <base> external_declaration function_definition block_item 

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%

program: 
	translation_unit {
		ast_root = $1;
	}
	;

/*基本表达式*/
primary_expression: 
	IDENTIFIER {
		string id = text;
		$$ = new VariableExp(id);
	}
	|
	TRUE {
	}
	|
	FALSE {
	}
	| CONSTANT_INT {
		int num = atoi(text.c_str());
		Value* v = new Value();
		v->base_type = V_INT;
		v->val.integer_value = num;
		$$ = new ConstantExp(v);
	}
	| CONSTANT_DOUBLE {
	}
	| '(' expression ')'{
	}
	;

/*后缀表达式*/
postfix_expression:
	primary_expression{
		$$ = $1;
	}
	| 	postfix_expression '[' expression ']'{
	}
	| 	postfix_expression '(' ')'{
	}
	| 	postfix_expression '(' argument_expression_list ')'{
	}
	| 	postfix_expression INC_OP{
	}
	| 	postfix_expression DEC_OP{
	}
	;

argument_expression_list:
	assignment_expression{
	}
	| 	argument_expression_list ',' assignment_expression {
	}
	;

/*一元表达式*/
unary_expression:
	postfix_expression{
		$$ = $1;
	}
	| 	INC_OP unary_expression{
		$$ = new UnaryExp(OP_INC, $2);
	}
	| 	DEC_OP unary_expression{
		$$ = new UnaryExp(OP_DEC, $2);
	}
	| 	unary_operator unary_expression{
	}
	;

/*单目运算符*/
unary_operator:
	'+' {
	}
	| '-' {
	}
	| '~' {
	}
	| '!' {
	}
	;

/*可乘表达式*/
multiplicative_expression:
	unary_expression {
		$$ = $1;
	}
	| multiplicative_expression '*' unary_expression {
		$$ = new BinaryExp(OP_MUL, $1, $3);
	}
	| multiplicative_expression '/' unary_expression {
		$$ = new BinaryExp(OP_DIV, $1, $3);
	}
	| multiplicative_expression '%' unary_expression {
		$$ = new BinaryExp(OP_MOD, $1, $3);
	}
	;

/*可加表达式*/
additive_expression:
	multiplicative_expression  {
		$$ = $1;
	}
	| additive_expression '+' multiplicative_expression {
		$$ = new BinaryExp(OP_ADD, $1, $3);
	}
	| additive_expression '-' multiplicative_expression {
		$$ = new BinaryExp(OP_MINUS, $1, $3);
	}
	;

/*左移右移*/
shift_expression:
	additive_expression {
		$$ = $1;
	}
	| shift_expression LEFT_OP additive_expression {
	}
	| shift_expression RIGHT_OP additive_expression {
	}
	;

/*关系表达式*/
relational_expression:
	shift_expression {
		$$ = $1;
	}
	| relational_expression '<' shift_expression {
		$$ = new BinaryExp(OP_LESS, $1, $3);
	}
	| relational_expression '>' shift_expression {
		$$ = new BinaryExp(OP_GREATER, $1, $3);
	}
	| relational_expression LE_OP shift_expression {
		$$ = new BinaryExp(OP_LESS_EQUAL, $1, $3);
	}
	| relational_expression GE_OP shift_expression {
		$$ = new BinaryExp(OP_GREATER_EQUAL, $1, $3);
	}
	;

/*相等表达式*/
equality_expression:
	relational_expression {
		$$ = $1;
	}
	| equality_expression EQ_OP relational_expression {
		$$ = new BinaryExp(OP_EQUAL, $1, $3);
	}
	| equality_expression NE_OP relational_expression {
		$$ = new BinaryExp(OP_NOT_EQUAL, $1, $3);
	}
	;

and_expression:
	equality_expression {
		$$ = $1;
	}
	| and_expression '&' equality_expression {
		$$ = new BinaryExp(OP_AND, $1, $3);
	}
	;

/*异或*/
exclusive_or_expression:
	and_expression {
		$$ = $1;
	}
	| exclusive_or_expression '^' and_expression {
		$$ = new BinaryExp(OP_EXCLUSIVE_OR, $1, $3);
	}
	;

/*或*/
inclusive_or_expression:
	exclusive_or_expression {
		$$ = $1;
	}
	| inclusive_or_expression '|' exclusive_or_expression {
		$$ = new BinaryExp(OP_OR, $1, $3);
	}
	;

/*and逻辑表达式*/
logical_and_expression:
	inclusive_or_expression {
		$$ = $1;
	}
	| logical_and_expression AND_OP inclusive_or_expression {
		$$ = new BinaryExp(OP_AND, $1, $3);
	}
	;

/*or 逻辑表达式*/
logical_or_expression:
	logical_and_expression {
		$$ = $1;
	}
	| logical_or_expression OR_OP logical_and_expression {
		$$ = new BinaryExp(OP_OR, $1, $3);
	}
	;

/*赋值表达式*/
assignment_expression:
	logical_or_expression {
		$$ = $1;
	}
	| unary_expression assignment_operator assignment_expression {
		$$ = new AssignExp($1, $3);
	}
	;

/*赋值运算符*/
assignment_operator:
	'=' {
	}
	| MUL_ASSIGN {
	}
	| DIV_ASSIGN {
	}
	| MOD_ASSIGN {
	}
	| ADD_ASSIGN {
	}
	| SUB_ASSIGN {
	}
	| LEFT_ASSIGN {
	}
	| RIGHT_ASSIGN {
	}
	| AND_ASSIGN {
	}
	| XOR_ASSIGN {
	}
	| OR_ASSIGN {
	}
	;

/*表达式*/
expression:
	assignment_expression {
		$$ = new ExpStm();
		$$->addExp($1);
	}
	| expression ',' assignment_expression {
		$$ = $1;
		$$->addExp($3);
	}
	;


declaration:
	type_specifier ';' {

	}
	| type_specifier init_declarator_list ';' {
		$$ = $2;
		$$->setType($1);
	}
	;


init_declarator_list:
	init_declarator {
		$$ = new Dec();
		$$->addDecItem($1);
	}
	| init_declarator_list ',' init_declarator {
		$$ = $1;
		$$->addDecItem($3);
	}
	;

init_declarator:
	declarator {
		$$ = new DecItem($1, nullptr);
	}
	| declarator '=' initializer {
		$$ = new DecItem($1, $3);
	}
	;


/*类型说明符*/
type_specifier:
	VOID {
		$$ = new Type(T_VOID);
	}
	| CHAR {
		$$ = new Type(T_CHAR);
	}
	| INT {
		$$ = new Type(T_INTEGER);
	}
	| DOUBLE {
		$$ = new Type(T_DOUBLE);
	}
	| BOOL {
		$$ = new Type(T_BOOL);
	}
	;



declarator:
	IDENTIFIER {
		// variable
		string id = text;
		$$ = new Declarator(D_ID, id);
	}
	| '(' declarator ')' {
	}
	| declarator '[' assignment_expression ']' {
		// array of fixed size
		if($1->d_type == D_ID){
			$$ = new DeclaratorArray(D_ARRAY, $1->name);
			delete($1);
		} else {
			$$ = $1;
		}
		((DeclaratorArray *)$$)->addArraySize($3);
	}
	| declarator '[' '*' ']' {
	}
	| declarator '[' ']' {
		// array of unfixed size : defined by initializer
		if($1->d_type == D_ID){
			$$ = new DeclaratorArray(D_ARRAY, $1->name);
			delete($1);
		} else {
			$$ = $1;
		}
		((DeclaratorArray*)$$)->addArraySize(nullptr);
	}
	| declarator '(' parameter_list ')' {
		if($1->d_type == D_ID) {
			$$ = new DeclaratorFunc(D_FUNC_DEF, $1->name);
			delete($1);
		} else {
			cout << "Error node type of declarator () " << endl;
		}
		((DeclaratorFunc*)$$)->setParaDef($3);
	}
	| declarator '(' identifier_list ')' {
		if($1->d_type == D_ID) {
			$$ = new DeclaratorFunc(D_FUNC_CALL, $1->name);
			delete($1);
		} else {
			cout << "Error node type of declarator () " << endl;
		}
		((DeclaratorFunc*)$$)->setParaCall($3);
	}
	| declarator '(' ')' {
		if($1->d_type == D_ID) {
			$$ = new DeclaratorFunc(D_FUNC_EMPTY, $1->name);
			delete($1);
		} else {
			cout << "Error node type of declarator () " << endl;
		}
	}
	;


//参数列表
parameter_list:
	parameter_declaration {
		$$ = new ParaList();
		$$->addPara($1);
	}
	| parameter_list ',' parameter_declaration {
		$$ = $1;
		$$->addPara($3);
	}
	;

parameter_declaration:
	type_specifier declarator {
		$$ = new ParaItem($1, $2);
	}
	| type_specifier abstract_declarator {
	}
	| type_specifier {
	}
	;

identifier_list:
	IDENTIFIER {
		string id = text;
		VariableExp* e = new VariableExp(id);
		$$ = new IDList();
		$$->addID(e);
	}
	| identifier_list ',' IDENTIFIER {
		string id = text;
		VariableExp* e = new VariableExp(id);
		$$ = $1;
		$$->addID(e);
	}
	;

abstract_declarator:
	'(' abstract_declarator ')' {
	}
	| '[' ']' {
	}
	| '[' assignment_expression ']' {
	}
	| abstract_declarator '[' ']' {
	}
	| abstract_declarator '[' assignment_expression ']' {
	}
	| '[' '*' ']' {
	}
	| abstract_declarator '[' '*' ']' {
	}
	| '(' ')' {
	}
	| '(' parameter_list ')' {
	}
	| abstract_declarator '(' ')' {
	}
	| abstract_declarator '(' parameter_list ')' {
	}
	;

//初始化
initializer:
	assignment_expression {
		$$ = new Initializer(I_EXP);
		$$->assign_exp = $1;
	}
	| '{' initializer_list '}' {
		$$ = $2;
	}
	| '{' initializer_list ',' '}' {
	}
	;

initializer_list:
	initializer {
		$$ = new Initializer(I_ARRAY);
		$$->addArrayItem($1);
	}
	| designation initializer {
	}
	| initializer_list ',' initializer {
		$$ = $1;
		$$->addArrayItem($3);
	}
	| initializer_list ',' designation initializer {
	}
	;

designation:
	designator_list '=' {
	}
	;

designator_list:
	designator {
	}
	| designator_list designator {
	}
	;

designator: 
	'[' logical_or_expression ']' {
	}
	| '.' IDENTIFIER {
	}
	;

//声明
statement:
	labeled_statement {
	}
	| compound_statement {
		$$ = $1;
	}
	| expression_statement{
		$$ = $1;
	}
	| selection_statement {
		$$ = $1;
	}
	| iteration_statement {
		$$ = $1;
	}
	| jump_statement {
		$$ = $1;
	}
	;

//标签声明
labeled_statement:
	IDENTIFIER ':' statement {

	}
	| CASE logical_or_expression ':' statement {
		$$ = new CaseStm($2, $4);
	}
	;

//复合语句
compound_statement:
	'{' '}' {
		// empty block
	}
	| '{' block_item_list '}' {
		$$ = $2;
	}
	;

block_item_list:
	block_item {
		$$ = new Block();
		if($1->node_type == N_DEC){
			$$->addDec((Dec *)$1);
		} else if(isStm($1)) {
			$$->addStm((Stm *)$1);
		} else {
			cout << "Error! Wrong Node Type!" << endl;
		}
	}
	| block_item_list block_item {
		$$ = $1;
		if($2->node_type == N_DEC){
			$$->addDec((Dec *)$2);
		} else if(isStm($2)) {
			$$->addStm((Stm *)$2);
		} else {
			cout << "Error! Wrong Node Type! of block_item_list" << endl;
		}
	}
	;

block_item:
	declaration {
		$$ = $1;
	}
	| statement {
		$$ = $1;
	}
	;

expression_statement:
	';' {
		$$ = new ExpStm();
	}
	| expression ';' {
		$$ = $1;
	}
	;

//条件语句
selection_statement:
	IF '(' assignment_expression ')' statement %prec LOWER_THAN_ELSE {
		$$ = new SelectStm($3, $5, nullptr);
	}
    | IF '(' assignment_expression ')' statement ELSE statement {
		$$ = new SelectStm($3, $5, $7);
	}
    | SWITCH '(' assignment_expression ')' statement {
		$$ = new SwitchStm($3, $5);
	}
    ;

//循环语句
iteration_statement:
	WHILE '(' expression ')' statement {
		$$ = new WhileStm($3, $5, false);
	}
	| DO statement WHILE '(' expression ')' ';' {
		$$ = new WhileStm($5, $2, true);
	}
	| FOR '(' expression_statement expression_statement ')' statement {
		$$ = new ForStm($3, $4, nullptr, $6);
	}
	| FOR '(' expression_statement expression_statement expression ')' statement {
		$$ = new ForStm($3, $4, $5, $7);
	}
	| FOR '(' declaration expression_statement ')' statement {
		$$ = new ForStm($3, $4, nullptr, $6);
	}
	| FOR '(' declaration expression_statement expression ')' statement {
		$$ = new ForStm($3, $4, $5, $7);
	}
	;

//跳转指令
jump_statement:
	GOTO IDENTIFIER ';' {
	}
	| CONTINUE ';' {
		$$ = new JumpStm(J_CONTINUE);
	}
	| BREAK ';' {
		$$ = new JumpStm(J_BREAK);
	}
	| RETURN ';' {
		$$ = new JumpStm(J_RETURN);
	}
	| RETURN expression ';' {
		$$ = new JumpStm(J_RETURN);
		$$->setReturnVal($2);
	}
	;

translation_unit:
	external_declaration {
		$$ = new Program("Program");
		if($1->node_type == N_FUNC_DEC){
			$$->addFunc((FuncDec *)$1);
		} else if($1->node_type == N_DEC) {
			$$->addDec((Dec *)$1);
		} else {
			cout << "Error! Wrong Type of Node." << endl;
		}
	}
	| translation_unit external_declaration {
		$$ = $1;
		if($2->node_type == N_FUNC_DEC){
			$$->addFunc((FuncDec *)$2);
		} else if($2->node_type == N_DEC) {
			$$->addDec((Dec *)$2);
		} else {
			cout << "Error! Wrong Type of Node." << endl;
		}
	}
	;

external_declaration:
	function_definition {
		$$ = $1;
	}
	| declaration {
		$$ = $1;
	}
	;

function_definition:
	type_specifier declarator declaration_list compound_statement {

	}
	| type_specifier declarator compound_statement {
		// maybe check declarator
		$$ = new FuncDec($2->name, $1, $2, $3);
	}
	;

declaration_list:
	declaration {
	}
	| declaration_list declaration {
	}
	;

%%


void yyerror(char const *s)
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}


int main(int argc,char* argv[]) {

	yyin = fopen(argv[1],"r");
	
	yyparse();

	ast_root->print(0);

	fclose(yyin);
	return 0;
}