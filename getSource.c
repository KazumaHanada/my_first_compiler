#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"getSource.h"


#define MAXLINE 120
#define MAXERROR 30


static FILE *fpi;
static FILE *fptex;//LateX�����p


static char line[MAXLINE]; //1�s���̓��̓o�b�t�@
static int  lineIndex;     //��̃o�b�t�@�̃C���f�b�N�X
static char ch;            //nextChar()�œǂݎ����1�������i�[����


static Token cToken;//LateX�����p
static KindT idKind;//LateX�����p
static int spaces;  //LateX�����p
static int CR;      //LateX�����p
static int printed; //LateX�����p


static int errorNo = 0;
static char nextChar();
static void printcToken();//LateX�����p


struct keyWd {                   //�\���
	char *word;
	KeyId keyId;
};

static struct keyWd KeyWdT[] = { //�\���̎���Ƃ���ɑΉ�����g�[�N����
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
	
	//�Ƃ肠�����A�S�Ă̔z���other����
	for(i=0; i<256; i++)
		charClassT[i] = others;
	
	//ASC�U�R�[�h�Ɋ�Â����Y�����g��
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
	printed = 1; //LateX�����p
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
	//�ŏ��̓ǂݍ���
	if(lineIndex == -1){
		
		//1�s�P�ʂœǂݍ���
		if(fgets(line, MAXLINE,fpi) != NULL){
			
			lineIndex = 0;
			
		}else{
			
			errorF("end of file\n");
			exit(1);
		}
	}
	
	
	//�擾�s����1�����Ƃ肾���A�C���f�b�N�X�����ɐi�߂�
	char ch = line[lineIndex++];
	
	
	if(ch == '\n'){
		
		//�s���ɂ�����A�C���f�b�N�X��-1�ɖ߂�
		lineIndex = -1;
		return '\n';
	}

	return ch;
}


Token nextToken()
{
	int i = 0;                    //����̕������J�E���g�Ɏg��
	int num;                      //���傪�萔�l�̏ꍇ�Ɏg�p����
	KeyId cc;                     //����p�^�[���Ɋ܂܂�镶��(�����l�ŕ\��)���i�[����
	Token temp;                   //�߂�l�Ƃ��Đ�������g�[�N��
	char ident[MAXNAME];          //������͗p�̃o�b�t�@
	memset(ident, '\0', MAXNAME); //ident��\0�ł��߂� ������NULL�����Ȃ�
	printcToken();
	spaces = 0;                   //�󔒃J�E���g�Ɏg��
	CR = 0;                       //���s�J�E���g�Ɏg��
	
	
	//���̃g�[�N���܂ł̋󔒂���s���J�E���g
	while(1){
		
		if(ch == ' ' ){
			
			spaces++;
			
		}else if(ch == '\t'){
			
			spaces+= 5;
		
		//nextChar�����߂ČĂ񂾏ꍇ(initSource��\n�ɏ���������Ă��邩��) or nextChar�ōs���ɒB�����ꍇ
		}else if(ch == '\n'){
			
			printf("\n");
			spaces = 0;
			CR++;
		
		//�󔒂ł����s�ł��Ȃ��ꍇ�̓��[�v�𔲂���
		}else{
			
			break;
		}
		
		ch = nextChar();
		printf("%c", ch);//�o�͊m�F�p
	}
	
	
	//����p�^�[���𒲂ׂ�
	switch (cc = charClassT[ch]){
		
		
		//�ŏ��̕�����letter(�p��)
		case letter:  
			
			do{
				if(i < MAXNAME){
					
					ident[i] = ch;
					i++;
					
					ch = nextChar();
					printf("%c", ch);//�o�͊m�F�p
				}
			
			}while( charClassT[ch] == letter || charClassT[ch] == digit );
		
			//����̍ő咷�𒴂��Ă�����G���[
			if(i >= MAXNAME){
				errorMessage("too long");
				i = MAXNAME - 1;
			}
			
			//���[�v���o���Ƃ��̍Ō�̕����́A���̎���̃p�^�[���O�Ȃ̂ŁA�k�������łԂ�(ch�ɂ͎c���Ă���)
			ident[i] = '\0';
			
			//�p�^�[���Ɉ�v�������傪�\���Ɋ܂܂�Ă��Ȃ������ׂ�
			for(i=0; i<end_of_KeyWd; i++){
			
				//���傪�\���Ɋ܂܂�Ă�����
				if(strcmp(ident, KeyWdT[i].word) == 0){	
					
					//���̎���ɑΉ�����g�[�N�����͂��̗\���ƂȂ�
					temp.kind = KeyWdT[i].keyId;
					strcpy(temp.u.id, KeyWdT[i].word);//�o�͊m�F�p
					
					cToken = temp;//LateX�����p
					printed = 0;  //LateX�����p
					
					return temp;
				}
			}
			
			//�\���Ɋ܂܂�Ă��Ȃ��Ƃ��A���̎���ɑΉ�����g�[�N�����͎��ʎq�ƂȂ�
			temp.kind = Id;
			strcpy(temp.u.id , ident);
			
			break;
		
		
		//�ŏ��̕�����digit(����)
		case digit:  
			
			num = 0;
			do{
				//���̕����������������ꍇ�ɔ����āA�����̌J��グ�������s��
				num = 10*num + (ch-'0');
				i++;
				
				ch = nextChar();
				printf("%c", ch);//�l�p
			
			}while(charClassT[ch] == digit);
		
			//����̍ő咷�𒴂��Ă�����G���[
			if(i >= MAXNAME){
				errorMessage("too long");
			}
			
			//����ɑΉ�����g�[�N�����͒萔�l�ƂȂ�
			temp.kind = Num;
		    temp.u.value = num;
			
			break;
		
		
		//�ŏ��̕�����colon(.)
		case colon:
		
			if( (ch = nextChar()) == '='){
				
				printf("%c", ch);//�o�͊m�F�p
				//����ɑΉ�����g�[�N������Assign�ƂȂ�
				temp.kind = Assign;
				strcpy(temp.u.id, ":=");//�o�͊m�F�p
				
				ch = nextChar();
				printf("%c", ch);//�o�͊m�F�p
				
				break;
			
			}else{
				
				//����ɑΉ�����g�[�N������nul�ƂȂ�
				temp.kind = nul;
				strcpy(temp.u.id, "nul");//�o�͊m�F�p
				
				break;
			}
		
		
		//�ŏ��̕�����Lss(<)
		case Lss:
		
			if( (ch = nextChar()) == '='){
				
				printf("%c", ch);//�o�͊m�F�p
				//����ɑΉ�����g�[�N������LssEq�ƂȂ�
				temp.kind = LssEq;
				strcpy(temp.u.id, "<=");//�o�͊m�F�p
				
				ch = nextChar();
				printf("%c", ch);//�o�͊m�F�p
				
				break;
				
			}else if(ch == '>'){
				
				printf("%c", ch); //�o�͊m�F�p
				//����ɑΉ�����g�[�N������NotEq�ƂȂ�
				temp.kind = NotEq;
				strcpy(temp.u.id, "<>");//�o�͊m�F�p
				
				ch = nextChar();
				printf("%c", ch);//�o�͊m�F�p
				
				break;
			
			}else{
				
				printf("%c", ch);//�o�͊m�F�p
				//����ɑΉ�����g�[�N������Lss�ƂȂ�
				temp.kind = Lss;
				strcpy(temp.u.id, "<");
				
				break;
			}
		
		
		//�ŏ��̕�����Gtr(>)
		case Gtr:
		
			if((ch = nextChar()) == '=' ){
				
				printf("%c", ch);//�o�͊m�F�p
				//����ɑΉ�����g�[�N������GtrEq�ƂȂ�
				temp.kind = GtrEq;
				strcpy(temp.u.id, ">=");
				
				ch = nextChar();
				printf("%c", ch);//�o�͊m�F�p
				
				break;
				
			}else{
				
				printf("%c", ch);      //�o�͊m�F�p
				//����ɑΉ�����g�[�N������Gtr�ƂȂ�
				temp.kind = Gtr;
				strcpy(temp.u.id, ">");//�o�͊m�F�p
				
				break;
			}
		
		
		default:
		
			//����p�^�[���ɓ��Ă͂܂�Ȃ��ꍇ�A�g�[�N������other�ƂȂ�
			temp.kind = cc;
			strcpy(temp.u.id, "other");//�o�͊m�F�p
			ch = nextChar();
			printf("%c", ch);          //�o�͊m�F�p
			
			break;
	}
	
	
	cToken =temp;//LateX�����p
	printed = 0; //LateX�����p
	
	
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