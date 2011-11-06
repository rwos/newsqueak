/* old */

#ifdef	SYSVR3
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#define	remove(v)			unlink(v)
#define	WEXITSTATUS(s)			(((s)>>8)&0xFF)
extern	char *getenv(char*);
extern	char *getlogin(void);
extern	char *strerror(int);
extern	void *memmove(void*, const void*, size_t);
#define	NEEDMEMMOVE
#define	NEEDSTRERROR
#define	NEEDVARARG
#endif	/* SYSVR3 */

#ifdef	IRIX
extern	void *memmove(void*, const void*, size_t);
#define	NEEDMEMMOVE
#endif	/* IRIX */

#ifdef	UMIPS
typedef unsigned long	ulong;
typedef	unsigned short	ushort;
#define	const			/* mips compiler doesn't support const */
extern	char *strerror(int);
extern	void *memmove(void*, const void*, size_t);
#define	NEEDMEMMOVE
#define	NEEDSTRERROR
#define	NEEDVARARG
#define	NOFIFO			/* turn off exstart in samterm/unix.c */
#endif	/* UMIPS */

#ifdef	SUNOS
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
extern	char *strerror(int);
extern	void *memmove(void*, const void*, size_t);
extern	void *memcpy(void*, const void*, size_t);
#define	NEEDMEMMOVE
#define	NEEDSTRERROR
#endif	/* SUNOS */

#ifdef	AIX
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#endif	/* AIX */

#ifdef	OSF1
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
extern	void *memmove(void*, const void*, size_t);
#endif	/* OSF1 */

#ifdef  HPUX
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#define	NEEDSTRERROR
#endif  /* HPUX */

#ifdef  APOLLO
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#endif  /* APOLLO */

#ifdef  CONVEX
typedef unsigned long	ulong;
#endif  /* CONVEX */

#ifdef  DYNIX
#define	SIG_ERR		BADSIG
#define	NEEDMEMMOVE
#define	remove(v)			unlink(v)
#define	WEXITSTATUS(s)			(((s)>>8)&0xFF)
#define	NEEDMEMMOVE
#define	NOFIFO			/* turn off exstart in samterm/unix.c */
#endif  /* DYNIX */

#ifdef	PTX
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#endif	/* PTX */

#ifdef	v10
typedef	unsigned short	ushort;
typedef unsigned long	ulong;
#endif

#ifdef __FreeBSD__
typedef unsigned long	ulong;
#define REMOTE
#endif

/* new */

#ifdef __linux__
#define _USE_BSD
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#ifdef __STRICT_ANSI__
#	define __USE_MISC
#	include <setjmp.h>
#	undef __USE_MISC
	void *sbrk(long increment);
	int kill(pid_t pid, int sig);
	void usleep(unsigned long usec);
	char *strdup(const char *s);
	pid_t wait3(int *status, int options, struct rusage *rusage);
	typedef unsigned int		uint;
	typedef unsigned short		ushort;
	typedef unsigned long		ulong;
#else
#	include <sched.h>
#	include <setjmp.h>
#endif

typedef unsigned char		uchar;
typedef long long		vlong;
typedef unsigned long long	uvlong;
#endif


#ifdef	sun
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>

typedef unsigned char uchar;
typedef unsigned long long uvlong;
typedef long long vlong;
#endif

#ifdef __osf__
#define _XOPEN_SOURCE_EXTENDED

#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/resource.h>

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long uvlong;
typedef long long vlong;
#endif

#ifdef bsdi
#define _XOPEN_SOURCE_EXTENDED

#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <sys/resource.h>

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned long long uvlong;
typedef long long vlong;
#endif

#ifdef sgi
#include <stdarg.h>
#include <setjmp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <sys/resource.h>

typedef unsigned char uchar;
typedef unsigned long long uvlong;
typedef long long vlong;
#endif

#undef getc
#undef ungetc
#define getc p9_getc
#define ungetc p9_ungetc
#undef stdin
#define stdin p9_stdin

