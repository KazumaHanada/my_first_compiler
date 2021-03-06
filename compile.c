#include<stdio.h>
#include "getSource.h"
#include "codegen.h"


#define FIRSTADDR 2


static Token token;//現在取得しているトークン


static void block(int pIndex);
static void constDecl();
static void varDecl();
static void funcDecl();
static void statement();
static void expression();
static void term();
static void factor();
static void condition();
static int isStBeginKey(Token t);


int compile()
{
	printf("strat compilation\n");
	printf("\n");
	printf("\n");
	printf("\n");
	initSource();
	token = nextToken();
	blockBegin(FIRSTADDR);
	
	//↓構文解析の実装:完了 意味解析の実装:未 コード生成の実装:未
	block(0);
	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("complete compilation\n");
	
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
		
		//トークン名がFunc
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
	
	statement();
	
	genCodeR();
	blockEnd();
}


//定数宣言の構文解析 p40
void constDecl()
{
	Token temp;
	
	while(1){
		                                         // [const] [ident or (?)]↓
		if(token.kind == Id){                   // [const] [ident]↓
			setIdKind(constId);//LateX処理用
			temp = token;                   // [const] [ident]↓
			//getSource.cから
			checkGet(nextToken(), Equal);	// [const] [ident] [?]↓
                                                        // [const] [ident] [=] [num]↓
			if(token.kind == Num){
				enterTconst(temp.u.id, token.u.value);
			}else{
				errorType("number");
			}
			
			token = nextToken();            // [const] [ident] [=] [num] [ (,) or (;) or (num) or (?) ]↓
                                                        
		}else{                                  // [const] [?]↓
			errorMissingId();
		}


                                                        // [const] [ident] [=] [num] [(,) or (num) or (;) or (?)]↓
		if(token.kind != Comma){                // [const] [ident] [=] [num] [       (num) or (;) or (?)]↓
			
			if(token.kind == Id){           // [const] [ident] [=] [num] [num]↓
				errorInsert(Comma);
				continue;
			}else{                          // [const] [ident] [=] [num] [(;) or (?)]↓
				break;
			}
		}else{                                  // [const] [ident] [=] [num] [,]↓  
			token = nextToken();            // [const] [ident] [=] [num] [,] [?]↓
		}
	}
	
                                                        // [const] [ident] [=] [num] [(;) or (?)]↓
	token = checkGet(token, Semicolon);             // [const] [ident] [=] [num] [;] [?] ↓
                                                        
}


//変数宣言の構文解析 p40
void varDecl()
{
	while(1){
                                                       // [var] [?]↓
		if(token.kind == Id){                  // [var] [ident]↓
			setIdKind(varId);//LateX処理用
			enterTvar(token.u.id);
			token = nextToken();           // [var] [ident] [?]↓                                 
		}else{
			errorMissingId();
		}		

                 
                                                       // [var] [ident] [(,) or (ident) or (;) or (?)]↓
		if(token.kind != Comma){               // [var] [ident] [       (ident) or (;) or (?)]↓	
			if(token.kind == Id){          // [var] [ident] [ident]↓
				errorInsert(Comma);
				continue;	
			}else{                         // [var] [ident] [ (;) or (?)]↓
				break;
			}	
		}else{                                 // [var] [ident] [,]↓
			token = nextToken();	       // [var] [ident] [,] [?]↓
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
	
	if (token.kind == Id){                                //[function] [ident]↓
		setIdKind(funcId);//LateX処理用
		fIndex = enterTfunc(token.u.id, nextCode());//codegen.cより
		token = checkGet(nextToken(), Lparen);        //[function] [ident] [?]↓
                                                              //[function] [ident] [(] [(ident) or (?)]↓
		blockBegin(FIRSTADDR);
														
		while(1){                                     //[function] [ident] [(] [(ident) or (?)]↓
			
                        if(token.kind == Id){                 //[function] [ident] [(] [ident]↓
				setIdKind(parId);//LateX処理用
				enterTpar(token.u.id);
				token = nextToken();          //[function] [ident] [(] [ident] [(,) or (ident) or (?)]↓
			}else{                                //[function] [ident] [(] [?]↓
				
				break;
			}


                                                              //[function] [ident] [(] [ident] [(,) or (ident) or (?)]↓
			if(token.kind != Comma){              //[function] [ident] [(] [ident] [       (ident) or (?)]↓
				
				if(token.kind == Id){         //[function] [ident] [(] [ident] [ident]↓
					errorInsert(Comma);   //[function] [ident] [(] [ident] [,]↓
					continue;
				}else{                        //[function] [ident] [(] [ident] [?]↓
					break;
				}	
			}else{                                //[function] [ident] [(] [ident] [,]↓
				token = nextToken();          //[function] [ident] [(] [ident] [,] [?]↓	                              
			}
		}
                                                              //[function] [ident] [(] ... [?]↓
		token = checkGet(token, Rparen);              //[function] [ident] [(] ... [)] [(;) or (?)]↓
                endpar();
		
		if(token.kind == Semicolon){                  //[function] [ident] [(] ... [)] [;]↓	
			errorDelete();
			token = nextToken();   
		}
                                                              //[function] [ident] [(] ... [)] [?]↓
		block(fIndex);                                
                                                              //[function] [ident] [(] ... [)]〈block〉[?]↓
		token = checkGet(token, Semicolon);           //[function] [ident] [(] ... [)]〈block〉[;]↓                                                              


	}else{
		errorMissingId();
	}
}


//文の構文解析
void statement()
{
	int tIndex;
	KindT k;
        int backP,backP2;
		
	while(1){
		
		switch(token.kind){
					
		case Id:	                                            //[ident] 
			tIndex = searchT(token.u.id,varId);
			setIdKind(k=kindT(tIndex));
			if(k != varId && k != parId) errorType("var/par");
			
			token = checkGet(nextToken(), Assign);	            //[ident] [:=]
									    //[ident] [:=] [?]
			expression();                                       //[ident] [:=]〈expression〉
			genCodeT(sto, tIndex);
			
			return;
		
		case Begin:                                            //[begin] 	
			token = nextToken();
			                                               //[begin] [?]
			while(1){
				
				statement();
									
				while(1){			       //[begin]〈statement〉[(;) or (end) or (begin) or (?)]
					
					if(token.kind == Semicolon){   //[begin]〈statement〉[;]	
						token = nextToken();   //[begin]〈statement〉[;] [?]
						break;
					}
					
					if(token.kind == End){         //[begin]〈statement [end]	
						token = nextToken();   //[begin]〈statement〉[end] [?]
						return;
					}
					
					if(isStBeginKey(token)){       //[begin]〈statement〉[begin}
						errorInsert(Semicolon);//[begin]〈statement〉[?]
						break;
					}				
								       //[begin]〈statement〉[?]
					errorDelete();		       //[begin]〈statement〉
					token = nextToken();           //[begin]〈statement〉[?]
				}
			}
						
		case End:                                //[begin]〈statement〉[;]、、、〈statement〉[end]		
			return;

		case If:                                 //[if]			
			token = nextToken();             //[if] [?]
			condition();                     //[if]〈condition〉
			token = checkGet(token, Then);   //[if]〈condition〉[then]
			backP = genCodeV(jpc, 0);
			statement();                     //[if]〈condition〉[then]〈statement〉
			backPatch(backP);
			return;
		
		case While:                              //[while]			
			token  = nextToken();            //[while] [?]
			backP2 = nextCode();
			condition();                     //[while]〈condition〉
			token = checkGet(token, Do);     //[while]〈condition〉[do]
			backP = genCodeV(jpc, 0);
			statement();                     //[while]〈condition〉[do]〈statement〉
			backP = genCodeV(jpc, backP2);
			backPatch(backP);
			return;
						
		case Ret:                                //[return]
			token = nextToken();             //[return] [?]
			expression();                    //[return]〈expression〉
			genCodeR();
			return;
			
		case Write:                              //[write]
			token = nextToken();             //[write] [?]
			expression();                    //[write]〈expression〉
			genCodeO(wrt);
			return;
					
		case WriteLn:                            //[writeln]
			token = nextToken();
			genCodeO(wrt);
			return;
						
		case Period:                             //[.]
			return;

		case Semicolon:                          //[;]		
			return;		

	    default:
			errorDelete();
			token = nextToken();
			continue;
		}	
	}
}


int isStBeginKey(Token t)
{
	switch (t.kind){
	case If: 
	case Begin: 
	case Ret:
	case While: 
	case Write: 
	case WriteLn:		
		return 1;
	default:
		return 0;
	}
}
				
				
//式の構文解析 p40
void expression()
{
	KeyId k;
	k = token.kind;
	                                    //[(+) or (-) or (なし)]
	if (k==Plus || k==Minus){           //[(+) or (-)]
	                                  
		token = nextToken();        //[(+) or (-)] [?]
		term();                     //[(+) or (-)]〈term〉
		if (k==Minus) genCodeO(neg);
	}else{                              //[なし]
		term();                     //[なし]〈term〉
	}
		                            //[(+) or (-) or (なし)]〈term〉
	
	k = token.kind;
	
	while (k==Plus || k==Minus){        //[(+) or (-) or (なし)]〈term〉[(+) or (-)]
		
		token = nextToken();        //[(+) or (-) or (なし)]〈term〉[(+) or (-)] [?]
		term();                     //[(+) or (-) or (なし)]〈term〉[(+) or (-)]〈term〉
			
		if (k==Minus)
			genCodeO(sub);
		else
			genCodeO(add);
			
		k = token.kind;
	}
}


//式の項の構文解析
void term()
{
	KeyId k;
	
	factor();                   //〈factor〉
	
	k = token.kind;
	
	while (k==Mult || k==Div){  //〈factor〉[(*) or (/)]
		
		token = nextToken();//〈factor〉[(*) or (/)] [?]
		
		factor();           //〈factor〉[(*) or (/)]〈factor〉
		
		if (k==Mult)
			genCodeO(mul);
		else
			genCodeO(div);
		
		k = token.kind;
	}
}


//式の因子の構文解析
void factor()
{
	int tIndex, i;
	KeyId k;
	
	if (token.kind==Id){

		tIndex = searchT(token.u.id, varId);
		setIdKind(k=kindT(tIndex));//LateX用処理
		
		switch (k) {
		
		//変数名かパラメータ名
		case varId: case parId:            //[ident]
			genCodeT(lod, tIndex);
			token = nextToken(); 
			break;
		
		//定数名
		case constId:                      //[number]
			genCodeV(lit, val(tIndex));
			token = nextToken(); 
			break;
		
		//関数呼び出し
		case funcId:                                                    //[ident] [(]〈expression〉[)] or [ident] [(]〈expression〉[,] 、、、〈expression〉[)] or [ident] [(] [)]
			
			token = nextToken();
			
			if (token.kind==Lparen){                                //[ident] [(]
				
				//iは実引数の個数
				i=0; 					
				
				token = nextToken();                            //[ident] [(] [?]
				
				if (token.kind != Rparen) {                     //[ident] [(] [?]
					
  					for (; ; ) {
						
						expression();                   //[ident] [(]〈expression〉[?]
						i++;	
						
						if (token.kind==Comma){         //[ident] [(]〈expression〉[,]
							token = nextToken();    //[ident] [(]〈expression〉[,] [?]
							continue;
						}
							
						token = checkGet(token, Rparen);//[ident] [(]〈expression〉[)]
						break;
					}
					
				} else{                                         //[ident] [(] [)]
					
					token = nextToken();
					if (pars(tIndex) != i) errorMessage("\\#par");	
				}
					
				
			
			}else{
				errorInsert(Lparen);
				errorInsert(Rparen);
			}
			
			genCodeT(cal, tIndex);
			break;
		}




	}else if (token.kind==Num){              //[定数]
		
		genCodeV(lit, token.u.value);
		token = nextToken();
	
	}else if (token.kind==Lparen){           //[(]
	
		token = nextToken();
		expression();                    //[(]〈expression
		token = checkGet(token, Rparen); //[(]〈expression〉[)]
	}
		
	//因子の後がまた因子ならエラー
	switch (token.kind){
	case Id: case Num: case Lparen:
		errorMissingOp();
		factor();
	default:
		return;
	}	
}


//条件式の構文解析
void condition()					
{
	KeyId k;
	if (token.kind==Odd){                  //[odd]
	
		token = nextToken();           //[odd] [?]
		expression();                  //[odd]〈expression〉
		genCodeO(odd);
	
	}else{                                 //[?]
		expression();                  //〈expression〉
		k = token.kind;
		
		switch(k){                     //〈expression〉[(=) or (<>) or (<) or (>) or (<=) or (>=)]
		case Equal: case Lss: case Gtr:
		case NotEq: case LssEq: case GtrEq:
			break;
		default:
			errorType("rel-op");
			break;
		}
		
		token = nextToken();           //〈expression〉[(=) or (<>) or (<) or (>) or (<=) or (>=)] [?]
		expression();                  //〈expression〉[(=) or (<>) or (<) or (>) or (<=) or (>=)]〈expression〉
		switch(k){
		case Equal:	genCodeO(eq);   break;
		case Lss:	genCodeO(ls);   break;
		case Gtr:	genCodeO(gr);   break;
		case NotEq:	genCodeO(neq);  break;
		case LssEq:	genCodeO(lseq); break;
		case GtrEq:	genCodeO(greq); break;
		}
	}
}