%{

#include <cstdlib>
#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cctype>
#include "macro.h"
#include "parser.hpp"
extern "C" int yylex();
extern "C" void yyerror(const char* s);

using namespace std;

void count(void);
void comment();
int check_type();
int column = 0;
string text;
int myyylineno = 0;

%}

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
E			([Ee][+-]?{D}+)
P           ([Pp][+-]?{D}+)
FS			(f|F|l|L)
IS          ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))

%%

"/*"			{ comment();/*娉ㄩ噴*/}
"//"[^\n]*      { /* consume //-comment */ }

"bool"			{ count(); return(BOOL);	}
"int"			{ count(); return(INT); }
"char"			{ count(); return(CHAR); }
"double"		{ count(); return(DOUBLE); }
"break"			{ count(); return(BREAK); }
"case"			{ count(); return(CASE); }
"continue"		{ count(); return(CONTINUE); }
"do"			{ count(); return(DO); }
"else"			{ count(); return(ELSE); }
"for"			{ count(); return(FOR); }
"goto"			{ count(); return(GOTO); }
"if"			{ count(); return(IF); }
"return"		{ count(); return(RETURN); }
"switch"		{ count(); return(SWITCH); }
"void"			{ count(); return(VOID); }
"while"			{ count(); return(WHILE); }
"true"			{ count(); return(TRUE); }
"false"			{ count(); return(FALSE); }

{L}({L}|{D})* 	{count(); text = yytext; return(check_type());/*IDENTIFIER*/ }

0[xX]{H}+{IS}?	{ count(); return(CONSTANT_INT); /*16杩涘埗*/}
0[0-7]*{IS}?	{ count(); return(CONSTANT_INT); /*8杩涘埗*/}
[1-9]{D}*{IS}?	{ count(); text = yytext; return(CONSTANT_INT); /*10杩涘埗*/}
L?'(\\.|[^\\'\n])+'		{ count(); return(CONSTANT); }
L?'(\\.|[^\\'])+'	{ count(); text = yytext; return(CONSTANT_CHAR); }

{D}+{E}{FS}?				{ count(); return(CONSTANT_DOUBLE); /*娴偣鏁?/}
{D}*"."{D}+{E}?{FS}?		{ count(); return(CONSTANT_DOUBLE); /*娴偣鏁?/}
{D}+"."{D}*{E}?{FS}?		{ count(); return(CONSTANT_DOUBLE); /*娴偣鏁?/}
0[xX]{H}+{P}{FS}?			{ count(); return(CONSTANT); }
0[xX]{H}*"."{H}+{P}?{FS}?   { count(); return(CONSTANT); }
0[xX]{H}+"."{H}*{P}?{FS}?   { count(); return(CONSTANT); }


L?\"(\\.|[^\\"\n])*\"	{ count();text = yytext; return(STRING_LITERAL); /*瀛楃涓插父閲?/}

">>="	{ count(); return(RIGHT_ASSIGN); }
"<<="	{ count(); return(LEFT_ASSIGN); }
"+="	{ count(); return(ADD_ASSIGN); }
"-="	{ count(); return(SUB_ASSIGN); }
"*="	{ count(); return(MUL_ASSIGN); }
"/="	{ count(); return(DIV_ASSIGN); }
"%="	{ count(); return(MOD_ASSIGN); }
"&="	{ count(); return(AND_ASSIGN); }
"^="	{ count(); return(XOR_ASSIGN); }
"|="	{ count(); return(OR_ASSIGN); }
">>"	{ count(); return(RIGHT_OP); }
"<<"	{ count(); return(LEFT_OP); }
"++"	{ count(); return(INC_OP); }
"--"	{ count(); return(DEC_OP); }
"->"	{ count(); return(PTR_OP); }
"&&"	{ count(); return(AND_OP); }
"||"	{ count(); return(OR_OP); }
"<="	{ count(); return(LE_OP); }
">="	{ count(); return(GE_OP); }
"=="	{ count(); return(EQ_OP); }
"!="	{ count(); return(NE_OP); }
";"		{ count(); return(';'); }
","		{ count(); return(','); }
":"		{ count(); return(':'); }
"="		{ count(); return('='); }
"["		{ count(); return('['); }
"]"		{ count(); return(']'); }
"."		{ count(); return('.'); }
"&"		{ count(); return('&'); }
"!"		{ count(); return('!'); }
"~"		{ count(); return('~'); }
"-"		{ count(); return('-'); }
"+"		{ count(); return('+'); }
"*"		{ count(); return('*'); }
"/"		{ count(); return('/'); }
"%"		{ count(); return('%'); }
"<"		{ count(); return('<'); }
">"		{ count(); return('>'); }
"^"		{ count(); return('^'); }
"|"		{ count(); return('|'); }
"?"		{ count(); return('?'); }
"{"		{ count(); return('{'); }
"}"		{ count(); return('}'); }
"("		{ count(); return('('); }
")"		{ count(); return(')'); }

"\n"    { count(); myyylineno = myyylineno + 1;}

[\t\v\f ]		{ count();}
.			{ /* Add code to complain about unmatched characters */ }

%%


int yywrap() {
	return 1;
}


void comment() {
	column = 0;
	char c, prev = 0;
	
	while (cin >> c)      /* (EOF maps to 0) */
	{
		if (c == '/' && prev == '*')
			return;
		prev = c;
	}
	printf("unterminated comment");
}



void count(void)
{
	int i;

	for (i = 0; yytext[i] != '\0'; i++)
		if (yytext[i] == '\n')
			column = 0;
		else if (yytext[i] == '\t')
			column += 4 - (column % 4);
		else
			column++;
	ECHO;
}


int check_type() {
/*
* pseudo code --- this is what it should check
*
*	if (yytext == type_name)
*		return TYPE_NAME;
*
*	return IDENTIFIER;
*/

/*
*	it actually will only return IDENTIFIER
*/

	return IDENTIFIER;
}
