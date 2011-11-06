extern int iadd(Proc*);
extern int iand(Proc*);
extern int ibecome(Proc*);
extern int ibegin(Proc*);
extern int icall(Proc*);
extern int icat(Proc*);
extern int icommcln(Proc*);
extern int icommcln1(Proc*);
extern int icommset(Proc*);
extern int icommset1(Proc*);
extern int idec(Proc*);
extern int idecary(Proc*);
extern int idecauto(Proc*);
extern int idecref(Proc*);
extern int idecrefauto(Proc*);
extern int idef(Proc*);
extern int idefary(Proc*);
extern int idefauto(Proc*);
extern int idel(Proc*);
extern int idiv(Proc*);
extern int idone(Proc*);
extern int idup(Proc*);
extern int idupptr(Proc*);
extern int ieq(Proc*);
extern int ige(Proc*);
extern int igt(Proc*);
extern int iinc(Proc*);
extern int iincary(Proc*);
extern int iincauto(Proc*);
extern int ijmp(Proc*);
extern int ijmpfalse(Proc*);
extern int ijmptrue(Proc*);
extern int ile(Proc*);
extern int ilen(Proc*);
extern int ilnot(Proc*);
extern int ilsh(Proc*);
extern int ilt(Proc*);
extern int imalloc(Proc*);
extern int imallocarychar(Proc*);
extern int imallocstruct(Proc*);
extern int imax(Proc*);
extern int imemcpy(Proc*);
extern int imemcpychar(Proc*);
extern int imemcpycharint(Proc*);
extern int imemcpystruct(Proc*);
extern int imod(Proc*);
extern int imul(Proc*);
extern int ine(Proc*);
extern int ineg(Proc*);
extern int inot(Proc*);
extern int ior(Proc*);
extern int ipop(Proc*);
extern int ipopptr(Proc*);
extern int iprint(Proc*);
extern int iprintary(Proc*);
extern int iprintblank(Proc*);
extern int iprintchar(Proc*);
extern int iprintint(Proc*);
extern int iprintnewline(Proc*);
extern int iprintunit(Proc*);
extern int ipush(Proc*);
extern int ipush0(Proc*);
extern int ipush1(Proc*);
extern int ipush_1(Proc*);
extern int ipush10(Proc*);
extern int ipush2(Proc*);
extern int ipush_2(Proc*);
extern int ipush3(Proc*);
extern int ipush4(Proc*);
extern int ipush5(Proc*);
extern int ipush6(Proc*);
extern int ipush7(Proc*);
extern int ipush8(Proc*);
extern int ipush9(Proc*);
extern int ipushary(Proc*);
extern int ipusharyauto(Proc*);
extern int ipusharychar(Proc*);
extern int ipusharycharauto(Proc*);
extern int ipusharycharexpr(Proc*);
extern int ipusharyexpr(Proc*);
extern int ipusharyptr(Proc*);
extern int ipusharyptrauto(Proc*);
extern int ipusharyptrexpr(Proc*);
extern int ipushauto(Proc*);
extern int ipushconst(Proc*);
extern int ipushdata(Proc*);
extern int ipushfp(Proc*);
extern int ipushptr(Proc*);
extern int ipushptrauto(Proc*);
extern int ipushuniq(Proc*);
extern int ipushuniqary(Proc*);
extern int ipushuniqauto(Proc*);
extern int iref(Proc*);
extern int iret(Proc*);
extern int irsh(Proc*);
extern int isnd(Proc*);
extern int isndptr(Proc*);
extern int isprint(Proc*);
extern int istore(Proc*);
extern int istoreary(Proc*);
extern int istoreauto(Proc*);
extern int istorechar(Proc*);
extern int istorecharauto(Proc*);
extern int istoreptr(Proc*);
extern int istoreptrauto(Proc*);
extern int istrcmp(Proc*);
extern int isub(Proc*);
extern int ivalnoresult(Proc*);
extern int ixor(Proc*);
struct Insttab insttab[]={
	{iadd, "Iadd"},
	{iand, "Iand"},
	{ibecome, "Ibecome"},
	{ibegin, "Ibegin"},
	{icall, "Icall"},
	{icat, "Icat"},
	{icommcln, "Icommcln"},
	{icommcln1, "Icommcln1"},
	{icommset, "Icommset"},
	{icommset1, "Icommset1"},
	{idec, "Idec"},
	{idecary, "Idecary"},
	{idecauto, "Idecauto"},
	{idecref, "Idecref"},
	{idecrefauto, "Idecrefauto"},
	{idef, "Idef"},
	{idefary, "Idefary"},
	{idefauto, "Idefauto"},
	{idel, "Idel"},
	{idiv, "Idiv"},
	{idone, "Idone"},
	{idup, "Idup"},
	{idupptr, "Idupptr"},
	{ieq, "Ieq"},
	{ige, "Ige"},
	{igt, "Igt"},
	{iinc, "Iinc"},
	{iincary, "Iincary"},
	{iincauto, "Iincauto"},
	{ijmp, "Ijmp"},
	{ijmpfalse, "Ijmpfalse"},
	{ijmptrue, "Ijmptrue"},
	{ile, "Ile"},
	{ilen, "Ilen"},
	{ilnot, "Ilnot"},
	{ilsh, "Ilsh"},
	{ilt, "Ilt"},
	{imalloc, "Imalloc"},
	{imallocarychar, "Imallocarychar"},
	{imallocstruct, "Imallocstruct"},
	{imax, "Imax"},
	{imemcpy, "Imemcpy"},
	{imemcpychar, "Imemcpychar"},
	{imemcpycharint, "Imemcpycharint"},
	{imemcpystruct, "Imemcpystruct"},
	{imod, "Imod"},
	{imul, "Imul"},
	{ine, "Ine"},
	{ineg, "Ineg"},
	{inot, "Inot"},
	{ior, "Ior"},
	{ipop, "Ipop"},
	{ipopptr, "Ipopptr"},
	{iprint, "Iprint"},
	{iprintary, "Iprintary"},
	{iprintblank, "Iprintblank"},
	{iprintchar, "Iprintchar"},
	{iprintint, "Iprintint"},
	{iprintnewline, "Iprintnewline"},
	{iprintunit, "Iprintunit"},
	{ipush, "Ipush"},
	{ipush0, "Ipush0"},
	{ipush1, "Ipush1"},
	{ipush_1, "Ipush_1"},
	{ipush10, "Ipush10"},
	{ipush2, "Ipush2"},
	{ipush_2, "Ipush_2"},
	{ipush3, "Ipush3"},
	{ipush4, "Ipush4"},
	{ipush5, "Ipush5"},
	{ipush6, "Ipush6"},
	{ipush7, "Ipush7"},
	{ipush8, "Ipush8"},
	{ipush9, "Ipush9"},
	{ipushary, "Ipushary"},
	{ipusharyauto, "Ipusharyauto"},
	{ipusharychar, "Ipusharychar"},
	{ipusharycharauto, "Ipusharycharauto"},
	{ipusharycharexpr, "Ipusharycharexpr"},
	{ipusharyexpr, "Ipusharyexpr"},
	{ipusharyptr, "Ipusharyptr"},
	{ipusharyptrauto, "Ipusharyptrauto"},
	{ipusharyptrexpr, "Ipusharyptrexpr"},
	{ipushauto, "Ipushauto"},
	{ipushconst, "Ipushconst"},
	{ipushdata, "Ipushdata"},
	{ipushfp, "Ipushfp"},
	{ipushptr, "Ipushptr"},
	{ipushptrauto, "Ipushptrauto"},
	{ipushuniq, "Ipushuniq"},
	{ipushuniqary, "Ipushuniqary"},
	{ipushuniqauto, "Ipushuniqauto"},
	{iref, "Iref"},
	{iret, "Iret"},
	{irsh, "Irsh"},
	{isnd, "Isnd"},
	{isndptr, "Isndptr"},
	{isprint, "Isprint"},
	{istore, "Istore"},
	{istoreary, "Istoreary"},
	{istoreauto, "Istoreauto"},
	{istorechar, "Istorechar"},
	{istorecharauto, "Istorecharauto"},
	{istoreptr, "Istoreptr"},
	{istoreptrauto, "Istoreptrauto"},
	{istrcmp, "Istrcmp"},
	{isub, "Isub"},
	{ivalnoresult, "Ivalnoresult"},
	{ixor, "Ixor"},
};
