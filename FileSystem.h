#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "Util.h"
#define DATABLOCK_SIZE 1024
#define DATABLOCK_CHARSIZE SIZETOCHAR(DATABLOCK_SIZE)
#define INODE_SIZE 512
#define INODE_CHARSIZE SIZETOCHAR(INODE_SIZE)
#define DATABLOCK_BITE 128
#define MAX_INDIRECT 102

typedef struct FileSystem_ FileSystem; 
typedef struct Inode_ Inode;			
typedef struct DataBlock_ DataBlock;	
typedef struct SuperBlock_ SuperBlock;
typedef struct Time_ Time;
typedef struct DataBlockNode_ DataBlockNode;
typedef struct DataBlockList_ DataBlockList;
typedef struct FileSystem_
{
	unsigned int bootBlock : 16;
	DataBlock * dataBlock[DATABLOCK_SIZE];
	Inode * inode[INODE_SIZE];
	SuperBlock * superBlock;
}FileSystem;
typedef struct Inode_
{
	int num;
	char type;	// 0 : File 1 : Directory
	Time *lastUsedTime;
	int size;
	DataBlockList * data;
}Inode;
typedef struct DataBlock_
{
	int num;
	char data[DATABLOCK_BITE];	
}DataBlock;
typedef struct SuperBlock_
{
	char inodeCheck[INODE_CHARSIZE];
	char dataBlockCheck[DATABLOCK_CHARSIZE];
}SuperBlock;
typedef struct DataBlockNode_
{
	DataBlock * value;
	struct DataBlockNode_ * next;
	struct DataBlockNode_ * prev;
}DataBlockNode;
typedef struct DataBlockList_
{
	int size;
	DataBlockNode * head;
	DataBlockNode * tail;
}DataBlockList;
typedef struct Time_
{
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int sec;	
}Time;

void initFileSystem(FileSystem ** address);
void initDataBlock(DataBlock ** address);
void initInode(Inode ** address);
void initSuperBlock(SuperBlock ** address);
void renewTime(Time * t);
void printTime(Time * t);
DataBlock * getEmptyDataBlock(FileSystem * fileSystem);
Inode * getEmptyInode(FileSystem * fileSystem);
void freeDataBlock(FileSystem * fileSystem , int num);
void freeInode(FileSystem * fileSystem , int num);
void initDataBlockList(DataBlockList ** address);
void pushDataBlockList(DataBlockList * list, DataBlock * dataBlock);
void makeSingleIndirect(FileSystem * fileSystem , Inode * inode);
void makeDoubleIndirect(FileSystem * fileSystem , Inode * inode);
DataBlock * getDataBlockIndirect(FileSystem * fileSystem , DataBlock * dataBlock , int n);
void setDataBlockIndirect(DataBlock * dataBlock , int n , int num);
void pushDataBlockInode(FileSystem * fileSystem ,Inode * inode , DataBlock *dataBlock);
DataBlock * getDataBlockInode(FileSystem * fileSystem , Inode * inode , int n);
char * getDataInode(FileSystem * fileSystem , Inode * inode , int fromBit,  int nBit);
void setDataInode(FileSystem * fileSystem, Inode * inode, int fromBit, int nBit, int fromSourceBit ,void * source);
int getRealSize(Inode * inode);
#endif
