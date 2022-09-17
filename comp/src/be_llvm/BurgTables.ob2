-- source grammar = eppc.b
<*+WOFF*>
MODULE BurgTables;

IMPORT ir, RD := RDefs, BurgNT, SYSTEM;
TYPE Index *= INTEGER;
TYPE NTnts_nType = ARRAY OF BurgNT.NT;
CONST NTnts_n*= NTnts_nType {
	BurgNT.NTnowhere , (*0*)
	BurgNT.NTreg, BurgNT.NTnowhere , (*1*)
	BurgNT.NTconst, BurgNT.NTnowhere , (*2*)
	BurgNT.NTlconst, BurgNT.NTnowhere , (*3*)
	BurgNT.NTbased, BurgNT.NTnowhere , (*4*)
	BurgNT.NTreg, BurgNT.NTreg, BurgNT.NTnowhere , (*5*)
	BurgNT.NTindexed, BurgNT.NTnowhere , (*6*)
	BurgNT.NTaddr, BurgNT.NTnowhere , (*7*)
	BurgNT.NTconstaddr, BurgNT.NTnowhere , (*8*)
	BurgNT.NTmem, BurgNT.NTnowhere , (*9*)
	BurgNT.NTlocal, BurgNT.NTnowhere , (*10*)
	BurgNT.NTreg, BurgNT.NTplist, BurgNT.NTnowhere , (*11*)
	BurgNT.NTplist, BurgNT.NTnowhere , (*12*)
	BurgNT.NTreg, BurgNT.NTconst, BurgNT.NTnowhere , (*13*)
	BurgNT.NTconst, BurgNT.NTreg, BurgNT.NTnowhere , (*14*)
	BurgNT.NTreg, BurgNT.NTrc, BurgNT.NTnowhere , (*15*)
	BurgNT.NTrc, BurgNT.NTreg, BurgNT.NTnowhere , (*16*)
	BurgNT.NTlconst, BurgNT.NTreg, BurgNT.NTnowhere , (*17*)
	BurgNT.NTaddr, BurgNT.NTreg, BurgNT.NTnowhere , (*18*)
	BurgNT.NTconstaddr, BurgNT.NTreg, BurgNT.NTnowhere , (*19*)
	BurgNT.NTreg, BurgNT.NTreg, BurgNT.NTreg, BurgNT.NTnowhere , (*20*)
	BurgNT.NTreg, BurgNT.NTreg, BurgNT.NTconst, BurgNT.NTnowhere , (*21*)
	BurgNT.NTreg, BurgNT.NTreg, BurgNT.NTlconst, BurgNT.NTnowhere , (*22*)
	BurgNT.NTrc, BurgNT.NTrc, BurgNT.NTnowhere , (*23*)
	BurgNT.NTrc, BurgNT.NTnowhere , (*24*)
	BurgNT.NTfmul, BurgNT.NTnowhere , (*25*)
	BurgNT.NTfmul, BurgNT.NTreg, BurgNT.NTnowhere , (*26*)
	BurgNT.NTreg, BurgNT.NTfmul, BurgNT.NTnowhere , (*27*)
	BurgNT.NTctr, BurgNT.NTnowhere  (*28*)
};
CONST nRules = BurgNT.Rule{ 168 };
TYPE RuleRange   = BurgNT.Rule[BurgNT.Rule{0}..nRules];
     NTntsType = ARRAY RuleRange OF Index;
CONST NTnts*= NTntsType {
	Index{ 0 }, (*0*)
	Index{ 0 }, (*1->0*) 
	Index{ 0 }, (*2->0*) 
	Index{ 1 }, (*3->1*) 
	Index{ 3 }, (*4->2*) 
	Index{ 5 }, (*5->3*) 
	Index{ 3 }, (*6->2*) 
	Index{ 3 }, (*7->2*) 
	Index{ 0 }, (*8->0*) 
	Index{ 0 }, (*9->0*) 
	Index{ 0 }, (*10->0*) 
	Index{ 1 }, (*11->1*) 
	Index{ 0 }, (*12->0*) 
	Index{ 0 }, (*13->0*) 
	Index{ 7 }, (*14->4*) 
	Index{ 7 }, (*15->4*) 
	Index{ 7 }, (*16->4*) 
	Index{ 1 }, (*17->1*) 
	Index{ 0 }, (*18->0*) 
	Index{ 0 }, (*19->0*) 
	Index{ 9 }, (*20->5*) 
	Index{ 1 }, (*21->1*) 
	Index{ 1 }, (*22->1*) 
	Index{ 1 }, (*23->1*) 
	Index{ 1 }, (*24->1*) 
	Index{ 12 }, (*25->6*) 
	Index{ 0 }, (*26->0*) 
	Index{ 7 }, (*27->4*) 
	Index{ 12 }, (*28->6*) 
	Index{ 1 }, (*29->1*) 
	Index{ 14 }, (*30->7*) 
	Index{ 16 }, (*31->8*) 
	Index{ 0 }, (*32->0*) 
	Index{ 0 }, (*33->0*) 
	Index{ 0 }, (*34->0*) 
	Index{ 0 }, (*35->0*) 
	Index{ 0 }, (*36->0*) 
	Index{ 0 }, (*37->0*) 
	Index{ 0 }, (*38->0*) 
	Index{ 1 }, (*39->1*) 
	Index{ 14 }, (*40->7*) 
	Index{ 14 }, (*41->7*) 
	Index{ 14 }, (*42->7*) 
	Index{ 16 }, (*43->8*) 
	Index{ 0 }, (*44->0*) 
	Index{ 18 }, (*45->9*) 
	Index{ 20 }, (*46->10*) 
	Index{ 0 }, (*47->0*) 
	Index{ 1 }, (*48->1*) 
	Index{ 0 }, (*49->0*) 
	Index{ 0 }, (*50->0*) 
	Index{ 0 }, (*51->0*) 
	Index{ 0 }, (*52->0*) 
	Index{ 0 }, (*53->0*) 
	Index{ 0 }, (*54->0*) 
	Index{ 1 }, (*55->1*) 
	Index{ 0 }, (*56->0*) 
	Index{ 1 }, (*57->1*) 
	Index{ 0 }, (*58->0*) 
	Index{ 22 }, (*59->11*) 
	Index{ 25 }, (*60->12*) 
	Index{ 25 }, (*61->12*) 
	Index{ 25 }, (*62->12*) 
	Index{ 22 }, (*63->11*) 
	Index{ 22 }, (*64->11*) 
	Index{ 22 }, (*65->11*) 
	Index{ 9 }, (*66->5*) 
	Index{ 27 }, (*67->13*) 
	Index{ 30 }, (*68->14*) 
	Index{ 9 }, (*69->5*) 
	Index{ 27 }, (*70->13*) 
	Index{ 30 }, (*71->14*) 
	Index{ 1 }, (*72->1*) 
	Index{ 33 }, (*73->15*) 
	Index{ 36 }, (*74->16*) 
	Index{ 9 }, (*75->5*) 
	Index{ 1 }, (*76->1*) 
	Index{ 33 }, (*77->15*) 
	Index{ 9 }, (*78->5*) 
	Index{ 27 }, (*79->13*) 
	Index{ 30 }, (*80->14*) 
	Index{ 27 }, (*81->13*) 
	Index{ 30 }, (*82->14*) 
	Index{ 27 }, (*83->13*) 
	Index{ 30 }, (*84->14*) 
	Index{ 27 }, (*85->13*) 
	Index{ 30 }, (*86->14*) 
	Index{ 9 }, (*87->5*) 
	Index{ 9 }, (*88->5*) 
	Index{ 9 }, (*89->5*) 
	Index{ 1 }, (*90->1*) 
	Index{ 1 }, (*91->1*) 
	Index{ 9 }, (*92->5*) 
	Index{ 9 }, (*93->5*) 
	Index{ 9 }, (*94->5*) 
	Index{ 1 }, (*95->1*) 
	Index{ 39 }, (*96->17*) 
	Index{ 9 }, (*97->5*) 
	Index{ 1 }, (*98->1*) 
	Index{ 14 }, (*99->7*) 
	Index{ 42 }, (*100->18*) 
	Index{ 45 }, (*101->19*) 
	Index{ 33 }, (*102->15*) 
	Index{ 36 }, (*103->16*) 
	Index{ 33 }, (*104->15*) 
	Index{ 36 }, (*105->16*) 
	Index{ 0 }, (*106->0*) 
	Index{ 33 }, (*107->15*) 
	Index{ 36 }, (*108->16*) 
	Index{ 9 }, (*109->5*) 
	Index{ 33 }, (*110->15*) 
	Index{ 36 }, (*111->16*) 
	Index{ 9 }, (*112->5*) 
	Index{ 36 }, (*113->16*) 
	Index{ 1 }, (*114->1*) 
	Index{ 20 }, (*115->10*) 
	Index{ 1 }, (*116->1*) 
	Index{ 1 }, (*117->1*) 
	Index{ 1 }, (*118->1*) 
	Index{ 48 }, (*119->20*) 
	Index{ 52 }, (*120->21*) 
	Index{ 56 }, (*121->22*) 
	Index{ 1 }, (*122->1*) 
	Index{ 1 }, (*123->1*) 
	Index{ 60 }, (*124->23*) 
	Index{ 60 }, (*125->23*) 
	Index{ 33 }, (*126->15*) 
	Index{ 36 }, (*127->16*) 
	Index{ 33 }, (*128->15*) 
	Index{ 36 }, (*129->16*) 
	Index{ 1 }, (*130->1*) 
	Index{ 63 }, (*131->24*) 
	Index{ 63 }, (*132->24*) 
	Index{ 0 }, (*133->0*) 
	Index{ 1 }, (*134->1*) 
	Index{ 1 }, (*135->1*) 
	Index{ 1 }, (*136->1*) 
	Index{ 1 }, (*137->1*) 
	Index{ 1 }, (*138->1*) 
	Index{ 1 }, (*139->1*) 
	Index{ 1 }, (*140->1*) 
	Index{ 1 }, (*141->1*) 
	Index{ 1 }, (*142->1*) 
	Index{ 1 }, (*143->1*) 
	Index{ 0 }, (*144->0*) 
	Index{ 65 }, (*145->25*) 
	Index{ 9 }, (*146->5*) 
	Index{ 1 }, (*147->1*) 
	Index{ 1 }, (*148->1*) 
	Index{ 9 }, (*149->5*) 
	Index{ 9 }, (*150->5*) 
	Index{ 9 }, (*151->5*) 
	Index{ 9 }, (*152->5*) 
	Index{ 48 }, (*153->20*) 
	Index{ 67 }, (*154->26*) 
	Index{ 70 }, (*155->27*) 
	Index{ 48 }, (*156->20*) 
	Index{ 67 }, (*157->26*) 
	Index{ 70 }, (*158->27*) 
	Index{ 48 }, (*159->20*) 
	Index{ 67 }, (*160->26*) 
	Index{ 70 }, (*161->27*) 
	Index{ 48 }, (*162->20*) 
	Index{ 67 }, (*163->26*) 
	Index{ 70 }, (*164->27*) 
	Index{ 0 }, (*165->0*) 
	Index{ 0 }, (*166->0*) 
	Index{ 73 }, (*167->28*) 
	Index{ 73 }  (*168->28*) 
};
<* IF ~nodebug THEN*>
TYPE RuleNamesType = ARRAY RuleRange OF ARRAY 64 OF CHAR;
CONST RuleNames *= RuleNamesType {
    'zerorule',
    'const: o_par',
    'lconst: o_par',
    'rc: reg',
    'rc: const',
    'reg: lconst',
    'reg: const',
    'reg: const',
    'reg: o_par',
    'reg: o_par',
    'constaddr: o_par',
    'based: reg',
    'based: o_par',
    'based: o_par',
    'based: o_add(based,o_par)',
    'based: o_sub(based,o_par)',
    'reg: based',
    'indexed: reg',
    'indexed: o_par',
    'indexed: o_par',
    'indexed: o_add(reg,reg)',
    'indexed: o_add(reg,o_par)',
    'indexed: o_add(o_par,reg)',
    'indexed: o_add(reg,o_par)',
    'indexed: o_add(o_par,reg)',
    'reg: indexed',
    'addr: o_par',
    'addr: based',
    'addr: indexed',
    'addr: reg',
    'reg: addr',
    'reg: constaddr',
    'reg: o_par',
    'reg: o_par',
    'reg: o_par',
    'reg: o_par',
    'reg: o_par',
    'reg: o_par',
    'reg: o_loadr(o_par)',
    'stm: o_storer(o_par,reg)',
    'reg: o_loadr(addr)',
    'mem: o_loadr(addr)',
    'local: o_loadr(addr)',
    'reg: o_loadr(constaddr)',
    'mem: o_par',
    'reg: mem',
    'reg: local',
    'reg: o_par',
    'local: reg',
    'reg: o_getpar',
    'reg: o_getpar',
    'reg: o_getpar',
    'local: o_getpar',
    'local: o_getpar',
    'reg: o_getpar',
    'stm: o_putpar(reg)',
    'stm: o_ret',
    'stm: o_retfun(reg)',
    'plist: o_empty',
    'plist: o_comma(reg,plist)',
    'reg: o_call(o_par,plist)',
    'local: o_call(o_par,plist)',
    'stm: o_call(o_par,plist)',
    'reg: o_call(reg,plist)',
    'local: o_call(reg,plist)',
    'stm: o_call(reg,plist)',
    'reg: o_add(reg,reg)',
    'reg: o_add(reg,const)',
    'reg: o_add(const,reg)',
    'reg: o_sub(reg,reg)',
    'reg: o_sub(reg,const)',
    'reg: o_sub(const,reg)',
    'reg: o_neg(reg)',
    'reg: o_logical(reg,rc)',
    'reg: o_logical(rc,reg)',
    'reg: o_andnot(reg,reg)',
    'reg: o_not(reg)',
    'reg: o_shift(reg,rc)',
    'reg: o_mul(reg,reg)',
    'reg: o_mul(reg,const)',
    'reg: o_mul(const,reg)',
    'reg: o_mul(reg,const)',
    'reg: o_mul(const,reg)',
    'reg: o_mul(reg,const)',
    'reg: o_mul(const,reg)',
    'reg: o_mul(reg,const)',
    'reg: o_mul(const,reg)',
    'reg: o_mulh(reg,reg)',
    'reg: o_dvd(reg,reg)',
    'reg: o_div(reg,reg)',
    'reg: o_div(o_par,reg)',
    'reg: o_div(o_par,reg)',
    'reg: o_div(reg,reg)',
    'reg: o_rem(reg,reg)',
    'reg: o_mod(reg,reg)',
    'reg: o_mod(o_par,reg)',
    'reg: o_mod(lconst,reg)',
    'reg: o_mod(reg,reg)',
    'reg: o_sgnext(reg)',
    'stm: o_storer(addr,o_par)',
    'stm: o_storer(addr,reg)',
    'stm: o_storer(constaddr,reg)',
    'reg: o_move_eq(reg,rc)',
    'reg: o_move_eq(rc,reg)',
    'reg: o_move_le(reg,rc)',
    'reg: o_move_le(rc,reg)',
    'stm: o_goto',
    'stm: o_le(reg,rc)',
    'stm: o_le(rc,reg)',
    'stm: o_fle(reg,reg)',
    'stm: o_eq(reg,rc)',
    'stm: o_eq(rc,reg)',
    'stm: o_feq(reg,reg)',
    'stm: o_in(rc,reg)',
    'stm: o_case(reg)',
    'stm: local',
    'stm: reg',
    'reg: o_loset(reg)',
    'reg: o_hiset(reg)',
    'stm: o_copy(o_comma(reg,reg),reg)',
    'stm: o_copy(o_comma(reg,reg),const)',
    'stm: o_copy(o_comma(reg,reg),lconst)',
    'reg: o_alloca(reg)',
    'local: o_alloca(reg)',
    'reg: o_incl(rc,rc)',
    'reg: o_excl(rc,rc)',
    'stm: o_checklo(reg,rc)',
    'stm: o_checklo(rc,reg)',
    'stm: o_checkhi(reg,rc)',
    'stm: o_checkhi(rc,reg)',
    'stm: o_checknil(reg)',
    'stm: o_error(rc)',
    'stm: o_stop(rc)',
    'reg: o_base',
    'reg: o_val(reg)',
    'reg: o_cast(reg)',
    'reg: o_val(reg)',
    'reg: o_cast(reg)',
    'reg: o_val(reg)',
    'reg: o_cast(reg)',
    'reg: o_val(reg)',
    'reg: o_cast(reg)',
    'reg: o_val(reg)',
    'reg: o_cast(reg)',
    'fmul: o_par',
    'reg: fmul',
    'fmul: o_fbin(reg,reg)',
    'reg: o_funary(reg)',
    'reg: o_funary(o_funary(reg))',
    'reg: o_fbin(reg,reg)',
    'reg: o_fbin(reg,reg)',
    'reg: o_fbin(reg,reg)',
    'reg: o_fbin(reg,reg)',
    'reg: o_fbin(o_fbin(reg,reg),reg)',
    'reg: o_fbin(fmul,reg)',
    'reg: o_fbin(reg,fmul)',
    'reg: o_fbin(o_fbin(reg,reg),reg)',
    'reg: o_fbin(fmul,reg)',
    'reg: o_fbin(reg,fmul)',
    'reg: o_funary(o_fbin(o_fbin(reg,reg),reg))',
    'reg: o_funary(o_fbin(fmul,reg))',
    'reg: o_funary(o_fbin(reg,fmul))',
    'reg: o_funary(o_fbin(o_fbin(reg,reg),reg))',
    'reg: o_funary(o_fbin(fmul,reg))',
    'reg: o_funary(o_fbin(reg,fmul))',
    'ctr: o_sub(o_par,o_par)',
    'ctr: o_par',
    'stm: o_eq(ctr,o_par)',
    'stm: o_eq(o_par,ctr)'
    };
<* END *>
PROCEDURE NTkids* (n: RD.DAGNODE; eruleno: BurgNT.Rule;
		VAR kids: ARRAY OF RD.DAGNODE);
BEGIN
    CASE eruleno OF
	|BurgNT.Rule{ 166 }	(* ctr: o_par *) ,
	 BurgNT.Rule{ 165 }	(* ctr: o_sub(o_par,o_par) *),
	 BurgNT.Rule{ 144 }	(* fmul: o_par *),
	 BurgNT.Rule{ 133 }	(* reg: o_base *),
	 BurgNT.Rule{ 106 }	(* stm: o_goto *),
	 BurgNT.Rule{ 58 }	(* plist: o_empty *),
	 BurgNT.Rule{ 56 }	(* stm: o_ret *),
	 BurgNT.Rule{ 54 }	(* reg: o_getpar *),
	 BurgNT.Rule{ 53 }	(* local: o_getpar *),
	 BurgNT.Rule{ 52 }	(* local: o_getpar *),
	 BurgNT.Rule{ 51 }	(* reg: o_getpar *),
	 BurgNT.Rule{ 50 }	(* reg: o_getpar *),
	 BurgNT.Rule{ 49 }	(* reg: o_getpar *),
	 BurgNT.Rule{ 47 }	(* reg: o_par *),
	 BurgNT.Rule{ 44 }	(* mem: o_par *),
	 BurgNT.Rule{ 38 }	(* reg: o_loadr(o_par) *),
	 BurgNT.Rule{ 37 }	(* reg: o_par *),
	 BurgNT.Rule{ 36 }	(* reg: o_par *),
	 BurgNT.Rule{ 35 }	(* reg: o_par *),
	 BurgNT.Rule{ 34 }	(* reg: o_par *),
	 BurgNT.Rule{ 33 }	(* reg: o_par *),
	 BurgNT.Rule{ 32 }	(* reg: o_par *),
	 BurgNT.Rule{ 26 }	(* addr: o_par *),
	 BurgNT.Rule{ 19 }	(* indexed: o_par *),
	 BurgNT.Rule{ 18 }	(* indexed: o_par *),
	 BurgNT.Rule{ 13 }	(* based: o_par *),
	 BurgNT.Rule{ 12 }	(* based: o_par *),
	 BurgNT.Rule{ 10 }	(* constaddr: o_par *),
	 BurgNT.Rule{ 9 }	(* reg: o_par *),
	 BurgNT.Rule{ 8 }	(* reg: o_par *),
	 BurgNT.Rule{ 2 }	(* lconst: o_par *),
	 BurgNT.Rule{ 1 }	(* const: o_par *):

	|BurgNT.Rule{ 145 }	(* reg: fmul *) ,
	 BurgNT.Rule{ 116 }	(* stm: reg *),
	 BurgNT.Rule{ 115 }	(* stm: local *),
	 BurgNT.Rule{ 48 }	(* local: reg *),
	 BurgNT.Rule{ 46 }	(* reg: local *),
	 BurgNT.Rule{ 45 }	(* reg: mem *),
	 BurgNT.Rule{ 31 }	(* reg: constaddr *),
	 BurgNT.Rule{ 30 }	(* reg: addr *),
	 BurgNT.Rule{ 29 }	(* addr: reg *),
	 BurgNT.Rule{ 28 }	(* addr: indexed *),
	 BurgNT.Rule{ 27 }	(* addr: based *),
	 BurgNT.Rule{ 25 }	(* reg: indexed *),
	 BurgNT.Rule{ 17 }	(* indexed: reg *),
	 BurgNT.Rule{ 16 }	(* reg: based *),
	 BurgNT.Rule{ 11 }	(* based: reg *),
	 BurgNT.Rule{ 7 }	(* reg: const *),
	 BurgNT.Rule{ 6 }	(* reg: const *),
	 BurgNT.Rule{ 5 }	(* reg: lconst *),
	 BurgNT.Rule{ 4 }	(* rc: const *),
	 BurgNT.Rule{ 3 }	(* rc: reg *):
		kids[0] := n;

	|BurgNT.Rule{ 167 }	(* stm: o_eq(ctr,o_par) *) ,
	 BurgNT.Rule{ 147 }	(* reg: o_funary(reg) *),
	 BurgNT.Rule{ 143 }	(* reg: o_cast(reg) *),
	 BurgNT.Rule{ 142 }	(* reg: o_val(reg) *),
	 BurgNT.Rule{ 141 }	(* reg: o_cast(reg) *),
	 BurgNT.Rule{ 140 }	(* reg: o_val(reg) *),
	 BurgNT.Rule{ 139 }	(* reg: o_cast(reg) *),
	 BurgNT.Rule{ 138 }	(* reg: o_val(reg) *),
	 BurgNT.Rule{ 137 }	(* reg: o_cast(reg) *),
	 BurgNT.Rule{ 136 }	(* reg: o_val(reg) *),
	 BurgNT.Rule{ 135 }	(* reg: o_cast(reg) *),
	 BurgNT.Rule{ 134 }	(* reg: o_val(reg) *),
	 BurgNT.Rule{ 132 }	(* stm: o_stop(rc) *),
	 BurgNT.Rule{ 131 }	(* stm: o_error(rc) *),
	 BurgNT.Rule{ 130 }	(* stm: o_checknil(reg) *),
	 BurgNT.Rule{ 123 }	(* local: o_alloca(reg) *),
	 BurgNT.Rule{ 122 }	(* reg: o_alloca(reg) *),
	 BurgNT.Rule{ 118 }	(* reg: o_hiset(reg) *),
	 BurgNT.Rule{ 117 }	(* reg: o_loset(reg) *),
	 BurgNT.Rule{ 114 }	(* stm: o_case(reg) *),
	 BurgNT.Rule{ 99 }	(* stm: o_storer(addr,o_par) *),
	 BurgNT.Rule{ 98 }	(* reg: o_sgnext(reg) *),
	 BurgNT.Rule{ 76 }	(* reg: o_not(reg) *),
	 BurgNT.Rule{ 72 }	(* reg: o_neg(reg) *),
	 BurgNT.Rule{ 57 }	(* stm: o_retfun(reg) *),
	 BurgNT.Rule{ 55 }	(* stm: o_putpar(reg) *),
	 BurgNT.Rule{ 43 }	(* reg: o_loadr(constaddr) *),
	 BurgNT.Rule{ 42 }	(* local: o_loadr(addr) *),
	 BurgNT.Rule{ 41 }	(* mem: o_loadr(addr) *),
	 BurgNT.Rule{ 40 }	(* reg: o_loadr(addr) *),
	 BurgNT.Rule{ 23 }	(* indexed: o_add(reg,o_par) *),
	 BurgNT.Rule{ 21 }	(* indexed: o_add(reg,o_par) *),
	 BurgNT.Rule{ 15 }	(* based: o_sub(based,o_par) *),
	 BurgNT.Rule{ 14 }	(* based: o_add(based,o_par) *):
		kids[0] := n.l;

	|BurgNT.Rule{ 158 }	(* reg: o_fbin(reg,fmul) *) ,
	 BurgNT.Rule{ 157 }	(* reg: o_fbin(fmul,reg) *),
	 BurgNT.Rule{ 155 }	(* reg: o_fbin(reg,fmul) *),
	 BurgNT.Rule{ 154 }	(* reg: o_fbin(fmul,reg) *),
	 BurgNT.Rule{ 152 }	(* reg: o_fbin(reg,reg) *),
	 BurgNT.Rule{ 151 }	(* reg: o_fbin(reg,reg) *),
	 BurgNT.Rule{ 150 }	(* reg: o_fbin(reg,reg) *),
	 BurgNT.Rule{ 149 }	(* reg: o_fbin(reg,reg) *),
	 BurgNT.Rule{ 146 }	(* fmul: o_fbin(reg,reg) *),
	 BurgNT.Rule{ 129 }	(* stm: o_checkhi(rc,reg) *),
	 BurgNT.Rule{ 128 }	(* stm: o_checkhi(reg,rc) *),
	 BurgNT.Rule{ 127 }	(* stm: o_checklo(rc,reg) *),
	 BurgNT.Rule{ 126 }	(* stm: o_checklo(reg,rc) *),
	 BurgNT.Rule{ 125 }	(* reg: o_excl(rc,rc) *),
	 BurgNT.Rule{ 124 }	(* reg: o_incl(rc,rc) *),
	 BurgNT.Rule{ 113 }	(* stm: o_in(rc,reg) *),
	 BurgNT.Rule{ 112 }	(* stm: o_feq(reg,reg) *),
	 BurgNT.Rule{ 111 }	(* stm: o_eq(rc,reg) *),
	 BurgNT.Rule{ 110 }	(* stm: o_eq(reg,rc) *),
	 BurgNT.Rule{ 109 }	(* stm: o_fle(reg,reg) *),
	 BurgNT.Rule{ 108 }	(* stm: o_le(rc,reg) *),
	 BurgNT.Rule{ 107 }	(* stm: o_le(reg,rc) *),
	 BurgNT.Rule{ 105 }	(* reg: o_move_le(rc,reg) *),
	 BurgNT.Rule{ 104 }	(* reg: o_move_le(reg,rc) *),
	 BurgNT.Rule{ 103 }	(* reg: o_move_eq(rc,reg) *),
	 BurgNT.Rule{ 102 }	(* reg: o_move_eq(reg,rc) *),
	 BurgNT.Rule{ 101 }	(* stm: o_storer(constaddr,reg) *),
	 BurgNT.Rule{ 100 }	(* stm: o_storer(addr,reg) *),
	 BurgNT.Rule{ 97 }	(* reg: o_mod(reg,reg) *),
	 BurgNT.Rule{ 96 }	(* reg: o_mod(lconst,reg) *),
	 BurgNT.Rule{ 94 }	(* reg: o_mod(reg,reg) *),
	 BurgNT.Rule{ 93 }	(* reg: o_rem(reg,reg) *),
	 BurgNT.Rule{ 92 }	(* reg: o_div(reg,reg) *),
	 BurgNT.Rule{ 89 }	(* reg: o_div(reg,reg) *),
	 BurgNT.Rule{ 88 }	(* reg: o_dvd(reg,reg) *),
	 BurgNT.Rule{ 87 }	(* reg: o_mulh(reg,reg) *),
	 BurgNT.Rule{ 86 }	(* reg: o_mul(const,reg) *),
	 BurgNT.Rule{ 85 }	(* reg: o_mul(reg,const) *),
	 BurgNT.Rule{ 84 }	(* reg: o_mul(const,reg) *),
	 BurgNT.Rule{ 83 }	(* reg: o_mul(reg,const) *),
	 BurgNT.Rule{ 82 }	(* reg: o_mul(const,reg) *),
	 BurgNT.Rule{ 81 }	(* reg: o_mul(reg,const) *),
	 BurgNT.Rule{ 80 }	(* reg: o_mul(const,reg) *),
	 BurgNT.Rule{ 79 }	(* reg: o_mul(reg,const) *),
	 BurgNT.Rule{ 78 }	(* reg: o_mul(reg,reg) *),
	 BurgNT.Rule{ 77 }	(* reg: o_shift(reg,rc) *),
	 BurgNT.Rule{ 75 }	(* reg: o_andnot(reg,reg) *),
	 BurgNT.Rule{ 74 }	(* reg: o_logical(rc,reg) *),
	 BurgNT.Rule{ 73 }	(* reg: o_logical(reg,rc) *),
	 BurgNT.Rule{ 71 }	(* reg: o_sub(const,reg) *),
	 BurgNT.Rule{ 70 }	(* reg: o_sub(reg,const) *),
	 BurgNT.Rule{ 69 }	(* reg: o_sub(reg,reg) *),
	 BurgNT.Rule{ 68 }	(* reg: o_add(const,reg) *),
	 BurgNT.Rule{ 67 }	(* reg: o_add(reg,const) *),
	 BurgNT.Rule{ 66 }	(* reg: o_add(reg,reg) *),
	 BurgNT.Rule{ 65 }	(* stm: o_call(reg,plist) *),
	 BurgNT.Rule{ 64 }	(* local: o_call(reg,plist) *),
	 BurgNT.Rule{ 63 }	(* reg: o_call(reg,plist) *),
	 BurgNT.Rule{ 59 }	(* plist: o_comma(reg,plist) *),
	 BurgNT.Rule{ 20 }	(* indexed: o_add(reg,reg) *):
		kids[0] := n.l;
		kids[1] := n.r;

	|BurgNT.Rule{ 168 }	(* stm: o_eq(o_par,ctr) *) ,
	 BurgNT.Rule{ 95 }	(* reg: o_mod(o_par,reg) *),
	 BurgNT.Rule{ 91 }	(* reg: o_div(o_par,reg) *),
	 BurgNT.Rule{ 90 }	(* reg: o_div(o_par,reg) *),
	 BurgNT.Rule{ 62 }	(* stm: o_call(o_par,plist) *),
	 BurgNT.Rule{ 61 }	(* local: o_call(o_par,plist) *),
	 BurgNT.Rule{ 60 }	(* reg: o_call(o_par,plist) *),
	 BurgNT.Rule{ 39 }	(* stm: o_storer(o_par,reg) *),
	 BurgNT.Rule{ 24 }	(* indexed: o_add(o_par,reg) *),
	 BurgNT.Rule{ 22 }	(* indexed: o_add(o_par,reg) *):
		kids[0] := n.r;

	|BurgNT.Rule{ 156 }	(* reg: o_fbin(o_fbin(reg,reg),reg) *) ,
	 BurgNT.Rule{ 153 }	(* reg: o_fbin(o_fbin(reg,reg),reg) *),
	 BurgNT.Rule{ 121 }	(* stm: o_copy(o_comma(reg,reg),lconst) *),
	 BurgNT.Rule{ 120 }	(* stm: o_copy(o_comma(reg,reg),const) *),
	 BurgNT.Rule{ 119 }	(* stm: o_copy(o_comma(reg,reg),reg) *):
		kids[0] := n.l.l;
		kids[1] := n.l.r;
		kids[2] := n.r;

	|BurgNT.Rule{ 148 }	(* reg: o_funary(o_funary(reg)) *) :
		kids[0] := n.l.l;

	|BurgNT.Rule{ 162 }	(* reg: o_funary(o_fbin(o_fbin(reg,reg),reg)) *) ,
	 BurgNT.Rule{ 159 }	(* reg: o_funary(o_fbin(o_fbin(reg,reg),reg)) *):
		kids[0] := n.l.l.l;
		kids[1] := n.l.l.r;
		kids[2] := n.l.r;

	|BurgNT.Rule{ 164 }	(* reg: o_funary(o_fbin(reg,fmul)) *) ,
	 BurgNT.Rule{ 163 }	(* reg: o_funary(o_fbin(fmul,reg)) *),
	 BurgNT.Rule{ 161 }	(* reg: o_funary(o_fbin(reg,fmul)) *),
	 BurgNT.Rule{ 160 }	(* reg: o_funary(o_fbin(fmul,reg)) *):
		kids[0] := n.l.l;
		kids[1] := n.l.r;

    END;
END NTkids;
CONST MAXNKIDS *= 3;

TYPE NTarity_type = ARRAY BurgNT.OpRange OF SHORTINT;
CONST NTarity*= NTarity_type {
	0,	(* 0 *)
	0,	(* 1 *)
	0,	(* 2 *)
	2,	(* 3=o_copy *)
	1,	(* 4=o_val *)
	1,	(* 5=o_cast *)
	0,	(* 6 *)
	0,	(* 7 *)
	0,	(* 8 *)
	0,	(* 9 *)
	0,	(* 10 *)
	2,	(* 11=o_add *)
	2,	(* 12=o_mul *)
	2,	(* 13=o_mulh *)
	2,	(* 14=o_div *)
	2,	(* 15=o_dvd *)
	2,	(* 16=o_mod *)
	2,	(* 17=o_rem *)
	0,	(* 18 *)
	0,	(* 19 *)
	0,	(* 20 *)
	2,	(* 21=o_andnot *)
	0,	(* 22 *)
	1,	(* 23=o_not *)
	2,	(* 24=o_incl *)
	2,	(* 25=o_excl *)
	1,	(* 26=o_loset *)
	1,	(* 27=o_hiset *)
	0,	(* 28 *)
	0,	(* 29 *)
	0,	(* 30 *)
	0,	(* 31 *)
	0,	(* 32 *)
	1,	(* 33=o_sgnext *)
	0,	(* 34=o_bf_get *)
	0,	(* 35=o_bf_put *)
	2,	(* 36=o_call *)
	0,	(* 37=o_ret *)
	1,	(* 38=o_checknil *)
	2,	(* 39=o_checklo *)
	2,	(* 40=o_checkhi *)
	1,	(* 41=o_stop *)
	1,	(* 42=o_error *)
	0,	(* 43 *)
	0,	(* 44 *)
	1,	(* 45=o_loadr *)
	2,	(* 46=o_storer *)
	0,	(* 47 *)
	0,	(* 48 *)
	2,	(* 49=o_eq *)
	2,	(* 50=o_le *)
	0,	(* 51 *)
	2,	(* 52=o_in *)
	0,	(* 53=o_odd *)
	1,	(* 54=o_case *)
	0,	(* 55=o_goto *)
	0,	(* 56=o_getpar *)
	2,	(* 57=o_move_eq *)
	2,	(* 58=o_move_le *)
	1,	(* 59=o_alloca *)
	1,	(* 60=o_neg *)
	2,	(* 61=o_sub *)
	1,	(* 62=o_putpar *)
	2,	(* 63=o_comma *)
	0,	(* 64=o_par *)
	1,	(* 65=o_retfun *)
	2,	(* 66=o_shift *)
	2,	(* 67=o_logical *)
	2,	(* 68=o_fbin *)
	1,	(* 69=o_funary *)
	2,	(* 70=o_fle *)
	2,	(* 71=o_feq *)
	0,	(* 72 *)
	0,	(* 73 *)
	0,	(* 74 *)
	0,	(* 75 *)
	0,	(* 76 *)
	0,	(* 77 *)
	0,	(* 78 *)
	0,	(* 79 *)
	0,	(* 80=o_empty *)
	0,	(* 81=o_neg_abs *)
	0 	(* 82=o_base *)
};

END BurgTables.
