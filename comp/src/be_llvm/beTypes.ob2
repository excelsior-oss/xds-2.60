--------------------------------------------------------------------------------
--                       Excelsior PowerPC Native Compiler
--                          (c) 2015, Excelsior LLC.
-- Module:   beTypes
-- Mission:  Back-End Specific Types and IR Access Operations
-- Synonym:  tps
-- Authors:  Lvov Konstantin
-- Created:  09-Jul-2015
--------------------------------------------------------------------------------
MODULE beTypes;

IMPORT sys := SYSTEM;

IMPORT ir;
IMPORT Calc;
IMPORT  bv := BitVect;
IMPORT  LC := LCDef;
IMPORT  rd := RDefs;
IMPORT prc := opProcs;

--------------------------------------------------------------------------------
CONST   
  UnsignedTypes *= ir.TypeTypeSet{ir.t_unsign, ir.t_ref};

--------------------------------------------------------------------------------
PROCEDURE In *(p-: bv.BitVector; e: LONGINT): BOOLEAN;
BEGIN
<* IF backend = "C" THEN *>
  RETURN bv.In(p, e);
<* ELSE *>
  RETURN ({e MOD bv.BitsPerSet} * p.v[e DIV bv.BitsPerSet]) # {};
<* END *>
END In;

--------------------------------------------------------------------------------
PROCEDURE Incl *(p: bv.BitVector; e: LONGINT);
BEGIN
<* IF backend = "C" THEN *>
  bv.Incl(p, e);
<* ELSE *>
  INCL (p.v[e DIV bv.BitsPerSet], e MOD bv.BitsPerSet);
<* END *>
END Incl;


--------------------------------------------------------------------------------
-- Определить естественное выравнивание значения. 
-- @param[in] value - значение для которого необходимо определить естественное выравнивание.
-- @param[in] max_align - верхняя граница для естественного выравнивания.
-- @return естественное выравнивание. 
PROCEDURE GetValueAlignment *(value: LONGINT; max_align: ir.AlignType): ir.AlignType; 
VAR align: ir.AlignType;
BEGIN
  align := max_align;
  WHILE (value MOD align) # 0 DO
    align := align DIV 2;
  END;
  RETURN align;
END GetValueAlignment; 

--------------------------------------------------------------------------------
-- Makes the value aligned. 
-- @param[in,out] value - value to be aligned.
-- @param[in] align - alignment to be applied.
PROCEDURE AlignValue *(VAR value: LONGINT; align: ir.AlignType);
BEGIN
  IF value > 0 THEN
    value :=   ((value + align  - 1) DIV align) * align;
  ELSIF value < 0 THEN
    value := -(((-value + align - 1) DIV align) * align);
  END;
END AlignValue;


--------------------------------------------------------------------------------
--      Functions to get types and sizes from triade parameter and DAG        --
--------------------------------------------------------------------------------

PROCEDURE GetParamType *(p: ir.ParamPtr): ir.TypeType;
BEGIN
  IF (p.tag IN LC.RealConstTags) THEN 
    RETURN ir.t_float
  ELSE                             
    RETURN ir.ParamType(p.triade, p.paramnumber);
  END;
END GetParamType;

--------------------------------------------------------------------------------
PROCEDURE GetSize *(p: rd.DAGNODE): ir.SizeType; 
VAR sz: ir.SizeType;
BEGIN 
  IF p.op = ir.o_par THEN
    sz := ir.ParamSize(p.par.triade, p.par.paramnumber);
  ELSE
    sz := p.tr.ResSize;
  END;
  RETURN sz;
END GetSize; 

--------------------------------------------------------------------------------
PROCEDURE CalcSizeType *( p: rd.DAGNODE
                        ; VAR sz: ir.SizeType
                        ; VAR type: ir.TypeType );
BEGIN
  IF p.op = ir.o_par THEN
    sz   := ir.ParamSize(p.par.triade, p.par.paramnumber);
    type := GetParamType(p.par);
  ELSE
    sz   := p.tr.ResSize;
    type := p.tr.ResType;
  END;
END CalcSizeType;


--------------------------------------------------------------------------------
--         Functions to get constant value from triade parameter              --
--------------------------------------------------------------------------------

PROCEDURE ToCardinal *(v: ir.VALUE; sz: ir.SizeType): LONGINT;
BEGIN
  RETURN sys.VAL(LONGINT, Calc.ToCardinal(v, sz));
END ToCardinal;

--------------------------------------------------------------------------------
PROCEDURE Par2Cardinal *(p: ir.ParamPtr): LONGINT;
BEGIN
  RETURN ToCardinal(p.value, ir.ParamSize(p.triade, p.paramnumber));
END Par2Cardinal;

--------------------------------------------------------------------------------
PROCEDURE ToInteger *(v: ir.VALUE; sz: ir.SizeType): LONGINT;
BEGIN
  IF Calc.IsNegative(v, sz) THEN 
    RETURN Calc.ToInteger(v, sz);
  ELSE                           
    RETURN sys.VAL(LONGINT, Calc.ToCardinal(v, sz));
  END;
END ToInteger;

--------------------------------------------------------------------------------
PROCEDURE Par2Integer *(p: ir.ParamPtr) : LONGINT;
BEGIN
  RETURN ToInteger(p.value, ir.ParamSize(p.triade, p.paramnumber));
END Par2Integer;


--------------------------------------------------------------------------------
-- Triade parameter to integer 32-bit value.
-- @param[in]: p - triade parameter      
PROCEDURE Par2Number *(p: ir.ParamPtr): LONGINT;
VAR type: ir.TypeType;
BEGIN
  CASE p.triade.Op OF
  | ir.o_call:
     type := prc.GetParamType(p);
  ELSE
     type := GetParamType(p);
  END;

  CASE type OF
  | ir.t_int
  , ir.t_void:
      RETURN Par2Integer(p);
  | ir.t_unsign
  , ir.t_ref:
      RETURN Par2Cardinal(p);
  END;
END Par2Number;


--------------------------------------------------------------------------------
TYPE
  -- Back-end representation of immediate value.  
  Immediate *= RECORD
    hi_word     -: LONGINT;  -- high word (63..32 bits) of immediate value
    lo_word     -: LONGINT;  -- low word (31..0 bits) of immediate value
    is_negative -: BOOLEAN;  -- is it negative value
  END;

--------------------------------------------------------------------------------
-- Sets immediate value by triade parameter. 
-- @param[in]: p - triade parameter      
PROCEDURE (VAR imm: Immediate) SetParamValue *(p: ir.ParamPtr);
BEGIN
  imm.hi_word     := p.value.get_NDWord(1);
  imm.lo_word     := Par2Number(p);
  imm.is_negative := p.value.is_neg(); 
END SetParamValue;

--------------------------------------------------------------------------------
-- Sets immediate value by 32-bit integer value. 
-- @param[in]: val - 32-bit integer value      
PROCEDURE (VAR imm: Immediate) SetInt32Value *(val: LONGINT);
BEGIN
  IF val >= 0 THEN  imm.hi_word := 0;
  ELSE              imm.hi_word := -1;
  END;
  imm.lo_word     := val;
  imm.is_negative := val < 0; 
END SetInt32Value;


--------------------------------------------------------------------------------
-- Triade parameter to intrenal immediate value.
-- @param[in]: p - triade parameter      
PROCEDURE Par2Immediate *(p: ir.ParamPtr): Immediate;
VAR imm: Immediate;
BEGIN
  imm.SetParamValue(p);
  RETURN imm;
END Par2Immediate;

--------------------------------------------------------------------------------
-- Interger 32-bit value to intrenal immediate value.
-- @param[in]: p - triade parameter      
PROCEDURE Int32Immediate *(val: LONGINT): Immediate;
VAR imm: Immediate;
BEGIN
  imm.SetInt32Value(val);
  RETURN imm;
END Int32Immediate;

--------------------------------------------------------------------------------
PROCEDURE isWholeType *(tp: ir.TypeType): BOOLEAN;
BEGIN
  RETURN NOT (tp IN ir.TypeTypeSet{ir.t_float, ir.t_RR});
END isWholeType;  

END beTypes.