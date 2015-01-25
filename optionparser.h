#include <string>
#include "common.h"

using namespace std;

class OptionParser{
 private:
  string InputFilename;
  string OutputFilename;
  int Argc;
  char **Argv;

 public:
  OptionParser(int argc, char **argv)
    : Argc(argc), Argv(argv){}
  void printHelp(); 
  string getInputFileName(){
    return InputFilename;
  }
  string getOutputFileName(){
    return OutputFilename;
  }
  bool parseOption();
};
