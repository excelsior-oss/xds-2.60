-- source grammar = eppc.b
<*+WOFF*>
MODULE BurgNT;
IMPORT ir, SYSTEM;
TYPE NT *= (
     NTnowhere,
     NTstm,
     NTconst,
     NTlconst,
     NTreg,
     NTrc,
     NTbased,
     NTindexed,
     NTaddr,
     NTconstaddr,
     NTmem,
     NTlocal,
     NTplist,
     NTfmul,
     NTctr
     );
TYPE
	CostArray   *= ARRAY NT OF INTEGER;
	NTSet       *= PACKEDSET OF NT;
	Rule        *= INTEGER;
	RuleArray   *= ARRAY NT OF Rule;
	OpRange     *= ir.Operation[ir.o_invalid..ir.o_base];

<* IF ~nodebug THEN *>
TYPE ArrayOfString = ARRAY NT OF ARRAY 20 OF CHAR;
CONST
    NTName* = ArrayOfString {
      'XXX',
      'stm',
      'const',
      'lconst',
      'reg',
      'rc',
      'based',
      'indexed',
      'addr',
      'constaddr',
      'mem',
      'local',
      'plist',
      'fmul',
      'ctr'
    };
<* END *>

END BurgNT.
