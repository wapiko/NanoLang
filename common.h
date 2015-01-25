#ifndef __COMMON_H_
#define __COMMON_H_

#include <stdio.h>

#define SAFE_DELETE(x) {delete x; x=NULL;}
#define SAFE_DELETEA(x) {delete[] x; x=NULL;}


#endif
