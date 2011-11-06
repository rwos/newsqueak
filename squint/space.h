enum{
	RData,		/* from newsqueak to world */
	RCtl,		/* from newsqueak to interface */
	WData,		/* from world to newsqueak */
	WCtl,		/* from interface to newsqueak */
	NQ
};
typedef struct Space Space;
typedef struct Bind Bind;
typedef struct Pend Pend;
typedef struct Queue Queue;
struct Space{
	int	busy;
	int	seq;
	Bind	*bind;
};
struct Queue{
	Proc	*proc;
	Store	*buf;	/* RData and RCtl only */
	long	off;	/* RData only */
	int	synch;	/* flag: waiting for a syscall to come in */
	Pend	*pend;
};
struct Bind{
	Bind	*next;
	Space	*space;
	short	seq;
	short	nclient;	/* number of clients holding it (includes unopened) */
	short	nopen;		/* number of clients with it open */
	short	removed;	/* squeak asked to remove it */
	char	name[NAMELEN];
	long	qid;
	long	perm;
	Queue	q[NQ];
};
struct Pend{
	Pend	*next;
	int	fid;
	int	cnt;
	int	tag;
	Store	*wrbuf;	/* place to hold incoming data until picked up */
};

Bind	*tobind(Space *, int);
Space	*tospace(int);
Bind	*spacelookup(int, char *);
int	spaceinstall(int, char *, long);
Bind	*spaceinstallb(int, char *, long);
void	fmountinit(void);
int	fmountproc(int, char *[]);
int	fmwrite(Proc *, Bind *, int, int);
int	fmread(Proc *, Bind *, int, Store *, int);
int	ckspace(int);
void	binduninstallb(Bind *);
void	drain(Bind *);
