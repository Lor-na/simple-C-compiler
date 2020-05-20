flex parser.l
bison -d parser.y
g++ -o parser tree.h tree.cpp lex.yy.c parser.tab.c