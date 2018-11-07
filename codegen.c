#include <stdio.h>
#include "codegen.h"
#include "getSource.h"

#define MAXCODE 200


typedef struct inst{
	
	OpCode opCode;
	union{
		RelAddr addr;
		int value;
		Operator optr;
	} u;
	
} Inst;


static Inst code[MAXCODE];
static int cIndex = -1;
static void checkMax();


int nextCode()
{
	return cIndex + 1;
}


int genCodeV(OpCode op,  int v)
{
	checkMax();
	code[cIndex].opCode  = op;
	code[cIndex].u.value =  v;
	return cIndex;
}


int genCodeR()
{
	if (code[cIndex].opCode == ret)
		return cIndex;
	checkMax();
	code[cIndex].opCode = ret;
	code[cIndex].u.addr.level = bLevel();
	code[cIndex].u.addr.addr = fPars();
	return cIndex;
}


void checkMax()
{
	if(++cIndex < MAXCODE) return;
	
	errorF("too many code");
}


void backPatch(int i)
{
	code[i].u.value = cIndex + 1;
}
