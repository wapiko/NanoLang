#include <stdio.h>

int printstr(char c[]){
  return printf("%s\n", c);
}


int test(){
  char c[] = "hello";
  return printstr(c);

}
