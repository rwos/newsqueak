counter:=prog(end: int, c: chan of int)
{
	i:int;
	for(i=2; i<end; i++)
		c<-=i;
};

filter:=prog(listen: chan of int, send: chan of int)
{
	i:int;
	prime:=<-listen;
	print(prime, " ");
	for(;;)
		if((i=<-listen)%prime)
			send<-=i;
};

sieve:=prog(c: chan of int)
{
	for(;;){
		newc:=mk(chan of int);
		begin filter(c, newc);
		c=newc;
	}
};

count:=mk(chan of int);

begin counter(100, count);
begin sieve(count);
"";
