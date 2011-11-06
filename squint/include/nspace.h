type NSIO: struct of{
	rdata:	chan of array of char;
	rctl:	chan of array of char;
	wdata:	chan of array of char;
	wctl:	chan of array of char;
};

builtin nsnew:prog() of int;
builtin nsinstall:prog(ns:int, name:array of char, perm:int) of int;
builtin nsuninstall:prog(ns:int, b:int);
builtin nslookup:prog(ns:int, name:array of char);
builtin nsproc:prog(ns:int, argv:array of array of char);
builtin nsread:prog(ns:int, b:int, data: array of char, synch:int) of int;
builtin nswrite:prog(ns:int, b:int, synch:int) of array of char;
builtin nsreadctl:prog(ns:int, b:int, data: array of char, synch:int) of int;
builtin nswritectl:prog(ns:int, b:int, synch:int) of array of char;

mkNSIO:=prog() of NSIO
{
	n:=mk(NSIO);
	n.rdata=mk();
	n.rctl=mk();
	n.wdata=mk();
	n.wctl=mk();
	become n;
};

nsbind:=prog(ns:int, name:array of char, mode:int, perm:int) of NSIO
{
	b:=nsinstall(ns, name, perm);
	n:=mk(NSIO);
	if(mode==0 || mode==2)
		n.rdata=mk();
	if(mode==1 || mode==2)
		n.wdata=mk();
	begin prog(){
		for(;;)
			nswritectl(ns, b, 0);
	}();
	if(def n.rdata)
		begin prog(){
			a:array of char;
			for(;;){
				a=nswrite(ns, b, 0);
				n.rdata<-=a;
			}
		}();
	if(def n.wdata)
		begin prog(){
			for(;;)
				nsread(ns, b, <-n.wdata, 0);
		}();
	become n;
};

nsbindctl:=prog(ns:int, name:array of char, mode:int, perm:int) of NSIO
{
	b:=nsinstall(ns, name, perm);
	n:=mk(NSIO);
	if(mode==0 || mode==2)
		n.rdata=mk();
	if(mode==1 || mode==2)
		n.wdata=mk();
	n.rctl=mk();
	n.wctl=mk();
	begin prog(){	# copy "open" and "close" control messages until error
		a:array of char;
		while(len (a=nswritectl(ns, b, 0))>=0)
			n.rctl<-=a;		
	}();
	begin prog(){	# turn "remove" control messages into uninstalls
		a:=<-n.wctl;
		if(a!="remove")
			suspend("unknown control message in nsbindctl");
		nsuninstall(ns, b);
		if(def n.wdata)
			n.wdata<-="";	# wake up wdata slave
	}();
	if(def n.rdata)
		begin prog(){
			a:array of char;
			while(len (a=nswrite(ns, b, 0))>=0)
				n.rdata<-=a;
		}();
	if(def n.wdata)
		begin prog(){
			do; while(nsread(ns, b, <-n.wdata, 0)>=0);
		}();
	become n;
};
