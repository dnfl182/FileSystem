#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#define DATABLOCK_SIZE 1024
#define DATABLOCK_CHARSIZE SIZETOCHAR(DATABLOCK_SIZE)
#define INODE_SIZE 512
#define INODE_CHARSIZE SIZETOCHAR(INODE_SIZE)
#define DATABLOCK_BITE 128
#define MAX_INDIRECT 102

typedef struct FileSystem_ FileSystem;	// 파일 시스템 
typedef struct Inode_ Inode;			// 아이노드 
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
	bool type;
	Time *lastUsedTime;
	int size;
	DataBlockList * data;
}Inode;
typedef struct DataBlock_
{
	int plus;
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

void addDataBlockList(Inode * inode, DataBlock * data);
DataBlock * getEmptyDataBlock(FileSystem * fileSystem);	//비어 있는 데이터 블럭 얻기
Inode * getEmptyInode(FileSystem * fileSystem);	//비어잇는 아이노드 얻어오기


void initDataBlockList(DataBlockList ** address);
//데이터 리스트에 데이터 노드 넣는 함수 
void pushDataBlockList(DataBlockList * list, DataBlock * dataBlock);
// 싱글 인다이렉트 블럭 생성 
void makeSingleIndirect(FileSystem * fileSystem , Inode * inode);
// 더블 인다이렉트 블럭 생성 
void makeDoubleIndirect(FileSystem * fileSystem , Inode * inode);
// 인다이렉트 데이터 블럭에서 데이터블럭 얻어오기 
DataBlock * getDataBlockIndirect(FileSystem * fileSystem , DataBlock * dataBlock , int n);
// 인다이렉트 데이터 블럭에 노드 번호 저장하기 
void setDataBlockIndirect(DataBlock * dataBlock , int n , int num);
//아이노드에 데이터블럭 저장하기 
void pushDataBlockInode(FileSystem * fileSystem ,Inode * inode , DataBlock *dataBlock);
// 아이노드 에서 n번째 데이터 얻어오기 
DataBlock * getDataBlockInode(FileSystem * fileSystem , Inode * inode , int n);
char * getDataInode(FileSystem * fileSystem , Inode * inode , int fromBit,  int nBit);	//fromBit 부터 nBit만큼의 데이터 얻기 

void setDataInode(FileSystem * fileSystem, Inode * inode, int fromBit, int nBit, int fromSourceBit ,void * source);

#endif
