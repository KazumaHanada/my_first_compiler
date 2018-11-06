typedef enum keys{
	
	Begin,  End,
	If,     Then,
	While,  Do,
	Ret,    Func,
	Var,    Const,  Odd,
	Write,  WriteLn,
	end_of_KeyWd,
	Plus,   Minus,
	Mult,   Div,
	Lparen, Rparen,
	Equal,  Lss,    Gtr,
	NotEq,  LssEq,  GtrEq,
	Comma,  Period, Semicolon,
	Assign,
	end_of_KeySym,
	Id,     Num,    nul,
	end_of_Token,
	letter, digit,  colon, others
	
} KeyId;


int openSource(char fileName[]);
void closeSource();
void initSource();