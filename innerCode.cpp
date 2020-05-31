#include <iostream>
#include <vector>


#include "innerCode.h"



// int getRecordIndex(tree::Type *recType, std::string name)  // 对于记录类型，寻找某一个变量的 index
// {
//     for (int i = 0; i < recType->child_type.size(); i++) {
//         if (name == recType->child_type[i]->name) {
//             return i;
//         }
//     }
// 	std::cout << "[Error] Unknown name: " << name << " (in record: " << recType->name << ")" << std::endl;
//     exit(0);
// }


////////////////////////////////////////////////////////////////////////////


llvm::Function *createPrintf(CodeGenContext *context)
{
    std::vector<llvm::Type *> argTypes;
    argTypes.push_back(llvm::Type::getInt8PtrTy(MyContext));  // push 第一个参数的类型 char *
    llvm::FunctionType *printType = llvm::FunctionType::get(  // 构造函数类型
		llvm::Type::getInt32Ty(MyContext),  // Type *Result 			// 返回 int
		llvm::makeArrayRef(argTypes),       // ArrayRef<Type *> Params  // 参数表
        true);                              // bool isVarArg            // 是否为变长参数
    llvm::Function *func = llvm::Function::Create(  // 根据函数类型构造函数体
		printType,                        // FunctionType *Ty      // 函数类型
		llvm::Function::ExternalLinkage,  // LinkageTypes Linkage  // 定义外部链接
		llvm::Twine("printf"),            // const Twine &N        // 函数名称
		context->module);                  // Module *M             // 装载的 module
    func->setCallingConv(llvm::CallingConv::C);  // 设置调用常数
    return func;
}

///////////////  获取某个数组元素地址
llvm::Value *getArrRef(tree::ArrayExp *exp, CodeGenContext *context)  // 获得一个 llvm 数组某下标的地址
{
	std::string arrName = exp->name;
    
	std::vector<llvm::Value *> arrIdx(2);
	arrIdx[0] = llvm::ConstantInt::get(
		MyContext,
		llvm::APInt(32, 0, true));  // 构建 llvm 整型常数，值为 0
	
	tree::ExpStm *index;
	index = (tree::ExpStm*)(exp->index[0]);
	tree::ConstantExp *conexp;
	conexp = (tree::ConstantExp*) index->exps[0];
	arrIdx[1] = conexp->codeGen(context);  //  获取数组表达式的第二个变量的值

	
   
	return llvm::GetElementPtrInst::CreateInBounds(
		context->getValue(arrName),               // Value *Ptr                // 从 context 中找到数组变量
		llvm::ArrayRef<llvm::Value *>(arrIdx),  // ArrayRef<Value *> IdxList
		llvm::Twine("tempname"),                 // const Twine &NameStr
		context->getCurBlock());                  // BasicBlock *InsertAtEnd

}
 



///////////////  generate inner code   /////////////////

///////  program 
llvm::Value *tree::Program::codeGen(CodeGenContext *context)
{
    llvm::Value *lastDef = nullptr;

	

        for (tree::Dec *dec_d : this->dec) {
            lastDef = dec_d->codeGen(context);
	        std::cout << "[Success] Program dec " << std::endl;
        }
        
    

    
        for (tree::FuncDec *funDef : this->func_dec) {  
            lastDef = funDef->codeGen(context);
	        std::cout << "[Success] Program func_dec "<< std::endl;
        }
    

    
    std::cout << "[Success] Program generated" << std::endl;

    return lastDef;
}

llvm::Value* tree::Block::codeGen(CodeGenContext *context)
{
	
	llvm::Value *lastDef = nullptr;

		for(tree::Dec *dec_: this->dec){
			lastDef = dec_->codeGen(context);
	    }

	
		for(tree::Stm *stm_: this->stm){
			 lastDef = stm_->codeGen(context);
	    }


	std::cout << "[Success] block codeGen done" << std::endl;
	
	return lastDef;
	
	
	
}

llvm::Value* tree::Declarator::codeGen(CodeGenContext *context)
{
	return nullptr;
}


llvm::Value* tree::Initializer::codeGen(CodeGenContext *context)
{
	return nullptr;
}


llvm::Value* tree::FuncExp::codeGen(CodeGenContext *context)
{
	llvm::Function *func = context->module->getFunction(this->name.c_str());  // 找到这个函数
    if (func == nullptr) {
        std::cout << "[Error] Function not defined" << std::endl;
        exit(0);
    }

	
    std::vector<llvm::Value *> argValues;
    auto funcArgs_iter = func->arg_begin();
    for (tree::Exp *arg : this->argu) {
		
        llvm::Value *funcArgValue = static_cast<llvm::Value *>(funcArgs_iter++);
		
        
		if (arg->node_type == N_ARRAY_EXP) {
			ArrayExp* node = static_cast<ArrayExp*>(arg);	
			llvm::Value *ptr = getArrRef(node, context);	
			argValues.push_back(ptr);
		
		} 
		else {
			
            argValues.push_back(arg->codeGen(context));
        }
    }

    std::cout << "[Success] Function called." << std::endl;
    return llvm::CallInst::Create(
		func,
		llvm::makeArrayRef(argValues),
		llvm::Twine(""),
		context->getCurBlock());
}

llvm::Value* tree::ParaList::codeGen(CodeGenContext *context)
{
	return nullptr;
}

llvm::Value* tree::ParaItem::codeGen(CodeGenContext *context)
{
	return nullptr;
}

llvm::Value* tree::IDList::codeGen(CodeGenContext *context)
{
	return nullptr;
}




////////////////////  function 
llvm::Value* tree::FuncDec::codeGen(CodeGenContext *context)
{
    std::cout << "Function defining: " << this->name << std::endl;
    context->defined_functions[this->name] = this;

    std::vector<llvm::Type *> argTy;
	tree::DeclaratorFunc *func_para;
	func_para =  (DeclaratorFunc*)(this->para);
    if(this->para->d_type==D_FUNC_DEF){
			
		for(tree::ParaItem *para: func_para->para_def->para){
				argTy.push_back(context->getLLVMTy(para->type));

		}

	}

	llvm::FunctionType *funcType = llvm::FunctionType::get(
			context->getLLVMTy(this->return_type),  // Type *Ty                 // 函数返回值类型
			llvm::makeArrayRef(argTy),          // ArrayRef<Type *> Params  // 参数类型
		    false);                             // bool isVarArg            // 定长
	llvm::Function *func = llvm::Function::Create(
		funcType,
		llvm::GlobalValue::InternalLinkage,
		llvm::Twine(this->name),             // 函数名
		context->module);

	llvm::BasicBlock *block = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("entry"),
		func,
		nullptr);              // BasicBlock *InsertBefore
    


    llvm::Function *oldFunc = context->getCurFunc();
    context->setCurFunc(func);
    llvm::BasicBlock *oldBlock = context->getCurBlock();
    context->parent[func] = oldFunc;

    context->pushBlock(block);

    llvm::Value *argValue;
    llvm::Argument *argValue_iter = func->arg_begin();

	if(this->para->d_type==D_FUNC_DEF){
		for (tree::ParaItem *para: func_para->para_def->para) {
			llvm::Type *ty;

			ty = context->getLLVMTy(para->type);
			
			llvm::Value *alloc = new llvm::AllocaInst(  // 为参数分配空间
				ty,                               // 参数类型
				0,
				llvm::Twine(para->var->name),  // 参数名
				context->getCurBlock());
			argValue = argValue_iter++;
			argValue->setName(llvm::Twine(para->var->name));
			new llvm::StoreInst(  // 存参数值
				argValue,  // 参数值
				alloc,     // 参数地址
				false,     // 非常量
				block);
		}
	}

    if (this->return_type != nullptr) {
		new llvm::AllocaInst(  // 为返回值分配地址
			context->getLLVMTy(this->return_type),
			0,
			llvm::Twine(this->name),
			context->getCurBlock());
        std::cout << "|--- Function return value declaration" << std::endl;
    }

    std::cout << "|--- [Success] Function header pfunc_para->para_def->parart generated success!" << std::endl;

    if (this->block != nullptr) {
        std::cout << "|--- Function body" << std::endl;
        for (tree::Dec *dec_d : this->block->dec) {
            dec_d->codeGen(context);
        }
        std::cout << "|--- [Success] Function define part generated success!" << std::endl;
    

        for (tree::Stm *stm_m : this->block->stm) {
            stm_m->codeGen(context);
        }

        std::cout << "|--- [Success] Function body part generated success!" << std::endl;
    }

    

    if (this->return_type != nullptr) {
        std::cout << "|--- Generating return value for function" << std::endl;
        llvm::Value *load = new llvm::LoadInst(  // 加载返回值的地址
			context->getValue(this->name),
			llvm::Twine(""),
			false,
			context->getCurBlock());
        llvm::ReturnInst::Create(
			MyContext,
			load,
			context->getCurBlock());
        std::cout << "|--- Function returned" << std::endl;
    } else {
        std::cout << "|--- Generating return void for procedure" << std::endl;
        llvm::ReturnInst::Create(
			MyContext,
			context->getCurBlock());
        std::cout << "|--- Procedure returned" << std::endl;
    }

    while (context->getCurBlock() != oldBlock) {  // 函数定义完成
        context->popBlock();
    }
    context->setCurFunc(oldFunc);

    std::cout << "[Success] Defined function: " << this->name << std::endl;
    return func;
}


////////////  dec
llvm::Value *tree::Dec::codeGen(CodeGenContext *context)
{
    llvm::Value *alloc;
	for (tree::DecItem *dec_item : this->dec_list) {

		if(dec_item->declarator->d_type==D_ID){
        
			alloc = new llvm::AllocaInst(  // 为局部变量分配地址
				context->getLLVMTy(this->type),  // 类型
				0,
				llvm::Twine(dec_item->declarator->name),        // 常量名称
				context->getCurBlock()
			);
			dec_item->codeGen(context);
		}
		else if(dec_item->declarator->d_type==D_ARRAY){      //  为数组变量分配地址
			tree::DeclaratorArray *array;
	        array =  (DeclaratorArray*)(dec_item->declarator);


			std::cout << " |---------Array variable" << std::endl;

            std::vector<llvm::Constant *> vec = std::vector<llvm::Constant *>();
            llvm::Constant *eleOfArr;
            switch (this->type->base_type) {  // 初始化
                case T_INTEGER: eleOfArr = llvm::ConstantInt::get(llvm::Type::getInt32Ty(MyContext), 0, true);break;
                case T_DOUBLE   : eleOfArr = llvm::ConstantFP ::get(llvm::Type::getFloatTy(MyContext), 0);       break;
                case T_CHAR   : eleOfArr = llvm::ConstantInt::get(llvm::Type::getInt8Ty (MyContext), 0, true); break;
                case T_BOOL: eleOfArr = llvm::ConstantInt::get(llvm::Type::getInt1Ty (MyContext), 0, true); break;
                default:
					std::cout << "[Warning] Uncomplete feature for global array of record" << std::endl;
					exit(0);
            }
			int size;
			if (llvm::ConstantInt* CI = dyn_cast<llvm::ConstantInt>(array->array_size[0]->codeGen(context))) {
				if (CI->getBitWidth() <= 32) {
					size = CI->getSExtValue();
				}
				}
			
			

            for (int i = 0; i < size; i++) {
				
                vec.push_back(eleOfArr);
            }

			llvm::ArrayType *arrType = static_cast<llvm::ArrayType *>(context->getLLVMTy(this->type));
            llvm::Constant *arrConst = llvm::ConstantArray::get(
				arrType,
				vec);

			
			
			std::cout<<dec_item->declarator->name<<std::endl;
            alloc = new llvm::GlobalVariable(
				*context->module,                     // Module &M
				llvm::ArrayType::get(context->getLLVMTy(this->type),size),       // Type *Ty               // 全局变量的类型
				false,                               // bool isConstant        // 是否常量
				llvm::GlobalValue::ExternalLinkage,  // LinkageTypes Linkage
				arrConst,                            // Constant *Initializer  // 全局变量初始化为 0
				llvm::Twine(dec_item->declarator->name));            // const Twine &Name      // 变量名称
			
			dec_item->codeGen(context);
			

		}
		else if(dec_item->declarator->d_type==D_FUNC_DEF){  //不支持单独定义，仅支持函数体实现位于调用前的定义方法
			return nullptr;
		}
		else if(dec_item->declarator->d_type==D_FUNC_EMPTY){
			return nullptr;
		}
        

		
    }

    std::cout << "[Success] Variable defined" << std::endl;
	
	return alloc;
}

///////////  decItem
llvm::Value *tree::DecItem::codeGen(CodeGenContext *context)
{
	llvm::Value *ret;

	if(this->initializer!=nullptr){
		if(this->initializer->init_type==I_EXP){

			llvm::Value *tmp = context->getValue(this->declarator->name);
			llvm::Value *load;
			do {
				load = tmp;
				tmp = new llvm::LoadInst(
					tmp,
					llvm::Twine(""),
					false,
					context->getCurBlock());
			} while (tmp->getType()->isPointerTy());

			return new llvm::StoreInst(
				this->initializer->assign_exp->codeGen(context),  // 值（右）
				load,          // 地址（左）
				false,
				context->getCurBlock());
		}
		else if(this->initializer->init_type==I_ARRAY){   

            for(int i=0;i<this->initializer->assign_array.size();i++){



				std::vector<llvm::Value *> arrIdx(2);
				arrIdx[0] = llvm::ConstantInt::get(
					MyContext,
					llvm::APInt(32, 0, true));  // 构建 llvm 整型常数，值为 0
				
				
				arrIdx[1] = llvm::ConstantInt::get(
					MyContext,
					llvm::APInt(32, i, true));  // 构建 llvm 整型常数，值为 0

				llvm::Value *elementptr = llvm::GetElementPtrInst::CreateInBounds(
					context->getValue(this->declarator->name), // Value *Ptr    // 从 context 中找到数组变量
					llvm::ArrayRef<llvm::Value *>(arrIdx),  // ArrayRef<Value *> IdxList
					llvm::Twine("tempname"),                 // const Twine &NameStr
					context->getCurBlock());                  // BasicBlock *InsertAtEnd
				
				
				ret = new llvm::StoreInst(
				this->initializer->assign_array[i]->assign_exp->codeGen(context),  // 值
				elementptr,                           // 地址
				false,
				context->getCurBlock());


			}
			return ret;

			
	    }
		else{
			std::cout << "[Wrong] invalid initializer!" << std::endl;
		}
		
						
	}
	else{
		
		std::cout << "No initialization!"<<std::endl;
		return nullptr;
	}
	
}

////////////   expstm
llvm::Value *tree::ExpStm::codeGen(CodeGenContext *context)
{
	llvm::Value *lastDef = nullptr;
    for (tree::Exp *assignexp : this->exps) {
        lastDef = assignexp->codeGen(context);
        std::cout << "[Success] A dot expstm generated" << std::endl;
    }
    return lastDef;
} 


///////////   assign exp
llvm::Value *tree::AssignExp::codeGen(CodeGenContext *context)
{
    std::cout << "Creating assignment statment..." << std::endl;

	if (this->left_exp->node_type == N_ARRAY_EXP) {  // 如果左值为数组的某个元素
        tree::ArrayExp *op1 = static_cast<tree::ArrayExp *>(this->left_exp);
            llvm::Value *elementPtr = getArrRef(op1, context);  // 取得目标地址
            std::cout << "[Success] Assignment statment generate" << std::endl;
            return new llvm::StoreInst(
				this->right_exp->codeGen(context),  // 值
				elementPtr,                           // 地址
				false,
				context->getCurBlock());
        
    } 
	else if (this->left_exp->node_type == N_VARIABLE_EXP) {  // 如果左值为变量
	       
			tree::VariableExp *op1 = static_cast<tree::VariableExp *>(this->left_exp);
			if (op1->codeGen(context)->getType()->isArrayTy()) {  // 如果左值是数组类型
				std::cout << "[Error] Wrong left value type [array type]" << std::endl;
				exit(0);
			} 
			else {  // 如果左值不为数组类型
				llvm::Value *tmp = context->getValue(op1->name);
				llvm::Value *load;
				do {
					load = tmp;
					tmp = new llvm::LoadInst(
						tmp,
						llvm::Twine(""),
						false,
						context->getCurBlock());
				} while (tmp->getType()->isPointerTy());

				return new llvm::StoreInst(
					this->right_exp->codeGen(context),  // 值（右）
					load,          // 地址（左）
					false,
					context->getCurBlock());
			}
	} 
    else {  // 如果左值不为变量
        std::cout << "[Error] Wrong left value type" << std::endl;
        exit(0);
    }
}

///////////////    UnaryExp
llvm::Value *tree::UnaryExp::codeGen(CodeGenContext *context)
{
	switch (this->op_code) {
		

    	case OP_DEC:
      		return llvm::BinaryOperator::Create(  // m - 1
				llvm::Instruction::Sub,
				this->operand->codeGen(context),
				llvm::ConstantInt::get(llvm::Type::getInt8Ty(MyContext), 1, true),
				llvm::Twine(""),
				context->getCurBlock());
		case OP_INC:
			return llvm::BinaryOperator::Create(  // m + 1
				llvm::Instruction::Add,
				this->operand->codeGen(context),
				llvm::ConstantInt::get(llvm::Type::getInt8Ty(MyContext), 1, true),
				llvm::Twine(""),
				context->getCurBlock());
    	
  	}
}





//////////////////  binary exp

llvm::Value *tree::BinaryExp::codeGen(CodeGenContext *context)
{
 	// if (this->op_code == OP_DOT) {  // A.B形式
    // 	if(this->operand2->node_type == N_VARIABLE_EXP) {  // 操作符 2 是变量
    //   		tree::VariableExp* op2 = static_cast<tree::VariableExp *>(this->operand2);
    //   		int index = getRecordIndex(  // 找到所求变量在记录中的位置
	// 			this->operand1->return_type,
	// 			op2->name);
    //   		std::vector<llvm::Value*> arrIdx(2);
    //   		arrIdx[0] = llvm::ConstantInt::get(  // 0
	// 			MyContext,
	// 			llvm::APInt(32, 0, true));
    //   		arrIdx[1] = llvm::ConstantInt::get(  // index
	// 			MyContext,
	// 			llvm::APInt(32, index, true));  // create member_index
    //   		llvm::Value* memPtr = llvm::GetElementPtrInst::Create(
	// 			context->getLLVMTy(this->operand1->return_type),
	// 			context->getValue(op2->name),
	// 			arrIdx,
	// 			llvm::Twine(""),
	// 			context->getCurBlock());
    //   		return new llvm::LoadInst(
	// 			memPtr,
	// 			llvm::Twine(""),
	// 			false,
	// 			context->getCurBlock());
    // 	} else {
    //   		std::cout << "[Error] Wrong member variable for record" << std::endl;
    //         exit(0);
    // 	}
  	// } else 
    if (this->op_code == OP_INDEX) {   //  A[B]
    	if (this->operand1->node_type == N_VARIABLE_EXP) {
      		tree::VariableExp* op1 = static_cast<tree::VariableExp *>(this->operand1);
	      	std::vector<llvm::Value *> arrIdx(2);
	      	arrIdx[0] = llvm::ConstantInt::get(
				MyContext,
				llvm::APInt(32, 0, true));
	      	arrIdx[1] = this->operand2->codeGen(context);
	      	llvm::Value *memPtr = llvm::GetElementPtrInst::CreateInBounds(
				context->getValue(op1->name),
				llvm::ArrayRef<llvm::Value *>(arrIdx),
				llvm::Twine("tempname"),
			 	context->getCurBlock());
	      	return new llvm::LoadInst(
				memPtr,
				llvm::Twine(""),
				false,
				context->getCurBlock());
    	} else {
      		std::cout << "[Error] Array's Ref is not an array type variable" << std::endl;
      		exit(0);
    	}
  	}
  	llvm::Value *op1Val = this->operand1->codeGen(context);
  	llvm::Value *op2Val = this->operand2->codeGen(context);

  	if (op1Val->getType()->isFloatTy() || op2Val->getType()->isFloatTy()){  // 如果是浮点数运算
    	switch (this->op_code) {
      		case OP_ADD:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::FAdd,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_MINUS:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::FSub,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_MUL:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::FMul,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_DIV:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::FDiv,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		
      		case OP_MOD:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::SRem,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_AND:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::And,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_OR:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::Or,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		//logical
      		case OP_LESS:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SLT,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_GREATER:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SGT,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_LESS_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SGE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_GREATER_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SLE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_EQ,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_NOT_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_NE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		default:
				std::cout << "[Error] Unknown type of op_code:" << this->op_code << std::endl; 
				exit(0);
    	}
  	} else {  // 整型运算
    	switch (this->op_code) {
      		case OP_ADD:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::Add,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_MINUS:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::Sub,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_MUL:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::Mul,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_DIV:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::UDiv,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_MOD:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::SRem,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_AND:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::And,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		case OP_OR:
				return llvm::BinaryOperator::Create(
					llvm::Instruction::Or,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_LESS:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SLT,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_GREATER:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SGT,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_LESS_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SGE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_GREATER_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_SLE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_EQ,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
	      	case OP_NOT_EQUAL:
				return llvm::CmpInst::Create(
					llvm::Instruction::ICmp,
					llvm::CmpInst::ICMP_NE,
					op1Val, op2Val,
					llvm::Twine(""),
					context->getCurBlock());
      		default:
				std::cout << "[Error] Unknown type of op_code:" << this->op_code << std::endl;
				exit(0);
    	}
  	}
}
////////////////  constant exp
llvm::Value *tree::ConstantExp::codeGen(CodeGenContext *context)
{
	return value->codeGen(context);
}
///////////////   variable exp
llvm::Value *tree::VariableExp::codeGen(CodeGenContext *context)
{
	std::cout << "loading variable: " << this->name << std::endl;
	llvm::Value *ptr = context->getValue(this->name);
  	ptr = new llvm::LoadInst(
		ptr,
		llvm::Twine(""),
		false,
		context->getCurBlock());
  	if (ptr->getType()->isPointerTy()) {
    	ptr = new llvm::LoadInst(
			ptr,
			llvm::Twine(""),
			false,
			context->getCurBlock());
  	}
  	return ptr;
}

///////////////   Array exp       
llvm::Value* tree::ArrayExp::codeGen(CodeGenContext *context)
{
	std::cout << "loading array: " << this->name << std::endl;
	
    llvm::Value *ptr = getArrRef(this, context);  // 取得目标地址
  	ptr = new llvm::LoadInst(
		ptr,
		llvm::Twine(""),
		false,
		context->getCurBlock());
  	if (ptr->getType()->isPointerTy()) {
    	ptr = new llvm::LoadInst(
			ptr,
			llvm::Twine(""),
			false,
			context->getCurBlock());
	  }
	
	return ptr;
}


///////////////////  Type
llvm::Value* tree::Type::codeGen(CodeGenContext *context)
{
	return nullptr;
}

//////////////////   Value 
llvm::Value *tree::Value::codeGen(CodeGenContext *context)
{
	switch (this->base_type) {
		case T_INTEGER:
		    
			return llvm::ConstantInt::get(
				llvm::Type::getInt32Ty(MyContext),
				this->val.integer_value,
				true);
		case T_DOUBLE:
			return llvm::ConstantFP::get(
				MyContext,
				llvm::APFloat(this->val.double_value));
		case T_CHAR:
			return llvm::ConstantInt::get(
				llvm::Type::getInt8Ty(MyContext),
				this->val.char_value,
				true);
		case T_BOOL:
			return llvm::ConstantInt::get(
				llvm::Type::getInt1Ty(MyContext),
				this->val.bool_value,
				true);
		default:
			return nullptr;
	}
}


//////////////////////////       SelectStm
llvm::Value *tree::SelectStm::codeGen(CodeGenContext *context)
{
    std::cout << "Creating if statment" << std::endl;
    llvm::Value* cond = condition->codeGen(context);
    std::cout << "|--- [Success] Condition generated" << std::endl;
    llvm::BasicBlock *trueBlock = llvm::BasicBlock::Create(
        MyContext,
	    llvm::Twine("then"),
	    context->getCurFunc());
    llvm::BasicBlock *falseBlock = llvm::BasicBlock::Create(
	    MyContext,
	    llvm::Twine("else"),
	    context->getCurFunc());
    llvm::BasicBlock *mergeBlock = llvm::BasicBlock::Create(
	    MyContext,
	    llvm::Twine("merge"),
	    context->getCurFunc());

    llvm::Value *ret = llvm::BranchInst::Create(
	    trueBlock,               // BasicBlock *True
	    falseBlock,              // BasicBlock *False
	    cond,                    // Value *cond
	    context->getCurBlock());  // BasicBlock *InsertAtEnd

    context->pushBlock(trueBlock);
    this->if_do->codeGen(context);
    std::cout << "|--- [Success] True block generated" << std::endl;
    llvm::BranchInst::Create(  // 为真的语句生成完成，回到 merge
	    mergeBlock,
	    context->getCurBlock());
    context->popBlock();
    context->pushBlock(falseBlock);
    if (this->else_do != nullptr) {
        this->else_do->codeGen(context);
        std::cout << "|--- [Success] False block generated" << std::endl;
    }
    llvm::BranchInst::Create(  // 为假的语句生成完成，回到 merge
	    mergeBlock,
	    context->getCurBlock());
    context->popBlock();
    context->pushBlock(mergeBlock);
    std::cout << "[Success] If statment generated" << std::endl;
    return ret;
}



///////////////////////        while stm   待实现 do while 和 while的区分
llvm::Value* tree::WhileStm::codeGen(CodeGenContext *context)
{
    std::cout << "Creating while statement" << std::endl;

    if(!this->do_first)   //while 句式
	{
		llvm::BasicBlock *startBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("start"),
			context->getCurFunc());
		llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("loop"),
			context->getCurFunc());
		llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("exit"),
			context->getCurFunc());
		llvm::BranchInst::Create(  // 首先跳转入 startBlock
			startBlock,
			context->getCurBlock());
		context->pushBlock(startBlock);  // startBlock

		llvm::Value *ret = llvm::BranchInst::Create(
			loopBlock,                           // 符合 cond ，继续 loopBlock
			exitBlock,                          // 不符合 cond ，跳入 exitBlock
			this->cond->codeGen(context),
			context->getCurBlock());
		context->popBlock();
		context->pushBlock(loopBlock);  // loopBlock
		this->body->codeGen(context);  // 生成代码块
		llvm::BranchInst::Create(  // 继续循环，跳入 startBlock
			startBlock,
			context->getCurBlock());
		context->popBlock();
		context->pushBlock(exitBlock);  // exitBlock
		std::cout << "[Success] While loop generated" << std::endl;
		return ret;
	}

	else{      // do-while 句式
		llvm::BasicBlock *startBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("start"),
			context->getCurFunc());
		llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("loop"),
			context->getCurFunc());
		llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("exit"),
			context->getCurFunc());
		llvm::BranchInst::Create(  // 首先跳转入 startBlock
			startBlock,
			context->getCurBlock());
		context->pushBlock(startBlock);  // startBlock

		this->body->codeGen(context);    //先无论条是否符合，做一次body    ///////////  瞎写的 不知道行不行

		llvm::Value *ret = llvm::BranchInst::Create(
			loopBlock,                           // 符合 cond ，继续 loopBlock
			exitBlock,                          // 不符合 cond ，跳入 exitBlock
			this->cond->codeGen(context),
			context->getCurBlock());
		context->popBlock();
		context->pushBlock(loopBlock);  // loopBlock
		this->body->codeGen(context);  // 生成代码块
		llvm::BranchInst::Create(  // 继续循环，跳入 startBlock
			startBlock,
			context->getCurBlock());
		context->popBlock();
		context->pushBlock(exitBlock);  // exitBlock
		std::cout << "[Success] While loop generated" << std::endl;
		return ret;
	}
}

//////////////////  jump stm

llvm::Value* tree::JumpStm::codeGen(CodeGenContext *context)
{
	std::cout << jumpBlock.size() << std::endl;
	if(this->jump_type == J_BREAK){
		

		llvm::BasicBlock *j_block = jumpBlock.back();
		
		std::cout << jumpBlock.size() << std::endl;
		llvm::Instruction *ret = llvm::BranchInst::Create(
			j_block,
			context->getCurBlock());
		
		jumpBlock.pop_back();
		return ret;
		
	}
	else if (this->jump_type == J_CONTINUE){
		

		llvm::BasicBlock *c_block = continueBlock.back();
		
		std::cout << continueBlock.size() << std::endl;
		llvm::Instruction *ret = llvm::BranchInst::Create(
			c_block,
			context->getCurBlock());
		
		continueBlock.pop_back();
		return ret;
	}
	else if(this->jump_type == J_RETURN){
		llvm::Value *returnValue = this->return_exp->codeGen(context);
		// context->setCurrentReturnValue(returnValue);
		return returnValue;
	}
	else{
		std::cout << "[ERROR] Wrong jump stm" << std::endl;
	}
}


////////////////    for stm
llvm::Value* tree::ForStm::codeGen(CodeGenContext *context)
{
    std::cout << "Creating for statement" << std::endl;
    llvm::BasicBlock *startBlock = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("start"),
		context->getCurFunc());
    llvm::BasicBlock *loopBlock = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("loop"),
		context->getCurFunc());
	llvm::BasicBlock *iterBlock = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("iter"),
		context->getCurFunc());
    llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("exit"),
		context->getCurFunc());
	
    jumpBlock.push_back(exitBlock);
	continueBlock.push_back(iterBlock);

	// 循环变量初始化
	if( this->init )
        this->init->codeGen(context);
    
    llvm::BranchInst::Create(  // 跳转入 startBlock
		startBlock,
		context->getCurBlock());

	// 循环过程
    context->pushBlock(startBlock);  // startBlock

    llvm::Instruction *ret = llvm::BranchInst::Create(
		exitBlock,                // 到达终点值，跳入 exitBlock
		loopBlock,                // 未到达终点值，跳入 loopBlock
		this->cond->codeGen(context),
		context->getCurBlock());

    context->popBlock();
    context->pushBlock(loopBlock);  // loopBlock

	
    this->body->codeGen(context);  // 生成代码块

	llvm::BranchInst::Create(  // 跳转入 startBlock
		iterBlock,
		context->getCurBlock());
    
	context->popBlock();
	context->pushBlock(iterBlock);
	
	// 循环变量的更新
    this->iter->codeGen(context);   

	

    
	llvm::BranchInst::Create(  // 跳入 startBlock
		startBlock,
		context->getCurBlock());
    context->popBlock();
    context->pushBlock(exitBlock);  // exitBlock
    // this->body->codeGen(context);  // 最后一次循环
    std::cout << "[Success] For loop generated" << std::endl;

    jumpBlock.pop_back();
    continueBlock.pop_back();
    return ret;
}



/////////////  switch
llvm::Value* tree::SwitchStm::codeGen(CodeGenContext *context)
{
	std::cout << "Creating switch-case statment" << std::endl;
	llvm::BasicBlock *exitBlock = llvm::BasicBlock::Create(
		MyContext,
		llvm::Twine("exit"),
		context->getCurFunc());
    
	
	
	std::vector<llvm::BasicBlock *> blocks;
    
	jumpBlock.push_back(exitBlock);

    llvm::Value *ret;

    tree::Block *cmth ;
	cmth = (tree::Block *)(this->body);

    int group_num;
	group_num=0;
	std::vector<int> c_set;
	for(int m=0;m<cmth->stm.size();m++){
		if(cmth->stm[m]->node_type==N_CASE_STM){
			group_num++;
			c_set.push_back(m);
			
		}
		   
	}

    for (int i = 0; i < group_num; i++) {  // 对于每一种 situation
	    
		std::cout << group_num << std::endl;
		
		llvm::BasicBlock *basBlock = llvm::BasicBlock::Create(
			MyContext,
			llvm::Twine("caseStmt"),
			context->getCurFunc());
		blocks.push_back(basBlock);  // 建立一个 block 并 push
		
    }

	//llvm::Value *ret;
    for (int i = 0, p = 0; i < group_num; i++, p++) {
		//tree::CaseStm *stm = (tree::CaseStm*)(cmth->stm[i]); 
		
		std::cout << "in case No." << i << std::endl;
		tree::CaseStm *stm = (tree::CaseStm*)(cmth->stm[c_set[i]]); 
		
		tree::BinaryExp *cond = new tree::BinaryExp(
		OP_EQUAL,
		this->cond,
		stm->value);
	
		
		
	
		llvm::BasicBlock* nextBlock;
		if (p == blocks.size() - 1) {
			
		
			ret = llvm::BranchInst::Create(
				blocks[p],               // BasicBlock *IfTrue
				exitBlock,                // BasicBlock *IfFalse
				cond->codeGen(context),    // Value Cond
				context->getCurBlock());  // BasicBlock *InsertAtEnd
		} else {
			std::cout << p <<  std::endl;
			nextBlock = llvm::BasicBlock::Create(
				MyContext,
				"next",
				context->getCurFunc());

			llvm::BranchInst::Create(
				blocks[p],               // BasicBlock *IfTrue
				nextBlock,                // BasicBlock *IfFalse
				cond->codeGen(context),    // Value Cond
				context->getCurBlock());  // BasicBlock *InsertAtEnd

			context->pushBlock(nextBlock);
		}
		
		
		
		
    }


    for (int i = 0, p = 0; i < group_num; i++, p++) {
		tree::CaseStm *stm = (tree::CaseStm*)(cmth->stm[c_set[i]]); ////////
		    
			context->pushBlock(blocks[p]);
			// tree::ExpStm* if_do_list ;
			// if_do_list = (tree::ExpStm*)(stm->if_do);
			stm->if_do->codeGen(context);

			for(int x=c_set[i]+1;x<c_set[i+1];x++){
				cmth->stm[x]->codeGen(context);
				
			}
					
			
			if(p == blocks.size() - 1){
				llvm::BranchInst::Create(
					exitBlock,
					context->getCurBlock());
			}
			else{
				std::cout << "here wwww " << c_set[i] <<  std::endl;
				llvm::BranchInst::Create(
					blocks[p+1],
					context->getCurBlock());
				std::cout << "here cccc " << c_set[i] <<  std::endl;
			}
			
            std::cout << "|--- [Success] In case " << i <<  std::endl;
			context->popBlock();
		
    }

    context->pushBlock(exitBlock);

    jumpBlock.pop_back();
    return ret;
}


/////////////   Case
llvm::Value* tree::CaseStm::codeGen(CodeGenContext *context)
{
	return nullptr;
}





