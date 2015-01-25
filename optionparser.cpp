#include "optionparser.h"

void OptionParser::printHelp(){
  
} 

bool OptionParser::parseOption(){
  if(Argc < 2){
    fprintf(stderr, "引数がたりないわ\n");
    return false;
  }
  
  for(int i = 1; i < Argc; i++){
    if(Argv[i][0] == '-' && Argv[i][1] == 'o' && Argv[i][2] == '\0'){
      //output filename
      OutputFilename.assign(Argv[++i]);
    }
    else if(Argv[i][0] == '-' && Argv[i][1] == 'h' && Argv[i][2] == '\0'){
      printHelp();
      return false;
    }
    else if(Argv[i][0] == '-'){
      fprintf(stderr, "%sは不明なオプションよ\n", Argv[i]);
      return false;
    }
    else{
      //inputfilename
      InputFilename.assign(Argv[i]);
    }
  }

  //OutputFilename
  string ifn = InputFilename;
  int len = ifn.length();
  if(OutputFilename.empty() && (len > 2) &&
     ifn[len-5] == '.' &&
     ((ifn[len-4] == 'n' && ifn[len-3] == 'a' && 
       ifn[len-2] == 'n' && ifn[len-1] =='o'))){
    OutputFilename = string(ifn.begin(), ifn.end()-5);
    OutputFilename += ".ll";
  }
  else if(OutputFilename.empty()){
    OutputFilename = ifn;
    OutputFilename += ".ll";
  }

  return true;
}
