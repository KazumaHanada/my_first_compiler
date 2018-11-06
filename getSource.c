#include<stdio.h>
#include<string.h>
#include"getSource.h"


static FILE *fpi;
//static FILE *fptex;


static int  lineIndex;
static char ch;


static KeyId charClassT[256];


static void initCharClassT()
{
	int i;
	
	//とりあえず、全ての配列にotherを代入
	for(i=0; i<256; i++)
		charClassT[i] = others;
	
	//ASCⅡコードに基づいた添字を使う
	for(i='0'; i<='9';i++)
		charClassT[i] = digit;
	
	for(i='A'; i<='Z'; i++)
	    charClassT[i] = letter;
	
	for(i='a'; i<='z'; i++)
		charClassT[i] = letter;
	
	charClassT['+'] = Plus;
	charClassT['-'] = Minus;
	charClassT['*'] = Mult;
	charClassT['/'] = Div;
	charClassT['('] = Lparen;
	charClassT[')'] = Rparen;
	charClassT['='] = Equal;
	charClassT['<'] = Lss;
	charClassT['>'] = Gtr;
	charClassT[','] = Comma;
	charClassT['.'] = Period;
	charClassT[';'] = Semicolon;
	charClassT[':'] = colon;
}


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


void initSource()
{
	lineIndex = -1;
	ch = '\n';
	//printed = 1;
	initCharClassT();
	
	/*
	//LaTeXコマンド
	fprintf(fptex,"\\documentstyle[12pt]{article}\n");   
	fprintf(fptex,"\\begin{document}\n");
	fprintf(fptex,"\\fboxsep=0pt\n");
	fprintf(fptex,"\\def\\insert#1{$\\fbox{#1}$}\n");
	fprintf(fptex,"\\def\\delete#1{$\\fboxrule=.5mm\\fbox{#1}$}\n");
	fprintf(fptex,"\\rm\n");
	*/
}