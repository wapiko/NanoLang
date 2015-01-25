#ifndef __UTFSTRING_H_
#define __UTFSTRING_H_

#include "common.h"
#include <string>
#include <vector>
#include <iostream>

using namespace std;

class UTFString{
 private:
  vector<string> str;
 public:
  UTFString(){}
  UTFString(string strs){
    str = this->split_utf8(strs);
  }

  string getString();
  const char* c_str();
  int length();
  string at(int index);
  bool add(string s);
  string substr(int a, int b);
  
  vector<string> split_utf8(const string& str);

  int split_utf8_test();


};



#endif
