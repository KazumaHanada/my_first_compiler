#include<stdio.h>
#include "getSource.h"
#include "codegen.h"


#define FIRSTADDR 2


static Token token;//���ݎ擾���Ă���g�[�N��


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
	
	//���\����͂̎���:���� �Ӗ���͂̎���:�� �R�[�h�����̎���:��
	block(0);
	
	printf("\n");
	printf("\n");
	printf("\n");
	printf("complete compilation\n");
	
	return 1;
};


//�\����� p40
void block(int pIndex)
{
	int backP = genCodeV(jmp, 0);
	
	while(1) {
		
		switch(token.kind){
		
		//�g�[�N������const
		case Const:
			token = nextToken();
			//�萔�錾�̍\�����
			constDecl();
			continue;
		
		//�g�[�N������Var
		case Var:	
			token = nextToken();
			//�ϐ��錾�̍\�����
			varDecl();
			continue;
		
		//�g�[�N������Func
		case Func:	
			token = nextToken();
			//�֐��錾�̍\�����
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


//�萔�錾�̍\����� p40
void constDecl()
{
	Token temp;
	
	while(1){
		                                         // [const] [ident or (?)]��
		if(token.kind == Id){                   // [const] [ident]��
			setIdKind(constId);//LateX�����p
			temp = token;                   // [const] [ident]��
			//getSource.c����
			checkGet(nextToken(), Equal);	// [const] [ident] [?]��
                                                        // [const] [ident] [=] [num]��
			if(token.kind == Num){
				enterTconst(temp.u.id, token.u.value);
			}else{
				errorType("number");
			}
			
			token = nextToken();            // [const] [ident] [=] [num] [ (,) or (;) or (num) or (?) ]��
                                                        
		}else{                                  // [const] [?]��
			errorMissingId();
		}


                                                        // [const] [ident] [=] [num] [(,) or (num) or (;) or (?)]��
		if(token.kind != Comma){                // [const] [ident] [=] [num] [       (num) or (;) or (?)]��
			
			if(token.kind == Id){           // [const] [ident] [=] [num] [num]��
				errorInsert(Comma);
				continue;
			}else{                          // [const] [ident] [=] [num] [(;) or (?)]��
				break;
			}
		}else{                                  // [const] [ident] [=] [num] [,]��  
			token = nextToken();            // [const] [ident] [=] [num] [,] [?]��
		}
	}
	
                                                        // [const] [ident] [=] [num] [(;) or (?)]��
	token = checkGet(token, Semicolon);             // [const] [ident] [=] [num] [;] [?] ��
                                                        
}


//�ϐ��錾�̍\����� p40
void varDecl()
{
	while(1){
                                                       // [var] [?]��
		if(token.kind == Id){                  // [var] [ident]��
			setIdKind(varId);//LateX�����p
			enterTvar(token.u.id);
			token = nextToken();           // [var] [ident] [?]��                                 
		}else{
			errorMissingId();
		}		

                 
                                                       // [var] [ident] [(,) or (ident) or (;) or (?)]��
		if(token.kind != Comma){               // [var] [ident] [       (ident) or (;) or (?)]��	
			if(token.kind == Id){          // [var] [ident] [ident]��
				errorInsert(Comma);
				continue;	
			}else{                         // [var] [ident] [ (;) or (?)]��
				break;
			}	
		}else{                                 // [var] [ident] [,]��
			token = nextToken();	       // [var] [ident] [,] [?]��
		}	
	}
                                                       // [var] [ident] [ (;) or (?)]��
	token = checkGet(token, Semicolon);
                                                       // [var] [ident] [;] [?]��
}


//�֐��錾�̍\����� p40
void funcDecl()
{
	int fIndex;
	
	if (token.kind == Id){                                //[function] [ident]��
		setIdKind(funcId);//LateX�����p
		fIndex = enterTfunc(token.u.id, nextCode());//codegen.c���
		token = checkGet(nextToken(), Lparen);        //[function] [ident] [?]��
                                                              //[function] [ident] [(] [(ident) or (?)]��
		blockBegin(FIRSTADDR);
														
		while(1){                                     //[function] [ident] [(] [(ident) or (?)]��
			
                        if(token.kind == Id){                 //[function] [ident] [(] [ident]��
				setIdKind(parId);//LateX�����p
				enterTpar(token.u.id);
				token = nextToken();          //[function] [ident] [(] [ident] [(,) or (ident) or (?)]��
			}else{                                //[function] [ident] [(] [?]��
				
				break;
			}


                                                              //[function] [ident] [(] [ident] [(,) or (ident) or (?)]��
			if(token.kind != Comma){              //[function] [ident] [(] [ident] [       (ident) or (?)]��
				
				if(token.kind == Id){         //[function] [ident] [(] [ident] [ident]��
					errorInsert(Comma);   //[function] [ident] [(] [ident] [,]��
					continue;
				}else{                        //[function] [ident] [(] [ident] [?]��
					break;
				}	
			}else{                                //[function] [ident] [(] [ident] [,]��
				token = nextToken();          //[function] [ident] [(] [ident] [,] [?]��	                              
			}
		}
                                                              //[function] [ident] [(] ... [?]��
		token = checkGet(token, Rparen);              //[function] [ident] [(] ... [)] [(;) or (?)]��
                endpar();
		
		if(token.kind == Semicolon){                  //[function] [ident] [(] ... [)] [;]��	
			errorDelete();
			token = nextToken();   
		}
                                                              //[function] [ident] [(] ... [)] [?]��
		block(fIndex);                                
                                                              //[function] [ident] [(] ... [)]�qblock�r[?]��
		token = checkGet(token, Semicolon);           //[function] [ident] [(] ... [)]�qblock�r[;]��                                                              


	}else{
		errorMissingId();
	}
}


//���̍\�����
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
			expression();                                       //[ident] [:=]�qexpression�r
			genCodeT(sto, tIndex);
			
			return;
		
		case Begin:                                            //[begin] 	
			token = nextToken();
			                                               //[begin] [?]
			while(1){
				
				statement();
									
				while(1){			       //[begin]�qstatement�r[(;) or (end) or (begin) or (?)]
					
					if(token.kind == Semicolon){   //[begin]�qstatement�r[;]	
						token = nextToken();   //[begin]�qstatement�r[;] [?]
						break;
					}
					
					if(token.kind == End){         //[begin]�qstatement [end]	
						token = nextToken();   //[begin]�qstatement�r[end] [?]
						return;
					}
					
					if(isStBeginKey(token)){       //[begin]�qstatement�r[begin}
						errorInsert(Semicolon);//[begin]�qstatement�r[?]
						break;
					}				
								       //[begin]�qstatement�r[?]
					errorDelete();		       //[begin]�qstatement�r
					token = nextToken();           //[begin]�qstatement�r[?]
				}
			}
						
		case End:                                //[begin]�qstatement�r[;]�A�A�A�qstatement�r[end]		
			return;

		case If:                                 //[if]			
			token = nextToken();             //[if] [?]
			condition();                     //[if]�qcondition�r
			token = checkGet(token, Then);   //[if]�qcondition�r[then]
			backP = genCodeV(jpc, 0);
			statement();                     //[if]�qcondition�r[then]�qstatement�r
			backPatch(backP);
			return;
		
		case While:                              //[while]			
			token  = nextToken();            //[while] [?]
			backP2 = nextCode();
			condition();                     //[while]�qcondition�r
			token = checkGet(token, Do);     //[while]�qcondition�r[do]
			backP = genCodeV(jpc, 0);
			statement();                     //[while]�qcondition�r[do]�qstatement�r
			backP = genCodeV(jpc, backP2);
			backPatch(backP);
			return;
						
		case Ret:                                //[return]
			token = nextToken();             //[return] [?]
			expression();                    //[return]�qexpression�r
			genCodeR();
			return;
			
		case Write:                              //[write]
			token = nextToken();             //[write] [?]
			expression();                    //[write]�qexpression�r
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
				
				
//���̍\����� p40
void expression()
{
	KeyId k;
	k = token.kind;
	                                    //[(+) or (-) or (�Ȃ�)]
	if (k==Plus || k==Minus){           //[(+) or (-)]
	                                  
		token = nextToken();        //[(+) or (-)] [?]
		term();                     //[(+) or (-)]�qterm�r
		if (k==Minus) genCodeO(neg);
	}else{                              //[�Ȃ�]
		term();                     //[�Ȃ�]�qterm�r
	}
		                            //[(+) or (-) or (�Ȃ�)]�qterm�r
	
	k = token.kind;
	
	while (k==Plus || k==Minus){        //[(+) or (-) or (�Ȃ�)]�qterm�r[(+) or (-)]
		
		token = nextToken();        //[(+) or (-) or (�Ȃ�)]�qterm�r[(+) or (-)] [?]
		term();                     //[(+) or (-) or (�Ȃ�)]�qterm�r[(+) or (-)]�qterm�r
			
		if (k==Minus)
			genCodeO(sub);
		else
			genCodeO(add);
			
		k = token.kind;
	}
}


//���̍��̍\�����
void term()
{
	KeyId k;
	
	factor();                   //�qfactor�r
	
	k = token.kind;
	
	while (k==Mult || k==Div){  //�qfactor�r[(*) or (/)]
		
		token = nextToken();//�qfactor�r[(*) or (/)] [?]
		
		factor();           //�qfactor�r[(*) or (/)]�qfactor�r
		
		if (k==Mult)
			genCodeO(mul);
		else
			genCodeO(div);
		
		k = token.kind;
	}
}


//���̈��q�̍\�����
void factor()
{
	int tIndex, i;
	KeyId k;
	
	if (token.kind==Id){

		tIndex = searchT(token.u.id, varId);
		setIdKind(k=kindT(tIndex));//LateX�p����
		
		switch (k) {
		
		//�ϐ������p�����[�^��
		case varId: case parId:            //[ident]
			genCodeT(lod, tIndex);
			token = nextToken(); 
			break;
		
		//�萔��
		case constId:                      //[number]
			genCodeV(lit, val(tIndex));
			token = nextToken(); 
			break;
		
		//�֐��Ăяo��
		case funcId:                                                    //[ident] [(]�qexpression�r[)] or [ident] [(]�qexpression�r[,] �A�A�A�qexpression�r[)] or [ident] [(] [)]
			
			token = nextToken();
			
			if (token.kind==Lparen){                                //[ident] [(]
				
				//i�͎������̌�
				i=0; 					
				
				token = nextToken();                            //[ident] [(] [?]
				
				if (token.kind != Rparen) {                     //[ident] [(] [?]
					
  					for (; ; ) {
						
						expression();                   //[ident] [(]�qexpression�r[?]
						i++;	
						
						if (token.kind==Comma){         //[ident] [(]�qexpression�r[,]
							token = nextToken();    //[ident] [(]�qexpression�r[,] [?]
							continue;
						}
							
						token = checkGet(token, Rparen);//[ident] [(]�qexpression�r[)]
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




	}else if (token.kind==Num){              //[�萔]
		
		genCodeV(lit, token.u.value);
		token = nextToken();
	
	}else if (token.kind==Lparen){           //[(]
	
		token = nextToken();
		expression();                    //[(]�qexpression
		token = checkGet(token, Rparen); //[(]�qexpression�r[)]
	}
		
	//���q�̌オ�܂����q�Ȃ�G���[
	switch (token.kind){
	case Id: case Num: case Lparen:
		errorMissingOp();
		factor();
	default:
		return;
	}	
}


//�������̍\�����
void condition()					
{
	KeyId k;
	if (token.kind==Odd){                  //[odd]
	
		token = nextToken();           //[odd] [?]
		expression();                  //[odd]�qexpression�r
		genCodeO(odd);
	
	}else{                                 //[?]
		expression();                  //�qexpression�r
		k = token.kind;
		
		switch(k){                     //�qexpression�r[(=) or (<>) or (<) or (>) or (<=) or (>=)]
		case Equal: case Lss: case Gtr:
		case NotEq: case LssEq: case GtrEq:
			break;
		default:
			errorType("rel-op");
			break;
		}
		
		token = nextToken();           //�qexpression�r[(=) or (<>) or (<) or (>) or (<=) or (>=)] [?]
		expression();                  //�qexpression�r[(=) or (<>) or (<) or (>) or (<=) or (>=)]�qexpression�r
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