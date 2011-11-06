type File: struct of{
	fd:		array[2] of int;
	in, out:	chan of array of char;
};
builtin read:prog(f:int) of array of char;
builtin bread:prog(f,n:int) of array of char;
builtin write:prog(f:int, a:array of char) of int;
builtin open:prog(a:array of char, m:int) of int;
builtin close:prog(f:int);

iobuf:=mk(array[256] of int);

fopen:=prog(f: array of char, m: int) of File{
	F:=mk(File);
	F.fd={-1, -1};
	fd:int;
	if(m==0 || m==2){
		fd=open(f, 0);
		F.fd[0]=fd;
		F.in=mk();
		iobuf[fd]=1;
		begin prog(){
			while(iobuf[fd])
				F.in<-=read(fd);
		}();
	}
	if(m==1 || m==2){
		fd=open(f, 1);
		F.fd[1]=fd;
		F.out=mk();
		iobuf[fd]=1;
		begin prog(){
			while(iobuf[fd])
				write(fd, <-F.out);
		}();
	}
	become F;
};

fclose:=prog(F: File){
	if(F.fd[0]>=0){
		iobuf[F.fd[0]]=0;
		close(F.fd[0]);
	}
	if(F.fd[1]>=0){
		iobuf[F.fd[1]]=0;
		close(F.fd[1]);
	}
};
