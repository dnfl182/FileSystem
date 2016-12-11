#include <stdio.h>
#include <string.h>
#include "FileSystem.h"
#include "Shall.h"

void testMakeFile(Shall * shall);
int main(void)
{
	Shall *shall = NULL;
	int i;
	while(1)
		runCommand(&shall);
}
