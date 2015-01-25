#ifndef __BASEAST_H_
#define __BASEAST_H_

#include "common.h"
#include <iostream>

using namespace std;

enum AstID{
  BaseID,  //0
  TranslationUnitID,
  ExternalDeclID,
  FunctionDefID,
  FunctionBodyID,
  FunctionParamsID,//5
  FunctionDeclID,  
  VariableDeclID,
  StatementID,
  ExprID,
  ExprStmtID,//10
  CallStmtID,   
  ReturnStmtID,
  CallExprID,
  VariableID,
  AssignmentExprID,//15
  AdditiveExprID,    
  PrimaryExprID,
  BinaryExprID,
  NumberID,
  StringID,//20
  IdentifierID     
};

class BaseAST{
 private:
  AstID ID;

 public:
  BaseAST(AstID id):ID(id){};
  virtual ~BaseAST(){};
  AstID getValueID() const{
    return ID;
  }
  virtual void print(){};
};



#endif
