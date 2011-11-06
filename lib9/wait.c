#include <u.h>
#include <lib9.h>
#undef wait
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

int
__waitcommon(int pid, int status, struct rusage *rusage, Waitmsg *w) {
	long utime, stime;

	sprint(w->pid, "%d", pid);

	/* The time array contains the
         * time the child and its descendants spent in user code, the
         * time spent in system calls, and the child's elapsed real
         * time, all in units of milliseconds.
	 */
	utime = rusage->ru_utime.tv_sec*1000 + rusage->ru_utime.tv_usec/1000;
	stime = rusage->ru_stime.tv_sec*1000 + rusage->ru_stime.tv_usec/1000;
	sprint(w->time, "%11d %11d %11d", utime, stime, utime+stime);
	if (status == 0)
		w->msg[0] = '\0';
	else
		sprint(w->msg, "%d", status);
	return pid;
}

int
__plan9nbwait(Waitmsg *w) {
	int pid, status;
	struct rusage rusage;

	pid = wait3(&status, WNOHANG, &rusage);
	if (pid <= 0) return pid;
	return __waitcommon(pid, status, &rusage, w);
}

int
__plan9wait(Waitmsg *w) {
	int pid, status;
	struct rusage rusage;

	pid = wait3(&status, 0, &rusage);
	if (pid < 0) return pid;
	return __waitcommon(pid, status, &rusage, w);
}
