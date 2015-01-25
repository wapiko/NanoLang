#ifndef __TOKEN_H_
#define __TOKEN_H_

#include "common.h"
#include <iostream>
#include <stdlib.h>
#include "utfstring.h"

using namespace std;

enum TokenType{
  TOK_MOJIRETSU,
  TOK_DIGIT,
  TOK_LEFTBRACE,
  TOK_RIGHTBRACE,
  TOK_LEFTPARENTHESIS,
  TOK_RIGHTPARENTHESIS,
  TOK_COLON,
  TOK_LEFTKAKKO,
  TOK_RIGHTKAKKO,
  TOK_NANO,
  TOK_KOTONANO,
  TOK_KAESHITE,
  TOK_KAKETE,
  TOK_WATTE,
  TOK_TASHITE,
  TOK_HIITE,
  TOK_MEIN,
  TOK_NULL,
  TOK_EOF,

  TOK_KAZU,
  TOK_MOJI
};

class Token{
private:
  TokenType Type;
  UTFString *TokenString;
  int Number;
  int Line;

 public:
  Token(UTFString *str, TokenType type, int line)
    : TokenString(str), Type(type), Line(line){
    if(type == TOK_DIGIT)
      Number = atoi(str->c_str());
    else
      Number = 0x7fffffff;
  };
  ~Token(){};

  TokenType getTokenType(){
    return Type;
  };

  UTFString *getTokenString(){
    return TokenString;
  };

  int getNumberValue(){
    return Number;
  };

  int getLine(){
    return Line;
  }

  int getLength(){
    UTFString *str = getTokenString();
    return str->length();
  }

  string getChar(int i){
    UTFString *str = getTokenString();
    return str->at(i);
  }

  string getLastChar(){
    UTFString *str = getTokenString();
    int len = str->length();
    return str->at(len-1);
  }
  string getExceptLastChar(){
    UTFString *str = getTokenString();
    int len = str->length();
    string ret = "";
    for(int i = 0; i < len-1; i++){
      ret = ret + str->at(i);
    }
    return ret;
  }
  string getSubstr(int a, int b){
    UTFString *str = getTokenString();
    return str->substr(a,b);
  }
};


#endif
