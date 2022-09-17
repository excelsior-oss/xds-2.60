MODULE opTune;
(* ���ଠ�� ��� ����ன�� �࠭���� �� ��設� *)
IMPORT ir_def, SYSTEM, pcK;

VAR
    nil_val* : ir_def.VALUE;

TYPE
    opTune_IDB     *= POINTER TO opTune_IDB_Rec;
    opTune_IDB_Rec *= RECORD
                        ld_real_sz *: ir_def.SizeType;
                      END;
VAR
  IDB *: opTune_IDB;

  BIG_END*   : BOOLEAN;    (* code for bigendian machine *)

CONST  (* -- ���ଠ�� � ॠ����樨 �������� ⨯�� �� �����⭮� ��設� -- *)

  jmp_buf_size = 180;       (* ࠧ��� ���� ��� set_jmp *)

  BITSET_LEN*  = ir_def.SizeType{32};       (* ����� ���ᨬ��쭮�� "᪠��୮��" ������⢠ *)
VAR
  BITSET_LEN_InInclExcl*  : LONGINT;       (* ����� ���ᨬ��쭮�� ������⢠ ��� ����権 In Incl Excl*)
  BITSET_LEN_scalar*      : LONGINT;       (* ����� ���ᨬ��쭮�� ������⢠ ��� ��⠫��� ����権 *)
  lset_sz_InInclExcl*     : LONGINT;
CONST
  (* ⨯ � ࠧ��� ����� ���樨 �������� ������⢠ *)
  lset_ty_InInclExcl* = ir_def.t_unsign;
  lset_ty* = ir_def.t_unsign;
  lset_sz* = BITSET_LEN DIV 8;
--<* END *>

  seq_item_type*= ir_def.t_unsign;
  seq_item_sz* = VAL(ir_def.SizeType,4);         (* ����� ������ ����� SEQ-��᫥����⥫쭮�� *)

  bool_ty* = ir_def.t_unsign;    (* BOOLEAN *)
  bool_sz* = VAL(ir_def.SizeType,1);

  char_ty* = ir_def.t_unsign;    (* CHAR *)
  char_sz* = VAL(ir_def.SizeType,1);

  protect_ty* = ir_def.t_unsign; (* PROTECTION *)
  protect_sz* = VAL(ir_def.SizeType,2);

  index_ty* = ir_def.t_int;      (* �⠭����� ⨯ ������ *)
  index_sz* = VAL(ir_def.SizeType,4);

  addr_ty* = ir_def.t_ref;       (* ���� � ��������� � �.�. *)
  addr_sz* = VAL(ir_def.SizeType,4);

  proc_ty* = ir_def.t_ref;       (* ��楤�୮� ���祭�� *)
  proc_sz* = VAL(ir_def.SizeType,4);

  counter_ty* = ir_def.t_int;    (* ���稪 � 横�� FOR *)
  counter_sz* = VAL(ir_def.SizeType,4);

  element_ty* = ir_def.t_int;    (* ����� ������⢠ *)
  element_sz* = VAL(ir_def.SizeType,4);

  process_ty* = ir_def.t_rec;    (* �. ⨯ xmRTS.X2C_XHandler_STR *)
  process_buffer_offs* = 2*2 + 2*ORD(addr_sz);
  process_size* = process_buffer_offs + jmp_buf_size;

  real_sz*      = VAL(ir_def.SizeType,4);        (* ����⢥���  *)
  longreal_sz*  = VAL(ir_def.SizeType,8);        (* ࠧ���        *)
                            (* �����         *)
  hword_sz * = 2;
  word_sz  * = 4;
  dword_sz * = 8;

  max_alignment *= 8; -- bytes


(* ------------------------------------------------------------------ *)

(* ����� �� ��� SR 㬭������ �� ��� 楫�� ����⠭�� *)
PROCEDURE(idb : opTune_IDB)
         NeedSR* (v: ir_def.VALUE; t: ir_def.TypeType; s: ir_def.SizeType): BOOLEAN;
BEGIN
  ASSERT(FALSE);
END NeedSR;

(* ����� �� ��� "������᭮�" 㬭������ �������� ᤢ����� � ᫮����ﬨ *)

PROCEDURE(idb : opTune_IDB)
         NeedAdd* (v: ir_def.VALUE; t: ir_def.TypeType; s: ir_def.SizeType): BOOLEAN;
BEGIN
  ASSERT(FALSE);
END NeedAdd;

PROCEDURE(idb : opTune_IDB)
        MinLocalStart* (proc:pcK.OBJECT):LONGINT;(* ����� ��ࢮ�� ������ - ����� �뢠�� *)
BEGIN
  ASSERT(FALSE);
END MinLocalStart;

(* ------------------------------------------------------------------ *)

VAR PARAM_START*,   (* ᬥ饭�� ��� ��ࢮ�� ��ࠬ��� - ����� �����⠥� *)
    LOCAL_START*    (* ����� ��ࢮ�� ������ - ����� �뢠�� *)
    : LONGINT;

(* ------------- t r a p s ------------------------------------------ *)

CONST -- ����⠭�� ��।����� � RTS (������ �����)
  indexException        *= 0;
  rangeException        *= 1;
  caseSelectException   *= 2;
  invalidLocation       *= 3;
  functionException     *= 4;
  wholeValueException   *= 5;
  wholeDivException     *= 6;
  realValueException    *= 7;
  realDivException      *= 8;
  complexValueException *= 9;
  complexDivException   *= 10;
  protException         *= 11;
  sysException          *= 12;
  coException           *= 13;
  exException           *= 14;
(* Oberon-2 Exceptions ----- *)
  assertException       *= 15;
  guardException        *= 16;
(* RTS Exceptions ---------- *)
  noMemoryException     *= 17;
  internalError         *= 18;
(* �������⥫쭮 ----------- *)
  undefinedException    *= 19;

CONST
  ABORT_EXIT_CODE       *= 9;      (* see TERMINATION.X2C_ABORT *)

(* -------- R T S - � � � � � � � � � ---------------------------------- *)
           ---------------------------
CONST
  x2c_mno* = MAX(pcK.Mno);    (* -- m n o   o f   R T S - l i b r a r y --- *)

CONST
  rts_name*   = "X2C";
  X2C_OFS_END* = 1;                       (* see file X2C.H *)
  X2C_OFS_ARR* = 2;
  X2C_OFS_REC* = 3;
  X2C_BASE*    = 4;

  empty_rec_size* = 4;       (* ࠧ��� ���⮩ ����� *)

  MOD_DESC_size* =  7 * addr_sz;
  TYP_DESC_size* =  index_sz + 3*addr_sz + 2*2 + 16*addr_sz + 6*addr_sz + 4;

  DYNARR_ADDR_offset* =  0;  (* ᬥ饭�� ��뫪� �� "墮��" � ���ਯ�� ������� ���ᨢ� *)
  DYNARR_LENs_offset* =  4;  (* ᬥ饭�� ⠡���� ���� �� ����७�� *)

  TD_PTR_offset*      = -8;  (* ᬥ饭�� 㪠��⥫� �� ⨯ � ���. ������� *)

  BASE_size*          =  4;  (* ࠧ��� ����� ⠡���� ������� ⨯��      *)
  BASEs_offset*       = 20;  (* ᬥ饭�� ⠡���� ������� ⨯��             *)
  PROC_offset*        = 84;  (* ᬥ饭�� 㪠��⥫� �� ⠡���� ��⮤�� ⨯� *)
  SELF_offset*        = 104; (* ᬥ饭�� �� 㪠��⥫� �� ᠬ ���ਯ�� ⨯� *)

(* � ����� ������ ॠ������ ����⠭� �� ⠪�� �������� ���ਯ�� ⨯�:

X2C_TD_STR = RECORD
 (*  0*)   size   : SYSTEM.size_t;
 (*  4*)   name   : POINTER TO CHAR; 	(* type name 			*)
 (*  8*)   module : X2C_MD;		(* host module			*)
 (* 12*)   next   : X2C_TD;		(* next type in module types list*)
 (* 16*)   methods: SYSTEM.INT16;
 (* 18*)   level  : SYSTEM.INT16;
 (* 20*)   base   : ARRAY [0..15] OF X2C_TD;
 (* 84*)   proc   : POINTER TO PROC;
 (* 88*)   offs   : POINTER TO POINTER TO SYSTEM.void;
 (* 92*)   succ   : X2C_TD;
 (* 96*)   link   : X2C_LINK;
 (*100*)   tail   : X2C_LINK;
 (*104*)   self   : X2C_TD;   (* -- new field: pointer to the type descriptor -- *)
 (*108*)   res    : SYSTEM.CARD32;
END;

*)

BEGIN
    NEW(IDB);
    IDB.ld_real_sz := -1;
<* IF    TARGET_68K  THEN *>  
    BIG_END := TRUE;
<* ELSIF TARGET_RISC OR TARGET_SPARC THEN *>  
    BIG_END := TRUE;
<* ELSE *>                    
    BIG_END := FALSE;
<* END *>
END opTune.

(*  ⠪ �뫮 �� ����୮ ���� ...
���饭��  ������
                                                                 | . . .    |
  -4       -4*n    base[X2C_EXT];  ��뫪� �� ������ ⨯�        | bases    |
   0         4     size;           ࠧ��� �������             | size     |
   4         4     offs;           �-⥫� �� ⠡���� 㪠��⥫�� | ptr_offs +-------+
   8         4     name;           �-⥫� �� ��ப�             | name     |       |
  12         4     module;         �-⥫� �� ���ਯ�� �����  | module   |       V
         void *      ext;           ???                          |          |    +--------+
  16         2     methods;        �᫮ ��⮤�� ??              | methods  |    |  ofs0  |
  18         2     level;          �஢��� ���७�� �����     | level    |    |  ofs1  |
  20        4*m    proc;           ⠡��� ��⮤��               | procs    |    |  . . . |
                                                                 | . . .    |
*)
