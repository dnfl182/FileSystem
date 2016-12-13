#ifndef UIIL_H
#define UTIL_H
#include <string.h>
#include <stdio.h>
#define SIZETOCHAR(X) ((((X) - 1) / 8) + 1) 
int getBit(void * buffer,int n); 
void setBit(void * buffer,int n,int value); 
int stringToInt(char * str);
void copyBit(void * targetVoid , void * sourceVoid,int fromTarget,int fromSource, int n);
void printBit(void * source , int from , int n);
int isEmptyBuffer(void * buffer , int nBit);
#endif
