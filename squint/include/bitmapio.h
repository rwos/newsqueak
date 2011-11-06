builtin readbitmap:prog(f:int) of Bitmap;

freadbitmap:=prog(F:File) of Bitmap{
	if(F.fd[0]==-1)
		abort("freadbitmap: file not open for read");
	become readbitmap(F.fd[0]);
};
