<* IF NOT TARGET_LLVM THEN *> This module can be used only in TARGET_LLVM <* END *>
MODULE opTune_I;

IMPORT  pc := pcK;
IMPORT plt := xmPlatform;

IMPORT ir;
IMPORT opTune;


--------------------------------------------------------------------------------
TYPE
  opTune_LLVM_IDB = POINTER TO RECORD (opTune.opTune_IDB_Rec) END;
    
--------------------------------------------------------------------------------
-- ����� �� ��� "������᭮�" 㬭������ �������� ᤢ����� � ᫮����ﬨ. 
PROCEDURE (idb: opTune_LLVM_IDB)
          NeedAdd (v: ir.VALUE; t#: ir.TypeType; s: ir.SizeType): BOOLEAN;
BEGIN
  RETURN FALSE;
END NeedAdd;

--------------------------------------------------------------------------------
-- ����� �� ��� SR 㬭������ �� ��� 楫�� ����⠭�� 
PROCEDURE (idb: opTune_LLVM_IDB)
          NeedSR (v#: ir.VALUE; t#: ir.TypeType; s#: ir.SizeType): BOOLEAN;
BEGIN
  RETURN FALSE;
END NeedSR;

--------------------------------------------------------------------------------
-- ����� ��ࢮ�� ������ - ����� �뢠�� 
PROCEDURE (idb: opTune_LLVM_IDB) MinLocalStart (proc#: pc.OBJECT): LONGINT;
BEGIN
  RETURN 0;
END MinLocalStart;

--------------------------------------------------------------------------------
VAR IDB: opTune_LLVM_IDB;
BEGIN
  NEW(IDB);
  IDB.ld_real_sz := 8;
  opTune.IDB     := IDB;
  opTune.BIG_END := plt.targetBigendian;
END opTune_I.
