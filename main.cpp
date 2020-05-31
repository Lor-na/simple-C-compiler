#include <iostream>

#include "tree.h"
#include "parser.hpp"
#include "innerCode.h"

extern int doparse(char* file);
extern tree::Program* ast_root;


int main(int argc,char* argv[]) {


	// parsing and generating AST
	std::cout << "start parsing" << std::endl;
	doparse(argv[1]);
	std::cout << "parsing done" << std::endl;

	// AST visualization
	ast_root->print(0);

	// semantic analysis
	bool is_legal = ast_root->checkSemantics();
	if(!is_legal)
		return 0;

	// generating code
	InitializeNativeTargetAsmPrinter();
	InitializeNativeTargetAsmParser();
	InitializeNativeTarget();
	CodeGenContext context;
	
	context.generateCode(*ast_root);
	
	context.runCode();

	
	return 0;
}