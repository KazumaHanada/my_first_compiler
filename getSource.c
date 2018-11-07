#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"getSource.h"


#define MAXLINE 120
#define MAXERROR 30


static FILE *fpi;
static FILE *fptex;//LateX処理用


static char line[MAXLINE]; //1行分の入力バッファ
static int  lineIndex;     //上のバッファのインデックス
static char ch;            //nextChar()で読み取った1文字を格納する


static Token cToken;//LateX処理用
static KindT idKind;//LateX処理用
static int spaces;  //LateX処理用
static int CR;      //LateX処理用
static int printed; //LateX処理用


static int errorNo = 0;
static char nextChar();
static void printcToken();//LateX処理用


struct keyWd {                   //予約語
	char *word;
	KeyId keyId;
};

static struct keyWd KeyWdT[] = { //予約語の字句とそれに対応するトークン名
	{"begin", Begin},
	{"end", End},
	{"if", If},
	{"then", Then},
	{"while", While},
	{"do", Do},
	{"return", Ret},
	{"function", Func},
	{"var", Var},
	{"const", Const},
	{"odd", Odd},
	{"write", Write},
	{"writeln",WriteLn},
	{"$dummy1",end_of_KeyWd},
							
	{"+", Plus},
	{"-", Minus},
	{"*", Mult},
	{"/", Div},
	{"(", Lparen},
	{")", Rparen},
	{"=", Equal},
	{"<", Lss},
	{">", Gtr},
	{"<>", NotEq},
	{"<=", LssEq},
	{">=", GtrEq},
	{",", Comma},
	{".", Period},
	{";", Semicolon},
	{":=", Assign},
	{"$dummy2",end_of_KeySym}
};


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
	printed = 1; //LateX処理用
	initCharClassT();
	
	/*
	fprintf(fptex,"\\documentstyle[12pt]{article}\n");   
	fprintf(fptex,"\\begin{document}\n");
	fprintf(fptex,"\\fboxsep=0pt\n");
	fprintf(fptex,"\\def\\insert#1{$\\fbox{#1}$}\n");
	fprintf(fptex,"\\def\\delete#1{$\\fboxrule=.5mm\\fbox{#1}$}\n");
	fprintf(fptex,"\\rm\n");
	*/
}


void errorNoCheck()
{
	if (errorNo++ > MAXERROR){
		//fprintf(fptex, "too many errors\n</PRE>\n</BODY>\n</HTML>\n");
		//fprintf(fptex, "too many errors\n\\end{document}\n"); 
		printf("abort compilation\n");	
		exit (1);
	}
}


void errorMessage(char *m)
{
	//fprintf(fptex, "$^{%s}$", m);
	errorNoCheck();
}


void errorF(char *m)
{
	errorMessage(m);
	//fprintf(fptex, "fatal errors\n\\end{document}\n");
	if (errorNo) printf("total %d errors\n", errorNo);
	printf("abort compilation\n");	
	exit (1);
}


char nextChar()
{
	//最初の読み込み
	if(lineIndex == -1){
		
		//1行単位で読み込む
		if(fgets(line, MAXLINE,fpi) != NULL){
			
			lineIndex = 0;
			
		}else{
			
			errorF("end of file\n");
			exit(1);
		}
	}
	
	
	//取得行から1文字とりだし、インデックスを次に進める
	char ch = line[lineIndex++];
	
	
	if(ch == '\n'){
		
		//行末にきたら、インデックスを-1に戻す
		lineIndex = -1;
		return '\n';
	}

	return ch;
}


Token nextToken()
{
	int i = 0;                    //字句の文字数カウントに使う
	int num;                      //字句が定数値の場合に使用する
	KeyId cc;                     //字句パターンに含まれる文字(整数値で表現)を格納する
	Token temp;                   //戻り値として生成するトークン
	char ident[MAXNAME];          //字句入力用のバッファ
	memset(ident, '\0', MAXNAME); //identを\0でうめる 末尾にNULLをつけない
	printcToken();
	spaces = 0;                   //空白カウントに使う
	CR = 0;                       //改行カウントに使う
	
	
	//次のトークンまでの空白や改行をカウント
	while(1){
		
		if(ch == ' ' ){
			
			spaces++;
			
		}else if(ch == '\t'){
			
			spaces+= 5;
		
		//nextCharを初めて呼んだ場合(initSourceで\nに初期化されているから) or nextCharで行末に達した場合
		}else if(ch == '\n'){
			
			printf("\n");
			spaces = 0;
			CR++;
		
		//空白でも改行でもない場合はループを抜ける
		}else{
			
			break;
		}
		
		ch = nextChar();
		printf("%c", ch);//出力確認用
	}
	
	
	//字句パターンを調べる
	switch (cc = charClassT[ch]){
		
		
		//最初の文字がletter(英字)
		case letter:  
			
			do{
				if(i < MAXNAME){
					
					ident[i] = ch;
					i++;
					
					ch = nextChar();
					printf("%c", ch);//出力確認用
				}
			
			}while( charClassT[ch] == letter || charClassT[ch] == digit );
		
			//字句の最大長を超えていたらエラー
			if(i >= MAXNAME){
				errorMessage("too long");
				i = MAXNAME - 1;
			}
			
			//ループを出たときの最後の文字は、この字句のパターン外なので、ヌル文字でつぶす(chには残っている)
			ident[i] = '\0';
			
			//パターンに一致した字句が予約語に含まれていないか調べる
			for(i=0; i<end_of_KeyWd; i++){
			
				//字句が予約語に含まれていたら
				if(strcmp(ident, KeyWdT[i].word) == 0){	
					
					//その字句に対応するトークン名はその予約語となる
					temp.kind = KeyWdT[i].keyId;
					strcpy(temp.u.id, KeyWdT[i].word);//出力確認用
					
					cToken = temp;//LateX処理用
					printed = 0;  //LateX処理用
					
					return temp;
				}
			}
			
			//予約語に含まれていないとき、その字句に対応するトークン名は識別子となる
			temp.kind = Id;
			strcpy(temp.u.id , ident);
			
			break;
		
		
		//最初の文字がdigit(数字)
		case digit:  
			
			num = 0;
			do{
				//次の文字が数字だった場合に備えて、数字の繰り上げ処理を行う
				num = 10*num + (ch-'0');
				i++;
				
				ch = nextChar();
				printf("%c", ch);//個人用
			
			}while(charClassT[ch] == digit);
		
			//字句の最大長を超えていたらエラー
			if(i >= MAXNAME){
				errorMessage("too long");
			}
			
			//字句に対応するトークン名は定数値となる
			temp.kind = Num;
		    temp.u.value = num;
			
			break;
		
		
		//最初の文字がcolon(.)
		case colon:
		
			if( (ch = nextChar()) == '='){
				
				printf("%c", ch);//出力確認用
				//字句に対応するトークン名はAssignとなる
				temp.kind = Assign;
				strcpy(temp.u.id, ":=");//出力確認用
				
				ch = nextChar();
				printf("%c", ch);//出力確認用
				
				break;
			
			}else{
				
				//字句に対応するトークン名はnulとなる
				temp.kind = nul;
				strcpy(temp.u.id, "nul");//出力確認用
				
				break;
			}
		
		
		//最初の文字がLss(<)
		case Lss:
		
			if( (ch = nextChar()) == '='){
				
				printf("%c", ch);//出力確認用
				//字句に対応するトークン名はLssEqとなる
				temp.kind = LssEq;
				strcpy(temp.u.id, "<=");//出力確認用
				
				ch = nextChar();
				printf("%c", ch);//出力確認用
				
				break;
				
			}else if(ch == '>'){
				
				printf("%c", ch); //出力確認用
				//字句に対応するトークン名はNotEqとなる
				temp.kind = NotEq;
				strcpy(temp.u.id, "<>");//出力確認用
				
				ch = nextChar();
				printf("%c", ch);//出力確認用
				
				break;
			
			}else{
				
				printf("%c", ch);//出力確認用
				//字句に対応するトークン名はLssとなる
				temp.kind = Lss;
				strcpy(temp.u.id, "<");
				
				break;
			}
		
		
		//最初の文字がGtr(>)
		case Gtr:
		
			if((ch = nextChar()) == '=' ){
				
				printf("%c", ch);//出力確認用
				//字句に対応するトークン名はGtrEqとなる
				temp.kind = GtrEq;
				strcpy(temp.u.id, ">=");
				
				ch = nextChar();
				printf("%c", ch);//出力確認用
				
				break;
				
			}else{
				
				printf("%c", ch);      //出力確認用
				//字句に対応するトークン名はGtrとなる
				temp.kind = Gtr;
				strcpy(temp.u.id, ">");//出力確認用
				
				break;
			}
		
		
		default:
		
			//字句パターンに当てはまらない場合、トークン名はotherとなる
			temp.kind = cc;
			strcpy(temp.u.id, "other");//出力確認用
			ch = nextChar();
			printf("%c", ch);          //出力確認用
			
			break;
	}
	
	
	cToken =temp;//LateX処理用
	printed = 0; //LateX処理用
	
	
	return temp;
}


static void printSpaces()
{
	while (CR-- > 0){
		//fprintf(fptex, "\\ \\par\n");
	}
	
	while (spaces-- > 0){
		//fprintf(fptex, "\\ ");
	}
	
	CR = 0; spaces = 0;
}


static void printcToken()
{
	int i=(int)cToken.kind;
	if (printed){
		printed = 0; return;
	}
	printed = 1;
	printSpaces();
	if (i < end_of_KeyWd){
		//fprintf(fptex, "{\\bf %s}", KeyWdT[i].word);
	}else if (i < end_of_KeySym){					
		//fprintf(fptex, "$%s$", KeyWdT[i].word);
	}else if (i==(int)Id){
		switch (idKind) {
		case varId: 
			//fprintf(fptex, "%s", cToken.u.id); 
			return;
		case parId: 
			//fprintf(fptex, "{\\sl %s}", cToken.u.id); 
			return;
		case funcId: 
			//fprintf(fptex, "{\\it %s}", cToken.u.id); 
			return;
		case constId: 
			//fprintf(fptex, "{\\sf %s}", cToken.u.id); 
			return;
		}
	}else if (i==(int)Num){
		//fprintf(fptex, "%d", cToken.u.value);
	}
}