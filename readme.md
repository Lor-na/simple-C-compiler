# AST part
+ funcdec中AST没有处理返回值的问题，只说明了返回值类型，需要LLVM部分处理。
+ 当前没有处理空Block的逻辑。
+ switch case语法出大问题
    + 好像case中只能处理单条语句。考虑把statement改成block_item_list.
    + 现在case后面可以是表达式，c应该只能是个常量好像。
    + 没有实现default的语法，可以强查IDENTIFIER作代替。
+ switch语句应该做语义检查，它的body应该只能是label_statement.

# LLVM part

# Semantic part