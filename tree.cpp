#include "tree.h"

treeNode::treeNode(const char* token){
	this->token.assign(token);
	this->parent = NULL;
}

treeNode::treeNode(string token){
	this->token.assign(token);
	this->parent = NULL;
}

void treeNode::treePrint(){
	cout << this->token << endl;
	for(int i = 0; i < this->children.size(); i++){
		this->children[i]->treePrint();
	}
}

void treeNode::addChild(treeNode* child){
	this->children.push_back(child);
	child->parent = this;
}
