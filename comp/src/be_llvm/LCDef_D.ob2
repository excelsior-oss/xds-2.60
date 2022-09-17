<* IF ~TARGET_PPC AND ~TARGET_LLVM THEN *> This module can be used only in TARGET_PPC <* END *>
MODULE LCDef_D;

IMPORT pc := pcK;
IMPORT ir;

--------------------------------------------------------------------------------
CONST
  UNKNOWN_UsageCount *= -1;

TYPE
  LCUseInfo *= POINTER TO LCUseInfoDesc;
  LCUseInfoDesc *= RECORD
    param *: ir.ParamPtr;
    next  *: LCUseInfo; 
  END;

  LCDesc *= RECORD
    aux_obj *: pc.OBJECT;   --  вспомогательный объект
    base    *: ir.LCNum;    --  номер базы для базированных констант
    usage   *: LONGINT;     --  число использований константы 
    use     *: LCUseInfo;   --  список мест где используется
  END;


--------------------------------------------------------------------------------
PROCEDURE (VAR d : LCDesc) Init *();
BEGIN
  d.aux_obj := NIL;
  d.base    := ir.LCUNDEFINED;
  d.usage   := 0;
  d.use     := NIL;
END Init;

--------------------------------------------------------------------------------
PROCEDURE (VAR d: LCDesc) AddUse *(param: ir.ParamPtr);
VAR use: LCUseInfo;
BEGIN
  NEW(use);
  use.param := param;
  use.next  := d.use;
  d.use     := use;
END AddUse;

--------------------------------------------------------------------------------
PROCEDURE (VAR d: LCDesc) GetUsageCount *(node: ir.Node): LONGINT;
VAR p: ir.ParamPtr;
    usage_info: LCUseInfo;
    usage_in_node: LONGINT;
BEGIN
  IF (d.use # NIL) THEN
    usage_in_node := 0;
    usage_info    := d.use;
    WHILE usage_info # NIL DO
      p := usage_info.param;
      IF (p.triade.NodeNo = node) & (p.triade.Op # ir.o_fi) THEN
        INC(usage_in_node);
      END;
      usage_info := usage_info.next;
    END;
  ELSE
    usage_in_node := UNKNOWN_UsageCount;
  END;
  RETURN usage_in_node;
END GetUsageCount;


END LCDef_D.
