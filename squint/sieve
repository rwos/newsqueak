counter:=prog(end: int, c: chan of int)
{
	i:int;
	for(i=2; i<end; i=i+1)
		c<-=i;
};

filter:=prog(prime: int, listen: chan of int, send: chan of int)
{
	i:int;
	for(;;)
		if((i=<-listen)%prime)
			send<-=i;
};

sieve:=prog(c: chan of int)
{
	prime:int;
	newc:chan of int;
	for(;;){
		prime=<-c;
#		print(prime, " ");
		newc=mk();
		begin filter(prime, c, newc);
		c=newc;
	}
};

count:=mk(chan of int);

begin counter(2000, count);
begin sieve(count);
"";
