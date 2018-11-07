#include<stdio.h>
#include "getSource.h"
#include "codegen.h"

#define FIRSTADDR 2


static Token token;//現在取得しているトークン


static void block(int pIndex);


static void constDecl();
static void varDecl();
static void funcDecl();

int compile()
{
	printf("strat compilation\n");
	printf("\n");
	printf("\n");
	printf("\n");
	
	initSource();
	token = nextToken();
	blockBegin(FIRSTADDR);
	block(0);
	
	return 1;
};


//構文解析 p40
void block(int pIndex)
{
	int backP = genCodeV(jmp, 0);
	
	while(1) {
		
		switch(token.kind){
		
		//トークン名がconst
		case Const:
		
			token = nextToken();
			//定数宣言の構文解析
			constDecl();
			continue;
		
		//トークン名がVar
		case Var:
			
			token = nextToken();
			//変数宣言の構文解析
			varDecl();
			continue;
			
		case Func:
			
			token = nextToken();
			//関数宣言の構文解析
			funcDecl();
			continue;
			
		default:
			
			break;
		}
		
		break;
	}
	
	
	backPatch(backP);
	changeV(pIndex, nextCode());
	genCodeV(ict, frameL());
	
	//statement
	
	genCodeR();
	blockEnd();
}


//定数宣言の構文解析 p40
void constDecl()
{
	Token temp;
	
	while(1){
		
		if(token.kind == Id){                // [const] [ident]↓
			
			setIdKind(constId);//LateX処理用
			temp = token;                    // [const] [ident]↓
			//getSource.cから
			checkGet(nextToken(), Equal);	 // [const] [ident] [?]↓
                                             // [const] [ident] [=] [num]↓
			if(token.kind == Num){
				
				enterTconst(temp.u.id, token.u.value);
				
			}else{
				
				errorType("number");
			}
			
			token = nextToken();
                                             // [const] [ident] [=] [num] [ (,) or (;) or (num) or (?) ]↓
			
		}else{
	
			errorMissingId();
		}
		
		
                                             // [const] [ident] [=] [num] [(,) or (num) or (;) or (?)]↓
		if(token.kind != Comma){             // [const] [ident] [=] [num] [       (num) or (;) or (?)]↓
			
			if(token.kind == Id){            // [const] [ident] [=] [num] [num]↓
				
				errorInsert(Comma);
				continue;
				
			}else{                           // [const] [ident] [=] [num] [(;) or (?)]↓
				
				break;
			}
			
		}else{                               // [const] [ident] [=] [num] [,]↓  

			token = nextToken();             // [const] [ident] [=] [num] [,] [?]↓
		}
	}
	
                                             // [const] [ident] [=] [num] [(;) or (?)]↓
	token = checkGet(token, Semicolon);
                                             // [const] [ident] [=] [num] [;] [?] ↓
}


//変数宣言の構文解析 p40
void varDecl()
{
	while(1){
                                        // [var] [?]↓
		if(token.kind == Id){           // [var] [ident]↓
			
			setIdKind(varId);//LateX処理用
			enterTvar(token.u.id);
			token = nextToken();
                                        // [var] [ident] [?]↓
		}else{
			
			errorMissingId();
		}
		
                                        // [var] [ident] [(,) or (ident) or (;) or (?)]↓
		if(token.kind != Comma){        // [var] [ident] [       (ident) or (;) or (?)]↓
			
			if(token.kind == Id){       // [var] [ident] [ident]↓
				
				errorInsert(Comma);
				continue;
				
			}else{                      // [var] [ident] [ (;) or (?)]↓
				
				break;
			}
				
		}else{                          // [var] [ident] [,]↓
			
			token = nextToken();	    // [var] [ident] [,] [?]↓
		}	
		
	}

                                        // [var] [ident] [ (;) or (?)]↓
	token = checkGet(token, Semicolon);
                                        // [var] [ident] [;] [?]↓
}


//関数宣言の構文解析 p40
void funcDecl()
{
	int fIndex;
	
	if (token.kind == Id){                            //[function] [ident]↓
		
		setIdKind(funcId);//LateX処理用
		                               //codegen.cより
		fIndex = enterTfunc(token.u.id, nextCode());
		
		token = checkGet(nextToken(), Lparen);        //[function] [ident] [?]↓
                                                      //[function] [ident] [(] [?]↓
		blockBegin(FIRSTADDR);
		
		
		while(1){
			
			if(token.kind == Id){                     //[function] [ident] [(] [ident]↓
				
				setIdKind(parId);//LateX処理用
				enterTpar(token.u.id);
				token = nextToken();                  //[function] [ident] [(] [ident] [(,) or (ident) or (?)]↓
				
				
			}else{                                    //[function] [ident] [(] [?]↓
				
				break;
			}
				
                                                      //[function] [ident] [(] [ident] [(,) or (ident) or (?)]↓
			if(token.kind != Comma){                  //[function] [ident] [(] [ident] [       (ident) or (?)]↓
				
				if(token.kind == Id){                 //[function] [ident] [(] [ident] [ident]↓
					
					errorInsert(Comma);
					continue;
					
				}else{                                //[function] [ident] [(] [ident] [?]↓
					
					break;
				}


			}else{                                    //[function] [ident] [(] [ident] [,]↓
				
				token = nextToken();
                                                      //[function] [ident] [(] [ident] [,] [?]↓
			}
		}
		
                                                      //[function] [ident] [(] ... [?]↓
		token = checkGet(token, Rparen);
                                                      //[function] [ident] [(] ... [)] [(;) or (?)]↓
		endpar();
		
		if(token.kind == Semicolon){                  //[function] [ident] [(] ... [)] [;]↓
			
			errorDelete();
			token = nextToken();
                                                      
		}
                                                      //[function] [ident] [(] ... [)] [?]↓
		
		block(fIndex);                                
                                                      //[function] [ident] [(] ... [)] (block) [?]↓
		token = checkGet(token, Semicolon);
                                                      //[function] [ident] [(] ... [)] (block) [;]↓

	}else{
		
		errorMissingId();
	}
}