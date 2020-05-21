%{
#include <cstdlib>
#include <cstdio>
#include "tree.h"

extern char yytext[];
extern int column;
extern FILE * yyin;
extern FILE * yyout;

treeNode *root;

int yylex();
void yyerror(const char*);

%}

%union{
	treeNode *node;
}

%token IDENTIFIER CONSTANT STRING_LITERAL SIZEOF
%token PTR_OP INC_OP DEC_OP LEFT_OP RIGHT_OP LE_OP GE_OP EQ_OP NE_OP
%token AND_OP OR_OP MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN ADD_ASSIGN
%token SUB_ASSIGN LEFT_ASSIGN RIGHT_ASSIGN AND_ASSIGN
%token XOR_ASSIGN OR_ASSIGN TYPE_NAME

%token TYPEDEF EXTERN STATIC AUTO REGISTER
%token CHAR SHORT INT LONG SIGNED UNSIGNED FLOAT DOUBLE CONST VOLATILE VOID
%token STRUCT UNION ENUM ELLIPSIS

%token CASE DEFAULT IF ELSE SWITCH WHILE DO FOR GOTO CONTINUE BREAK RETURN

%type <node> declaration declaration_specifiers init_declarator_list init_declarator type_specifier declarator direct_declarator
%type <node> program translation_unit

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start translation_unit
%%

primary_expression
	: IDENTIFIER
	| CONSTANT
	| STRING_LITERAL
	| '(' expression ')'
	;

postfix_expression
	: primary_expression
	| postfix_expression '[' expression ']'
	| postfix_expression '(' ')'
	| postfix_expression '(' argument_expression_list ')'
	| postfix_expression '.' IDENTIFIER
	| postfix_expression PTR_OP IDENTIFIER
	| postfix_expression INC_OP
	| postfix_expression DEC_OP
	;

argument_expression_list
	: assignment_expression
	| argument_expression_list ',' assignment_expression
	;

unary_expression
	: postfix_expression
	| INC_OP unary_expression
	| DEC_OP unary_expression
	| unary_operator cast_expression
	| SIZEOF unary_expression
	| SIZEOF '(' type_name ')'
	;

unary_operator
	: '&'
	| '*'
	| '+'
	| '-'
	| '~'
	| '!'
	;

cast_expression
	: unary_expression
	| '(' type_name ')' cast_expression
	;

multiplicative_expression
	: cast_expression
	| multiplicative_expression '*' cast_expression
	| multiplicative_expression '/' cast_expression
	| multiplicative_expression '%' cast_expression
	;

additive_expression
	: multiplicative_expression
	| additive_expression '+' multiplicative_expression
	| additive_expression '-' multiplicative_expression
	;

shift_expression
	: additive_expression
	| shift_expression LEFT_OP additive_expression
	| shift_expression RIGHT_OP additive_expression
	;

relational_expression
	: shift_expression
	| relational_expression '<' shift_expression
	| relational_expression '>' shift_expression
	| relational_expression LE_OP shift_expression
	| relational_expression GE_OP shift_expression
	;

equality_expression
	: relational_expression
	| equality_expression EQ_OP relational_expression
	| equality_expression NE_OP relational_expression
	;

and_expression
	: equality_expression
	| and_expression '&' equality_expression
	;

exclusive_or_expression
	: and_expression
	| exclusive_or_expression '^' and_expression
	;

inclusive_or_expression
	: exclusive_or_expression
	| inclusive_or_expression '|' exclusive_or_expression
	;

logical_and_expression
	: inclusive_or_expression
	| logical_and_expression AND_OP inclusive_or_expression
	;

logical_or_expression
	: logical_and_expression
	| logical_or_expression OR_OP logical_and_expression
	;

conditional_expression
	: logical_or_expression
	| logical_or_expression '?' expression ':' conditional_expression
	;

/*表达式 优先级就是当前的顺序 从下往上看*/
assignment_expression
	: conditional_expression
	| unary_expression assignment_operator assignment_expression
	;

assignment_operator
	: '='
	| MUL_ASSIGN
	| DIV_ASSIGN
	| MOD_ASSIGN
	| ADD_ASSIGN
	| SUB_ASSIGN
	| LEFT_ASSIGN
	| RIGHT_ASSIGN
	| AND_ASSIGN
	| XOR_ASSIGN
	| OR_ASSIGN
	;

expression
	: assignment_expression
	| expression ',' assignment_expression
	;

constant_expression
	: conditional_expression
	;

/*声明*/
declaration
	: declaration_specifiers ';'
	| declaration_specifiers init_declarator_list ';'
	{
		root = new treeNode("declaration");
		root->addChild($1);
		root->addChild($2);
	}
	;

/*定义前缀*/
declaration_specifiers
	/*typedef*/
	: storage_class_specifier
	{
		$$ = NULL;
	}
	| storage_class_specifier declaration_specifiers
	{
		$$ = NULL;
	}
	/*int char double*/
	| type_specifier
	{
		$$ = new treeNode("declaration_specifiers");
		$$->addChild($1);
	}
	| type_specifier declaration_specifiers
	{
		$$ = NULL;
	}
	/*const*/
	| type_qualifier
	{
		$$ = NULL;
	}
	| type_qualifier declaration_specifiers
	{
		$$ = NULL;
	}
	;

init_declarator_list
	: init_declarator
	{
		$$ = $1;
	}
	| init_declarator_list ',' init_declarator
	;

/*单个变量，有可能带有赋值 如 int a=0*/
init_declarator
	: declarator
	{
		$$ = $1;
	}
	| declarator '=' initializer
	;

storage_class_specifier
	: TYPEDEF
	| EXTERN
	| STATIC
	| AUTO
	| REGISTER
	;

/*变量类型*/
type_specifier
	: VOID
	{
		$$ = NULL;
	}
	| CHAR
	{
		$$ = NULL;
	}
	| SHORT
	{
		$$ = NULL;
	}
	| INT
	{
		$$ = new treeNode("int");
	}
	| LONG
	{
		$$ = NULL;
	}
	| FLOAT
	{
		$$ = NULL;
	}
	| DOUBLE
	{
		$$ = NULL;
	}
	| SIGNED
	{
		$$ = NULL;
	}
	| UNSIGNED
	{
		$$ = NULL;
	}
	| struct_or_union_specifier
	{
		$$ = NULL;
	}
	| enum_specifier
	{
		$$ = NULL;
	}
	| TYPE_NAME
	{
		$$ = NULL;
	}
	;

struct_or_union_specifier
	: struct_or_union IDENTIFIER '{' struct_declaration_list '}'
	| struct_or_union '{' struct_declaration_list '}'
	| struct_or_union IDENTIFIER
	;

struct_or_union
	: STRUCT
	| UNION
	;

struct_declaration_list
	: struct_declaration
	| struct_declaration_list struct_declaration
	;

struct_declaration
	: specifier_qualifier_list struct_declarator_list ';'
	;

specifier_qualifier_list
	: type_specifier specifier_qualifier_list
	{
		$1 = NULL;
	}
	| type_specifier
	{
		$1 = NULL;
	}
	| type_qualifier specifier_qualifier_list
	| type_qualifier
	;

struct_declarator_list
	: struct_declarator
	| struct_declarator_list ',' struct_declarator
	;

struct_declarator
	: declarator
	{
		$1 = NULL;
	}
	| ':' constant_expression
	| declarator ':' constant_expression
	{
		$1 = NULL;
	}
	;

enum_specifier
	: ENUM '{' enumerator_list '}'
	| ENUM IDENTIFIER '{' enumerator_list '}'
	| ENUM IDENTIFIER
	;

enumerator_list
	: enumerator
	| enumerator_list ',' enumerator
	;

enumerator
	: IDENTIFIER
	| IDENTIFIER '=' constant_expression
	;

type_qualifier
	: CONST
	| VOLATILE
	;

declarator
	: pointer direct_declarator
	{
		$$ = NULL;
	}
	| direct_declarator
	{
		$$ = $1;
	}
	;

/*变量——普通变量 数组 ()是函数的参数列表 int f(int a)*/
direct_declarator
	: IDENTIFIER
	{
		$$ = new treeNode("hhlskdjf");
	}
	| '(' declarator ')'
	{
		$$ = NULL;
	}
	| direct_declarator '[' constant_expression ']'
	{
		$$ = NULL;
	}
	| direct_declarator '[' ']'
	{
		$$ = NULL;
	}
	| direct_declarator '(' parameter_type_list ')'
	{
		$$ = NULL;
	}
	| direct_declarator '(' identifier_list ')'
	{
		$$ = NULL;
	}
	| direct_declarator '(' ')'
	{
		$$ = NULL;
	}
	;

pointer
	: '*'
	| '*' type_qualifier_list
	| '*' pointer
	| '*' type_qualifier_list pointer
	;

type_qualifier_list
	: type_qualifier
	| type_qualifier_list type_qualifier
	;


parameter_type_list
	: parameter_list
	| parameter_list ',' ELLIPSIS
	;

parameter_list
	: parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration
	: declaration_specifiers declarator
	{
	}
	| declaration_specifiers abstract_declarator
	{

	}
	| declaration_specifiers
	{

	}
	;

identifier_list
	: IDENTIFIER
	| identifier_list ',' IDENTIFIER
	;

type_name
	: specifier_qualifier_list
	| specifier_qualifier_list abstract_declarator
	;

abstract_declarator
	: pointer
	| direct_abstract_declarator
	| pointer direct_abstract_declarator
	;

direct_abstract_declarator
	: '(' abstract_declarator ')'
	| '[' ']'
	| '[' constant_expression ']'
	| direct_abstract_declarator '[' ']'
	| direct_abstract_declarator '[' constant_expression ']'
	| '(' ')'
	| '(' parameter_type_list ')'
	| direct_abstract_declarator '(' ')'
	| direct_abstract_declarator '(' parameter_type_list ')'
	;

initializer
	: assignment_expression
	| '{' initializer_list '}'
	| '{' initializer_list ',' '}'
	;

initializer_list
	: initializer
	| initializer_list ',' initializer
	;

/*各种结构语句*/
statement
	/*比如 switch case*/
	: labeled_statement
	/*嵌套大括号 不知道有啥用*/
	| compound_statement
	/**/
	| expression_statement
	| selection_statement
	| iteration_statement
	| jump_statement
	;

labeled_statement
	: IDENTIFIER ':' statement
	| CASE constant_expression ':' statement
	| DEFAULT ':' statement
	;

/*函数体*/
compound_statement
	: '{' '}'
	| '{' statement_list '}'
	| '{' declaration_list '}'
	/*定义临时变量 / 程序语句 这样写的话不支持在函数体中间定义变量了 */
	| '{' declaration_list statement_list '}'
	;

declaration_list
	: declaration
	{}
	| declaration_list declaration
	;

statement_list
	: statement
	| statement_list statement
	;

expression_statement
	: ';'
	| expression ';'
	;

selection_statement
	: IF '(' expression ')' statement %prec LOWER_THAN_ELSE
	| IF '(' expression ')' statement ELSE statement
	| SWITCH '(' expression ')' statement
	;

iteration_statement
	: WHILE '(' expression ')' statement
	| DO statement WHILE '(' expression ')' ';'
	| FOR '(' expression_statement expression_statement ')' statement
	| FOR '(' expression_statement expression_statement expression ')' statement
	;

jump_statement
	: GOTO IDENTIFIER ';'
	| CONTINUE ';'
	| BREAK ';'
	| RETURN ';'
	| RETURN expression ';'
	;

program
	: translation_unit{
		// root = new treeNode("program");
		// root->addChild($1);
		// cout << "here" << endl;
	} 
	;
/*程序起始*/
translation_unit
	: external_declaration{
		// $$ = new treeNode("translation_unit");
	}
	| translation_unit external_declaration
	;

/*外部声明——函数定义/声明*/
external_declaration
	: function_definition
	| declaration
	{}
	;
/*函数定义*/
function_definition
	/*declaration_list 不知道在这里是干嘛的*/
	: declaration_specifiers declarator declaration_list compound_statement
	{}
	/*int f (int a) {}*/
	| declaration_specifiers declarator compound_statement
	{}
	| declarator declaration_list compound_statement
	{}
	| declarator compound_statement
	{}
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

	root->treePrint();

	printf("\n");

	fclose(yyin);
	return 0;
}