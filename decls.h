#ifndef __DECLS_H_
#define __DECLS_H_

#include "common.h"
#include <iostream>
#include <vector>
#include <map>
#include "ast.h"
#include "tokenstream.h"
#include "lexer.h"

using namespace std;

enum Type{
  INT,
  STRING
};

class NanoVariable{
 private:
  string name;
  Type type;
 public:
  NanoVariable(string n, Type t) : name(n), type(t){}
  ~NanoVariable(){}
    string getName(){return name;}
      Type getType(){return type;}
};

class NanoFunction{
 private:
string name;
Type returnType;
int argNum;
vector<Type> argTypes;

public:
 NanoFunction(string n, Type rt, int an, vector<Type> at)
 : name(n), returnType(rt), argNum(an), argTypes(at){}
 ~NanoFunction(){}
 string getName(){return name;}
 Type getReturnType(){return returnType;}
 int getArgNum(){return argNum;}

 bool checkArgs(vector<Type> args){
   if(argNum != args.size()){
     return false;
   }
   else{
     for(int i = 0; i < argNum; i++){
       if(argTypes.at(i) != args.at(i))
         return false;
     }
     return true;
   }
 }
};


#endif
