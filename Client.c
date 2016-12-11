#include <stdio.h>
#include <string.h>
#include "FileSystem.h"
#include "Shall.h"

int main(void)
{
	Shall *shall = NULL;
	int i;
	for(i = 0; i < 10; i++)
	{
		runCommand(&shall);
		if(i == 0)
		{
			testMakeFile(shall);
		}
	}
	
}

void testMakeFile(Shall * shall)
{
	FileSystem * fileSystem = shall -> fileSystem;
	printf("파일만들기\n"); 
	Inode * inode = getEmptyInode(fileSystem);
	char hello[] = "Hello World";
	char bye[] = "Bye";
	DataBlock * data = getEmptyDataBlock(fileSystem);
	DataBlock * data2 = getEmptyDataBlock(fileSystem);
	pushDataBlockInode(fileSystem , inode , data);
	pushDataBlockInode(fileSystem , inode , data2);
	copyBit(data -> data , hello , 0 , 0,strlen(hello) * 8);
	copyBit(data2 -> data ,bye , 0 , 0,strlen(bye) * 8);
	printf("Test :getData 4 byte [%s]\n",getDataInode(fileSystem , inode, 0 , 32));
	printf("Test : [%s]\n",getDataInode(fileSystem , inode, 1024 , 24));
	//pushFileDirectory(shall , shall -> root , inode , "hell");
	//pushFileDirectory(shall , shall -> root , inode , "cho");
}
