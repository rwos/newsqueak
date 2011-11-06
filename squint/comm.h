typedef struct Chan Chan;
typedef struct Procnostack Procnostack;
typedef struct Pqueue Pqueue;
typedef struct Pqelem Pqelem;

#define	NSTACK	2048

struct Pqueue{
	Pqelem	*head;
	Pqelem	*tail;
};
struct Pqelem{
	Proc	*p;
	Pqelem	*next;
};
struct Chan{
	Store	hdr;
	Pqueue	sndq;
	Pqueue	rcvq;
};
struct Proc{
	SWord	*sp;
	SWord	*fp;
	int 	(**pc)(Proc *);
	long	procnum;
	Proc	*next;
	char	*prbuf;			/* for printing */
	int	nprbuf;
	int	maxprbuf;
	Chan	*chan;			/* for communication */
	Proc	*target;
	int	issnd;
	SWord	stack[NSTACK];
};
struct Procnostack{
	SWord	*sp;
	SWord	*fp;
	int 	(**pc)(Proc*);
	long	procnum;
	Proc	*next;
};

typedef enum Cflag{
	Cissnd=1,
	Cisary=2
}Cflag;

extern	Proc	**proctab;
extern	Proc	interpreter;
extern	Proc	*curproc(void);
extern	long	Nproc;

extern void	schedio(int);
extern int	moreio(void);

void	halt(Proc *);
void	run(Proc *);
Proc	*curproc(void);
void	erun(Proc *);
void	stacktrace(Proc *);
