<* IF ~TARGET_LLVM THEN *> This module can be used only in TARGET_PPC <* END *>
MODULE ir_D;

IMPORT ir_def;
IMPORT rgs := PPCreg;

--------------------------------------------------------------------------------
TYPE
  LCNum *= ir_def.LCNum;

  Param *= RECORD
    lcnum  *: LCNum;
    add2lc *: LCNum;
    dreg   *: rgs.REG;
  END;
  ParamPtr *= POINTER TO Param;

--------------------------------------------------------------------------------
CONST 
  UNDEFINED *= VAL(LCNum, -1);

--------------------------------------------------------------------------------
PROCEDURE (p: ParamPtr) Init *();
BEGIN
  p.lcnum := UNDEFINED;
  p.add2lc:= UNDEFINED;
  p.dreg  := rgs.UNDEF_REG;
END Init;

END ir_D.
