typedef struct Node Node;
typedef enum Ntype
{
	NNosuchnode,
	NArrayref,
	NArraycom,
	NBecome,
	NBegin,
	NBreak,
	NCall,
	NCase,
	NComplete,
	NContinue,
	NDecl,
	NDeclsc,
	NDefault,
	NElem,
	NExpr,
	NExprlist,
	NFormal,
	NID,
	NIf,
	NLabel,
	NList,
	NLoop,
	NLoopexpr,
	NMk,
	NNum,
	NProg,
	NRec,
	NResult,
	NSave,
	NScope,
	NSelect,
	NSmash,
	NString,
	NStructref,
	NSwitch,
	NType,
	NUnit,
	NVal
}Ntype;
typedef enum Ttype
{
	TNosuchtype,
	TAggr,
	TAny,
	TArray,
	TChan,
	TChar,
	TID,
	TInt,
	TProg,
	TStruct,
	TType,
	TUnit
}Ttype;
struct Node
{
	short	t;	/* Ntype */
	short	line;
	Node	*l;
	Node	*r;
	union{
		Node		*n;
		struct Symbol	*s;
		int		i;
		int		t;
		long		l;
		Store		*st;
	}o;
};

void	begrewrite(Node*);
int	compattype(Node*, Node*);
void	compile(Node*);
Node	*concat(Node*, Node*);
Node	*constants(Node*);
void	dclformals(Node*);
void	declare(Node*, int, int, int);
void	dump(Node*, int);
void	dupgen(Node*, int);
Node	*dupnode(Node*);
Node	*elemrewr(Node*, Node*, Node*, Ntype);
int	eqtype(Node*, Node*);
Node	*etypeof(Node*);
Node	*etypeoft(Node*);
void	freenode(Node*);
void	gen(Node*, int);
int	isinttype(Node*);
int	isptrtype(Node*);
int	length(Node*);
void	lgen(Node*);
int	compatmktype(Node*, Node*);
void	ndump(Node*);
Node	*new(Ntype, Node*, Node*, Node*);
Node	*newst(Ntype, Node*, Node*, Store *);
Node	*newi(Ntype, Node*, Node*, int);
Node	*newl(Ntype, Node*, Node*, long);
int	printable(Node*);
void	proglocals(Node*);
void	recrewrite(Node*);
void	type(Node*, int);
Node*	type_of(Node*);
Node*	typeoftid(Node*);
void	lerror(Node*, char *, ...);

