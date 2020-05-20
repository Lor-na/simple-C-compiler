#include <string>
#include <vector>
#include <iostream>

using namespace std;

class treeNode{
public:
	string token;
	vector<treeNode*> children;
	treeNode *parent;

	treeNode(char *token);

	void addChild(treeNode* child);
	// and set child's parent at the same time

	void treePrint();
};

