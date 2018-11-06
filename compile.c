#include<stdio.h>
#include "getSource.h"

static Token token;

int compile()
{
	printf("strat compilation\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
	initSource();
	token = nextToken();
	
	return 1;
};