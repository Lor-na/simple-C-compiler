#include <iostream>

#include "tree.h"
#include "parser.hpp"
#include "innerCode.h"

extern int doparse(char* file);
extern tree::Program* ast_root;


int main(int argc,char* argv[]) {

	doparse(argv[1]);
	std::cout << "parsing done" << std::endl;

	ast_root->print(0);

	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();
	InitializeNativeTarget();
	CodeGenContext context;
	
	context.generateCode(*ast_root);
	
	context.runCode();

	
	return 0;
}