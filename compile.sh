clang++ -std=c++11 -o nanolang.o utfstring.cpp tokenstream.cpp lexer.cpp ast.cpp parser.cpp optionparser.cpp codegen.cpp nanolang.cpp `llvm-config --cppflags --ldflags --libs`  -ldl -lpthread
