#include "codegen.h"

using namespace llvm;

CodeGen::CodeGen(){
  Builder = new IRBuilder<>(getGlobalContext());
  Mod = NULL;
}

CodeGen::~CodeGen(){
  SAFE_DELETE(Mod);
}

bool CodeGen::doCodeGen(TranslationUnitAST &tunit, string name){
  return generateTranslationUnit(tunit, name);
}

Module &CodeGen::getModule(){
  if(Mod)
    return *Mod;
  else
    return *(new Module("null", getGlobalContext()));
}


bool CodeGen::generateTranslationUnit(TranslationUnitAST &tunit,  string name){
  Mod = new Module(name, getGlobalContext());
  generatePrint(Mod);
  for(int i = 0;;i++){
    ExternalDeclAST *external = tunit.getExternal(i);
    if(!external)
      break;
    else if(!generateExternalDeclaration(external, Mod)){
      SAFE_DELETE(Mod);
      return false;
    }
  }
  return true;
}

Function *CodeGen::generateExternalDeclaration(ExternalDeclAST *external, Module *Mod){
  if(external->getValueID() == FunctionDeclID){
    Function *func = generateFunctionDeclaration((FunctionDeclAST*)external, Mod);
    if(func)
      return func;
    else 
      return NULL;
  }
  else return NULL;
}
Function *CodeGen::generateFunctionDeclaration(FunctionDeclAST *func_decl, Module *mod){
  FunctionDefAST *func_def = func_decl->getFunctionDef();
  
  //create arg_types
  vector<Type*> arg_types = vector<Type*>();
  for(int i = 0; i < func_def->getParamNum(); i++){
    if(func_def->getParamType(i) == INT){
      arg_types.push_back(Builder->getInt32Ty());
    }
    else if(func_def->getParamType(i) == STRING){
      arg_types.push_back(Builder->getInt8Ty()->getPointerTo());
    }
  }
  
  //create return type
  Type* returnType;
  if(func_def->getReturnType() == INT){
    returnType = Builder->getInt32Ty();
  }
  else if(func_def->getReturnType() == STRING){
    returnType = Builder->getInt8Ty()->getPointerTo();
  }
  else if(func_def->getReturnType() == VOID){
    returnType = Builder->getVoidTy();
  }

  //create func type
  FunctionType *func_type = FunctionType::get(returnType, arg_types, false);

  //create function
  string func_name = func_def->getName();
  if(func_name == "めいん") func_name = "main";
  Function *func = Function::Create(func_type, Function::ExternalLinkage, func_name, mod);

  //set name
  Function::arg_iterator arg_iter = func->arg_begin();
  for(int i = 0; i < func_def->getParamNum(); i++){
    arg_iter->setName(func_def->getParamName(i));
    arg_iter++;
  }

  CurFunc = func;
  BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", func);
  Builder->SetInsertPoint(bblock);
  //Functionの引数のAllocaを作る
  generateFunctionParams(func_def->getParamsAST());
  //Functionのボディを生成
  generateFunctionBody(func_decl->getBody());

  
  return func;
}

Value *CodeGen::generateFunctionParams(FunctionParamsAST *func_params){
  //create alloca
  for(int i = 0; i < func_params->getParamNum(); i++){
    Type *type;
    if(func_params->getParamType(i) == INT)
      type = Builder->getInt32Ty();
    else if(func_params->getParamType(i) == STRING)
      type = Builder->getInt8Ty()->getPointerTo();
    AllocaInst *alloca = Builder->CreateAlloca(type, 0, func_params->getParamName(i));
    Variables[func_params->getParamName(i)] = alloca;

    //引数と対応させる
    ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
    Builder->CreateStore(vs_table.lookup(func_params->getParamName(i)), alloca);

  }
  return NULL;
}

Value *CodeGen::generateFunctionBody(FunctionBodyAST *func_body){
  Value *v = NULL;
  StatementAST *stmt;
  for(int i = 0; ; i++){
    stmt = func_body->getStatement(i);
    if(stmt == NULL)
      break;
    else 
      v = generateStatement(stmt);
  }
  return v;
}

Value *CodeGen::generateStatement(StatementAST *stmt){
  if(stmt->getValueID() == ExprStmtID)
    return generateExprStmt((ExprStmtAST*)stmt);
  else if(stmt->getValueID() == ReturnStmtID)
    return generateReturnStmt((ReturnStmtAST*)stmt);
  else if(stmt->getValueID() == CallStmtID)
    return generateCallStmt((CallStmtAST*)stmt);
  else
    return NULL;
}

Value *CodeGen::generateExprStmt(ExprStmtAST *exprstmt){
  ExprAST *expr = exprstmt->getExpr();
  if(expr->getValueID() == AssignmentExprID)
    return generateAssignmentExpr((AssignmentExprAST*)expr);
  else 
    return generateAdditiveExpr((AdditiveExprAST*)expr);

  return NULL;
}
Value *CodeGen::generateReturnStmt(ReturnStmtAST *returnstmt){
  PrimaryExprAST *expr = returnstmt->getExpr();
  Value *ret_v = generatePrimaryExpr(expr);

  return Builder->CreateRet(ret_v);
}
Value *CodeGen::generateCallStmt(CallStmtAST *callstmt){
  CallExprAST *callexpr = callstmt->getExpr();
  
  vector<Value*> arg_vec;
  PrimaryExprAST *arg;
  Value *arg_v;
  ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
  for(int i = 0; ; i++){
    arg = callexpr->getArg(i);
    if(arg == NULL){
      break;
    }
    arg_v = generatePrimaryExpr(arg);
    arg_vec.push_back(arg_v);
  }
  string calleeName = callexpr->getCallee()->getName();
  if(calleeName == "出力して"){
    PrimaryExprAST *arg = callexpr->getArg(0);
    if(arg->getType() == INT){
      calleeName = printNum;
    }
    else if(arg->getType() == STRING){
      calleeName = printStr;
    }
  }
  return Builder->CreateCall(Mod->getFunction(calleeName), arg_vec, "call_tmp");
}

Value *CodeGen::generateAssignmentExpr(AssignmentExprAST *assignexpr){
  Value *val = generateAdditiveExpr(assignexpr->getAdditiveExpr());
  Type *type;
  if(assignexpr->getAdditiveExpr()->getType() == INT){
    type = Builder->getInt32Ty();
  }
  else if(assignexpr->getAdditiveExpr()->getType() == STRING){
    type = Builder->getInt8Ty()->getPointerTo();
  }

  ValueSymbolTable &vs_table = CurFunc->getValueSymbolTable();
  if(vs_table.lookup(assignexpr->getName()) == NULL){
    AllocaInst *alloca = Builder->CreateAlloca(type, 0, assignexpr->getName());
    Variables[assignexpr->getName()] = alloca;
    return Builder->CreateStore(val, alloca);
  }
  
  else{
    Value *var = generateIdentifier(assignexpr->getIdentifier());
    return Builder->CreateStore(val, var);
  }

}
Value *CodeGen::generateAdditiveExpr(AdditiveExprAST *additiveexpr){
  if(additiveexpr->getValueID() == BinaryExprID)
    return generateBinaryExpr((BinaryExprAST*)additiveexpr);
  else 
    return generatePrimaryExpr((PrimaryExprAST*)additiveexpr);
}

Value *CodeGen::generateBinaryExpr(BinaryExprAST *binaryexpr){
  PrimaryExprAST *lhs = binaryexpr->getLHS();
  PrimaryExprAST *rhs = binaryexpr->getRHS();

  Value *lhs_v = generatePrimaryExpr(lhs);
  Value *rhs_v = generatePrimaryExpr(rhs);

  //assignment
  if(binaryexpr->getOp() == "たして"){
    return Builder->CreateAdd(lhs_v, rhs_v, "add_tmp");
  }
  else if(binaryexpr->getOp() == "ひいて"){
    return Builder->CreateSub(lhs_v, rhs_v, "sub_tmp");
  }
  else if(binaryexpr->getOp() == "かけて"){
    return Builder->CreateMul(lhs_v, rhs_v, "mul_tmp");
  }
  else if(binaryexpr->getOp() == "わって"){
    return Builder->CreateSDiv(lhs_v, rhs_v, "div_tmp");
  }

  return NULL;
}
Value *CodeGen::generatePrimaryExpr(PrimaryExprAST *primaryexpr){
  if(primaryexpr->getValueID() == IdentifierID)
    return generateIdentifier((IdentifierAST*)primaryexpr);
  else if(primaryexpr->getValueID() == NumberID)
    return generateNumber((NumberAST*)primaryexpr);
  else if(primaryexpr->getValueID() == StringID)
    return generateString((StringAST*)primaryexpr);
  else
    return NULL;
}

Value *CodeGen::generateIdentifier(IdentifierAST* identifier){
  return Builder->CreateLoad(Variables[identifier->getName()], "var_tmp");
}

Value *CodeGen::generateNumber(NumberAST *num){
  return ConstantInt::get(Builder->getInt32Ty(), num->getNumberValue());
}
Value *CodeGen::generateString(StringAST *str){
  Value *val =  Builder->CreateGlobalStringPtr(str->getStringValue());
  return val;
}




void CodeGen::test(){
  Module *mod = new Module("testModule", getGlobalContext());
  IRBuilder<> *buil = new IRBuilder<>(getGlobalContext());
  vector<string> s;
  s.push_back("arg1");
  s.push_back("arg2");
  
  vector<Type*> arg_types = vector<Type*>();
  for(int i = 0; i < 2; i++){
    arg_types.push_back(buil->getInt32Ty());
  }
  Type* returnType = buil->getInt32Ty();
  FunctionType *func_type = FunctionType::get(returnType, arg_types, false);
  Function *func = Function::Create(func_type, Function::ExternalLinkage, "test_func", mod);

  Function::arg_iterator arg_iter = func->arg_begin();
  for(int i = 0; i < s.size(); i++){
    arg_iter->setName(s.at(i));
    arg_iter++;
  }

  BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", func);
  buil->SetInsertPoint(bblock);

  Type *type;
  AllocaInst *alloca;
  for(int i = 0; i < s.size(); i++){
    type = buil->getInt32Ty();
    alloca = buil->CreateAlloca(type, 0, s.at(i));
    ValueSymbolTable &vs_table = func->getValueSymbolTable();
    buil->CreateStore(vs_table.lookup(s.at(i)), alloca);
  }

  ValueSymbolTable &vs_table1 = func->getValueSymbolTable();
  Value *sdecl = vs_table1.lookup("ala");
  sdecl->dump();
  Value *lhs_v = buil->CreateLoad(alloca, "var_tmp");
  
  //  ValueSymbolTable &vs_table2 = func->getValueSymbolTable();
  //Value *rhs_v = buil->CreateLoad(vs_table2.lookup(s.at(1)), "var_tmp");
  

  mod->dump();
}


Value *CodeGen::generatePrint(Module *mod){

  //create arg_types
  vector<Type*> arg_types_num = vector<Type*>();
  vector<Type*> arg_types_str = vector<Type*>();
  arg_types_num.push_back(Builder->getInt32Ty());
  arg_types_str.push_back(Builder->getInt8Ty()->getPointerTo());
 
  
  //create return type
  Type* returnTypeNum;
  Type* returnTypeStr;
  returnTypeNum = Builder->getInt32Ty();
  returnTypeStr = Builder->getInt8Ty()->getPointerTo();

  //create func type
  FunctionType *func_type_num = FunctionType::get(returnTypeNum, arg_types_num, false);
  FunctionType *func_type_str = FunctionType::get(returnTypeStr, arg_types_str, false);

  //create function
  string func_name_num = printNum;
  string func_name_str = printStr;
  Function *func_num = Function::Create(func_type_num, Function::ExternalLinkage, func_name_num, mod);
  Function *func_str = Function::Create(func_type_str, Function::ExternalLinkage, func_name_str, mod);

  return NULL;
}
