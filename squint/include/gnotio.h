builtin nsmousewrite:prog(ns:int, b:int)of Mouse;
nsbindmouse:=prog(ns:int, name:array of char, perm:int,
		c: array of chan of Mouse){
	if(len c!=2)
		suspend("nsbindmouse: len c!=2");
	b:=nsinstall(ns, name, perm);
	if(def c[0])
		begin prog(){
			m:Mouse;
			for(;;){
				m=nsmousewrite(ns, b);
				c[0]<-=m;
			} 
		}();
};
