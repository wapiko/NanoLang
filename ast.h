#ifndef __AST_H_
#define __AST_H_

#include "common.h"
#include <iostream>
#include <vector>
#include <map>
#include "baseast.h"

using namespace std;

enum NanoType{
  ERROR,
  VOID,
  INT,
  STRING
};

class StatementAST: public BaseAST{
 public:
 StatementAST() : BaseAST(StatementID){}
 StatementAST(AstID id) : BaseAST(id){}
  ~StatementAST(){}
  void print();
};

class ExprAST : public BaseAST{
 public:
  ExprAST() : BaseAST(ExprID){}
 ExprAST(AstID id) : BaseAST(id){}
  ~ExprAST(){}
  void print();
};

class AdditiveExprAST : public ExprAST{
 public:
 AdditiveExprAST() : ExprAST(AdditiveExprID){}
 AdditiveExprAST(AstID ID) : ExprAST(ID){}
  ~AdditiveExprAST(){}
  void print();
  virtual NanoType getType(){ return VOID; }

};

class PrimaryExprAST : public AdditiveExprAST{
 public:
 PrimaryExprAST() : AdditiveExprAST(PrimaryExprID){}
 PrimaryExprAST(AstID ID) : AdditiveExprAST(ID){}
  ~PrimaryExprAST(){}
  void print();
  virtual string ifIdentifierGetString(){ return ""; }
  virtual bool ifIdentifierIsNULL(){ return false; }
  NanoType getType(){ return VOID; }
};

class IdentifierAST : public PrimaryExprAST{
  string Identifier;
  NanoType type;

 public:
  IdentifierAST(string identifier) 
    : PrimaryExprAST(IdentifierID), Identifier(identifier){ type = ERROR; }
  ~IdentifierAST(){}

  static inline bool classof(IdentifierAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == IdentifierID;
  }
  string getName();
  bool setName(string name);
  void print();
  string ifIdentifierGetString(){ return getName(); }
  bool ifIdentifierIsNULL(){ if(getName() == "") return true; else return false; }
  NanoType getType(){ return type; }
  void setType(NanoType t) { type = t; }
};



class ExprStmtAST : public StatementAST{
  ExprAST *Expr;

 public:
 ExprStmtAST(ExprAST *expr) 
   : StatementAST(ExprStmtID), Expr(expr){}
 ExprStmtAST(AstID id, ExprAST *expr) 
   : StatementAST(id), Expr(expr){}
  ~ExprStmtAST(){SAFE_DELETE(Expr);}

  ExprAST *getExpr();
  void print();
};

class VariableDeclAST: public BaseAST{
 public:
  typedef enum{
    param,
    local
  }DeclType;
 private:
  IdentifierAST *Name;
  DeclType Type;
 public:
  VariableDeclAST(IdentifierAST *name) 
    : BaseAST(VariableDeclID), Name(name){}
  ~VariableDeclAST(){SAFE_DELETE(Name);}

  static inline bool classof(VariableDeclAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == VariableDeclID;
  }

  string getName();
  bool setDeclType(DeclType type);
  DeclType getType(){ return Type; }
  void print();
};

class AssignmentExprAST : public ExprAST{
  IdentifierAST *Identifier;
  AdditiveExprAST *Expr;

  VariableDeclAST VariableDecl;
 public:
 AssignmentExprAST(IdentifierAST *identifier, AdditiveExprAST *expr)
   : ExprAST(AssignmentExprID), Identifier(identifier), Expr(expr), VariableDecl(NULL){}
  ~AssignmentExprAST(){SAFE_DELETE(Identifier); SAFE_DELETE(Expr);}

  IdentifierAST *getIdentifier();
  string getName();
  AdditiveExprAST *getAdditiveExpr();
  void print();
};

class ReturnStmtAST : public StatementAST{
  PrimaryExprAST *Expr;
 public:
  ReturnStmtAST(PrimaryExprAST *expr)
    :StatementAST(ReturnStmtID), Expr(expr){}
  ~ReturnStmtAST(){SAFE_DELETE(Expr)};

  static inline bool classof(ReturnStmtAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == ReturnStmtID;
  }
  PrimaryExprAST *getExpr();
  void print();
};


class CallExprAST : public ExprAST{
  IdentifierAST *Callee;
  vector<PrimaryExprAST*> *Args;

 public:
  CallExprAST(IdentifierAST *callee, vector<PrimaryExprAST*> *args)
    : ExprAST(CallExprID), Callee(callee), Args(args){}
  ~CallExprAST(){SAFE_DELETE(Callee); SAFE_DELETE(Args);}

  static inline bool classof(CallExprAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == CallExprID;
  }
  IdentifierAST *getCallee();
  vector<PrimaryExprAST*> *getArgs();
  PrimaryExprAST *getArg(int i);
  void print();
};

class CallStmtAST : public StatementAST{
  CallExprAST *Expr;
 public:
 CallStmtAST(CallExprAST *expr) 
   : StatementAST(CallStmtID), Expr(expr){}
 CallStmtAST(AstID id, CallExprAST *expr) 
   : StatementAST(id), Expr(expr){}
  ~CallStmtAST(){SAFE_DELETE(Expr);}

  CallExprAST *getExpr();
  void print();
};

class VariableAST : public BaseAST{
  IdentifierAST *Identifier;

 public:
 VariableAST(IdentifierAST *identifier) : BaseAST(VariableID), Identifier(identifier){}
  ~VariableAST(){SAFE_DELETE(Identifier);}

  static inline bool classof(VariableAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == VariableID;
  }
  IdentifierAST *getIdentifier();
  void print();
};

class BinaryExprAST : public AdditiveExprAST{
 private:
  string Op;
  PrimaryExprAST *LHS, *RHS;

 public:
 BinaryExprAST(string op, PrimaryExprAST *lhs, PrimaryExprAST *rhs)
   : AdditiveExprAST(BinaryExprID), Op(op), LHS(lhs), RHS(rhs){}
 BinaryExprAST(AstID ID, string op, PrimaryExprAST *lhs, PrimaryExprAST *rhs)
   : AdditiveExprAST(ID), Op(op), LHS(lhs), RHS(rhs){}
  ~BinaryExprAST(){
    SAFE_DELETE(LHS);
    SAFE_DELETE(RHS);
  }

  static inline bool classof(BinaryExprAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == BinaryExprID;
  }

  string getOp();
  PrimaryExprAST *getLHS();
  PrimaryExprAST *getRHS();
  NanoType getType();
  void print();
};



class NumberAST : public PrimaryExprAST{
  int Val;
  
 public:
 NumberAST(int val) : PrimaryExprAST(NumberID), Val(val){}
  ~NumberAST(){}

  static inline bool classof(NumberAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == NumberID;
  }
  int getNumberValue();
  void print();
  string ifIdentifierGetString(){ return ""; }
  bool ifIdentifierIsNULL(){return false;}
  NanoType getType(){ return INT; }
};

class StringAST : public PrimaryExprAST{
  string Val;

 public:
 StringAST(string val) : PrimaryExprAST(StringID), Val(val){}
  ~StringAST(){}

  static inline bool classof(StringAST const*){
    return true;
  }
  static inline bool classof(BaseAST const* base){
    return base->getValueID() == StringID;
  }
  string getStringValue();
  void print();
  string ifIdentifierGetString(){ return ""; }
  bool ifIdentifierIsNULL(){return false;}
  NanoType getType(){ return STRING; }
};

class FunctionParamsAST : public BaseAST{
  vector<IdentifierAST*> *Params;

 public:
  FunctionParamsAST(vector<IdentifierAST*> *params)
   : BaseAST(FunctionParamsID), Params(params){}
  ~FunctionParamsAST(){SAFE_DELETE(Params);}

  vector<IdentifierAST*> *getParams();
  IdentifierAST *getParamIdentifier(int i);
  string getParamName(int i);
  NanoType getParamType(int i);
  int getParamNum();
  bool checkArgs(vector<PrimaryExprAST*> *args);
  bool checkArgs(vector<IdentifierAST*> *args);
  void print();
};

class FunctionDefAST : public BaseAST{
  IdentifierAST *Identifier;
  FunctionParamsAST *Params;

  NanoType returnType;

 public:
 FunctionDefAST(IdentifierAST *identifier, FunctionParamsAST *params)
    : BaseAST(FunctionDefID), Identifier(identifier), Params(params){}
  ~FunctionDefAST(){SAFE_DELETE(Identifier); SAFE_DELETE(Params);}

  IdentifierAST *getIdentifier();
  string getName();
  FunctionParamsAST *getParamsAST();
  vector<IdentifierAST*> *getParams();
  IdentifierAST *getParamIdentifier(int i);
  string getParamName(int i);
  NanoType getParamType(int i);
  int getParamNum();
  NanoType getReturnType();
  void setReturnType(NanoType rt);
  bool checkArgs(vector<PrimaryExprAST*> *args);
  bool checkArgs(vector<IdentifierAST*> *args);
  void print();
};

class FunctionBodyAST : public BaseAST{
  vector<VariableDeclAST*> *VariableDecls;
  vector<StatementAST*> *StmtLists; 

 public:
 FunctionBodyAST() : BaseAST(FunctionBodyID){}
 FunctionBodyAST(vector<StatementAST*> *stmts) : BaseAST(FunctionBodyID), StmtLists(stmts){} 
  ~FunctionBodyAST(){SAFE_DELETE(VariableDecls); SAFE_DELETE(StmtLists);}

  bool addVariableDeclaration(VariableDeclAST *vdecl);
  bool addStatement(StatementAST *stmt);
  VariableDeclAST *getVariableDecl(int i);
  StatementAST *getStatement(int i);
  void print();
};

class ExternalDeclAST : public BaseAST{
 public:
 ExternalDeclAST() : BaseAST(ExternalDeclID){}
 ExternalDeclAST(AstID id) : BaseAST(id){}
  ~ExternalDeclAST(){}
  void print();
};


class FunctionDeclAST : public ExternalDeclAST{
  FunctionDefAST *FunctionDef;
  FunctionBodyAST *Body;

  vector<IdentifierAST*> *VariableTable;

 public:
  FunctionDeclAST(FunctionDefAST *proto, FunctionBodyAST *body)
    :ExternalDeclAST(FunctionDeclID), FunctionDef(proto), Body(body){}
  ~FunctionDeclAST(){SAFE_DELETE(FunctionDef); SAFE_DELETE(Body);}

  string getName();
  FunctionDefAST *getFunctionDef();
  FunctionBodyAST *getBody();
  bool addVariableTable(vector<IdentifierAST*> *table);
  void print();
  void printVariableTable();
};


 class TranslationUnitAST : public BaseAST{
  vector<ExternalDeclAST*> Externals;
  

 public:
 TranslationUnitAST() : BaseAST(TranslationUnitID){}
  ~TranslationUnitAST(){}

  bool addExternal(ExternalDeclAST *func);
  bool empty();
  ExternalDeclAST *getExternal(int i);
  void print();
};

#endif
