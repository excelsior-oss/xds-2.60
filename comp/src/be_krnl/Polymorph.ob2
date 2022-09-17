(* initialization of target defined interface for target 386 *)
MODULE Polymorph;

<* IF NOT TARGET_LLVM THEN *>
IMPORT DAG_I;
<* END *>
IMPORT PrepGen_I;
IMPORT opTune_I;
<* IF NOT TARGET_LLVM THEN *>
IMPORT Iselect_I;
<* END *>
       
<* IF TARGET_386 THEN *>
IMPORT RDefs_I;
<* END *>
<* IF TARGET_RISC OR TARGET_SPARC OR TARGET_68k THEN *>
IMPORT LCDef_I,
       LCGen_I,
       LCNumer_I;
<* END *>

-- ���樠������ ������஢ �������� ��ꥪ��� �ଠ⮢
<* IF OBJ_COFF  THEN *> IMPORT formCOFF; <* END *>
<* IF OBJ_ELF   THEN *> IMPORT formELF;  <* END *>
<* IF OBJ_OMF   THEN *> IMPORT formOMF;  <* END *>
<* IF OBJ_GO32  THEN *> IMPORT formGO32; <* END *>
<* IF OBJ_VMS   THEN *> IMPORT formVMS;  <* END *>

<* IF OBJ_GAS   THEN *> IMPORT form_gas; <* END *>
<* IF OBJ_ASM   THEN *> IMPORT form_asm; <* END *>

-- ���樠������ ������஢ �������� �ଠ⮢ �⫠��筮� ���ଠ樨
<* IF DBG_CV    THEN *> IMPORT dbgCV;    <* END *>
<* IF DBG_HLL   THEN *> IMPORT dbgHLL;   <* END *>
<* IF DBG_EDIF  THEN *> IMPORT dbgEDIF;  <* END *>
<* IF DBG_STAB  THEN *> IMPORT dbgSTAB;  <* END *>
<* IF DBG_REF   THEN *> IMPORT dbgREF;   <* END *>
<* IF DBG_DWARF THEN *> IMPORT dbgDWARF; <* END *>
<* IF DBG_GO32  THEN *> IMPORT dbgGO32;  <* END *>

-- ���樠������ ������� ⥪�⮢��� �।�⠢����� �⫠��筮�
-- ���ଠ樨, �ᯮ������ ⮫쪮 ��� �⫠����� 楫��
<* IF DBG_TEXT THEN *> IMPORT dbgTEXT;  <* END *>

END Polymorph.
