flex parser.l
bison -d parser.y
D:\software\anaconda\python.exe add_include.py
g++ -o parser tree.h tree.cpp lex.yy.c parser.tab.c