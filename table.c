#include "getSource.h"


#define MAXLEVEL   5


static int tIndex =  0;
static int level  = -1;
static int index[MAXLEVEL];
static int addr[MAXLEVEL];
static int localAddr;


void blockBegin(int firstAddr)
{
	if(level == -1){
		
		localAddr = firstAddr;
		tIndex = 0;
		level++;
		return;
	}
	
	if(level == MAXLEVEL - 1) errorF("too many nested blocks");
	index[level] = tIndex;
	addr[level]  = localAddr;
	localAddr = firstAddr;
	level++;
	
	return;
}