#define MAXNAME 31


//予約語、演算子、区切り記号、識別子などを含むトークン名と 字句パターンで使用される英数字などを整数値で扱う
typedef enum keys{  
	
	Begin,  End,
	If,     Then,
	While,  Do,
	Ret,    Func,
	Var,    Const,  Odd,
	Write,  WriteLn,
	end_of_KeyWd,				  //ここまで予約語
	Plus,   Minus,
	Mult,   Div,
	Lparen, Rparen,
	Equal,  Lss,    Gtr,
	NotEq,  LssEq,  GtrEq,
	Comma,  Period, Semicolon,
	Assign,
	end_of_KeySym,         		  //ここまで演算子、区切り記号
	Id,     Num,    nul,
	end_of_Token,				  //ここまで識別子、定数値など
	letter, digit,  colon, others 
								  //字句パターンで使用される英数字など
} KeyId;


typedef struct token{
	KeyId kind;
	union {
		char id[MAXNAME];
		int value;
	} u;
} Token;


Token nextToken();
int openSource(char fileName[]);
void closeSource();
void initSource();
