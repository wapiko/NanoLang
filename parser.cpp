#include "parser.h"


using namespace std;


Parser::Parser(string filename){
  lexer=new Lexer();
  error = false;
  Tokens=lexer->LexicalAnalysis(filename);
}

bool Parser::doParse(){
  if(!Tokens){
    fprintf(stderr, "字句解析で失敗してるわね...\n");
    return false;
  }
  else
    return visitTranslationUnit();
}

TranslationUnitAST &Parser::getAST(){
  if(TU)
    return *TU;
  else
    return *(new TranslationUnitAST());
}

bool Parser::visitTranslationUnit(){
  TU = new TranslationUnitAST();
  
  while(true){
    ExternalDeclAST *ed = visitExternalDeclaration();
    if(error){
      cerr<<errorlog<<endl;
      SAFE_DELETE(TU);
      return false;
    }
    else if(ed){
      TU->addExternal(ed);
    }
    else{
      SAFE_DELETE(TU);
      return false;
    }
    if(Tokens->getCurType() == TOK_EOF)
      break;
  }

  return true;
}

ExternalDeclAST *Parser::visitExternalDeclaration(){
  FunctionDeclAST *func_decl = visitFunctionDeclaration();
  if(func_decl){
    return func_decl;
  }

  return NULL;
}

FunctionDeclAST *Parser::visitFunctionDeclaration(){
  int bkup = Tokens->getCurIndex();

  SAFE_DELETE(VariableTable);
  VariableTable = new vector<IdentifierAST*>();
  returnType = VOID;

  FunctionDefAST *func_def = visitFunctionDef();
  if(!func_def){
    SAFE_DELETE(func_def);
    SAFE_DELETE(VariableTable);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
  FunctionBodyAST *func_body = visitFunctionBody();
  if(!func_body){
    SAFE_DELETE(func_body);
    SAFE_DELETE(VariableTable);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
  FunctionDeclAST *func_decl = new FunctionDeclAST(func_def, func_body);

  //変数テーブルとリターンの情報を関数に登録
  func_decl->addVariableTable(VariableTable);
  func_decl->getFunctionDef()->setReturnType(returnType);
 
  if(Tokens->getCurType() == TOK_KOTONANO){
    // ここで再定義されていないか確認
    if(checkFunctionTable(func_def->getName(), func_def->getParams())){
      error = true;
      errorlog = "「" + func_def->getName() + "」って名前の関数が再定義されてるわよ";
    }

    addFunctionTable(func_def);
    Tokens->getNextToken();
    return func_decl;
  }
  else{
    SAFE_DELETE(func_decl);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

FunctionDefAST *Parser::visitFunctionDef(){
  int bkup = Tokens->getCurIndex();

  IdentifierAST *identifier = new IdentifierAST("");
  if(Tokens->getCurType() == TOK_MOJIRETSU 
     && Tokens->getCurLastChar() == "は"){
    identifier->setName(Tokens->getCurExceptLastChar());
    Tokens->getNextToken();
  }
  else{
    SAFE_DELETE(identifier);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }

  FunctionParamsAST *parameters = visitFunctionParametersDecl();
  FunctionDefAST *func_def = new FunctionDefAST(identifier, parameters);
  //引数を変数テーブルに追加
  for(int i = 0; i < func_def->getParamNum(); i++){
    IdentifierAST *identifier = func_def->getParamIdentifier(i);
    string s = identifier->getName();
    if(checkVariableTable(s)){
      error = true;
      errorlog = "「" + func_def->getName() + "」って名前の関数が「" + s + "」って名前の変数を複数持ってるわよ";
    }
    else{
      addVariableTable(identifier);
    }
  }

  return func_def;
}


FunctionParamsAST *Parser::visitFunctionParametersDecl(){
  int bkup = Tokens->getCurIndex();
  vector<IdentifierAST*> *params = new vector<IdentifierAST*>();
  if(!visitFunctionParameterDecl(params)){
    Tokens->applyTokenIndex(bkup);
    return new FunctionParamsAST(new vector<IdentifierAST*>());
  }
  return new FunctionParamsAST(params);
}

bool Parser::visitFunctionParameterDecl(vector<IdentifierAST*> *params){
  NanoType type = ERROR;
  if(Tokens->getCurType() == TOK_KAZU){
    type = INT;
  }
  else if(Tokens->getCurType() == TOK_MOJI){
    type = STRING;
  }
  else{
    return false;
  }
  Tokens->getNextToken();
  if(!(Tokens->getCurType() == TOK_COLON)){
    return false;
  }
  Tokens->getNextToken();

  if(Tokens->getCurType() == TOK_MOJIRETSU
     && Tokens->getCurLastChar() == "と"){
    IdentifierAST *identifier = new IdentifierAST(Tokens->getCurExceptLastChar());
    identifier->setType(type);
    params->push_back(identifier);
    Tokens->getNextToken();
    if(!visitFunctionParameterDecl(params))
      return false;
    else
      return true;
  }
  else if(Tokens->getCurType() == TOK_MOJIRETSU
	  && Tokens->getCurLastChar() == "で"){
    IdentifierAST *identifier = new IdentifierAST(Tokens->getCurExceptLastChar());
    identifier->setType(type);
    params->push_back(identifier);
    Tokens->getNextToken();
    return true;
  }
  else return false;
}

FunctionBodyAST *Parser::visitFunctionBody(){
  vector<StatementAST*> *statements = visitStatements();
  FunctionBodyAST *func_body = new FunctionBodyAST(statements); 
  return func_body;
}

vector<StatementAST*> *Parser::visitStatements(){
  int bkup = Tokens->getCurIndex();
  vector<StatementAST*> *stmts = new vector<StatementAST*>(); 
  if(!visitStatement(stmts)){
    Tokens->applyTokenIndex(bkup);
    return new vector<StatementAST*>();
  }
  return stmts;
}

bool Parser::visitStatement(vector<StatementAST*> *stmts){
  ReturnStmtAST *rtstmt = visitReturnStatement();
  if(rtstmt){
    stmts->push_back(rtstmt);
    if(!visitStatement(stmts))
      return false;
    else
      return true;
  }

  CallStmtAST *callstmt = visitCallStatement();
  if(callstmt){
    stmts->push_back(callstmt);
    if(!visitStatement(stmts))
      return false;
    else
      return true;
  }

  ExprStmtAST *exprstmt = visitExpressionStatement();
  if(exprstmt){
    stmts->push_back(exprstmt);
    if(!visitStatement(stmts))
      return false;
    else
      return true;
  }
  else
    return true;;
}

ExprStmtAST *Parser::visitExpressionStatement(){
  int bkup = Tokens->getCurIndex();

  ExprAST *expr = visitExpression();
  if(!expr){
    SAFE_DELETE(expr);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
  ExprStmtAST *exprstmt = new ExprStmtAST(expr);
  if(Tokens->getCurType() == TOK_NANO){
    Tokens->getNextToken();
    return exprstmt;
  }
  else{
    SAFE_DELETE(exprstmt);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
}
CallStmtAST *Parser::visitCallStatement(){
  int bkup = Tokens->getCurIndex();

  CallExprAST *callexpr = visitCallExpression();
  if(!callexpr){
    SAFE_DELETE(callexpr);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
  CallStmtAST *callstmt = new CallStmtAST(callexpr);
  if(Tokens->getCurType() == TOK_NANO){
    Tokens->getNextToken();
    //関数の宣言確認
    string s = callexpr->getCallee()->getName();
    if(s == "出力して"){
      if(callexpr->getArgs()->size() != 1){
	error = true;
	errorlog = "出力はね、引数1つじゃないとダメなのよ";
      }
    }
    else if(!checkFunctionTable(s, callexpr->getArgs())){
      error = true;
      errorlog = "「" + s + "」なんて関数ないわよ";
    }

    return callstmt;
  }
  else{
    SAFE_DELETE(callstmt);
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
}

ReturnStmtAST *Parser::visitReturnStatement(){
  int bkup = Tokens->getCurIndex();

  if(Tokens->getCurType() == TOK_MOJIRETSU
     && Tokens->getCurLastChar() == "を"){
    PrimaryExprAST *pmexpr = visitPrimaryExpression(1);
    if(!pmexpr){
      SAFE_DELETE(pmexpr);
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }

    if(Tokens->getCurType() ==TOK_KAESHITE){
      Tokens->getNextToken();
      if(Tokens->getCurType() == TOK_NANO){
	Tokens->getNextToken();

	//返り値型チェック
	NanoType type = pmexpr->getType();
	if(returnType == VOID)
	  returnType = type;
	else if(returnType != type){
	  error = true;
	  errorlog = "この関数...毎回違う型を返してない？";
	}
        
	return new ReturnStmtAST(pmexpr);
      }
    }
  }
  Tokens->applyTokenIndex(bkup);
  return NULL;
}

ExprAST *Parser::visitExpression(){
  int bkup = Tokens->getCurIndex();

  AssignmentExprAST *asexpr = visitAssignmentExpression();
  if(asexpr){
    return asexpr;
  }

  AdditiveExprAST *adexpr = visitAdditiveExpression();
  if(adexpr){
    return adexpr;
  }
  
  Tokens->applyTokenIndex(bkup);
  return NULL;
}

AssignmentExprAST *Parser::visitAssignmentExpression(){
  int bkup = Tokens->getCurIndex();

  if(Tokens->getCurType() == TOK_MOJIRETSU
     && Tokens->getCurLastChar() == "は"){
    IdentifierAST *identifier = new IdentifierAST(Tokens->getCurExceptLastChar());

    Tokens->getNextToken();
    AdditiveExprAST *adexpr = visitAdditiveExpression();
    if(adexpr){
      AssignmentExprAST *asexpr = new AssignmentExprAST(identifier, adexpr);

      //上書きか新しい変数か判定      
      NanoType type = adexpr->getType();
      identifier->setType(type);
      if(checkVariableTable(asexpr->getName())){
	if(!checkTypeVariableTable(identifier->getName(), identifier->getType())){
	  error = true;
	  errorlog = "「" + identifier->getName() + "」って変数の型、間違えてるわよ";
	}
      }
      else {
	addVariableTable(identifier);
      }
      return asexpr;
    }
  }

  Tokens->applyTokenIndex(bkup);
  return NULL;
}

CallExprAST *Parser::visitCallExpression(){
  int bkup = Tokens->getCurIndex();

  vector<PrimaryExprAST*> *args = visitCallArgs();
  if(Tokens->getCurType() == TOK_MOJIRETSU){
    IdentifierAST *identifier = new IdentifierAST(Tokens->getCurString());
    Tokens->getNextToken();
    CallExprAST *callexpr = new CallExprAST(identifier, args);
    return callexpr;
  }
  
  Tokens->applyTokenIndex(bkup);
  return NULL;
}

vector<PrimaryExprAST*> *Parser::visitCallArgs(){
  int bkup = Tokens->getCurIndex();
  vector<PrimaryExprAST*> *args = new vector<PrimaryExprAST*>();
  if(!visitCallArg(args)){
    Tokens->applyTokenIndex(bkup);
    return new vector<PrimaryExprAST*>();
  }
  return args;
}

bool Parser::visitCallArg(vector<PrimaryExprAST*> *args){
  if(Tokens->getCurType() == TOK_MOJIRETSU
     && Tokens->getCurLastChar() == "と"){
    PrimaryExprAST *arg = visitPrimaryExpression(1);
    args->push_back(arg);
    if(!visitCallArg(args))
      return false;
    else
      return true;
  }
  else if(Tokens->getCurType() == TOK_MOJIRETSU
	  && Tokens->getCurLastChar() == "を"){
    PrimaryExprAST *arg = visitPrimaryExpression(1);
    args->push_back(arg);
    return true;
  }
  else return false;
}

AdditiveExprAST *Parser::visitAdditiveExpression(){
  int bkup = Tokens->getCurIndex();
  BinaryExprAST *biexpr = visitBinaryExpression();
  if(biexpr){
    return biexpr;
  }

  PrimaryExprAST *pmexpr = visitPrimaryExpression(0);
  if(pmexpr){
    return pmexpr;
  }


  Tokens->applyTokenIndex(bkup);
  return NULL;
}
BinaryExprAST *Parser::visitBinaryExpression(){
  int bkup = Tokens->getCurIndex();

  if(Tokens->getCurType() == TOK_MOJIRETSU
     && Tokens->getCurLastChar() == "と"){
    PrimaryExprAST *lhs = visitPrimaryExpression(1);
    if(lhs && Tokens->getCurType() == TOK_MOJIRETSU
       && Tokens->getCurLastChar() == "を"){
      PrimaryExprAST *rhs = visitPrimaryExpression(1);
      if(rhs){
	if(Tokens->getCurType() == TOK_KAKETE
	   || Tokens->getCurType() == TOK_WATTE
	   || Tokens->getCurType() == TOK_TASHITE
	   || Tokens->getCurType() == TOK_HIITE){
	  BinaryExprAST *biexpr = new BinaryExprAST(Tokens->getCurString(), lhs, rhs);
	  //型チェック
	  if(biexpr->getType() == ERROR){
	    error = true;
	    errorlog = "二項式の引数の型が違うみたいよ";
	  }
	  
	  Tokens->getNextToken();
	  return biexpr;
	}
      }
    }
  }
  
  Tokens->applyTokenIndex(bkup);
  return NULL;
}
PrimaryExprAST *Parser::visitPrimaryExpression(int flag){
  int bkup = Tokens->getCurIndex();

  StringAST *str = visitStrings(flag);
  if(str){
    return str;
  }

  NumberAST *num = visitInteger(flag);
  if(num){
    return num;
  }

  IdentifierAST *identifier = visitIdentifier(flag);
  if(identifier){
    return identifier;
  }

  //  Tokens->applyTokenIndex(bkup);
  return NULL;
}
IdentifierAST *Parser::visitIdentifier(int flag){
  int bkup = Tokens->getCurIndex();

  if(Tokens->getCurType() == TOK_MOJIRETSU){
    string s = Tokens->getCurSubstr(0, Tokens->getCurStringLength()-flag);
    Tokens->getNextToken();
    
    //宣言されているかチェック
    IdentifierAST *identifier = new IdentifierAST(s);
    NanoType type = getTypeVariableTable(identifier->getName());
    identifier->setType(type);
    if(s == ""){
      error = true;
      errorlog = "なんか...空白文字っぽい変数呼んでない？";
    }
    else if(type == ERROR){
      error = true;
      errorlog = "「" + identifier->getName() + "」なんて名前の変数ないわよ";
    }

    return identifier;
  }

  Tokens->applyTokenIndex(bkup);
  return NULL;
}

NumberAST *Parser::visitInteger(int flag){
  int bkup = Tokens->getCurIndex();
  
  int num = 0;
  int check = 0;
  for(int i = 0; i < Tokens->getCurStringLength() - flag; i++){
    int temp = visitDigit(Tokens->getCurSubstr(i, i+1));
    check = 1;
    if(temp == -1){
      Tokens->applyTokenIndex(bkup);
      return NULL;
    }
    num = num * 10 + temp;
  }

  if(check == 0){
    Tokens->applyTokenIndex(bkup);
    return NULL;
  }
  Tokens->getNextToken();
  return new NumberAST(num);  
}

int Parser::visitDigit(string s){
  if(s == "0" || s == "０") return 0;
  if(s == "1" || s == "１") return 1;
  if(s == "2" || s == "２") return 2;
  if(s == "3" || s == "３") return 3;
  if(s == "4" || s == "４") return 4;
  if(s == "5" || s == "５") return 5;
  if(s == "6" || s == "６") return 6;
  if(s == "7" || s == "７") return 7;
  if(s == "8" || s == "８") return 8;
  if(s == "9" || s == "９") return 9;
  return -1;
}
StringAST *Parser::visitStrings(int flag){
  int bkup = Tokens->getCurIndex();
  
  if(Tokens->getCurType() == TOK_MOJIRETSU
     &&Tokens->getCurChar(0) == "「"
     &&Tokens->getCurChar(Tokens->getCurStringLength() - 1 -flag) == "」"){
    string sub = Tokens->getCurSubstr(1,Tokens->getCurStringLength() - 1 - flag); 
    Tokens->getNextToken();
    return new StringAST(sub);
  }

  Tokens->applyTokenIndex(bkup);
  return NULL;
}

void Parser::printTokens(){
  Tokens->printTokens();
}

bool Parser::checkFunctionTable(string func_name, vector<PrimaryExprAST*> *args){
  for(int i = 0; i < FunctionTable.size(); i++){
    if(FunctionTable.at(i)->getName() == func_name
       && FunctionTable.at(i)->checkArgs(args)){
      return true;
    }
  }
  return false;
}

bool Parser::checkFunctionTable(string func_name, vector<IdentifierAST*> *args){
  for(int i = 0; i < FunctionTable.size(); i++){
    if(FunctionTable.at(i)->getName() == func_name
       && FunctionTable.at(i)->checkArgs(args)){
      return true;
    }
  }
  return false;
}

bool Parser::checkFunctionTable(string func_name){
 for(int i = 0; i < FunctionTable.size(); i++){
   if(FunctionTable.at(i)->getName() == func_name){
      return true;
    }
  }
  return false;;
}

void Parser::addFunctionTable(FunctionDefAST* func_def){
  FunctionTable.push_back(func_def);
}

bool Parser::checkVariableTable(string name){
  for(int i = 0; i < VariableTable->size(); i++){
    if(VariableTable->at(i)->getName() == name)
      return true;
  }
  return false;
}

bool Parser::checkTypeVariableTable(string name, NanoType type){
  for(int i = 0; i < VariableTable->size(); i++){
    if(VariableTable->at(i)->getName() == name){
      if(VariableTable->at(i)->getType() == type)
	return true;
      else return false;
    }
  }
  return false;
}
NanoType Parser::getTypeVariableTable(string name){
  for(int i = 0; i < VariableTable->size(); i++){
    if(VariableTable->at(i)->getName() == name){
      return VariableTable->at(i)->getType();	 
    }
  }
  return ERROR;
}
void Parser::addVariableTable(IdentifierAST *identifier){
  VariableTable->push_back(identifier);
}
