#ifndef SHALL_H
#define SHALL_H

#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "FileSystem.h"
typedef struct Shall_ Shall;
typedef struct DTree_ DTree;	//Directory Tree
typedef struct DNode_ DNode;//Directory Node - Tree


typedef struct Shall_
{
	char pwd[1000];	// 자기 위치 경로
	Inode * root;	//최상위 디렉토리 
	DTree * dTree;
	FileSystem * fileSystem;	//파일 시스템 
}Shall;
typedef struct DTree_	//Directory Tree
{
	char fileName[5];
	int value;
	DNode * childHead;	//List
}DTree;  
typedef struct DNode_	//Directory Node - Tree
{
	DTree * value;
	struct DNode_ * next;
}DNode;


void initDTree(DTree ** address , Inode * inode , char * fileName);	// Init DirectoryTree
void pushDTree(DTree * dTree , Inode * inode , char * fileName);	//다이렉트 트리에 새로운 요소 추가 
void initShall(Shall ** address, FileSystem * fileSystem);	//쉘 초기화 
DTree * findDTreeNum(DTree * root , int num);	// inodeNum 으로 dTree 찾기 
DTree * findDTreePath(DTree * root ,char * path);	//경로로 dTree 찾기 
//디렉토리에 파일 넣기 
void pushFileDirectory(Shall *shall , Inode * directory , Inode * file , char * fileName);
Inode * getFileDirectory(Shall *shall , Inode * directory, char * findFileName);	//NULL 값 반환하면 없는거다 알겟냐? ;


void mymkfs(Shall ** address);
void myls(Shall * shall, char * path , char * option);
//SAFE
void printPWD(Shall * shall);
void runCommand(Shall ** address);	//명령어 읽어오기 


#endif
