#include "Shall.h"

void initDTree(DTree ** address , Inode * inode , char *fileName)	// Init DirectoryTree
{
	*address = (DTree*)malloc(sizeof(DTree));
	DTree * dTree = *address;
	dTree -> value = inode -> num;
	strncpy(dTree -> fileName , fileName,4);
	dTree -> fileName[4] = 0;
	dTree -> childHead = (DNode*)malloc(sizeof(DNode));
	dTree -> childHead -> next = NULL;
}
void pushDTree(DTree * dTree , Inode * inode , char * fileName)	//push (fileName and Inode) in dTree 
{
	DNode * newNode = (DNode*)malloc(sizeof(DNode));
	initDTree(&(newNode -> value) , inode , fileName);
	newNode -> next =  dTree -> childHead -> next;
	dTree -> childHead -> next = newNode;
}
void removeDTree(DTree * parent , char * fileName)	//remove DTree and child
{
	DNode * header = parent -> childHead;
	DNode * bh;
	while(header -> next != NULL)
	{
		bh = header;
		header = header -> next;
		if(strcmp(header -> value -> fileName , fileName) == 0)
		{
			bh -> next = header -> next;
			removeDTreeHelper(header -> value);
			break;
		}
	}
}
void removeDTreeHelper(DTree * parent)	// removeDTree Helper
{
	DNode * header = parent -> childHead;
	DNode * bh;
	while(header -> next != NULL)
	{
		bh = header;
		header = header -> next;
		removeDTreeHelper(header -> value);	
	}
	free(header);
	free(parent);
}
DTree * findDTreeNum(DTree * root , int num)	// find dTree as Inode num 
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
DTree * findDTreePath(Shall * shall ,char * path)	//find dTree as file Path
{
	 
	char tPath[100][5] = {0};
	char ttPath[1500] = {0};
	int h = 0;
	int i = 0;
	int j = 0;
	//Change to Absoulte Path
	if(path[0] != '/')	//if Relative Path
	{
		char *ad = makeAPath(shall , path);
		strcpy(ttPath , ad);
		free(ad);
	}
	else
		strcpy(ttPath , path);
	if(strcmp(ttPath , "/") == 0) //if path is root
	 	return shall -> dTree;
	 	
	while(ttPath[++h] != 0)
	{
		if(ttPath[h] == '/')
		{
			i = 0;
			++j;
			continue;
		}
		tPath[j][i++] = ttPath[h];
	}
	

	for(i = 0; i < 100; i++)
		tPath[i][4] = 0;
	//find dTree
	DTree * dTreeHeader =  shall -> dTree;
	for(i = 0; i < j + 1; i++)
	{
		DNode * dNodeHeader = dTreeHeader -> childHead;
	
		while(1)
		{
			if(dNodeHeader -> next == NULL)//can`t Find
			{
				return NULL;
			}
			dNodeHeader = dNodeHeader -> next;
			if(strcmp(tPath[i] , dNodeHeader -> value -> fileName) == 0)	// find !!
			{
				dTreeHeader = dNodeHeader -> value;
				break;
			}
		}
	}
	return dTreeHeader;	//No Need
}

void initShall(Shall ** address, FileSystem * fileSystem)	//initialize shall
{
	*address = (Shall*)malloc(sizeof(Shall));
	Shall *shall = *address;
	shall -> fileSystem =  fileSystem;
	shall -> root = fileSystem -> inode[0];
	shall -> root -> type = 1;
	setBit(fileSystem -> superBlock -> inodeCheck,0,1);
	int i;
	for(i = 0; i < 1000; i++)
		shall -> pwd[i] = 0;
	shall -> pwd[0] = '/';
	initDTree(&(shall -> dTree) , shall -> root , "root");
}
// push file in directory
void pushFileDirectory(Shall *shall , Inode * directory , Inode * file , char * fileName)
{
	
	//use 41Bit
	renewTime(file -> lastUsedTime);
	renewTime(directory -> lastUsedTime);
	FileSystem * fileSystem = shall -> fileSystem;
	int num = file -> num;	// 
	int i = 0; 
    char tName[5] = {0};
    strncpy(tName, fileName, 4);
    tName[4] = 0;
    int j;
    int size = getRealSize(directory);
    while(1)
    {
    	if(i + 41 > size * 128)	// size is sufficant
    		pushDataBlockInode(fileSystem , directory , getEmptyDataBlock(fileSystem));
		if(isEmptyBuffer(getDataInode(fileSystem , directory , i , i + 41) , 41))	//find Empty Space
		{
			setDataInode(fileSystem , directory , i , 32 , 0 , tName);
			setDataInode(fileSystem , directory , i + 32, 9 , 23 , &num);	//From (i + 32) ~ To (i + 40) <= (23 ~ 32)Num
			break;	
		}
		i += 41;
    }
    //Save in dTree
    DTree * dTreeDirectory =  findDTreeNum(shall -> dTree , directory -> num);
    pushDTree(dTreeDirectory , file , tName);
	
}

//Unused
Inode * getFileDirectory(Shall *shall, Inode * directory, char * findFileName)	//NULL  
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
void removeFile(Shall * shall , char * path)
{
	Inode * file = getInodePath(shall , path);
	char tPath[1000];
	strcpy(tPath , path);
	//////////Remove DTree
	char * fileName = getFileNamePath(path);
	removeFileNamePath(path);
	DTree * parent = findDTreePath(shall , path);
	removeDTree(parent , fileName);
	////Remove File
	removeFileNamePath(tPath);
	Inode * parentDirectory =  getInodePath(shall , tPath);
	int parentSize = getRealSize(parentDirectory);
	int i = 0;	
	while(1) 
	{
		if(i * 41 + 41 < parentSize * 128)
			break;
		char * afile = getDataInode(shall -> fileSystem , parentDirectory , i * 41 , 32);
		if(strcmp(afile , fileName) == 0)
		{
			char source[10] = {0};
			setDataInode(shall -> fileSystem , parentDirectory , i * 41 , 41 , 0 , source);	// 비우기
			break;
		}
		++i; 
	}
	freeInode(shall -> fileSystem , file -> num); 
	free(fileName);
}
void runCommand(Shall ** address)	//Run Command 
{
	Shall * shall = *address;
	printPWD(shall);
	printf("$ ");
	char str[1000] = {0};
	gets(str);	
	char command[100] = {0};	
	char paramater[5][100] = {0};
	char option[100] = {0};	 
	int i,j;
	int h = -1;	//header
	
	i = -1;
	while(str[++h] != ' ' && str[h] != 0)
		command[++i] = str[h];
		
	i = 0;	 
	j = 0; 	 
	int mode = 0; //option mode : 1
	while(1)
	{
		if(str[++h] == 0)	// If end
		{
			break;
		}
		else if(str[h] == ' ')	//if space 
		{
			if(mode == 1)
				mode = 0;
			else
				++j;
			i = 0;
			continue;
		}
		if(str[h] == '-')	//if option
		{
			mode = 1;
			continue;
		}
		if(mode)
			option[i++] = str[h];
		else
			paramater[j][i++] = str[h];
	}
	for(i = 0; i < 5; i++)
	{
		if(paramater[i][0] == 0)
			break;
	}
	if(strcmp(command , "mymkfs") == 0)
	{
		mymkfs(address);
	}
	else if(strcmp(command,"myls") == 0)
	{
		if(paramater[0][0] == 0)
			myls(shall , shall -> pwd , option);
		else
			myls(shall , paramater[0] , option);
	}
	else if(strcmp(command,"mycat") == 0)
	{
		mycat(shall , paramater[0]);
	}
	else if(strcmp(command ,"myshowfile") == 0)
	{
		myshowfile(shall , paramater[0] , paramater[1] , paramater[2]);
	}
	else if(strcmp(command , "mypwd") == 0)
	{
		mypwd(shall);
	}
	else if(strcmp(command,"mycd") == 0)
	{
		mycd(shall , paramater[0]);
	}
	else if(strcmp(command,"mycp") == 0)
	{
		mycp(shall , paramater[0] , paramater[1]);
	}
	else if(strcmp(command,"mycpto") == 0)
	{
		mycpto(shall , paramater[0] , paramater[1]);
	}
	else if(strcmp(command,"mycpfrom") == 0)
	{
		mycpfrom(shall , paramater[0] , paramater[1]);
	}
	else if(strcmp(command,"mycpto") == 0)
	{
		mycpto(shall , paramater[0] , paramater[1]);
	}
	else if(strcmp(command,"myrm") == 0)
	{
		myrm(shall , paramater[0]);
	}
	else if(strcmp(command ,"myrmdir") == 0)
	{
		myrmdir(shall , paramater[0]);
	}
	else if(strcmp(command,"mymv") == 0)
	{
		mymv(shall , paramater[0] , paramater[1]);
	}
	else if(strcmp(command,"mytouch") == 0)
	{
		mytouch(shall , paramater[0]);
	}
	else if(strcmp(command,"mymkdir") == 0)
	{
		mymkdir(shall , paramater[0]);
	}
	else if(strcmp(command,"myshowinode") == 0)
	{
		myshowinode(shall , paramater[0]);
	}
	else if(strcmp(command,"myshowblock") == 0)
	{
		myshowblock(shall , paramater[0]);
	}
	else if(strcmp(command,"mystate") == 0)
	{
		mystate(shall);
	}
	else if(strcmp(command , "mytree") == 0)
	{
		mytree(shall , paramater[0]);
	}
	else if(strcmp(command,"byebye") == 0)
	{
		exit(1);
	}
	else
		system(command);
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

void myls(Shall * shall, char * path , char * option) 
{
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	DTree *dTree = findDTreePath(shall, path);
	if(dTree == NULL)
	{
		printf("error: wrong path\n");
		return;
	}
	DNode *header = dTree -> childHead;	
	int optionl = 0;
	int optioni = 0;
	int i;
	for(i = strlen(option) - 1;i >= 0; i--)
	{
		if(option[i] == 'l')
			optionl = 1;
		else if(option[i] == 'i')
			optioni = 1;
	}
	
	i = -1;
	DTree *data[1000] = {0};
	while(header -> next != NULL)
	{
		header = header -> next;
		data[++i] = header -> value;
	}
	int j,k;
	for(j = 0; j < i; j++)
	{
		DTree * max = data[j];
		int index = j;
		for(k = j + 1; k <= i; k++) // Sort
			if(strcmp(max -> fileName , data[k] -> fileName) == -1)
			{
				max = data[k];
				index = k;
			}
		DTree * t = data[j];
		data[j] = data[index];
		data[index] = t;
	}
	for(j = i;j >=0 ;j--)
	{
		Inode *inode = shall -> fileSystem -> inode[data[j] -> value];
		if(optionl == 1)
		{
			printf("%c ",(inode -> type == 0)? '-' : 'd');
			printf("%5d ",inode -> size * 128);
			printTime(inode -> lastUsedTime);
			printf(" ");
		}	
		char tName[5] = {0};
		strncpy(tName , data[j] -> fileName , 4);
		tName[4] = 0;
		printf("%s\n",tName);
		if(optioni == 1)
		{
			printf("file type : %s\n",(inode -> type == 0) ? "regular file" :"directory");
			printf("file size : %d byte\n",inode -> size * 128);
			int size = getRealSize(inode);
			int i;
			printf("data block list : ");
			if(size > 1)
				printf("%d ",inode -> data -> head -> next -> value -> num);
			if(size > 103)
			{
				DataBlock * ad = inode -> data -> head -> next -> next -> value;
				printf("%d ",ad -> num);
				for(i = 0; i < 102; i++)
				{
					DataBlock * aq = getDataBlockIndirect(shall -> fileSystem , ad , i);
					printf("%d ",aq -> num);
				}
			}
			for(i = 0; i < size; i++)
				printf("%d ",getDataBlockInode(shall -> fileSystem, inode , i) -> num);
			printf("\n");	
		}
			
	}
}
void mycat(Shall * shall , char * path)
{
	if(shall == NULL)
	{
		printf("Error - There is no shell\n");
		return;
	}
	Inode * inode = getInodePath(shall , path); 
	if(inode == NULL)
	{
		printf("Error - Wrong Path\n");
		return;
	}
	int size = getRealSize(inode);
	int i;
	for(i = 0; i < size; i++)
	{
		DataBlock *dataBlock = getDataBlockInode(shall -> fileSystem , inode , i);
		printf("%s",dataBlock -> data);
	}
	printf("\n");
}
void myshowfile(Shall * shall ,char * num1, char * num2, char *path)
{
	if(shall == NULL)
	{
		printf("Error - There is no shell\n");
		return;
	}
	Inode * inode = getInodePath(shall , path);
	int iNum1 = stringToInt(num1);
	int iNum2 = stringToInt(num2);
	if(iNum1 > iNum2)
	{
		printf("Error - Wrong Paramater\n");
		return;
	}
	if(iNum2 > inode -> size * 128)
	{
		printf("Error - Wrong Paramater(num bigger then size)\n");
		return ;
	}
	char * content = getDataInode(shall -> fileSystem, inode ,iNum1 * 8 , (iNum2 - iNum1 + 1) * 8);
	int i;
	for(i = 0; i < (iNum2 - iNum1 + 1); i++)
		putchar(content[i]);

}
void mypwd(Shall * shall)
{
	printf("%s\n",shall -> pwd);
}
void mycd(Shall * shall , char * path)
{
	if(shall == NULL)
	{
		printf("Error - There is no shell\n");
		return;
	}
	Inode * inode = getInodePath(shall , path);
	if(inode == NULL && path[0] != 0)
	{
		printf("wrong path\n");
		return;
	}
	if(inode -> type != 1)
	{
		printf("it is not directory\n");
		return ;
	}
	if(path[0] == 0)
		strcpy(shall -> pwd , "/"); 
	else if(path[0] != '/')	//상대경로 
	{
		if(strcmp(shall -> pwd , "/") == 0)	// 루트 일경우 
			strcat(shall -> pwd , path);
		else
		{
			strcat(shall -> pwd , "/");
			strcat(shall -> pwd , path);
		}
	}
	else	//절대 경로 
		strcpy(shall -> pwd , path);
}
void mycp(Shall * shall ,char * path1 , char * path2)
{
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	Inode * file1 = getInodePath(shall , path1);
	if(file1 == NULL)
	{
		printf("error: wrong path\n");
		return;
	}
	Inode * t = getInodePath(shall , path2);
	if(t != NULL)
	{
		printf("error: there is already file\n");
		return;
	}
	DNode * h= findDTreePath(shall , path1) -> childHead; 
	Inode * directory;
	int i;
	///////get File Name
	char *cpFileName = getFileNamePath(path2);
	char tPath[1000] = {0};
	strcpy(tPath , path2);
	removeFileNamePath(tPath);
	///////get Directory
	char * ad = makeAPath(shall , tPath);
	strcpy(tPath , ad);	
	free(ad);
	directory = getInodePath(shall , tPath);
	///////copy File
	Inode * newFile = getEmptyInode(shall -> fileSystem);
	int size = getRealSize(file1);
	for(i = 0; i < size; i++)
	{
		DataBlock * source = getDataBlockInode(shall -> fileSystem , file1 ,i);
		DataBlock * target = getEmptyDataBlock(shall -> fileSystem);
		strcpy(target -> data , source -> data);
		pushDataBlockInode(shall -> fileSystem , newFile ,target);
	}
	pushFileDirectory(shall , directory , newFile , cpFileName);
	
	while(h -> next != NULL)	//if it`s directory 
	{
		h = h -> next;
		strcat(path1 , "/");
		strcat(path1 , h -> value -> fileName);
		strcat(path2 , "/");
		strcat(path2 , h -> value -> fileName);
		mycp(shall , path1 , path2);
	}
	
	free(cpFileName);
}

void mycpfrom(Shall * shall , char * path1 , char * path2)
{
	FILE * file1;
	if(shall == NULL)
	{
		printf("error:there is no shell\n");
		return;
	}
	if((file1 = fopen(path1 , "rb")) == NULL)
	{
		printf("error:wrong path\n");
		return;
	}
	Inode *  at= getInodePath(shall , path2);
	if(at != NULL)
	{
		printf("error: there is already file\n");
		return;
	}
	//get FIle Size
	fseek(file1 , 0 , SEEK_END);
	int fileSize = ftell(file1);
	fseek(file1 , 0 , SEEK_SET);
	int blocksNum = fileSize / DATABLOCK_BITE;
	int remain  = fileSize % DATABLOCK_BITE;
	int i;
	char * cpFileName = getFileNamePath(path2);
	char tPath[1000];
	strcpy(tPath , path2); 
	removeFileNamePath(tPath);
	Inode * directory = getInodePath(shall , tPath);
	if(directory == NULL)
	{
		printf("error:wrong path\n");
		return;
	}
	if(directory -> type != 1)
	{
		printf("error:it`s not directory\n");
		return;
	}
	//makeNewFile
	Inode * newFile = getEmptyInode(shall -> fileSystem);
	char t[128] = {0};
	DataBlock * copyDataBlock;
	//copyFile
	for(i = 0; i < blocksNum; i++)
	{
		fread(t , sizeof(char) , DATABLOCK_BITE , file1);
		copyDataBlock = getEmptyDataBlock(shall -> fileSystem);
		strcpy(copyDataBlock -> data , t);
		pushDataBlockInode(shall -> fileSystem , newFile , copyDataBlock);
	}
	for(i = 0; i < 128; i++)
		t[i] = 0;
	fread(t , sizeof(char) ,remain , file1);
	copyDataBlock = getEmptyDataBlock(shall -> fileSystem);
	strcpy(copyDataBlock -> data , t);
	pushDataBlockInode(shall -> fileSystem , newFile  , copyDataBlock);
	pushFileDirectory(shall , directory , newFile , cpFileName);
	fclose(file1);

}
void mycpto(Shall * shall , char * path1 , char * path2)
{
	FILE * newFile;
	newFile = fopen(path2 , "wb");
	Inode * file1;
	file1 = getInodePath(shall , path1);
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	if(file1 == NULL)
	{
		printf("error: wrong path\n");
		return;
	}
	int size = getRealSize(file1);
	int i;
	for(i = 0; i < size; i++)
	{
		DataBlock * cpBlock = getDataBlockInode(shall -> fileSystem , file1 , i);
		fwrite(cpBlock -> data , sizeof(char) , 128 , newFile);
	}
	fclose(newFile);
	
}
void mymkdir(Shall * shall , char * path1)
{
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	Inode * at = getInodePath(shall , path1);
	if(at != NULL)
	{
		printf("error:there is already file\n");
		return;
	}
	Inode * newDirectory = getEmptyInode(shall -> fileSystem);
	newDirectory -> type = 1;	// Directory;
	char * fileName = getFileNamePath(path1);
	removeFileNamePath(path1);
	Inode * parent = getInodePath(shall , path1);
	if(parent == NULL)
	{
		printf("error: wrong path\n");
		return;
	}
	pushFileDirectory(shall , parent , newDirectory , fileName);
	free(fileName);
}

void myrmdir(Shall * shall , char *path)
{
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	char tPath[1000] = {0};
	strcpy(tPath , path);
	makeAPath(shall , tPath);
	Inode * inode = getInodePath(shall , tPath);
	if(inode == NULL)
	{
		printf("error: wrong path\n");
		return;
	}
	if(getInodePath(shall , tPath) -> type == 1)
		removeFile(shall , tPath);
	else
	{
		printf("error: it is not directory\n");
		return;
	}
	
}
void myrm(Shall * shall , char * path)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return;
	}
	char tPath[1000] = {0};
	strcpy(tPath , path);
	makeAPath(shall , tPath);
	Inode * inode = getInodePath(shall , tPath);
	if(inode == NULL)
	{
		printf("error : wrong path\n");
		return;
	}
	if(inode -> type == 0)
	{
		removeFile(shall , tPath);
	}
	else
	{
		printf("error : it`s not file\n");
		return ;
	}
}
void mymv(Shall * shall , char * path1 , char * path2)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return;
	}
	Inode * at = getInodePath(shall , path2);
	if(at != NULL)
	{
		printf("error:there is already file\n");
		return;
	}
	
	char tPath1[1000];
	strcpy(tPath1 , path1);
	Inode * file1 = getInodePath(shall , path1);
	Inode * file2 = getInodePath(shall , path2);
	if(file1 == NULL)
	{
		printf("error : wrong path\n");
		return;
	}
	if(file2 == NULL)	// FILE
	{
		mycp(shall , path1 , path2);
	}
	else
	{
		char * fileName = getFileNamePath(path1);
		char tPath[1000] = {0};
		strcpy(tPath , path2);
		if(strcmp(tPath , "/") != 0)
			strcat(tPath,  "/");
		strcat(tPath , fileName);
		mycp(shall , path1 , tPath);
	}
	if(file1 -> type == 0)
		myrm(shall , tPath1);
	else
		myrmdir(shall , tPath1);
}
void mytouch(Shall * shall , char * path)
{
	if(shall == NULL)
	{
		printf("error: there is no shell\n");
		return;
	}
	Inode * file = getInodePath(shall , path);
	if(file == NULL)
	{
		file = getEmptyInode(shall -> fileSystem);
		char * fileName = getFileNamePath(path);
		removeFileNamePath(path);
		Inode * parent = getInodePath(shall , path);
		if(parent == NULL)
		{
			printf("error : wrong path\n");
			return;
		}
		if(parent -> type != 1)
		{
			printf("error : target is not directory\n");
			return;
		}
		pushFileDirectory(shall , parent , file , fileName);
		free(fileName);
	}
	renewTime(file -> lastUsedTime);
}

void myshowblock(Shall * shall , char * arg)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return ;
	}
	int num = stringToInt(arg);
	DataBlock * ad = (shall -> fileSystem -> dataBlock[num]);
	printf("%s\n",ad->data);
}
void mystate(Shall * shall)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return ;
	}
	int inode = 0 , dataBlock = 0;
	SuperBlock * super = shall -> fileSystem -> superBlock;
	int i;
	for(i = 0; i < 1024; i++)
		if(getBit(super->dataBlockCheck , i) == 0)
			++dataBlock;
	for(i = 0; i < 512; i++)
		if(getBit(super->inodeCheck , i) == 0)
			++inode;
	printf("free inode : %d\n",inode);
	printf("free data block : %d\n",dataBlock);
		
}
void myshowinode(Shall * shall , char * arg)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return ;
	}
	int num = stringToInt(arg);
	Inode * inode = shall -> fileSystem -> inode[num];;
	printf("file type : %s\n",(inode -> type == 0) ? "regular file" :"directory");
	printf("file size : %d byte\n",inode -> size * 128);
	int size = getRealSize(inode);
	int i;
	printf("data block list : ");
	if(size > 1)
		printf("%d ",inode -> data -> head -> next -> next -> value -> num);
	if(size > 103)
	{
		DataBlock * ad = inode -> data -> head -> next ->next -> next -> value;
		printf("%d ",ad -> num);
		for(i = 0; i < 102; i++)
		{
			DataBlock * aq = getDataBlockIndirect(shall -> fileSystem , ad , i);
			printf("%d ",aq -> num);
		}
	}
	for(i = 0; i < size; i++)
		printf("%d ",getDataBlockInode(shall -> fileSystem, inode , i) -> num);
	printf("\n");
}

void mytree(Shall * shall , char * path)
{
	if(shall == NULL)
	{
		printf("error : there is no shell\n");
		return ;
	}
	DTree * dTree = findDTreePath(shall , path);
	if(dTree == NULL)
	{
		printf("error : wrong path\n");
		return;
	}
	printf("%s\n",makeAPath(shall ,path));
	mytreeHelper(shall , dTree,2);
}	
void mytreeHelper(Shall * shall , DTree * dTree , int level)
{
	DNode * header = dTree -> childHead;
	int i = 0;
	while(header -> next != NULL)
	{
		header = header -> next;
		for(i = 0; i < level; i++)
			printf("-");
		printf("*%s\n",header -> value -> fileName);
		mytreeHelper(shall , header -> value , level + 3);
	}
}
//print pwd near command
void printPWD(Shall * shall)
{
	if(shall == NULL)
		return;
	printf("[");
	printf("%s",shall -> pwd);
	printf("]");
}
//get Inode as path
Inode* getInodePath(Shall * shall , char * path)
{
	DTree * dtree = findDTreePath(shall, path);
	if(dtree == NULL)
		return NULL;
	Inode * inode = shall -> fileSystem -> inode[dtree -> value];
	return inode;
}
//if path is Absoulte path , than return path else interpret to relative path
char * makeAPath(Shall * shall , char * RPath)
{
	char *t = (char*)malloc(sizeof(char) * 1000);
	if(RPath[0] == '/')	//if path is absoulte path
	{
		strcpy(t , RPath);
		return t;
	}
	strcpy(t , shall -> pwd);
	if(strcmp(shall -> pwd ,"/") != 0)
		strcat(t , "/");
	strcat(t , RPath);
	return t;
}

//get filename as path
char *getFileNamePath(char * path)
{
	char tPath[1000];
	strcpy(tPath , path);
	char  * cpFileName = (char*)malloc(sizeof(char) * 5);
	int aq = strlen(tPath);
	while(path[--aq] != '/')
		if(aq == 0)
		{
			aq = -1;
			strncpy(cpFileName , tPath , 4);
			break;
		}
	if(aq != -1)
		strncpy(cpFileName , tPath + aq + 1, 4);
	return cpFileName;
}
//erase filename in path (erase with /)
void removeFileNamePath(char * path)
{
	int len = strlen(path);
	int h = len;
	while(path[--h] != '/')
	{
		path[h] = 0;
		if(h == 0)
			break;
	}
	if(strcmp(path , "/") != 0)
		path[h] = 0;
}
