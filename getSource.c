#include<stdio.h>
#include<string.h>


static FILE *fpi;
//static FILE *fptex;


int openSource(char fileName[])
{		
	if((fpi = fopen(fileName, "r")) == NULL){
	
		printf("can't open %s\n", fileName);
		return 0;
	}

	/*
	char fileNameO[30];
	strcpy(fileNameO,fileName);
	strcat(fileNameO,".tex");
	if((fptex = fopen(fileNameO, "w")) == NULL){
		
		printf("can't open %s\n", fileNameO);
		return 0;
	}
	*/

	printf("open\n");
	return 1;
}


void closeSource()
{
	fclose(fpi);
	//fclose(fptex);	
	printf("close\n");
}