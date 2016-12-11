#ifndef UIIL_H
#define UTIL_H
#include <string.h>

#define SIZETOCHAR(X) ((((X) - 1) / 8) + 1) 

//SAFE
int getBit(void * buffer,int n);	//buf 에잇는 n 번째 비트를 얻어온다 
void setBit(void * buffer,int n,int value);	//buf 에 있는 n 번째 비트에 value 저장 
int stringToInt(char * str);
void copyBit(void * targetVoid , void * sourceVoid,int fromTarget,int fromSource, int n);
void printBit(void * source , int from , int n);
//Buffer is Empty?
int isEmptyBuffer(void * buffer , int nBit);
#endif
