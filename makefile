chicken:Client.o FileSystem.o Shall.o Util.o
	gcc -o chicken Client.o FileSystem.o Shall.o Util.o
Client.o:Client.c
	gcc -c Client.c
FileSystem.o:FileSystem.c
	gcc -c FileSystem.c
Shall.o:Shall.c
	gcc -c Shall.c
Util.o:Util.c
	gcc -c Util.c
