#include "ast.h"


void StatementAST::print(){
  cout<<StatementID<<":"<<endl;
}

void ExprAST::print(){
  cout<<ExprID<<": ";
}

void AdditiveExprAST::print(){
  cout<<AdditiveExprID<<": ";  
}



void PrimaryExprAST::print(){
  cout<<PrimaryExprID<<": ";  
}

string IdentifierAST::getName(){
  return Identifier;
}
bool IdentifierAST::setName(string name){
  Identifier = name;
  return true;
}
void IdentifierAST::print(){
  cout<<IdentifierID<<":"<<getName();
}

void ExprStmtAST::print(){
  cout<<ExprStmtID<<": ";
  getExpr()->print();
  cout<<" なの"<<endl;
}
ExprAST *ExprStmtAST::getExpr(){
  return Expr;
}

string VariableDeclAST::getName(){
  return Name->getName();
}
bool VariableDeclAST::setDeclType(DeclType type){
  Type = type;
  return true;
}
void VariableDeclAST::print(){
  cout<<VariableDeclID<<":"<< getName() << endl;
}

IdentifierAST *AssignmentExprAST::getIdentifier(){
  return Identifier;
}
string AssignmentExprAST::getName(){
  return getIdentifier()->getName();
}
AdditiveExprAST *AssignmentExprAST::getAdditiveExpr(){
  return Expr;
}
void AssignmentExprAST::print(){
  cout<<AssignmentExprID<<": ";
  getIdentifier()->print();
  cout<<" は ";
  getAdditiveExpr()->print();
}

PrimaryExprAST *ReturnStmtAST::getExpr(){
  return Expr;
}
void ReturnStmtAST::print(){
  cout<<ReturnStmtID<<": ";
  getExpr()->print();
  cout<<" を かえして なの"<<endl;
}

IdentifierAST *CallExprAST::getCallee(){
  return Callee;
}
vector<PrimaryExprAST*> *CallExprAST::getArgs(){
  return Args;
}
PrimaryExprAST *CallExprAST::getArg(int i){
  if(i < Args->size())
    return Args->at(i);
  else 
    return NULL;
}
void CallExprAST::print(){
  cout<<CallExprID<<": ";
  for(int i = 0; i < getArgs()->size(); i++){
    PrimaryExprAST *Expr = getArgs()->at(i);
    Expr->print();
    if(i == getArgs()->size() - 1) cout<<" を ";
    else cout<<" と";
  }
  getCallee()->print();
}

CallExprAST *CallStmtAST::getExpr(){
  return Expr;
}
void CallStmtAST::print(){
  cout<<CallStmtID<<": ";
  getExpr()->print();
  cout<<" なの"<<endl;
}

IdentifierAST *VariableAST::getIdentifier(){
  return Identifier;
}
void VariableAST::print(){
  cout<<VariableID<<": ";
  getIdentifier()->print();
}

string BinaryExprAST::getOp(){
  return Op;
}
PrimaryExprAST *BinaryExprAST::getLHS(){
  return LHS;
}
PrimaryExprAST *BinaryExprAST::getRHS(){
  return RHS;
}
NanoType BinaryExprAST::getType(){
  NanoType l = LHS->getType();
  NanoType r = RHS->getType();
  if(l == r) return l;
  else return ERROR;
}
void BinaryExprAST::print(){
  cout<<BinaryExprID<<": ";
  getLHS()->print();
  cout<<" と ";
  getRHS()->print();
  cout<<" を ";
  cout<<getOp();
}

int NumberAST::getNumberValue(){
  return Val;
}
void NumberAST::print(){
  cout<<NumberID<<": ";
  cout<<getNumberValue();
}

string StringAST::getStringValue(){
  return Val;
}
void StringAST::print(){
  cout<<StringID<<": ";
  cout<<getStringValue();
}

vector<IdentifierAST*> *FunctionParamsAST::getParams(){
  return Params;
}
IdentifierAST *FunctionParamsAST::getParamIdentifier(int i){
  if(i < Params->size())
    return Params->at(i);
  else
    return NULL;
}
string FunctionParamsAST::getParamName(int i){
  if(i < Params->size())
    return Params->at(i)->getName();
  else
    return "";
}
NanoType FunctionParamsAST::getParamType(int i){
  if(i < Params->size())
    return Params->at(i)->getType();
  else
    return VOID;
}
int FunctionParamsAST::getParamNum(){
  return Params->size();
}
bool FunctionParamsAST::checkArgs(vector<PrimaryExprAST*> *args){
  if(getParamNum() != args->size()){
    return false;
  }
  else{
    for(int i = 0; i < getParamNum(); i++){
      if(Params->at(i)->getType() != args->at(i)->getType())
        return false;
    }
    return true;
  }
}
bool FunctionParamsAST::checkArgs(vector<IdentifierAST*> *args){
  if(getParamNum() != args->size()){
    return false;
  }
  else{
    for(int i = 0; i < getParamNum(); i++){
      if(Params->at(i)->getType() != args->at(i)->getType())
        return false;
    }
    return true;
  }
}
void FunctionParamsAST::print(){
  cout<<FunctionParamsID<<": ";
  for(int i = 0; i < Params->size(); i++){
    getParamIdentifier(i)->print();
    if(i == Params->size() - 1) cout<<" で ";
    else cout<<" と ";
  }
}

IdentifierAST *FunctionDefAST::getIdentifier(){
  return Identifier;
}
string FunctionDefAST::getName(){
  return getIdentifier()->getName();
}
FunctionParamsAST *FunctionDefAST::getParamsAST(){
  return Params;
}
vector<IdentifierAST*> *FunctionDefAST::getParams(){
  return Params->getParams();
}
IdentifierAST *FunctionDefAST::getParamIdentifier(int i){
  return Params->getParamIdentifier(i);
}
string FunctionDefAST::getParamName(int i){
  return Params->getParamName(i);
}
NanoType FunctionDefAST::getParamType(int i){
  return Params->getParamType(i);
}
int FunctionDefAST::getParamNum(){
  return Params->getParamNum();
}
NanoType FunctionDefAST::getReturnType(){
  return returnType;
}
void FunctionDefAST::setReturnType(NanoType rt){
  returnType = rt;
}
bool FunctionDefAST::checkArgs(vector<PrimaryExprAST*> *args){
  return Params->checkArgs(args);
}
bool FunctionDefAST::checkArgs(vector<IdentifierAST*> *args){
  return Params->checkArgs(args);
}
void FunctionDefAST::print(){
  cout<<FunctionDefID<<": ";
  getIdentifier()->print();
  cout<<" は ";
  Params->print();
}

bool FunctionBodyAST::addVariableDeclaration(VariableDeclAST *vdecl){
  VariableDecls->push_back(vdecl);
  return true;
}
bool FunctionBodyAST::addStatement(StatementAST *stmt){
  StmtLists->push_back(stmt);
  return true;
}
VariableDeclAST *FunctionBodyAST::getVariableDecl(int i){
  if(i < VariableDecls->size())
    return VariableDecls->at(i);
  else
    return NULL;
}
StatementAST *FunctionBodyAST::getStatement(int i){
  if(i < StmtLists->size())
    return StmtLists->at(i);
  else
    return NULL;
}
void FunctionBodyAST::print(){
  cout<<FunctionBodyID<<": ";
  for(int i = 0; i < StmtLists->size(); i++){
    getStatement(i)->print();
  }
}

void ExternalDeclAST::print(){
  cout<<ExternalDeclID<<": "<<endl;
}

string FunctionDeclAST::getName(){
  return FunctionDef->getName();
}
FunctionDefAST *FunctionDeclAST::getFunctionDef(){
  return FunctionDef;
}
FunctionBodyAST *FunctionDeclAST::getBody(){
  return Body;
}
bool FunctionDeclAST::addVariableTable(vector<IdentifierAST*> *table){
  VariableTable = table;
  return true;
}
void FunctionDeclAST::print(){
  cout<<FunctionDeclID<<": ";
  getFunctionDef()->print();
  cout<<endl;
  getBody()->print();
  cout<<endl;
  cout<<"ってことなの"<<endl;
}
void FunctionDeclAST::printVariableTable(){

  for(int i = 0; i < VariableTable->size(); i++){
    cout << VariableTable->at(i)->getType() << ":" << VariableTable->at(i)->getName() << endl;
  }
}

bool TranslationUnitAST::addExternal(ExternalDeclAST *func){
  Externals.push_back(func);
  return true;
}
bool TranslationUnitAST::empty(){
  if(Externals.size() == 0) return true;
  else return false;
}
ExternalDeclAST *TranslationUnitAST::getExternal(int i){
  if(i < Externals.size())
    return Externals.at(i);
  else
    return NULL;
}
void TranslationUnitAST::print(){
  cout<<TranslationUnitID<<": ";
  for(int i = 0; i < Externals.size(); i++){
    getExternal(i)->print();
  }
  cout<<endl;
}
