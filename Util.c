#include "Util.h"
int getBit(void * buffer,int n)	//buf 에잇는 n 번째 비트를 얻어온다 
{
	char *buf = (char*)buffer;
	int arrayNumber = n / 8;	// char buf 배열의 arrayNumber번쨰 위치 
	int rNumber = n % 8;		//	위내용의에 있는 rNumber번째 비트 
	char mask;
	
	mask = (1 << (7 - rNumber));
	if(buf[arrayNumber] & mask)	// 0 보다 크면 1 
		return 1;
	else			//아니면 0 
		return 0;
}
void setBit(void * buffer,int n,int value)	//buf 에 있는 n 번째 비트에 value 저장 
{
	
	char *buf = (char*)buffer;
	int arrayNumber = n / 8;	// char buf 배열의 arrayNumber번쨰 위치 
	int rNumber = n % 8;		//	위내용의에 있는 rNumber번째 비트
	char mask;
	mask = 1 << (7 - rNumber);
	if(value == 1)
		buf[arrayNumber] |= mask;	// OR 
	else
		buf[arrayNumber] &= (~mask);	// NAND 
}
int stringToInt(char * str)	//문자열을 숫자로 바꿔주는 것 
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
//source : 소스 target : 타겟 fromTarget : 타겟에서 몇비트 떨어진 부분부터 시작
//fromSource : 소스에서 몇비트 떨어진 부분부터 시작 n 개를 
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
void printBit(void * source , int from , int n)
{
	int i = 0;
	while(i < n)
	{
		printf("%d ",getBit(source,i++ + from));
		if(i % 8 == 0)
		{
			printf("\n");
		}
		
	}
}
