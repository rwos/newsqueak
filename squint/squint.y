%{
#include "store.h"
#include "node.h"
#include "symbol.h"
#include <u.h>
#include <libc.h>
#include <bio.h>
#include "fns.h"

#define	yyerror	error
#define	Z	(Node *)0

extern Node	unittype;
extern int	iflag;
%}

%union{
	Node	*n;
	Symbol	*s;
	Store	*st;
	long	l;
	int	i;
}

%token	<s>	ID
%token	<l>	NUM
%token	<st>	STRING
%token	<s>	UNIT INT CHAR
%token	<s>	CONST BUILTIN ARRAY OF CHAN STRUCT PROG TYPE
%token	<s>	IF ELSE FOR WHILE DO SELECT SWITCH CASE DEFAULT PRINT REC
%token	<s>	BEGIN BECOME RESULT
%token	<s>	MK LEN REF DEF
%token	<s>	BREAK CONTINUE

%type	<n>	arraysize basictype call caselist cdecl commcaselist
%type	<n>	commexpr commlabel compstmt decllist
%type	<n>	elemlist execstmt execstmtlist expr1 expr exprlist
%type	<n>	forexpr formals idlist initexpr label lval
%type	<n>	ncdecl oftype prog program program1 save sdecl stmt stmtlist
%type	<n>	type typevarlist
%type	<i>	stclass stclass1

%left		VAL
%right		'='
%left		ANDAND OROR CAT DEL
%left		'&' '|' '^'
%left		EQ NE LE '<' GE '>'
%left		'+' '-' LSH RSH
%left		'*' '/' '%'
%right		'~' '!' LEN REF DEF UMINUS ARROW SND RCV DEC INC
%left		'[' '.'
%%

program:
	{
		$$=Z;
	}
|	program1

program1:
	stmt
	{
		$$=$1;
    Compile:
		ndump($$);
		istart();
		if($$) switch($$->t){
		int pable;
		case NDecl:
		case NDeclsc:
			declare($$, 0, 1, 1);	/* BUG: freenode won't get called on error! */
			freenode($$);
			break;
		case NRec:
			recrewrite($$);
			declare($$->l, 0, 1, 1);	/* BUG: freenode won't get called on error! */
			compile($$->r);
			if(iflag)
				idump();
			if(executeallowed)
				execute();
			break;
		default:
			type($$, 1);
			pable=printable($$);
			if(pable)
				$$=newi(NExpr, $$, Z, PRINT);
			compile($$);
			if(iflag)
				idump();
			if(executeallowed){
				execute();
				if(pable)
					write(1, "\n", 1);
			}
		}
	}
|	program1 stmt
	{
		$$=$2;
		goto Compile;
	}

stmtlist:
	stmt
|	stmtlist stmt
	{
		$$=new(NList, $1, $2, Z);
	}

execstmtlist:
	execstmt
|	execstmtlist execstmt
	{
		$$=new(NList, $1, $2, Z);
	}

sdecl:
	ncdecl
|	stclass ncdecl
	{
		$$=newi(NDeclsc, $2, Z, $1);
	}
|	TYPE idlist ':' type
	{
		$$=newi(NType, $4, Z, TType);
		$$=new(NDecl, $2, $$, Z);
	}
|	REC sdecl
	{
		$$=new(NRec, $2, Z, Z);
	}

cdecl:
	REC '{' decllist '}'
	{
		$$=new(NRec, $3, Z, Z);
	}

decllist:
	sdecl ';'
|	cdecl
|	decllist sdecl ';'
	{
		$$=new(NList, $1, $2, Z);
	}
|	decllist cdecl
	{
		$$=new(NList, $1, $2, Z);
	}

ncdecl:
	idlist ':' type
	{
		$$=new(NDecl, $1, $3, Z);
	}
|	idlist ':' '=' initexpr
	{
		$$=new(NDecl, $1, Z, $4);
	}
|	idlist ':' type '=' initexpr
	{
		$$=new(NDecl, $1, $3, $5);
	}
|	idlist ':' type '=' MK '(' ')'
	{
		$$=new(NDecl, $1, $3, new(NMk, dupnode($3), Z, Z));
	}

stclass:
	stclass1
|	stclass stclass1
	{
		$$=$1|$2;
	}

stclass1:
	CONST
	{
		$$=SCconst;
	}
|	BUILTIN
	{
		$$=SCbltin;
	}
idlist:
	ID
	{
		$$=idnode($1);
	}
|	idlist ',' ID
	{
		$$=idnode($3);
		$$=new(NList, $1, $$, Z);
	}

type:
	basictype
|	ARRAY arraysize OF type
	{
		$$=newi(NType, $2, $4, TArray);
	}
|	CHAN OF type
	{
		$$=newi(NType, Z, $3, TChan);
	}
|	STRUCT OF '{' elemlist '}'
	{
		$$=newi(NType, $4, Z, TStruct);
	}
|	PROG '(' formals ')' oftype
	{
		$$=newi(NType, $3, $5, TProg);
	}

basictype:
	UNIT
	{
		$$=newi(NType, Z, Z, TUnit);
	}
|	INT
	{
		$$=newi(NType, Z, Z, TInt);
	}
|	CHAR
	{
		$$=newi(NType, Z, Z, TChar);
	}
|	ID
	{
		$$=idnode($1);
		$$=newi(NType, $$, Z, TID);
	}

arraysize:
	{
		$$=Z;
	}
|	'[' expr ']'
	{
		$$=$2;
	}

formals:
	{
		$$=Z;
	}
|	typevarlist

typevarlist:
	idlist ':' type
	{
		$$=elemrewr($1, $3, Z, NFormal);
	}
|	typevarlist ',' idlist ':' type
	{
		$$=elemrewr($3, $5, $1, NFormal);
	}

elemlist:
	idlist ':' type ';'
	{
		$$=elemrewr($1, $3, Z, NElem);
	}
|	elemlist idlist ':' type ';'
	{
		$$=elemrewr($2, $4, $1, NElem);
	}

expr:
	NUM
	{
		$$=newl(NNum, Z, Z, $1);
	}
|	STRING
	{
		$$=newst(NString, Z, Z, $1);
	}
|	UNIT
	{
		$$=new(NUnit, Z, Z, Z);
	}
|	PRINT '(' exprlist ')'
	{
		$$=newi(NExpr, $3, Z, PRINT);
	}
|	expr1
|	expr '+' expr
	{
		$$=newi(NExpr, $1, $3, '+');
	}
|	expr '-' expr
	{
		$$=newi(NExpr, $1, $3, '-');
	}
|	expr '*' expr
	{
		$$=newi(NExpr, $1, $3, '*');
	}
|	expr '/' expr
	{
		$$=newi(NExpr, $1, $3, '/');
	}
|	expr '%' expr
	{
		$$=newi(NExpr, $1, $3, '%');
	}
|	expr '&' expr
	{
		$$=newi(NExpr, $1, $3, '&');
	}
|	expr '|' expr
	{
		$$=newi(NExpr, $1, $3, '|');
	}
|	expr '^' expr
	{
		$$=newi(NExpr, $1, $3, '^');
	}
|	expr ANDAND expr
	{
		$$=newi(NExpr, $1, $3, ANDAND);
	}
|	expr OROR expr
	{
		$$=newi(NExpr, $1, $3, OROR);
	}
|	expr CAT expr
	{
		$$=newi(NExpr, $1, $3, CAT);
	}
|	expr DEL expr
	{
		$$=newi(NExpr, $1, $3, DEL);
	}
|	expr EQ expr
	{
		$$=newi(NExpr, $1, $3, EQ);
	}
|	expr NE expr
	{
		$$=newi(NExpr, $1, $3, NE);
	}
|	expr '<' expr
	{
		$$=newi(NExpr, $1, $3, '<');
	}
|	expr '>' expr
	{
		$$=newi(NExpr, $1, $3, '>');
	}
|	expr LE expr
	{
		$$=newi(NExpr, $1, $3, LE);
	}
|	expr GE expr
	{
		$$=newi(NExpr, $1, $3, GE);
	}
|	expr LSH expr
	{
		$$=newi(NExpr, $1, $3, LSH);
	}
|	expr RSH expr
	{
		$$=newi(NExpr, $1, $3, RSH);
	}
|	ARROW expr
	{
		$$=newi(NExpr, $2, Z, RCV);
	}
|	LEN expr
	{
		$$=newi(NExpr, $2, Z, LEN);
	}
|	REF expr
	{
		$$=newi(NExpr, $2, Z, REF);
	}
|	DEF expr
	{
		$$=newi(NExpr, $2, Z, DEF);
	}
|	DEC expr
	{
		$$=newi(NExpr, $2, Z, DEC);
	}
|	expr INC
	{
		$$=newi(NExpr, $1, Z, INC);
	}
|	'-' expr %prec UMINUS
	{
		$$=newi(NExpr, $2, Z, UMINUS);
	}
|	'!' expr
	{
		$$=newi(NExpr, $2, Z, '!');
	}
|	'~' expr
	{
		$$=newi(NExpr, $2, Z, '~');
	}
|	lval '=' expr
	{
		$$=newi(NExpr, $1, $3, '=');
	}
|	lval '=' MK '(' ')'
	{
		$$=new(NMk, Z, Z, Z);
		$$=newi(NExpr, $1, $$, '=');
	}
|	lval '=' '{' exprlist '}'
	{
		$$=new(NMk, dupnode(etypeof($4)), $4, Z);
		$$=newi(NExpr, $1, $$, '=');
	}
|	expr ARROW '=' expr
	{
		$$=newi(NExpr, $1, $4, SND);
	}
|	expr ARROW
	{
		$$=new(NUnit, Z, Z, Z);
		$$=newi(NExpr, $1, $$, SND);
	}
|	call
|	MK '(' type ')'
	{
		$$=new(NMk, $3, Z, Z);
	}
|	MK '(' type '=' initexpr ')'
	{
		$$=new(NMk, $3, $5, Z);
	}
|	VAL compstmt
	{
		$$=new(NVal, $2, Z, Z);
	}

lval:
	ID
	{
		$$=idnode($1);
	}
|	expr '[' expr ']'
	{
		$$=new(NArrayref, $1, $3, Z);
	}
|	expr '.' ID
	{
		$$=idnode($3);
		$$=new(NStructref, $1, $$, Z);
	}

call:
	expr1 '(' ')'
	{
		$$=new(NCall, $1, Z, Z);
	}
|	expr1 '(' exprlist ')'
	{
		$$=new(NCall, $1, $3, Z);
	}

expr1:
	lval
|	prog
|	'(' expr ')'
	{
		$$=$2;
	}

initexpr:
	expr
|	'{' exprlist '}'
	{
		$$=new(NMk, dupnode(etypeof($2)), $2, Z);
	}

exprlist:
	expr
|	'{' exprlist '}'
	{
		$$=new(NMk, dupnode(etypeof($2)), $2, Z);
	}
|	exprlist ',' expr
	{
		$$=new(NExprlist, $1, $3, Z);
	}
|	exprlist ',' '{' exprlist '}'
	{
		$$=new(NMk, dupnode(etypeof($4)), $4, Z);
		$$=new(NExprlist, $1, $$, Z);
	}

prog:
	PROG '(' formals ')' oftype compstmt
	{
		$$=newi(NType, $3, $5, TProg);
		$$=new(NProg, $$, $6, Z);
	}

oftype:
	{
		$$=dupnode(&unittype);
	}
|	OF type
	{
		$$=$2;
	}

stmt:
	execstmt
|	sdecl ';'
|	cdecl

execstmt:
	';'
	{
		$$=Z;
	}
|	expr ';'
	{
		$$=$1;
		if($$->t==NMk)
			$$=new(NComplete, $$, Z, Z);
	}
|	BEGIN call ';'
	{
		$$=new(NBegin, $2, Z, Z);
	}
|	BECOME expr ';'
	{
		$$=new(NBecome, $2, Z, Z);
	}
|	BECOME '{' exprlist '}' ';'
	{
		$$=new(NMk, dupnode(etypeof($3)), $3, Z);
		$$=new(NBecome, $$, Z, Z);
	}
|	RESULT expr ';'
	{
		$$=new(NResult, $2, Z, Z);
	}
|	BREAK ';'
	{
		$$=new(NBreak, Z, Z, Z);
	}
|	CONTINUE ';'
	{
		$$=new(NContinue, Z, Z, Z);
	}
|	DO execstmt WHILE '(' expr ')' ';'
	{
		$$=new(NLoopexpr, Z, $5, Z);
		$$=newi(NLoop, $2, $$, 1);
	}
|	compstmt
|	IF '(' expr ')' execstmt
	{
		$$=new(NIf, $5, Z, $3);
	}
|	IF '(' expr ')' execstmt ELSE execstmt
	{
		$$=new(NIf, $5, $7, $3);
	}
|	WHILE '(' expr ')' execstmt
	{
		$$=new(NLoopexpr, Z, $3, Z);
		$$=newi(NLoop, $5, $$, 0);
	}
|	FOR '(' forexpr ';' forexpr ';' forexpr ')' execstmt
	{
		$$=new(NLoopexpr, $3, $5, $7);
		$$=newi(NLoop, $9, $$, 0);
	}
|	SWITCH '(' expr ')' '{' caselist '}'
	{
		$$=new(NSwitch, $6, $3, Z);
	}
|	SELECT '{' commcaselist '}'
	{
		$$=new(NSelect, $3, Z, Z);
	}

compstmt:
	'{' '}'
	{
		$$=Z;
	}
|	'{' stmtlist '}'
	{
		$$=new(NScope, $2, Z, Z);
	}

forexpr:
	{
		$$=Z;
	}
|	expr

caselist:
	label execstmtlist
	{
		$$=new(NCase, $1, $2, Z);
	}
|	caselist label execstmtlist
	{
		$$=new(NCase, $2, $3, Z);
		$$=new(NList, $1, $$, Z);
	}

label:
	CASE expr ':'
	{
		$$=new(NLabel, $2, Z, Z);
	}
|	DEFAULT ':'
	{
		$$=new(NDefault, Z, Z, Z);
	}
|	label CASE expr ':'
	{
		$$=new(NLabel, $3, Z, Z);
		$$=new(NList, $1, $$, Z);
	}

commcaselist:
	commlabel execstmtlist
	{
		$$=new(NCase, $1, $2, Z);
	}
|	commcaselist commlabel execstmtlist
	{
		$$=new(NCase, $2, $3, Z);
		$$=new(NList, $1, $$, Z);
	}

commlabel:
	CASE commexpr ':'
	{
		$$=new(NLabel, $2, Z, Z);
	}
|	commlabel CASE commexpr ':'
	{
		$$=new(NLabel, $3, Z, Z);
		$$=new(NList, $1, $$, Z);
	}

commexpr:
	ARROW expr
	{
		$$=newi(NExpr, $2, Z, RCV);
	}
|	ARROW expr '[' save ']'
	{
		$$=new(NArraycom, $2, $4, Z);
		$$=newi(NExpr, $$, Z, RCV);
	}
|	lval '=' ARROW expr
	{
		$$=newi(NExpr, $4, Z, RCV);
		$$=newi(NExpr, $1, $$, '=');
	}
|	lval '=' ARROW expr '[' save ']'
	{
		$$=new(NArraycom, $4, $6, Z);
		$$=newi(NExpr, $$, Z, RCV);
		$$=newi(NExpr, $1, $$, '=');
	}
|	expr ARROW '=' expr
	{
		$$=newi(NExpr, $1, $4, SND);
	}
|	expr '[' save ']' ARROW '=' expr
	{
		$$=new(NArraycom, $1, $3, Z);
		$$=newi(NExpr, $$, $7, SND);
	}

save:
	{
		$$=Z;
	}
|	lval '='
	{
		$$=$1;
	}
