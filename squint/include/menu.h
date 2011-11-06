type Menu: struct of{
	lab:	array of array of char;
	last:	int;
};

menuspin:=prog(S:Bitmap, M:chan of Mouse, ms:Mouse, v:int) of int{
	while(ms.but)
		ms=<-M;
	become v;
};

menubox:=prog(S: Bitmap, r: Rectangle, d: Point, i: int){
	if(i==-1)
		become unit;
	r.min.y=r.min.y+i*d.y;
	r.max.y=r.min.y+d.y;
	bitblt(S, r.min, S, r, XXX&~DST);
};

menuhit:=prog(S:Bitmap, M:chan of Mouse, ms:Mouse, menu:Menu, b:int) of int{
	n:=len menu.lab;
	if(n==0)
		become menuspin(S, M, ms, -1);
	p,q:=strsize(font, menu.lab[0]);
	i:int;
	for(i=1; i<n; i++){
		p=strsize(font, menu.lab[i]);
		if(p.x>q.x)
			q.x=p.x;
	}
	# compute location of menu
	r:=mk(Rectangle);
	r.min={ms.xy.x-q.x/2, ms.xy.y-menu.last*q.y-q.y/2};
	r.max={r.min.x+q.x, r.min.y+len menu.lab*q.y};
#	R:=cover.cover.r;
	R:=getscreen().r;
	const D:=3+2;		# 3 for bitmap border, 2 for menu's own border
	d:int;
	if((d=R.max.x-r.max.x-D)<0){
		r.min.x=r.min.x+d;
		r.max.x=r.max.x+d;
	}
	if((d=R.max.y-r.max.y-D)<0){
		r.min.y=r.min.y+d;
		r.max.y=r.max.y+d;
	}
	if((d=r.min.x-R.min.x-D)<0){
		r.min.x=r.min.x-d;
		r.max.x=r.max.x-d;
	}
	if((d=r.min.y-R.min.y-D)<0){
		r.min.y=r.min.y-d;
		r.max.y=r.max.y-d;
	}
	rout:=rectinset(r, -2, -2);
	# post menu
	bm:=mkbitmap(rout, S.ldepth);
	bitblt(bm, rout.min, S, rout, SRC);
	bitblt(S, rout.min, S, rout, XXX);
	rtmp:=rectinset(rout, 1, 1);
	bitblt(S, rtmp.min, S, rtmp, 0);
	p={(r.min.x+r.max.x)/2, r.min.y};
	for(i=0; i<n; i++){
		w:=strsize(font, menu.lab[i]).x;
		string(S, {p.x-w/2, p.y}, font, menu.lab[i], SRC);
		p.y=p.y+q.y;
	}
	nitem,item:=-1;
	while(ms.but==b){
		if(ptinrect(ms.xy, r)==0)
			nitem=-1;
		else
			nitem=(ms.xy.y-r.min.y)/q.y;
		if(item!=nitem){
			menubox(S, r, q, item);
			item=nitem;
			menubox(S, r, q, item);
		}
		ms=<-M;
	}
	bitblt(S, rout.min, bm, rout, SRC);
	if(ms.but&~b)
		item=-1;
	become menuspin(S, M, ms, item);
};
