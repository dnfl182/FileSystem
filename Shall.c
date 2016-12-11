#include "Shall.h"

void initDTree(DTree ** address , Inode * inode , char *fileName)	// Init DirectoryTree
{
	*address = (DTree*)malloc(sizeof(DTree));
	DTree * dTree = *address;
	dTree -> value = inode -> num;
	strcpy(dTree -> fileName , fileName);
	dTree -> childHead = (DNode*)malloc(sizeof(DNode));
	dTree -> childHead -> next = NULL;
}
void pushDTree(DTree * dTree , Inode * inode , char * fileName)	//다이렉트 트리에 새로운 요소 추가 
{
	DNode * newNode = (DNode*)malloc(sizeof(DNode));
	initDTree(&(newNode -> value) , inode , fileName);
	newNode -> next =  dTree -> childHead -> next;
	dTree -> childHead -> next = newNode;
}
/////////////////////
DTree * findDTreeNum(DTree * root , int num)	// inodeNum 으로 dTree 찾기 
{
	
	if(root -> value == num)
		return root;
	DTree * r = NULL;
	DNode * header = root -> childHead;
	while(header -> next != NULL)
	{
		header =  header -> next;
		DTree * t = findDTreeNum(header -> value , num);
		if(t != NULL)
			r = t;
	}
	return r; 
}
DTree * findDTreePath(DTree * root ,char * path)
{
	if(strcmp(path , "/") == 0)
	 	return root;
	char tPath[100][5] = {0};
	int h = 0;
	int i = 0;
	int j = 0;
	int n;	// 경로 개수 
	while(path[++h] != 0)
	{
		if(path[h] == '/')
		{
			i = 0;
			++j;
			continue;
		}
		if(i == 4)	// 최대 파일이름 크기 4 
		{
			continue;
		}
		tPath[j][i++] = path[h];
	}
	n = j + 1;
	
	DTree * dTreeHeader =  root;
	for(i = 0; i < n; i++)
	{
		DNode * dNodeHeader = dTreeHeader -> childHead;
		while(1)
		{
			dNodeHeader = dNodeHeader -> next;
			if(strcpy(tPath[i] , dNodeHeader -> value -> fileName) == 0)	// 이름이 같으면 
			{
				dTreeHeader = dNodeHeader -> value;
				break;
			}
			if(dNodeHeader -> next = NULL)
				printf("Error - Wrong Path");
		}
	}
	return dTreeHeader;
}
//////////////////////////

void initShall(Shall ** address, FileSystem * fileSystem)	//쉘 초기화 
{
	*address = (Shall*)malloc(sizeof(Shall));
	Shall *shall = *address;
	shall -> fileSystem =  fileSystem;
	shall -> root = fileSystem -> inode[0];
	setBit(fileSystem -> superBlock -> inodeCheck,0,1);
	int i;
	for(i = 0; i < 1000; i++)
		shall -> pwd[i] = 0;
	shall -> pwd[0] = '/';
	initDTree(&(shall -> dTree) , shall -> root , "root");
}
// 디렉토리에 파일 넣기 
void pushFileDirectory(Shall *shall , Inode * directory , Inode * file , char * fileName)
{
	
	//41Bit
	//DataBlock 한개에 최대 24개
	FileSystem * fileSystem = shall -> fileSystem;
	int num = file -> num;	// 
	int i = 0; 
    int j;
    int size = directory -> size;
    while(1)
    {

    	if(i + 41 > size * 128)	// size is sufficant
    	{
    		directory -> size ++;
    		pushDataBlockInode(fileSystem , directory , getEmptyDataBlock(fileSystem));
    	}
		
		
		if(isEmptyBuffer(getDataInode(fileSystem , directory , i , i + 41) , 41))	//find Empty Space
		{
			setDataInode(fileSystem , directory , i , 32 , 0 , fileName);
			setDataInode(fileSystem , directory , i + 32, 9 , 23 , &num);	//From (i + 32) ~ To (i + 40) <= (23 ~ 32)Num
			break;	
		}
		i += 41;
    }
    //DTree 에 등록 
    DTree * dTreeDirectory =  findDTreeNum(shall -> dTree , directory -> num);
    pushDTree(dTreeDirectory , file , fileName);
	
}
Inode * getFileDirectory(Shall *shall, Inode * directory, char * findFileName)	//NULL 값 반환하면 없는거다 알겟냐? 
{
	FileSystem * fileSystem = shall -> fileSystem;
	int size = directory -> size;
	int i;
	char * fileData;
	Inode * r = NULL;
	while(1)
	{
		if(i + 41 < size * 128)	//없다 그런파일 
				return NULL;
		fileData = getDataInode(fileSystem ,directory,i, 41);
		if(isEmptyBuffer(fileData,41))	//혹시 비워있을경우(이런일 없겟지만) 
			continue;
		char fileName[5] = {0};
		copyBit(fileName,fileData,0,0,32);
		int num;
		copyBit(&num,fileData,32,23,9);
		if(strcmp(fileName,findFileName) == 0)	// 파일명이 같을경우 
		{
			r = fileSystem -> inode[num];
			break;
		}
		i += 41;
	}
	return r;
}
void runCommand(Shall ** address)	//명령어 읽어오기 
{
	Shall * shall = *address;
	printPWD(shall);
	printf("$ ");
	char str[1000];
	gets(str);	// 명령 얻어오기 
	char command[100] = {0};	//명령 
	char paramater[5][100] = {0};		//인자
	char option[100] = {0};	//옵션 
	
	int h = -1;	//header
	
	int i = -1;
	while(str[++h] != ' ' && str[h] != 0)
		command[++i] = str[h];
		
	i = 0;	// 문자 번호 
	int j = 0; 	// 파라미터 번호 
	int mode = 0; 
	while(1)
	{
		if(str[++h] == 0)	// 끝이면 
		{
			break;
		}
		else if(str[h] == ' ')	//스페이스 바이면 
		{
			if(mode == 1)
				mode = 0;
			else
				++j;
			i = 0;
			continue;
		}
		if(str[h] == '-')
		{
			mode = 1;
			continue;
		}
		if(mode)
			option[i++] = str[h];
		else
			paramater[j][i++] = str[h];
	}
	printf("Test : Command : %s\n" , command);
	for(i = 0; i < 5; i++)
	{
		if(paramater[i][0] == 0)
			break;
		printf("Test : Paramater[%d] : %s\n",i,paramater[i]);
	}
	if(option[0] != 0);
		printf("Test : Option : %s\n", option);
	if(strcmp(command , "mymkfs") == 0)
	{
		mymkfs(address);
	}
	else if(strcmp(command,"myls") == 0)
	{
		if(paramater[0][0] == 0)	//비워잇으면
			myls(shall , shall -> pwd , option);
		else
			myls(shall , paramater[0] , option);
	}
	
}


void mymkfs(Shall ** address)
{
	if(*address != NULL)
	{
		printf("error : myfs exists.");
		return;
	}
	FileSystem * fileSystem;
	initFileSystem(&fileSystem);
	initShall(address,fileSystem);
}

void myls(Shall * shall, char * path , char * option)	//아직은 옵션없이 
{
	DNode *header = findDTreePath(shall -> dTree , path) -> childHead;
	while(header -> next != NULL)
	{
		header = header -> next;
		printf("%s\n" ,header -> value -> fileName);
	}
}
void printPWD(Shall * shall)
{
	if(shall == NULL)
		return;
	printf("[");
	printf("%s",shall -> pwd);
	printf("]");
}


