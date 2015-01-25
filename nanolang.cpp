#include <iostream>
#include "token.h"
#include <stdlib.h>
#include "ast.h"
#include "parser.h"
#include "codegen.h"
#include "optionparser.h"

#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/Debug.h"
#include "llvm/IR/IRPrintingPasses.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/PassManager.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Transforms/Scalar.h"

using namespace std;
using namespace llvm;


/*int main(){
  Parser *parser = new Parser("/home/wapiko/Documents/programming/C_C++/NanoLang/test_code2.nano");
  //parser->printTokens();
  parser->doParse();
  parser->getAST().print();
  CodeGen *codegen = new CodeGen();
  codegen->doCodeGen(parser->getAST(), "test_code2.nano"); 
  codegen->getModule().dump();
  //  codegen->test();

  return 0;
  }*/

int main(int argc, char **argv){
  LLVMInitializeNativeTarget();
  sys::PrintStackTraceOnErrorSignal();
  PrettyStackTraceProgram X(argc, argv);
  EnableDebugBuffering = true;
  
  OptionParser opt(argc, argv);
  if(!opt.parseOption())
    exit(1);
  
  //check
  if(opt.getInputFileName().length()==0){
    fprintf(stderr,"入力ファイルが指定されていないわ\n");
    exit(1);
  }

  //lex and parse
  Parser *parser = new Parser(opt.getInputFileName());
  if(!parser->doParse()){
    fprintf(stderr, "構文解析か字句解析でエラーが出てるわね...");
    SAFE_DELETE(parser);
    exit(1);
  }

  //get AST
  TranslationUnitAST &tunit = parser->getAST();
  if(tunit.empty()){
    fprintf(stderr, "抽象構文木が空っぽだわ");
    SAFE_DELETE(parser);
    exit(1);
  }

  //cout<<"構文解析完了"<<endl;
  //tunit.print();
  
  CodeGen *codegen = new CodeGen();
  if(!codegen->doCodeGen(tunit, opt.getInputFileName())){
    fprintf(stderr, "LLVM IRを作るのに失敗したわ、ごめんね");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  //get Module
  Module &mod = codegen->getModule();
  if(mod.empty()){
    fprintf(stderr, "あら、ごめんね。LLVM IRの生成結果が空っぽなの");
    SAFE_DELETE(parser);
    SAFE_DELETE(codegen);
    exit(1);
  }

  //cout<<"コード変換完了"<<endl;
  //mod.dump();

  PassManager pm;

  //mem2regをPassManagerに登録
   pm.add(createPromoteMemoryToRegisterPass());
  
  //出力
  error_code ec;
  raw_fd_ostream raw_stream(opt.getOutputFileName(), ec, llvm::sys::fs::F_Text);
  pm.add(createPrintModulePass(raw_stream));
  pm.run(mod);
  raw_stream.close();

  //delete
  SAFE_DELETE(parser);
  SAFE_DELETE(codegen);

  return 0;
}
