#ifndef __PARSER_H_
#define __PARSER_H_

#include "common.h"
#include <iostream>
#include <vector>
#include <map>
#include "ast.h"
#include "tokenstream.h"
#include "lexer.h"

using namespace std;

class Parser{
 private:
  Lexer *lexer;
  TokenStream *Tokens;
  TranslationUnitAST *TU;
  bool error;
  string errorlog;

  // 意味解析用
  vector<IdentifierAST*> *VariableTable;
  vector<FunctionDefAST*> FunctionTable;
  NanoType returnType;

 public:
  Parser(string filename);
  ~Parser(){
    SAFE_DELETE(TU);
    SAFE_DELETE(Tokens);
  }
  bool doParse();
  void printTokens();
  TranslationUnitAST &getAST();

 private:
  bool visitTranslationUnit();
  ExternalDeclAST *visitExternalDeclaration();
  FunctionDeclAST *visitFunctionDeclaration();
  FunctionDefAST *visitFunctionDef();
  FunctionParamsAST *visitFunctionParametersDecl();
  bool visitFunctionParameterDecl(vector<IdentifierAST*> *params);
  FunctionBodyAST *visitFunctionBody();
  vector<StatementAST*> *visitStatements();
  bool visitStatement(vector<StatementAST*> *stmts);
  ExprStmtAST *visitExpressionStatement();
  CallStmtAST *visitCallStatement();
  ReturnStmtAST *visitReturnStatement();

  ExprAST *visitExpression();
  AssignmentExprAST *visitAssignmentExpression();
  CallExprAST *visitCallExpression();
  vector<PrimaryExprAST*> *visitCallArgs();
  bool visitCallArg(vector<PrimaryExprAST*> *args);
  AdditiveExprAST *visitAdditiveExpression();
  BinaryExprAST *visitBinaryExpression();
  PrimaryExprAST *visitPrimaryExpression(int flag);
  IdentifierAST *visitIdentifier(int flag);
  NumberAST *visitInteger(int flag);
  StringAST *visitStrings(int flag);

  int visitDigit(string s);

  bool checkFunctionTable(string func_name, vector<PrimaryExprAST*> *args);
  bool checkFunctionTable(string func_name, vector<IdentifierAST*> *args);
  bool checkFunctionTable(string func_name);
  void addFunctionTable(FunctionDefAST* func_def);
  bool checkVariableTable(string name);
  bool checkTypeVariableTable(string name, NanoType type);
  NanoType getTypeVariableTable(string name);
  void addVariableTable(IdentifierAST *identifier);

};

#endif
