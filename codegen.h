#include "common.h"
#include "parser.h"
#include <map>
#include <iostream>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/ValueSymbolTable.h"

using namespace std;
using namespace llvm;

class CodeGen{
 private:
  Function *CurFunc;
  Module *Mod;
  IRBuilder<> *Builder;
  map<string, AllocaInst*> Variables;

  string printNum = "printnum";
  string printStr = "printstr";

 public:
  CodeGen();
  ~CodeGen();
  bool doCodeGen(TranslationUnitAST &tunit, string name);
  Module  &getModule();

  void test();

 private:
  bool generateTranslationUnit(TranslationUnitAST &tunit, string name);
  Function *generateExternalDeclaration(ExternalDeclAST *external, Module *Mod);
  Function *generateFunctionDeclaration(FunctionDeclAST *func_decl, Module *mod); 
  Value *generateFunctionBody(FunctionBodyAST *func_body);
  Value *generateStatement(StatementAST *stmt);
  Value *generateFunctionParams(FunctionParamsAST *func_params);
  Value *generateExprStmt(ExprStmtAST *exprstmt);
  Value *generateReturnStmt(ReturnStmtAST *returnstmt);
  Value *generateCallStmt(CallStmtAST *callstmt);
  Value *generateAssignmentExpr(AssignmentExprAST *assignexpr);
  Value *generateAdditiveExpr(AdditiveExprAST *additiveexpr);
  Value *generateBinaryExpr(BinaryExprAST *binaryexpr);
  Value *generatePrimaryExpr(PrimaryExprAST *primaryexpr);
  Value *generateIdentifier(IdentifierAST *identifier);
  Value *generateNumber(NumberAST *num);
  Value *generateString(StringAST *str);

  Value *generatePrint(Module *mod);

};
