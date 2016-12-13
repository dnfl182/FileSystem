#ifndef SHALL_H
#define SHALL_H

#include <stdio.h>
#include <stdlib.h>
#include "Util.h"
#include "FileSystem.h"
typedef struct Shall_ Shall;
typedef struct DTree_ DTree;
typedef struct DNode_ DNode;
typedef struct Shall_
{
	char pwd[1000];	//
	Inode * root;	//Root
	DTree * dTree;	//RootDTree
	FileSystem * fileSystem;
}Shall;
typedef struct DTree_	//Directory Tree
{
	char fileName[5];
	int value;	//inode num
	DNode * childHead;	//Linked List
}DTree;  
typedef struct DNode_	//Directory Node - Tree
{
	DTree * value;//Child
	struct DNode_ * next;
}DNode;
void initDTree(DTree ** address , Inode * inode , char * fileName);
void pushDTree(DTree * dTree , Inode * inode , char * fileName);
void initShall(Shall ** address, FileSystem * fileSystem);
void removeDTree(DTree * parent , char * fileName);
void removeDTreeHelper(DTree * parent);
DTree * findDTreeNum(DTree * root , int num);
DTree * findDTreePath(Shall * shall , char * path); 
void pushFileDirectory(Shall *shall , Inode * directory , Inode * file , char * fileName);
Inode * getFileDirectory(Shall *shall , Inode * directory, char * findFileName);
void removeFile(Shall * shall , char * path);
void mymkfs(Shall ** address);
void myls(Shall * shall, char * path , char * option);
void mycat(Shall * shall , char * path);
void myshowfile(Shall * shall ,char * num1, char * num2, char *path);
void mypwd(Shall * shall);
void mycpfrom(Shall * shall , char * path1 , char * path2);
void myrmdir(Shall * shall , char *path);
void mycd(Shall * shall , char * path);
void mycp(Shall * shall ,char * path1 , char * path2);
void mycpto(Shall * shall , char * path1 , char * path2);
void myrm(Shall * shall , char * path);
void mymv(Shall * shall , char * path1 , char * path2);
void mytouch(Shall * shall , char * path);
void myshowinode(Shall * shall , char * arg);
void myshowblock(Shall * shall , char * arg);
void mymkdir(Shall * shall , char * path);
void mystate(Shall * shall);
void mytree(Shall * shall , char * path);
void mytreeHelper(Shall * shall , DTree * dTree , int level);
void mycommand(Shall * shall , char * command);
void printPWD(Shall * shall);
void runCommand(Shall ** address); 
Inode* getInodePath(Shall * shall , char * path);
char* makeAPath(Shall * shall , char * RPath); 
char* getFileNamePath(char * path);
void removeFileNamePath(char * path);
#endif
