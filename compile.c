#include<stdio.h>
#include "getSource.h"

#define FIRSTADDR 2


static Token token;

int compile()
{
	printf("strat compilation\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
	initSource();
	token = nextToken();
	blockBegin(FIRSTADDR);
	
	return 1;
};