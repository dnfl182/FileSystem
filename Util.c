#include "Util.h"
int getBit(void * buffer,int n)	//get nTh Bit in Buffer 
{
	char *buf = (char*)buffer;
	int arrayNumber = n / 8;	//	there is bit in n/8 array 
	int rNumber = n % 8;		//	setting mask 
	char mask;
	
	mask = (1 << (7 - rNumber));
	if(buf[arrayNumber] & mask)	// getInfo
		return 1;
	else		
		return 0;
}
void setBit(void * buffer,int n,int value)	//set nth Bit in buffer as n 
{
	
	char *buf = (char*)buffer;
	int arrayNumber = n / 8;
	int rNumber = n % 8;
	char mask;
	mask = 1 << (7 - rNumber);
	if(value == 1)
		buf[arrayNumber] |= mask;	// OR 
	else
		buf[arrayNumber] &= (~mask);	// NAND 
}
int stringToInt(char * str)	//interpret char* to int 
{
	int len = strlen(str);
	int i  = 0;
	int ten = 1;
	int r = 0;
	for(i = len - 1; i >= 0; i--)
	{
		r += ten * (str[i] - '0');
		ten *= 10;
	}
	return r;
}
//Copy nthBit from source to target
void copyBit(void * targetVoid , void * sourceVoid,int fromTarget,int fromSource, int n)
{
	char * target = (char*)targetVoid;
	char * source = (char*)sourceVoid;
	int i;
	for(i = 0; i < n; i++)
	{
		setBit(target,fromTarget + i,getBit(source,i + fromSource));
	}
}
//Buffer is Empty?
int isEmptyBuffer(void * buffer , int nBit)
{
	int r = 1;
	int i;
	for(i = 0; i < nBit; i++)
		if(getBit(buffer , i) == 1)
		{
			r = 0;
			break;
		}
	return r;
}
//only test
void printBit(void * source , int from , int n)
{
	int i = 0;
	for(i = 0 ; i < n; i++)
	{
		printf("%d ",getBit(source,i + from));
		if(i % 8 == 7)
		{
			printf("\n");
		}
		
	}
}
