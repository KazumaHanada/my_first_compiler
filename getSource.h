#include "table.h"


#define MAXNAME 31


//�\���A���Z�q�A��؂�L���A���ʎq�A�萔�l��\���g�[�N������ ����p�^�[���Ŏg�p�����p�����Ȃǂ𐮐��l�ň���
typedef enum keys{  
	
	Begin,/*0*/  End,/*1*/
	If,     Then,
	While,  Do,
	Ret,    Func,
	Var,    Const,  Odd,
	Write,  WriteLn,
	end_of_KeyWd,                 //�����܂ŗ\���
	Plus,   Minus,
	Mult,   Div,
	Lparen, Rparen,
	Equal,  Lss,    Gtr,
	NotEq,  LssEq,  GtrEq,
	Comma,  Period, Semicolon,
	Assign,
	end_of_KeySym,               //�����܂ŉ��Z�q�A��؂�L��
	Id,     Num,    nul,
	end_of_Token,                //�����܂Ŏ��ʎq�A�萔�l�Ȃ�
	letter, digit,  colon, others 
                                 //����p�^�[���Ŏg�p�����p�����Ȃ�
} KeyId;


typedef struct token{
	KeyId kind;
	union {
		char id[MAXNAME];
		int value;
	} u;
} Token;


Token nextToken();
Token checkGet(Token t, KeyId k);
int openSource(char fileName[]);
void closeSource();
void initSource();
void errorType(char *m);
void errorInsert(KeyId k);
void errorMissingId();
void errorMissingOp();
void errorDelete();
void errorMessage(char *m);
void errorF(char *m);
void setIdKind(KindT k);