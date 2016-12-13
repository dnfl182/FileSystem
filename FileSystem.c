
#include "FileSystem.h"
void initFileSystem(FileSystem ** address)	//File System Initialize
{
	*address = (FileSystem*)malloc(sizeof(FileSystem));	//give Extra Memoty
	FileSystem * fileSystem = *address;
	int i;
	for(i = 0; i < DATABLOCK_SIZE; i++)	//Data Block Setting
	{
		initDataBlock(&(fileSystem -> dataBlock[i]));	//DataBlock Initialize
		fileSystem -> dataBlock[i] -> num = i;
	}
	for(i = 0; i < INODE_SIZE; i++)	//Inode Setting
	{
		initInode(&(fileSystem -> inode[i]));	//Inode Initialize
		fileSystem -> inode[i] -> num = i;
	}
	initSuperBlock(&(fileSystem -> superBlock));
}
void initDataBlock(DataBlock ** address)	//DataBlock Initaalize
{
	*address = (DataBlock*)malloc(sizeof(DataBlock));// give ExtraMemory
	DataBlock * dataBlock = *address;

	int i;
	for(i = 0; i < DATABLOCK_BITE; i++)
		dataBlock -> data[i] = 0;	//defalut = 0
}
void initInode(Inode ** address)
{
	*address = (Inode*)malloc(sizeof(Inode));
	Inode *inode = *address;
	inode -> type = 0;	//defalut = 0 is filE
	inode -> size = 0;	//defalut = 0	
	inode -> lastUsedTime = (Time*)malloc(sizeof(Inode));
	int i;
	initDataBlockList(&(inode -> data));	//DataBlockList Initialize
}
void initSuperBlock(SuperBlock ** address)	//SuperBlock Initialize
{
	*address = (SuperBlock*)malloc(sizeof(SuperBlock));
	SuperBlock *superBlock = *address;
	int i = 0;
	for(i = 0; i < 	DATABLOCK_CHARSIZE; i++)
		superBlock -> dataBlockCheck[i] = 0;
	for(i = 0; i < 	INODE_CHARSIZE; i++)
		superBlock -> inodeCheck[i] = 0;		
}
void renewTime(Time * t)		// setTime to nowTime
{
	time_t time1;
	struct tm * time2;
	time(&time1);
	time2 = localtime(&time1);;
	t -> year = time2 -> tm_year;
	t -> month = time2 -> tm_mon;
	t -> day = time2 -> tm_mday;
	t -> hour = time2 -> tm_hour;
	t -> minute = time2 -> tm_min;
	t -> sec = time2 -> tm_sec;
}
void printTime(Time * t)	//print Time 
{
	printf("%d/%d/%d %2d:%2d:%2d",t->year + 1900,t->month,t->day,t->hour,t->minute,t->sec);
}
DataBlock * getEmptyDataBlock(FileSystem * fileSystem)	// getEmptyBlock from fileSystem
{
	SuperBlock *superBlock = fileSystem -> superBlock;
	int i = -1;
	while(getBit(superBlock -> dataBlockCheck , ++i));	//set SuperBlcok as using
	setBit(superBlock -> dataBlockCheck , i , 1);
	return fileSystem -> dataBlock[i];
}
Inode * getEmptyInode(FileSystem * fileSystem)	//getEmptyInode from fileSystme
{
	SuperBlock *superBlock = fileSystem -> superBlock;
	int i = -1;	
	while(getBit(superBlock -> inodeCheck , ++i));	//set SuperBlock as using
	setBit(superBlock -> inodeCheck , i , 1);
	return fileSystem -> inode[i];
}
void freeDataBlock(FileSystem * fileSystem , int num)	//DataBlock  Free
{
	memset(fileSystem -> dataBlock[num] -> data , 0 , 128);
	setBit(fileSystem -> superBlock -> dataBlockCheck , num , 0);
}
void freeInode(FileSystem * fileSystem , int num)	//Inode Free
{
	Inode * inode = fileSystem -> inode[num];
	inode -> type = 0;
	int size = inode -> size;
	int realSize = getRealSize(inode);
	inode -> size = 0;
	DataBlockList * list = inode -> data;
	DataBlockNode * header = list -> head;
	DataBlockNode * bh = NULL;
	int i = 0;
	////////////// Free DataBlcok in Inode
	for(i = 0; i < realSize; i++)
		freeDataBlock(fileSystem , getDataBlockInode(fileSystem ,inode , i) -> num);
	if(size > 1)
	{
		freeDataBlock(fileSystem , inode -> data -> head -> next -> next -> value  -> num);
	}
	if(size > 104)
	{
		DataBlock * ad = inode -> data -> head -> next -> next -> next -> value;
		DataBlock * ap;
		for(i = 0; i < 102; i++)
			freeDataBlock(fileSystem , getDataBlockIndirect(fileSystem , ad , i) -> num);
		freeDataBlock(fileSystem , ad -> num);
	}
	//////////////////////////////
	while(header -> next != list -> tail)
	{
		bh = header;
		header = header -> next;
		free(bh);
	}
	if(bh == NULL)
		free(list -> head);
	free(list -> tail);
	free(list);
	free(fileSystem -> inode[num]);
	initInode(&(fileSystem -> inode[num]));
	fileSystem -> inode[num] -> num = num;
	setBit(fileSystem -> superBlock -> inodeCheck , num , 0);
}
// DataBlockLinkedList initialize
void initDataBlockList(DataBlockList ** address)
{
	*address = (DataBlockList*)malloc(sizeof(DataBlockList));
	DataBlockList * list = *address;
	list -> head = (DataBlockNode*)malloc(sizeof(DataBlockNode));	
	list -> tail = (DataBlockNode*)malloc(sizeof(DataBlockNode));
	list -> head -> next = list -> tail;
	list -> head -> prev = list -> head;
	list -> tail -> next = list -> tail;
	list -> tail -> prev = list -> head;
	list -> size = 0;
}

// Push element to DataBlockList
void pushDataBlockList(DataBlockList * list, DataBlock * dataBlock)
{
	DataBlockNode * newNode = (DataBlockNode*)malloc(sizeof(DataBlockNode));
	newNode -> value = dataBlock;
	newNode -> next = list -> tail;
	newNode -> prev = list -> tail -> prev;
	newNode -> prev -> next = newNode;
	list -> tail -> prev = newNode;
	list -> size++;
}
//Make singleIndirect in inode
void makeSingleIndirect(FileSystem * fileSystem , Inode * inode)
{
	DataBlock * dataBlock = getEmptyDataBlock(fileSystem);
	pushDataBlockList(inode -> data , dataBlock);
	inode -> size++;
}
//Make DoubleIndirect in inode 
void makeDoubleIndirect(FileSystem * fileSystem , Inode * inode)
{
	DataBlock * dataBlock = getEmptyDataBlock(fileSystem);
	pushDataBlockList(inode -> data , dataBlock);
	inode -> size ++;
	int i;
	for(i = 0; i < MAX_INDIRECT; i++)
	{
		int num = getEmptyDataBlock(fileSystem) -> num;
		setDataBlockIndirect(dataBlock ,i,num);
		inode -> size++;
	}
}
//get Nth DataBlock in indirect
DataBlock * getDataBlockIndirect(FileSystem * fileSystem , DataBlock * dataBlock , int n)
{
	int num = 0;
	copyBit(&num , dataBlock -> data ,0,n*10, 8);		// int* -> char* Little Endian
	copyBit(&num , dataBlock -> data ,14,n*10 + 8,2);	 
	return fileSystem -> dataBlock[num];
}
// set Data Block Indriect  nth as num
void setDataBlockIndirect(DataBlock * dataBlock , int n , int num)
{
	copyBit(dataBlock -> data , &num , n * 10 , 0, 8);
	copyBit(dataBlock -> data , &num , n * 10 + 8 , 14 , 2);
}
//push DataBlock in inode
void pushDataBlockInode(FileSystem * fileSystem ,Inode * inode , DataBlock *dataBlock)
{
	int size = inode -> size;
	DataBlockNode * header = inode -> data -> head;
	int a[3] = {1 , MAX_INDIRECT , MAX_INDIRECT * MAX_INDIRECT};
	int b[3] = {a[0],a[0]+a[1],a[0]+a[1]+a[2]};
	int c[3] = {b[0],b[0]+b[1],b[0]+b[1]+b[2]};
	//if size is small extend size 
	if(size == c[0])
	{
		makeSingleIndirect(fileSystem , inode);
		++size;
	}
	else if(size == c[1])
	{
		makeDoubleIndirect(fileSystem , inode);
		size += b[2];
	}
	else if(size == c[2])
	{
		printf("Error - putDataInode");
		return;
	}
	// push DataBlock
	if(size == 0)
	{
		pushDataBlockList(inode -> data , dataBlock);
	}
	else if(size < c[1])
	{
		size -= (c[0] + b[0]);
		header = header -> next -> next;
		setDataBlockIndirect(header -> value , size, dataBlock -> num);
	}
	else if(size < c[2])
	{
		size -= (c[1] + b[1] + 1);
		header = header -> next -> next -> next;
		DataBlock * doubleIn = getDataBlockIndirect(fileSystem , header -> value , size / a[1]);
		setDataBlockIndirect(doubleIn , size % a[1] , dataBlock -> num);
	}
	inode -> size++;
}

// get nth DataBlcok excpet indirect Block 
DataBlock * getDataBlockInode(FileSystem * fileSystem , Inode * inode , int n)
{
	int a[3] = {1 , MAX_INDIRECT , MAX_INDIRECT * MAX_INDIRECT};
	int b[3] = {a[0],a[0]+a[1],a[0]+a[1]+a[2]};
	int c[3] = {b[0],b[0]+b[1],b[0]+b[1]+b[2]};
	DataBlockNode * header = inode -> data -> head;
	DataBlock * r = NULL;
	if(n == 0)
		r = header -> next -> value;
	else if(n < c[1])
	{
		header = header -> next -> next;
		n -= b[0];
		r = getDataBlockIndirect(fileSystem , header -> value , n);
	}
	else if(n < c[2])
	{
		header = header -> next -> next -> next;
		n -= (c[1]);
		r = getDataBlockIndirect(fileSystem , header -> value , n / a[1]);
		r = getDataBlockIndirect(fileSystem , r , n % a[1]);
	}
	return r;
}
// get Data from fromBit as n Bit
char * getDataInode(FileSystem * fileSystem , Inode * inode , int fromBit,  int nBit)
{
	int i;
	char * r = (char*)malloc(sizeof(char) * SIZETOCHAR(nBit));
	DataBlock * h = getDataBlockInode(fileSystem , inode , fromBit / (DATABLOCK_BITE * 8));
	for(i = 0; i < nBit; i++)
	{
		setBit(r , i , getBit(h -> data ,(fromBit + i) % (8 * DATABLOCK_BITE)));
		if(fromBit + i % (DATABLOCK_BITE * 8) == 0)
			h = getDataBlockInode(fileSystem , inode , (fromBit + i) / (DATABLOCK_BITE * 8));
	}
	return r;
}
//	setData from source to indoe
void setDataInode(FileSystem * fileSystem, Inode * inode, int fromBit, int nBit, int fromSourceBit ,void * source)
{
	int i;
	DataBlock * h = getDataBlockInode(fileSystem , inode , fromBit / (DATABLOCK_BITE * 8));
	for(i = 0; i < nBit; i++)
	{
		setBit(h -> data , (fromBit + i) % DATABLOCK_BITE , getBit(source , i + fromSourceBit));
		if(fromBit + i % (DATABLOCK_BITE * 8) == 0)	//경계를 넘어가면 
			h = getDataBlockInode(fileSystem , inode , (fromBit + i) / (DATABLOCK_BITE * 8));	//다음 데이터 블럭 할당 
		
	}
}
//get Size except indirect block size
int getRealSize(Inode * inode) 
{
	int size = inode -> size;
	if(size > 1)
		size -= 1;
	if(size > 1 + MAX_INDIRECT)
		size -= (1 + MAX_INDIRECT);
	return size;
}

