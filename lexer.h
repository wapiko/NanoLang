#ifndef __LEXER_H_
#define __LEXER_H_

#include "common.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include "token.h"
#include "tokenstream.h"
#include "utfstring.h"

using namespace std;

class Lexer{
 private:
 public:
  Lexer(){}
  TokenStream* LexicalAnalysis(string input_filename);
  bool isSpace(string s);
  bool isDigit(string s);
  bool isZero(string s);
  bool isLeftBrace(string s);
  bool isRightBrace(string s);
  bool isLeftParenthesis(string s);
  bool isRightParenthesis(string s);
  bool isLeftKakko(string s);
  bool isRightKakko(string s);
  bool isColon(string s);
  bool isSymbol(string s);
};

#endif
