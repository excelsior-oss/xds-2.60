--------------------------------------------------------------------------------
--                       Excelsior PowerPC Native Compiler
--                          (c) 2015, Excelsior, LLC.
-- Module:   ABI
-- Mission:  Aplication Binary Interface (ABI) Description
-- Synonym:  abi
-- Authors:  Lvov Konstantin
-- Created:  27-Jul-2000
--
-- NOTE: All definitions are given in terms of the cutrrent register window.
--------------------------------------------------------------------------------
<* +o2addkwd *>
MODULE ABI;

FROM SYSTEM IMPORT PRED, SUCC;
FROM PPCreg IMPORT Reg, RegSet, RegSet0;

IMPORT   ir;
IMPORT   at := opAttrs;
IMPORT  prc := opProcs;
IMPORT tune := opTune;

IMPORT rgs := PPCreg;

TYPE  RegNum  *= rgs.REG;

CONST NRegs   *= rgs.N_REGs;

--------------------------------------------------------------------------------
CONST
  WORD_SIZE  *= tune.word_sz;  -- bytes;
  ADDR_SIZE  *= tune.addr_sz;  -- bytes;
  DWORD_SIZE *= tune.dword_sz; -- bytes;

  MAX_ALIGNMENT *= tune.max_alignment; -- bytes;

--------------------------------------------------------------------------------
--                  Register Usage Calling Conventions                        --
--------------------------------------------------------------------------------
CONST
  SP *= rgs.GPR1;        -- stack pointer

  FP     *= rgs.GPR30;   -- frame pointer
  OLD_SP *= rgs.GPR29;   -- previous frame pointer, used for stacks frames with dynamic length
  
  -- Registers are reserved for system usage by POwerPC EABI.
  -- GPR1 is dedicated as the stack frame pointer (SP). 
  -- GPR2 is dedicated for use as a base pointer (anchor) for the read-only small data area. 
  -- GPR13 is dedicated for use as an anchor for addressing the read-write small data area. 
  -- Dedicated registers should never be used for any other purpose, not even 
  -- temporarily, because they may be needed by an exception handler at any time.
  SYS_REGs *= RegSet{SP, rgs.GPR2, rgs.GPR13};

--------------------------------------------------------------------------------
CONST
  LOCAL_TOC *= rgs.GPR31;  -- pointer to current module TOC

--------------------------------------------------------------------------------
CONST
  FirstReg *= rgs.GPR3;
  LastReg  *= rgs.LAST_FPR;

--------------------------------------------------------------------------------
VAR
  BASE_REG -: RegNum;  -- SP or FP


--------------------------------------------------------------------------------
--              Temporary registers, available only in emitter                --
--------------------------------------------------------------------------------
CONST
  TMP_GPR0  *= rgs.GPR11;
  TMP_GPR1  *= rgs.GPR12;
  TMP_GPR2  *= rgs.GPR15;
  TMP_FPR   *= rgs.FPR0;

  TMP_GPRs *= RegSet{TMP_GPR0, TMP_GPR1, TMP_GPR2}; -- temporary GPR registers
  TMP_FPRs *= RegSet{TMP_FPR};                      -- temporary FPR registers
  TMP_REGs *= TMP_GPRs + TMP_FPRs;                  -- all temporary registers

--------------------------------------------------------------------------------
--                    Volatile/NonVolatile Register                           --
--------------------------------------------------------------------------------
CONST
  -- Auxiliary registers with their own save/restore rules.
  AUX_REGs *= SYS_REGs + TMP_REGs + RegSet{rgs.ZERO_REG};

--------------------------------------------------------------------------------
CONST
  VOLATILE_GPR_REGs    *= RegSet{rgs.GPR0, rgs.GPR3..rgs.GPR12};
  NONVOLATILE_GPR_REGs *= RegSet{rgs.GPR14..rgs.GPR31};

  GPR_VOLATILE_REGS_NUMBER    = 1 + ORD(rgs.GPR12) - ORD(rgs.GPR3) + 1;  -- must be 11
  GPR_NONVOLATILE_REGS_NUMBER = ORD(rgs.GPR31) - ORD(rgs.GPR14) + 1;     -- must be 18

CONST
  VOLATILE_FPR_REGs      *= RegSet{rgs.FPR0..rgs.FPR13};
  NONVOLATILE_FPR_REGs   *= RegSet{rgs.FPR14..rgs.FPR31};

  FPR_VOLATILE_REGS_NUMBER    = ORD(rgs.FPR13) - ORD(rgs.FPR0) + 1;   -- must be 14
  FPR_NONVOLATILE_REGS_NUMBER = ORD(rgs.FPR31) - ORD(rgs.FPR14) + 1;  -- must be 18

CONST
  VOLATILE_REGs    *= VOLATILE_GPR_REGs    + VOLATILE_FPR_REGs;
  NONVOLATILE_REGs *= NONVOLATILE_GPR_REGs + NONVOLATILE_FPR_REGs;

CONST
  VOLATILE_REGS_SIZE *= GPR_VOLATILE_REGS_NUMBER * rgs.GPR_SIZE 
                      + FPR_VOLATILE_REGS_NUMBER * rgs.FPR_SIZE;

--------------------------------------------------------------------------------
CONST
  AllFPRs *= rgs.FPRs - TMP_FPRs;  -- Floating-Point registers available to register allocation

VAR
  AllGPRs    -: RegSet;  -- General-Purpose registers available to register allocation
  AllRegs    -: RegSet;  -- All registers available to register allocation
  CurAuxRegs -: RegSet;

--------------------------------------------------------------------------------
PROCEDURE AllRegsINCL *(r: Reg); 
BEGIN 
  ASSERT( r = OLD_SP );
  INCL(AllGPRs, r);
  INCL(AllRegs, r);
  EXCL(CurAuxRegs, r); 
END AllRegsINCL; 


--------------------------------------------------------------------------------
--                    Register to Parameter Passing                           --
--------------------------------------------------------------------------------
CONST
  -- GPR parameter area
  FIRST_GPR_PAR_REG *= rgs.GPR3;     
  LAST_GPR_PAR_REG  *= rgs.GPR10;
  GPR_PAR_REGs      *= RegSet{FIRST_GPR_PAR_REG..LAST_GPR_PAR_REG};

  -- Registers to return general-purpose value
  GPR_FUN_RES_REG *= rgs.GPR3;   

--------------------------------------------------------------------------------
CONST
  -- FPR parameter area
  FIRST_FPR_PAR_REG *= rgs.FPR1;     
  
  -- Registers to return floating-point value
  FPR_FUN_RES_REG *= rgs.FPR1;

--------------------------------------------------------------------------------
VAR
  LAST_FPR_PAR_REG -: RegNum;
  FPR_PAR_REGs -: RegSet;
  ALL_PAR_REGs -: RegSet;

--------------------------------------------------------------------------------
-- @return: register for function result  
PROCEDURE AllowedReg4FuncRes *(proto_no: prc.ProtoNum): RegNum;
BEGIN
  RETURN VAL(RegNum, prc.ProtoList[proto_no].where_res);
END AllowedReg4FuncRes;

--------------------------------------------------------------------------------
-- Recalculates procedure prototype to support calling conventions.
-- @param[in] proto_no - id of procedure prototype to be recalculated.
PROCEDURE EvalProto (proto_no: prc.ProtoNum);
VAR proto: prc.Proto;
    
   -- 1 -- EvalProto -----------------------------------------------------------
   PROCEDURE SetParamLocation(VAR par: prc.param; VAR reg: RegNum; allowed_regs-: RegSet); 
   VAR dual_reg_par: BOOLEAN;
   BEGIN 
     dual_reg_par := rgs.isDualRegSize(par.size, rgs.isRegGPR(reg));
     IF dual_reg_par & NOT (reg IN rgs.DUAL_REGs) THEN
       reg := SUCC(reg);
     END;
     IF (reg IN allowed_regs) THEN
       par.where := VAL(prc.MemType, reg);
       INCL(proto.par_regs, reg);
       reg := SUCC(reg);
       IF (reg IN allowed_regs) & dual_reg_par THEN
         INCL(proto.par_regs, reg);
         reg := SUCC(reg);
       END; 
     ELSE  
       par.where := prc.STACK;  -- parameter located in the stack frame
     END;
   END SetParamLocation; 

-- 0 -- EvalProto --------------------------------------------------------------
VAR pos: LONGINT;
    fpr, gpr: RegNum;
BEGIN
  proto := prc.ProtoList[proto_no];
  proto.par_regs := RegSet0; 
  
  -- recalculate location of return value 
  CASE proto.ret_type OF
  | ir.t_void:   proto.where_res := prc.STACK;
  | ir.t_RR
  , ir.t_float:  proto.where_res := VAL(prc.MemType, FPR_FUN_RES_REG);
  ELSE           proto.where_res := VAL(prc.MemType, GPR_FUN_RES_REG);
  END;
  
  pos  := 0;
  gpr  := FIRST_GPR_PAR_REG;
  fpr  := FIRST_FPR_PAR_REG;

  LOOP
    IF (pos = proto.npar) OR (proto.par[pos].mode = prc.pm_seq) THEN
      EXIT;
    END;
    
    CASE proto.par[pos].type OF
    | ir.t_RR
    , ir.t_float:  
        SetParamLocation(proto.par[pos], fpr, FPR_PAR_REGs);
    ELSE           
        SetParamLocation(proto.par[pos], gpr, GPR_PAR_REGs);
    END;
    
    INC(pos);
    IF ~(gpr IN GPR_PAR_REGs) AND ~(fpr IN FPR_PAR_REGs) THEN 
      EXIT; 
    END;
  END;
  
  WHILE pos < prc.ProtoList[proto_no].npar DO
    proto.par[pos].where := prc.STACK;  -- parameter located in the stack frame
    INC(pos);
  END;
END EvalProto;

--------------------------------------------------------------------------------
--                       Stack Frame Definition                               --
--------------------------------------------------------------------------------
(* The conventional stack frame appearance for PPC PowerOpen ABI is:
 ³ CALLER'S FRAME  ³
 ³                 ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-16  old SP, SAVEDREG_AREA_START = 0
 ³  FPR save area  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-8
 ³  GPR save area  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-4   = LOCAL_AREA_START = - SAVEDREG_AREA_SIZE
 ³     Locals      ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-4   = TEMP_AREA_START = LOCAL_AREA_START - LOCAL_AREA_SIZE
 ³_alignment_bytes_³
 ³    Temp area    ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-4
 ³ Parameter area  ³
 ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ´ <-8
 ³  Linkage area   ³
 ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ <-16  = new SP
 (stack grows down)
   Frame is built (SP is updated) by callee
  ------ 0 -------

 The conventional stack frame appearance for PPC eabi/v4abi is:
    SP-> +---------------------------------------+
         | back chain to caller                  | 0
         +---------------------------------------+
         | saved LR                              | 4
         +---------------------------------------+
         | Parameter save area (P)               | 8
         +---------------------------------------+
         ///////////////////////////////////////// -- possible padding bytes (pb)
         +---------------------------------------+
         | Alloca space (A)                      | 8+P+pb
         +---------------------------------------+
         | Local variable space (L)              | 8+P+A+pb
         +---------------------------------------+
         | saved CR (C)                          | 8+P+A+L+pb
         +---------------------------------------+
         | Save area for GP registers (G)        | 8+P+A+L+C+pb
         +---------------------------------------+
         | Save area for FP registers (F)        | 8+P+A+L+C+G+pb
         +---------------------------------------+
 old SP->| back chain to caller's caller         |
         +---------------------------------------+
*)

(* STARTS consts are defined in terms of offset with respect to old SP *)

--------------------------------------------------------------------------------
VAR
  STACK_ALIGNMENT     -: ir.AlignType;
  LINKAGE_AREA_SIZE   -: LONGINT;
  MIN_PARAM_AREA_SIZE -: LONGINT;

CONST
  SAVEDREG_AREA_START *= 0;
  SAVEDREG_AREA_SIZE  *= GPR_NONVOLATILE_REGS_NUMBER * rgs.GPR_SIZE
                       + FPR_NONVOLATILE_REGS_NUMBER * rgs.FPR_SIZE;
VAR
   LOCAL_AREA_START *: LONGINT;
   LOCAL_AREA_SIZE  *: LONGINT;

VAR
  TEMP_AREA_START  *: LONGINT;
  TEMP_AREA_SIZE   *: LONGINT;

CONST
  PARAM_AREA_ALIGNMENT *= 8;

VAR
  PARAM_AREA_SIZE  *: LONGINT;
  FRAME_SIZE       *: LONGINT;  -- size of procedure frame,


--------------------------------------------------------------------------------
VAR
  ProcABI *: at.AbiType;  -- ABI ãåíåðèðóåìîé ïðîöåäóðû 


--------------------------------------------------------------------------------
--                           Traps Handlers                                   --
--------------------------------------------------------------------------------
VAR 
  TrapSize -:  LONGINT;    -- number of instruction to invoke trap
  TrapReg  -:  RegNum;     -- register to passing error code to RTS


--------------------------------------------------------------------------------
--                       Initialization procedures                            --
--------------------------------------------------------------------------------
VAR  
  AllowedRegs -: ARRAY ir.TypeType OF RegSet;  -- registers allowed to data types 

--------------------------------------------------------------------------------
-- @param[im]: nonreserved_regs - all registers which aren't used for a special purpose.
PROCEDURE InitAllowedRegs (nonreserved_regs-: RegSet); 
VAR GPRs, FPRs: RegSet;
BEGIN 
  GPRs := rgs.GPRs * nonreserved_regs;
  FPRs := rgs.FPRs * nonreserved_regs;
  AllowedRegs[ir.t_void]    := GPRs;
  AllowedRegs[ir.t_int]     := GPRs;
  AllowedRegs[ir.t_unsign]  := GPRs;
  AllowedRegs[ir.t_float]   := FPRs;
  AllowedRegs[ir.t_complex] := GPRs;
  AllowedRegs[ir.t_ref]     := GPRs;
  AllowedRegs[ir.t_arr]     := GPRs;
  AllowedRegs[ir.t_rec]     := GPRs;
  AllowedRegs[ir.t_flxarr]  := GPRs;
  AllowedRegs[ir.t_ZZ]      := GPRs;
  AllowedRegs[ir.t_RR]      := FPRs;
END InitAllowedRegs; 

--------------------------------------------------------------------------------
-- Initialize working sets of registers wrt to current procedure's properties.
-- @param[in]: fp_need - Frame Pointer will be used in the current procedure.
PROCEDURE InitRegSets *(fp_need: BOOLEAN);
BEGIN
  CurAuxRegs := AUX_REGs;
  IF (at.use_toc IN at.COMP_MODE) THEN  
    INCL(CurAuxRegs, LOCAL_TOC); 
  END;
  IF fp_need THEN
    BASE_REG := FP;
    INCL(CurAuxRegs, FP); 
    INCL(CurAuxRegs, OLD_SP); 
  ELSE
    BASE_REG := SP;
  END;
  INCL(CurAuxRegs, BASE_REG); 

  AllGPRs := rgs.GPRs - CurAuxRegs;
  AllRegs := AllGPRs + AllFPRs;

  InitAllowedRegs(AllRegs);
END InitRegSets;

--------------------------------------------------------------------------------
PROCEDURE InitABI *();
BEGIN
  -- ABI of generated prcedure 
  ProcABI := at.ABI;
  ASSERT( ProcABI = at.GetABI(at.curr_proc) );
  
  -- trap emitter initialization
  IF (at.use_trap_cmd IN at.COMP_MODE) THEN
    TrapSize := 2;
    TrapReg  := rgs.GPR0;
  ELSE
    IF at.history IN at.COMP_MODE THEN
      TrapSize:= 2; -- additional commant to pass current value of LR as a argument
    ELSE
      TrapSize:= 1;
    END;
    TrapReg := FIRST_GPR_PAR_REG;
  END;

  InitRegSets(TRUE);  -- conservative assumption
END InitABI;

--------------------------------------------------------------------------------
PROCEDURE SetupABI *();
BEGIN
  CASE at.ABI OF
  | at.EABI:
      LINKAGE_AREA_SIZE   := 8;
      MIN_PARAM_AREA_SIZE := 0;
      STACK_ALIGNMENT     := 8;
      LAST_FPR_PAR_REG    := rgs.FPR8;
  END;

  tune.PARAM_START := LINKAGE_AREA_SIZE;      -- in terms of offset with respect to new SP
  tune.LOCAL_START := - SAVEDREG_AREA_SIZE;   -- in terms of offset with respect to FP (old SP)

  FPR_PAR_REGs := RegSet{FIRST_FPR_PAR_REG..LAST_FPR_PAR_REG};
  ALL_PAR_REGs := GPR_PAR_REGs + FPR_PAR_REGs;
END SetupABI;

--------------------------------------------------------------------------------
BEGIN
  ProcABI := at.StandardABI;
  InitAllowedRegs(rgs.GPRs + rgs.FPRs - AUX_REGs);
  prc.EvalProto := EvalProto;
END ABI.