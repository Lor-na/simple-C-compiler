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

void Dec::addDeclarator(Declarator *declarator) {
	this->declarators.push_back(declarator);
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

