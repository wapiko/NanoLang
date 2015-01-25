#include "utfstring.h"

string UTFString::getString(){
  string result = "";
  for(vector<string>::iterator it = (this->str).begin(); it!=(this->str).end(); it++){
    result.append(*it);
  }
  return result;
}

const char* UTFString::c_str(){
  return (this->getString()).c_str();
}

int UTFString::length(){
  return (this->str).size();
}

string UTFString::at(int index){
  return (this->str)[index];
}

bool UTFString::add(string s){
  (this->str).push_back(s);
  return true;
}

vector<string> UTFString::split_utf8(const string& str){
  vector<string> result;
  string tmp;
  bool first = true;
  if(str.size() == 0)
    return result;
  for(size_t i = 0; i <= str.size(); i++){
    if(first || (i != str.size() && (str.at(i) & 0xC0) == 0x80)){
      tmp += str.at(i);
      first = false;
      continue;
    }
    result.push_back(tmp);
    tmp.clear();
    if(i == str.size())
      break;
    tmp += str.at(i);
  } 
  return result;
}

int UTFString::split_utf8_test(){
  string str = "テストtest試験！!";
  int len = str.length();
  vector<string> words = split_utf8(str);
  for(vector<string>::iterator it = words.begin(); it!=words.end(); it++){
    cout << *it << endl;
  }

  return 0;
}

string UTFString::substr(int a, int b){
  string str = "";
  for(int i = a; i < b; i++){
    if(i >= length()){
      return str;
    }
    str = str + at(i);
  }
  return str;
}

/*int main(){
  UTFString utfstr = UTFString("あいうえおabc若菜だよん！!");
  string s = utfstr.getString();
  cout << s << endl;
  }*/
