type Cover:struct of{
	seq:	int;
	cover:	Bitmap;
	ground:	Bitmap;
	layer:	array of Bitmap;
};

builtin mkcover:prog(cover:Bitmap, ground:Bitmap) of Cover;
builtin lalloc:prog(cover:Cover, r:Rectangle) of Cover;
builtin ltofront:prog(cover:Cover, l:int) of Cover;
builtin ltoback:prog(cover:Cover, l:int) of Cover;
builtin lfree:prog(cover:Cover, l:int) of Cover;
