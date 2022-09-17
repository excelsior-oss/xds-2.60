--------------------------------------------------------------------------------
--                       Excelsior PowerPC Native Compiler
--                          (c) 2015, Excelsior LLC.
-- Module:   RDefs_D
-- Mission:  Type definitions for PowerPC Code Generation
-- Synonym:  rdd
-- Authors:  Kochetov Dmitry, Lvov Konstantin 
-- Created:  18-Feb-1997
--------------------------------------------------------------------------------
<* IF ~TARGET_PPC AND ~TARGET_LLVM THEN *> This module can be used only in TARGET_PPC or TARGET_LLVM <* END *>
MODULE RDefs_D;

IMPORT sys := SYSTEM;
IMPORT rgs := PPCreg;    
IMPORT ir;

FROM  PPCreg IMPORT RegSet,  RegSet0;

--------------------------------------------------------------------------------
--                    Foreign Module Globals Redefinition                     --
--------------------------------------------------------------------------------
TYPE  
  RegNum  *= rgs.REG;
  Reg     *= RegNum;

--------------------------------------------------------------------------------
TYPE  
  RegField    *= rgs.CRF;
  RegFieldSet *= rgs.CRFSet;

CONST 
  RF0 *= rgs.NoCRFs;

--------------------------------------------------------------------------------
TYPE  
  PosInSegm *= LONGINT;

CONST
  UNDEF_PosInSegm *= PosInSegm{ -1 };


--------------------------------------------------------------------------------
--      Implementation of Abstract Type "Data Used/Changed by Instruction"    --
--------------------------------------------------------------------------------
TYPE 
  IData *= RECORD
    reg *: RegSet;      -- GPR | FPR | SPR registers
    rfl -: RegFieldSet; -- SPR registers
    mem *: BOOLEAN;     -- memory access
  END;

--------------------------------------------------------------------------------
PROCEDURE (VAR data: IData) InclRegField *(reg_field: RegField);
BEGIN
  INCL(data.rfl, reg_field);
  INCL(data.reg, rgs.CRF_Reg[reg_field]);
END InclRegField;   


--------------------------------------------------------------------------------
PROCEDURE InitIData *(): IData;
VAR res: IData;
BEGIN
  res.reg := RegSet0;
  res.rfl := RF0;
  res.mem := FALSE;
  RETURN res;
END InitIData;

--------------------------------------------------------------------------------
PROCEDURE UnitIData *(dt1-, dt2-: IData): IData;
VAR res: IData;
BEGIN
  res.reg := dt1.reg + dt2.reg;
  res.rfl := dt1.rfl + dt2.rfl;
  res.mem := dt1.mem OR dt2.mem;
  RETURN res;
END UnitIData;

--------------------------------------------------------------------------------
PROCEDURE IntersectIData *(dt1-, dt2-: IData): IData;
VAR res: IData;
BEGIN
  res.reg := dt1.reg * dt2.reg;
  res.rfl := dt1.rfl * dt2.rfl;
  res.mem := dt1.mem & dt2.mem;
  RETURN res;
END IntersectIData;

--------------------------------------------------------------------------------
PROCEDURE isIDataIntersect *(d1-, d2-: IData): BOOLEAN;
BEGIN
  RETURN ((d1.reg * d2.reg) # RegSet0)
      OR ((d1.rfl * d2.rfl) # RF0)
      OR (d1.mem & d2.mem);
END isIDataIntersect;

--------------------------------------------------------------------------------
PROCEDURE isIDataEmpty *(dt-: IData): BOOLEAN;
BEGIN
  RETURN (dt.reg = RegSet0) & (dt.rfl = RF0) & ~dt.mem;
END isIDataEmpty;

--------------------------------------------------------------------------------
--                           Registers Contents                               --
--------------------------------------------------------------------------------
TYPE
  VCVal *= LONGINT;
  VCKind *= (
    _VAR     -- variable
  , _LCONST  -- long constant
  , _SCONST  -- short constant
  , _UNDEF   -- undefined
  );
  VCKindSet *= PACKEDSET OF VCKind;

  -- Variable or LongConst or ShortConst
  VC *= RECORD
    ckind *: VCKind;  -- contents kind:  Var_VC     LConst_VC  SConst_VC
    v     *: VCVal;   -- value        :  ir.VarNum  ir.LCNum   value
  END;

  -- Operand info 
  Register *= RECORD (VC)       
    r *: RegNum;   -- if r = UNDEF_REG: var# (>=0) or LC# (<0) 
  END;             -- UNDEF_REG | ZERO_REG | REG#

--------------------------------------------------------------------------------
CONST
  UNDEF_VC     *= VC{_UNDEF,  ir.UNDEFINED};
  UNDEF_VAR    *= VC{_VAR,    ir.UNDEFINED};
  UNDEF_LCONST *= VC{_LCONST, ir.LCUNDEFINED};
  UNDEF_SCONST *= VC{_SCONST, ir.UNDEFINED};

CONST
  CONSTANT *= VCKindSet{_LCONST, _SCONST};

-- Virtual Register Definition for RDef.ob2
CONST
  SPILLED_REG *= rgs.SPILLED_REG;
  UNDEF_REG   *= rgs.UNDEF_REG;

--------------------------------------------------------------------------------
PROCEDURE (VAR vc: VC) InitVC *( ckind := _UNDEF: VCKind
                               ; v := ir.UNDEFINED: VCVal );
BEGIN
  vc.ckind := ckind;
  vc.v     := v;
END InitVC;

--------------------------------------------------------------------------------
PROCEDURE (VAR vc: VC) getLCNum *(): ir.LCNum;
BEGIN
  ASSERT( vc.ckind IN CONSTANT );
  RETURN VAL(ir.LCNum, vc.v);
END getLCNum;

--------------------------------------------------------------------------------
PROCEDURE (VAR vc: VC) getVarNum *(): ir.VarNum;
BEGIN
  ASSERT( vc.ckind = _VAR );
  RETURN VAL(ir.VarNum, vc.v);
END getVarNum;

--------------------------------------------------------------------------------
PROCEDURE (VAR vc: VC) isEqual *(obj-: VC): BOOLEAN;
BEGIN
  RETURN (vc.ckind = obj.ckind) & (vc.v = obj.v);
END isEqual;

--------------------------------------------------------------------------------
PROCEDURE (VAR vc: VC) isUndefined *(): BOOLEAN;
BEGIN
  RETURN UNDEF_VC.isEqual(vc);
END isUndefined;


--------------------------------------------------------------------------------
PROCEDURE (VAR this: Register) SetReg *(r: Reg);
BEGIN
  this.r := r;  
END SetReg;

--------------------------------------------------------------------------------
PROCEDURE (VAR this: Register) InitVCReg *( r := UNDEF_REG   : Reg 
                                          ; ckind := _UNDEF  : VCKind
                                          ; v := ir.UNDEFINED: VCVal );
BEGIN
  this.InitVC(ckind, v);
  this.SetReg(r);  
END InitVCReg;


--------------------------------------------------------------------------------
--              DAG STRUCTURE AND LOCATIONS DEFINITION                        --
--------------------------------------------------------------------------------
TYPE
  Offset *= LONGINT;

  -- Addressing mode info
  AddrMode *= RECORD 
    base    *: Register;     -- base opnd
    index   *: Register;     -- index opnd
    offs    *: Offset;       -- offs disp
    bsconst *: ir.ParamPtr;  -- short constant to form 'base' 
    isconst *: ir.ParamPtr;  -- short constant to form 'index'
    align   *: ir.AlignType;
  END;

  FAddrMode *= AddrMode;

  --- DAG node descriptor
  TREE *= RECORD
    place *: Register;
    dreg  *: Reg;           -- desired reg for tree result alloc | risc
    a     *: AddrMode;      -- addressing mode
    fa    *: AddrMode;      -- loc=IN_FMUL
    align *: ir.AlignType;  -- operation alignment 
  END;

  -- Type Definition for RDef.ob2
  Contents *= ARRAY Reg OF ir.VarNum;

--------------------------------------------------------------------------------
--                         Current Registers Contents                         --
--------------------------------------------------------------------------------
TYPE
  ContentsArr *= ARRAY Reg OF VC;

VAR
  FreeRegs  *: RegSet;                   -- Free registers
  RContents -: ContentsArr;              -- Current stat of registers
  Counters  *: ARRAY Reg OF sys.CARD32;  -- For allocated registers: counter of using
  
  UsedRegs    *: RegSet;  -- What registers were used during compilation up to the moment
  OldUsedRegs *: RegSet;  -- What registers were used during compilation by previous iteration
  
  LastFoundReg *: rgs.RegSet; -- last allocated register or registers pair

--------------------------------------------------------------------------------
PROCEDURE SetRegContent *(r: Reg; obj-: VC; live: BOOLEAN);
BEGIN
  RContents[r] := obj;
  IF live THEN
    EXCL(FreeRegs, r);
    INCL(UsedRegs, r);
  END;
END SetRegContent;

--------------------------------------------------------------------------------
PROCEDURE AllocateRegister *(obj-: VC; r: Reg; count: sys.CARD32);
BEGIN
  SetRegContent(r, obj, TRUE);
  Counters[r] := count;
END AllocateRegister;

-------------------------------------------------------------------------------
-- Clears information about contents of register 'r'
PROCEDURE CleanRegContent *(r: Reg);
BEGIN
  RContents[r] := UNDEF_VC;
END CleanRegContent;

--------------------------------------------------------------------------------
PROCEDURE FreeRegister *(r: Reg); 
BEGIN 
  INCL(FreeRegs, r);
  CleanRegContent(r);
  Counters[r] := 0;
END FreeRegister; 

-------------------------------------------------------------------------------
-- Clears all information about all register
PROCEDURE FreeAllRegisters *(all_allowed_regs-: RegSet);
VAR r: Reg;
BEGIN
  FOR r := MIN(Reg) TO MAX(Reg) DO
    FreeRegister(r);
  END;
  FreeRegs := all_allowed_regs;
END FreeAllRegisters;


--------------------------------------------------------------------------------
--                          Node Information                                  --
--------------------------------------------------------------------------------
TYPE
 NodeInfoType * =  RECORD
   offs *: LONGINT;        -- offset from procedure start
   c    *: ContentsArr;    -- Regs status at node exit
   j    *: rgs.JumpCond;   -- jump condition
   j2   *: BOOLEAN;        -- uncond.jump follows cond.branch?
   l1   *: SHORTINT;
   l2   *: SHORTINT;       -- number of instructions to generate jump
   a    *: SHORTINT;       -- what exit edge (0/1) is 1st jump to?
   l    *: LONGINT;        -- alignment: 0..3 (now always 0)
   pos  *: ir.TPOS;        -- position in source code
   calls *: LONGINT;       -- number of procedure calls in node
 END;

--------------------------------------------------------------------------------
PROCEDURE (VAR node_info: NodeInfoType) ClearNodeExitInfo *();
VAR r: Reg;
BEGIN
  FOR r := MIN(Reg) TO MAX(Reg) DO
    node_info.c[r] := UNDEF_VC;
  END;
END ClearNodeExitInfo;


--------------------------------------------------------------------------------
--                         Current Codegeneration States                      --
--------------------------------------------------------------------------------
VAR
  Iteration *: BOOLEAN;
  DRegs     *: POINTER TO ARRAY (* @indexby: ir.VarNum *) OF RegNum;
  BURG_Iteration *: BOOLEAN;

END RDefs_D.
