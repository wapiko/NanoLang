#ifndef __TOKENSTREAM_H_
#define __TOKENSTREAM_H_

#include "common.h"
#include "token.h"
#include <iostream>
#include <stdlib.h>
#include <vector>

using namespace std;

class TokenStream {

 private:
  vector<Token*> Tokens;
  int CurIndex;

 public:
  TokenStream()
   :CurIndex(0){};
  ~TokenStream();

  bool ungetToken(int Times=1);
  bool getNextToken();
  bool pushToken(Token *token){
    Tokens.push_back(token);
    return true;
  };
  Token getToken();

  TokenType getCurType(){
    return Tokens[CurIndex]->getTokenType();
  };

  UTFString *getCurUTFString(){
    return Tokens[CurIndex]->getTokenString();
  };

  string getCurString(){
    return Tokens[CurIndex]->getTokenString()->getString();
  };

  string getCurLastChar(){
    return Tokens[CurIndex]->getLastChar();
  };

  string getCurExceptLastChar(){
    return Tokens[CurIndex]->getExceptLastChar();
  };
  
  string getCurChar(int i){
    return Tokens[CurIndex]->getChar(i);
  };

  int getCurStringLength(){
    return Tokens[CurIndex]->getLength();
  };

  string getCurSubstr(int a, int b){
    return Tokens[CurIndex]->getSubstr(a, b);
  }

  int getCurNumVal(){
    return Tokens[CurIndex]->getNumberValue();
  };

  int getCurIndex(){
    return CurIndex;
  };

  bool applyTokenIndex(int index){
    CurIndex = index;
    return true;
  };

  bool printTokens();
};

#endif
