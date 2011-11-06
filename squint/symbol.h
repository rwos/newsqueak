/*
 * Symbols
 */
enum{
	Namesize=16
};
typedef struct Value Value;
typedef struct Symbol Symbol;
struct Value{
	int	scope;		/* scope level, for error checking only */
	Value	*next;		/* next symbol with this name, of outer scope */
	Node	*type;
	char	isauto;
	char	stclass;
	union{
		long	l;		/* value of a number */
		long	off;		/* offset of an automatic */
/*		void	(**f)(Proc*);	// pointer of a prog var */
	}store;
	Symbol	*snext;		/* next symbol in this scope level */
};
struct Symbol{
	char	name[Namesize];
	int	token;
	Value	*val;
	Symbol	*hnext;		/* next in this hash chain */
};

enum{	/* val.stclass */
	SCconst=1,
	SCbltin=2
};

Symbol	*lookup(char *, int);
long	bltinval(char *, Node *);
void	genfreeauto(Symbol *);
Node	*idnode(Symbol *);
void	pushval(Symbol *, Node *);
