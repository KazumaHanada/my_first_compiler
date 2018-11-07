#include "getSource.h"


#define MAXTABLE 100
#define MAXLEVEL   5


typedef struct tableE {
	KindT kind;
	char name[MAXNAME];
	union {
		int value;
		struct {
			RelAddr raddr;
			int pars;
		}f;
		RelAddr raddr;
	}u;
} TabelE;


static TabelE nameTable[MAXTABLE];
static int tIndex =  0;
static int level  = -1;
static int index[MAXLEVEL];
static int addr[MAXLEVEL];
static int localAddr;
static int tfIndex;

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


void blockEnd()
{
	level --;
	tIndex    = index[level];
	localAddr = addr[level];
}


int bLevel()
{
	return level;
}


int fPars()
{
	return nameTable[index[level-1]].u.f.pars;
}


void enterT(char *id)
{
	if(++tIndex < MAXTABLE){
		
		//解決できず保留   警告: 組み込み関数 ‘strcpy’ の互換性がない暗黙的な宣言です
		//strcpy(nameTable[tIndex].name, id);
		
	}else{
	
		errorF("too many names");
	}
}


int enterTfunc(char *id, int v)
{
	enterT(id);
	nameTable[tIndex].kind = funcId;
	nameTable[tIndex].u.f.raddr.level = level;
	nameTable[tIndex].u.f.raddr.addr = v; 
	nameTable[tIndex].u.f.pars = 0;
	tfIndex = tIndex;
	return tIndex;
}


int enterTpar(char *id)
{
	enterT(id);
	nameTable[tIndex].kind = parId;
	nameTable[tIndex].u.raddr.level = level;
	nameTable[tfIndex].u.f.pars++;
	return tIndex;
}


int enterTvar(char *id)
{
	enterT(id);
	nameTable[tIndex].kind = varId;
	nameTable[tIndex].u.raddr.level = level;
	nameTable[tIndex].u.raddr.addr = localAddr++;
	return tIndex;
}


int enterTconst(char *id , int v)
{
	enterT(id);
	nameTable[tIndex].kind = constId;
	nameTable[tIndex].u.value = v;
	
	return tIndex;
}


void endpar()
{
	int i;
	int pars = nameTable[tfIndex].u.f.pars;
	if (pars == 0)  return;
	for (i=1; i<=pars; i++)
		 nameTable[tfIndex+i].u.raddr.addr = i-1-pars;
}


void changeV(int ti, int newVal)
{
	nameTable[ti].u.f.raddr.addr = newVal;
}


int frameL()
{
	return localAddr;
}