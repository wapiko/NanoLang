#include "lexer.h"

TokenStream* Lexer::LexicalAnalysis(string input_filename){
  TokenStream *tokens = new TokenStream(); 
  ifstream ifs;
  string cur_line;
  UTFString *cur_line_utf;
  UTFString *token_str_utf;
  int line_num=0;
  bool iscomment=false;

  ifs.open(input_filename.c_str(), ios::in);
  if(!ifs)
    return NULL;

  while(ifs && getline(ifs, cur_line)){
    cur_line_utf = new UTFString(cur_line);

    string next_char;
    string line;
    Token *next_token;
    int index = 0;
    int length = cur_line_utf->length();

    while(index < length){
      next_char = cur_line_utf->at(index++);
      token_str_utf = new UTFString();

      //コメントを読み飛ばし
      if(iscomment){
	if((length-index) >= 1 && 
	   (next_char == "*"  && (cur_line_utf->at(index)) == "/")){
	  iscomment = false;
	  index++;
        } 
        continue;
      }
      
      //改行か空白
      if(isSpace(next_char)){
	continue;
      }

      //数字
      /*else if(isDigit(next_char)){
	  if(isZero(next_char)){
	    token_str_utf->add(next_char);
	    next_token = new Token(token_str_utf, TOK_DIGIT, line_num);
	  }
	  else{
	    token_str_utf->add(next_char);
	    while(index != length){
	      next_char = cur_line_utf->at(index++);
	      if(isDigit(next_char)){
		  token_str_utf->add(next_char);
	      }
	      else{
		index--;
		break;
	      }
	    }
	    next_token = new Token(token_str_utf, TOK_DIGIT, line_num);
	  }
	  }*/
      
      //コメント
      else if(next_char == "/"){
	token_str_utf->add(next_char);
	next_char = cur_line_utf->at(index++);
	if(next_char == "/"){
	  break;
	}
	else if(next_char == "*"){
	  iscomment=true;
	  continue;
	}
	else{
	  fprintf(stderr, "unclear token: %s", next_char.c_str());
	  SAFE_DELETE(tokens);
	  return NULL;
	}
      }

      //記号たち
      else if(isLeftBrace(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_LEFTBRACE, line_num);
      }
      else if(isRightBrace(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_RIGHTBRACE, line_num);
      }
      else if(isLeftParenthesis(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_LEFTPARENTHESIS, line_num);
      }
      else if(isRightParenthesis(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_RIGHTPARENTHESIS, line_num);
      }
      else if(isColon(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_COLON, line_num);
      }
      /*else if(isLeftKakko(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_LEFTKAKKO, line_num);
      }
      else if(isRightKakko(next_char)){
	token_str_utf->add(next_char);
	next_token = new Token(token_str_utf, TOK_RIGHTKAKKO, line_num);
      }*/

      else if(isLeftKakko(next_char)){
	token_str_utf->add(next_char);
	while(index != length){
	  next_char = cur_line_utf->at(index++);
	  token_str_utf->add(next_char);
	  if(isRightKakko(next_char)){
	    next_token = new Token(token_str_utf, TOK_MOJIRETSU, line_num);
	    break;
	  }
	}
      }

      //文字列
      else{
	token_str_utf->add(next_char);
	while(index != length){
	  next_char = cur_line_utf->at(index++);
	  if(!isSpace(next_char) && !isSymbol(next_char)){
	    token_str_utf->add(next_char);
	  }
	  else {
	    index--;
	    break;
	  }
	}

	if(token_str_utf->getString() == "なの"){
	  next_token = new Token(token_str_utf, TOK_NANO, line_num);
	}
	else if(token_str_utf->getString() == "ってことなの"){
	  next_token = new Token(token_str_utf, TOK_KOTONANO, line_num);
	}
	else if(token_str_utf->getString() == "かえして"){
	  next_token = new Token(token_str_utf, TOK_KAESHITE, line_num);
	}
	else if(token_str_utf->getString() == "かけて"){
	  next_token = new Token(token_str_utf, TOK_KAKETE, line_num);
	}
	else if(token_str_utf->getString() == "わって"){
	  next_token = new Token(token_str_utf, TOK_WATTE, line_num);
	}
	else if(token_str_utf->getString() == "たして"){
	  next_token = new Token(token_str_utf, TOK_TASHITE, line_num);
	}
	else if(token_str_utf->getString() == "ひいて"){
	  next_token = new Token(token_str_utf, TOK_HIITE, line_num);
	}
	else if(token_str_utf->getString() == "かけて"){
	  next_token = new Token(token_str_utf, TOK_KAKETE, line_num);
	}
	else if(token_str_utf->getString() == "めいん"){
	  next_token = new Token(token_str_utf, TOK_MEIN, line_num);
	}
	else if(token_str_utf->getString() == "ぬる"){
	  next_token = new Token(token_str_utf, TOK_NULL, line_num);
	}
	else if(token_str_utf->getString() == "かず"){
	  next_token = new Token(token_str_utf, TOK_KAZU, line_num);
	}
	else if(token_str_utf->getString() == "もじ"){
	  next_token = new Token(token_str_utf, TOK_MOJI, line_num);
	}
	else{
	  next_token = new Token(token_str_utf, TOK_MOJIRETSU, line_num);
	}
      }
      tokens->pushToken(next_token);
    }
    line_num++;
  }

  //EOFの確認
  if(ifs.eof()){
    tokens->pushToken(new Token(new UTFString(), TOK_EOF, line_num));
  }

  ifs.close();

  return tokens;
}

bool Lexer::isSpace(string s){
  if(s == " " || s == "　" || s == "\n")
    return true;
  else 
    return false;
}

bool Lexer::isDigit(string s){
  if(s == "0" || s == "1" || s == "2" || s == "3" || s == "4" ||
     s == "5" || s == "6" || s == "7" || s == "8" || s == "9" ||
     s == "０" || s == "１" || s == "２" || s == "３" || s == "４" ||
     s == "５" || s == "６" || s == "７" || s == "８" || s == "９")
    return true;
  else 
    return false;
}

 bool Lexer::isZero(string s){
   if(s == "0" || s == "０")
     return true;
   else
     return false;
 }

bool Lexer::isLeftBrace(string s){
  if(s == "{" || s == "｛")
    return true;
  else
    return false;
}

bool Lexer::isRightBrace(string s){
  if(s == "}" || s == "｝")
    return true;
  else
    return false;
}

bool Lexer::isLeftParenthesis(string s){
  if(s == "(" || s == "（")
    return true;
  else
    return false;
}

bool Lexer::isRightParenthesis(string s){
  if(s == ")" || s == "）")
    return true;
  else
    return false;
}

bool Lexer::isLeftKakko(string s){
  if(s == "「")
    return true;
  else
    return false;
}

bool Lexer::isRightKakko(string s){
  if(s == "」")
    return true;
  else return false;
}

bool Lexer::isColon(string s){
  if(s == ":" || s == "：")
    return true;
  else
    return false;
}

bool Lexer::isSymbol(string s){
  if(isLeftBrace(s) || isRightBrace(s)
     || isLeftParenthesis(s) || isRightParenthesis(s)
     || isColon(s))
     //     || isLeftKakko(s) || isRightKakko(s))
    return true;
  else
    return false;
}

/*int main(){
  Lexer *lex = new Lexer();
  TokenStream* tokens = lex->LexicalAnalysis("/home/wapiko/Documents/programming/C_C++/NanoLang/test_code.nano");

  tokens->printTokens();

  return 0;
  }*/
