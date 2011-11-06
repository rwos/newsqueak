extern	int	fmountproc(int ns, char*argv[]);
extern	int	spacenew(void);
extern	Bind	*spacelookup(int, char*);
extern	int	spaceinstall(int, char*, long);
extern	void	spacedump(int);
extern	Store	*mkrectangle(int, int, int, int);
extern	void	inrectangle(Store*, Rectangle*);
extern	Bitmap	*inbitmap(Store*, int);
extern	Store	*mkbitmap(Bitmap*);
