#include<stdio.h>
#include "getSource.h"
#include "compile.h"


int main()
{
	char fileName[30];
	printf("enter source file name\n");
	scanf("%s", fileName);
	
	if(!openSource(fileName) ) return 0;
	compile();
	
	closeSource();
	
	return 1;
}