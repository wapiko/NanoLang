#include "tokenstream.h"

// デストラクタ
TokenStream::~TokenStream(){
  for(int i = 0; i < Tokens.size(); i++){
    SAFE_DELETE(Tokens[i]);
  }
  Tokens.clear();
}

Token TokenStream::getToken(){
  return *Tokens[CurIndex];
}

bool TokenStream::getNextToken(){
  int size=Tokens.size();
  if(--size<=CurIndex){
    return false;
  }
  else{
    CurIndex++;
    return true;
  }
}

bool TokenStream::ungetToken(int times){
  for(int i = 0; i < times; i++){
    if(CurIndex == 0)
      return false;
    else
      CurIndex--;
  }
  return true;
}

bool TokenStream::printTokens(){
  
  for(vector<Token*>::iterator titer = Tokens.begin(); titer != Tokens.end(); titer++){
    fprintf(stdout, "%d:", (*titer)->getTokenType());
    if((*titer)->getTokenType()!=TOK_EOF)
      fprintf(stdout, "%s\n", (*titer)->getTokenString()->getString().c_str());
  }
  return true;
}
