--------------------------------------------------------------------------------
--                       Excelsior PowerPC Native Compiler
--                          (c) 2015, Excelsior, LLC.
-- Module:   PPCreg
-- Mission:  PowerPC Register Set Description 
-- Synonym:  rgs
-- Authors:  Kochetov Dmitry, Lvov Konstantin
-- Created:  27-Jul-2000
--------------------------------------------------------------------------------
MODULE PPCreg;

CONST
  Single_SIZE  *= 4;  -- bytes;
  Double_SIZE  *= 8;  -- bytes;

  BITS_IN_BYTE *= 8;  -- bits

--------------------------------------------------------------------------------
--                             CPU Registers                                  --
--------------------------------------------------------------------------------
TYPE
  Reg *= (
  -- General Purpose Registers (GPRs)
    GPR0,   GPR1,   GPR2,   GPR3,    GPR4,   GPR5,   GPR6,   GPR7
  , GPR8,   GPR9,   GPR10,  GPR11,   GPR12,  GPR13,  GPR14,  GPR15
  , GPR16,  GPR17,  GPR18,  GPR19,   GPR20,  GPR21,  GPR22,  GPR23
  , GPR24,  GPR25,  GPR26,  GPR27,   GPR28,  GPR29,  GPR30,  GPR31

  -- Artificial Registers (APRs)
  , SPILLED_REG  -- register spilled into memory
  , UNDEF_REG    -- temporarily undefined register

  -- Floating-Point Registers (FPRs)
  , FPR0,   FPR1,   FPR2,   FPR3,    FPR4,   FPR5,   FPR6,   FPR7
  , FPR8,   FPR9,   FPR10,  FPR11,   FPR12,  FPR13,  FPR14,  FPR15
  , FPR16,  FPR17,  FPR18,  FPR19,   FPR20,  FPR21,  FPR22,  FPR23
  , FPR24,  FPR25,  FPR26,  FPR27,   FPR28,  FPR29,  FPR30,  FPR31

  -- Special-Purpose Registers (SPRs)
  , CR     -- Condition register
  , FPSCR  -- Floating-point status and control register
  , XER    -- Integer exception register
  , LR     -- Link register
  , CTR    -- Count register

  -- Fields of 'CR' and 'FPSCR' registers
  , CR0,    CR1,    CR2,    CR3,     CR4,    CR5,    CR6,    CR7
  , FPSCR0, FPSCR1, FPSCR2, FPSCR3,  FPSCR4, FPSCR5, FPSCR6, FPSCR7
  );

  RegSet *= PACKEDSET OF Reg;

CONST
  ZERO_REG *= GPR0; -- dummy argument reg

--------------------------------------------------------------------------------
TYPE
  ArtificalReg    *= Reg[SPILLED_REG..UNDEF_REG];
  ArtificalRegSet *= PACKEDSET OF ArtificalReg;
  
CONST 
  ARTIFICAL_REGs *= RegSet{SPILLED_REG..UNDEF_REG};

--------------------------------------------------------------------------------
TYPE
  GPR  *= Reg[GPR0..UNDEF_REG];         -- General Purpose Registers
  FPR  *= Reg[SPILLED_REG..FPR31];      -- Floating Point Registers
  SPR  *= Reg[CR..CTR];                 -- Special Purpose Registers  
  APR  *= Reg[SPILLED_REG..UNDEF_REG];  -- Artificial Purpose Registers

  GFR  *= Reg[GPR0..FPR31];
  REG  *= Reg[GPR0..CTR];

  CRF  *= Reg[CR0..FPSCR7];

  GPRSet *= PACKEDSET OF GPR;
  FPRSet *= PACKEDSET OF FPR;
  SPRSet *= PACKEDSET OF SPR;
  APRSet *= PACKEDSET OF APR;
  
  CRFSet *= PACKEDSET OF CRF;

--------------------------------------------------------------------------------
CONST
  -- All Registers
  RegSet0 *= RegSet{};     
  N_Regs  *= ORD(MAX(Reg)) + 1;
  ALLRegs *= - RegSet0;

  NoREGs *= RegSet {};     
  N_REGs *= ORD(MAX(REG)) - 1; -- SPILLED_REG, UNDEF_REG

  -- General Purpose Registers
  NoGPRs *= GPRSet{};     
  N_GPRs *= ORD(MAX(GPR)) - 1; -- SPILLED_REG, UNDEF_REG
  GPRs   *= RegSet{GPR0..GPR31};

  -- Floating Point Registers
  NoFPRs *= FPRSet{};     
  N_FPRs *= ORD(MAX(FPR)) - 1; -- SPILLED_REG, UNDEF_REG
  FPRs   *= RegSet{FPR0..FPR31};
  
  -- Special Purpose Registers
  NoSPRs *= SPRSet{};
  N_SPRs *= ORD(MAX(SPR)) + 1;
  SPRs   *= RegSet{CR..CTR};

  -- Artificial Purpose Registers
  APRSet0 *= APRSet{};    
  N_APRs  *= ORD(MAX(APR)) + 1;
  APRs    *= RegSet{SPILLED_REG..UNDEF_REG};

  -- Fields of 'CR' and 'FPSCR' Register
  NoCRFs *= CRFSet{};     
  N_CRFs *= ORD(MAX(CRF)) + 1;
  CRs    *= CRFSet{CR0..CR7};
  FPSCRs *= CRFSet{FPSCR0..FPSCR6};


--------------------------------------------------------------------------------
-- Register Size in bytes
CONST
  GPR_SIZE *= 4;  -- size of General Purpose Register
  FPR_SIZE *= 8;  -- size of Floating Point Register

  REG_SIZE *= GPR_SIZE; -- when register's type doesn't metter 

-- Register Size in bits
CONST
  BITS_IN_GPR *= GPR_SIZE * BITS_IN_BYTE;  -- bit size of a General Purpose Register 
  BITS_IN_FPR *= FPR_SIZE * BITS_IN_BYTE;  -- bit size of a Floating Point Register 

--------------------------------------------------------------------------------
CONST
  -- available general-porpose registers
  FIRST_GPR *= GPR0;
  LAST_GPR  *= GPR31;

  -- available floating point registers
  FIRST_FPR *= FPR0;
  LAST_FPR  *= FPR31;

--------------------------------------------------------------------------------
--         Register Names for assemler, binary and debug output
--------------------------------------------------------------------------------
TYPE 
  TextName *= ARRAY 8 OF CHAR;
  RegNameArray *= ARRAY Reg OF TextName;
  RegCodeArray *= ARRAY Reg OF LONGINT;
  RegArray     *= ARRAY Reg OF Reg;
  RegSetArray  *= ARRAY Reg OF RegSet;
  
VAR  
  RegNameAsm *: RegNameArray;  -- must be initialized in 'InstrEmit_A' and 'form_gas' module!
  RegCodeBin *: RegCodeArray;  -- must be initialized in 'InstrEmit_B' module!

--------------------------------------------------------------------------------
CONST
  PREV_REG *= RegArray{  -- ! must be synchronized with 'Reg'
    UNDEF_REG,  GPR0,   GPR1,   GPR2,   GPR3,    GPR4,   GPR5,   GPR6  
  , GPR7,       GPR8,   GPR9,   GPR10,  GPR11,   GPR12,  GPR13,  GPR14 
  , GPR15,      GPR16,  GPR17,  GPR18,  GPR19,   GPR20,  GPR21,  GPR22 
  , GPR23,      GPR24,  GPR25,  GPR26,  GPR27,   GPR28,  GPR29,  GPR30 

  -- Artificial Registers (APRs)
  , SPILLED_REG  -- register spilled into memory
  , UNDEF_REG    -- temporarily undefined register
   
  -- Floating-Point Registers (FPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
    
  -- Special-Purpose Registers (SPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
   
  -- Fields of 'CR' and 'FPSCR' registers
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  }; -- PrevReg  

--------------------------------------------------------------------------------
CONST
  NEXT_REG *= RegArray{  -- ! must be synchronized with 'Reg'
    GPR1,   GPR2,   GPR3,    GPR4,   GPR5,   GPR6,   GPR7,    GPR8      
  , GPR9,   GPR10,  GPR11,   GPR12,  GPR13,  GPR14,  GPR15,   GPR16     
  , GPR17,  GPR18,  GPR19,   GPR20,  GPR21,  GPR22,  GPR23,   GPR24     
  , GPR25,  GPR26,  GPR27,   GPR28,  GPR29,  GPR30,  GPR31,   UNDEF_REG 

  -- Artificial Registers (APRs)
  , SPILLED_REG  -- register spilled into memory
  , UNDEF_REG    -- temporarily undefined register
   
  -- Floating-Point Registers (FPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
    
  -- Special-Purpose Registers (SPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
   
  -- Fields of 'CR' and 'FPSCR' registers
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  }; -- NextReg  

--------------------------------------------------------------------------------
CONST
  DbgRegName *= RegNameArray { -- can't be changed!
  -- General Purpose Register (GPRs)
    "$r0",   "$r1",   "$r2",   "$r3",    "$r4",   "$r5",   "$r6",   "$r7"
  , "$r8",   "$r9",   "$r10",  "$r11",   "$r12",  "$r13",  "$r14",  "$r15"
  , "$r16",  "$r17",  "$r18",  "$r19",   "$r20",  "$r21",  "$r22",  "$r23"
  , "$r24",  "$r25",  "$r26",  "$r27",   "$r28",  "$r29",  "$r30",  "$r31"
  -- Artificial Registers
  , "$r!!"  -- SPILLED_REG - register spilled into memory
  , "$r??"  -- UNDEF_REG - temporarily undefined 
  -- Floating-Point Register (FPRs)
  , "$f0",   "$f1",   "$f2",   "$f3",    "$f4",   "$f5",   "$f6",   "$f7"
  , "$f8",   "$f9",   "$f10",  "$f11",   "$f12",  "$f13",  "$f14",  "$f15"
  , "$f16",  "$f17",  "$f18",  "$f19",   "$f20",  "$f21",  "$f22",  "$f23"
  , "$f24",  "$f25",  "$f26",  "$f27",   "$f28",  "$f29",  "$f30",  "$f31"
  -- Special-Purpose Regs (SPRs)
  , "$cr"     -- CR
  , "$fpscr"  -- FPSCR
  , "$xer"    -- XER
  , "$lr"     -- LR
  , "$ctr"    -- CTR
  -- Field of CR and FPSCR  Register
  --CR0,    CR1,    CR2,    CR3,     CR4,    CR5,    CR6,    CR7,
  , "$cr0",  "$cr1",  "$cr2",  "$cr3",   "$cr4",  "$cr5",  "$cr6",  "$cr7"
  --FPSCR0, FPSCR1, FPSCR2, FPSCR3,  FPSCR4, FPSCR5, FPSCR6, FPSCR7,
  , "$crf0", "$crf1", "$crf2", "$crf3",  "$crf4", "$crf5", "$crf6", "$crf7"
  };

--------------------------------------------------------------------------------
CONST
  RegCodeDbgInfo *= RegCodeArray {  -- ! must be synchronized with 'Reg'
  -- General Purpose Register (GPRs)
     0,   1,   2,   3,    4,   5,   6,   7
  ,  8,   9,  10,  11,   12,  13,  14,  15
  , 16,  17,  18,  19,   20,  21,  22,  23
  , 24,  25,  26,  27,   28,  29,  30,  31
  -- Artificial Registers
  , 127  -- SPILLED_REG
  , 127  -- UNDEF_REG temporarily undefined register
  -- Floating-Point Register (FPRs)
  ,  0+32,   1+32,   2+32,   3+32,    4+32,   5+32,   6+32,   7+32
  ,  8+32,   9+32,  10+32,  11+32,   12+32,  13+32,  14+32,  15+32
  , 16+32,  17+32,  18+32,  19+32,   20+32,  21+32,  22+32,  23+32
  , 24+32,  25+32,  26+32,  27+32,   28+32,  29+32,  30+32,  31+32
  -- Special-Purpose Regs (SPRs)
  , 32+32+1  -- CR
  , 32+32+2  -- FPSCR
  , 32+32+3  -- XER
  , 32+32+4  -- LR
  , 32+32+5  -- CTR
  -- Field of CR and FPSCR  Register
  --CR0,       CR1,       CR2,       CR3,       CR4,       CR5,       CR6,       CR7,
  , 32+32+10,  32+32+11,  32+32+12,  32+32+13,  32+32+14,  32+32+15,  32+32+16,  32+32+17
  --FPSCR0,    FPSCR1,    FPSCR2,    FPSCR3,    FPSCR4,    FPSCR5,    FPSCR6,    FPSCR7,
  , 32+32+20,  32+32+21,  32+32+22,  32+32+23,  32+32+24,  32+32+25,  32+32+26,  32+32+27
  };

--------------------------------------------------------------------------------
CONST
  ConstParamRegCode = 128;  -- смещение для номеров регистров на которых передаются константные параметры
  
PROCEDURE ConstParamRegCodeDbgInfo *(reg: Reg): LONGINT;
BEGIN
  ASSERT( reg IN GPRs );
  RETURN ConstParamRegCode + RegCodeDbgInfo[reg];
END ConstParamRegCodeDbgInfo;

--------------------------------------------------------------------------------
--                              Register Pairs
--------------------------------------------------------------------------------
CONST
  -- Register Size in bytes
  DUAL_GPR_SIZE *= GPR_SIZE * 2;  -- size of a Dual General Purpose Register
  DUAL_REG_SIZE *= REG_SIZE * 2;   -- when register's type doesn't metter 

  -- Register Size in bits
  BITS_IN_DUAL_GPR *= DUAL_GPR_SIZE * BITS_IN_BYTE;  -- bit size of a Dual General Purpose Register 

--------------------------------------------------------------------------------
CONST
  -- Dual General Porpose Registers
  DUAL_GPRs *= RegSet{
    GPR1,  GPR3,  GPR5,  GPR7,  GPR9,  GPR11, GPR13, GPR15
  , GPR17, GPR19, GPR21, GPR23, GPR25, GPR27, GPR29
  , SPILLED_REG -- register spilled into memory
  };

  DUAL_REGs *= DUAL_GPRs; 

--------------------------------------------------------------------------------
CONST
  PAIR_REG *= RegArray{  -- ! must be synchronized with 'Reg'  
  -- General Purpose Registers (GPRs)
    UNDEF_REG
  , GPR2,   GPR1,   GPR4,   GPR3,    GPR6,   GPR5,   GPR8,   GPR7  
  , GPR10,  GPR9,   GPR12,  GPR11,   GPR14,  GPR13,  GPR16,  GPR15 
  , GPR18,  GPR17,  GPR20,  GPR19,   GPR22,  GPR21,  GPR24,  GPR23 
  , GPR26,  GPR25,  GPR28,  GPR27,   GPR30,  GPR29,  UNDEF_REG 

  -- Artificial Registers (APRs)
  , SPILLED_REG  -- register spilled into memory
  , UNDEF_REG    -- temporarily undefined register
   
  -- Floating-Point Registers (FPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
    
  -- Special-Purpose Registers (SPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
   
  -- Fields of 'CR' and 'FPSCR' registers
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  }; -- PairReg

--------------------------------------------------------------------------------
CONST
  NEXT_DUAL_REG_UNIT *= RegArray{  -- ! must be synchronized with 'Reg'
  -- General Purpose Registers (GPRs)
    UNDEF_REG,  GPR2,   UNDEF_REG,   GPR4,   UNDEF_REG,  GPR6,   UNDEF_REG,   GPR8 
  , UNDEF_REG,  GPR10,  UNDEF_REG,   GPR12,  UNDEF_REG,  GPR14,  UNDEF_REG,   GPR16 
  , UNDEF_REG,  GPR18,  UNDEF_REG,   GPR20,  UNDEF_REG,  GPR22,  UNDEF_REG,   GPR24 
  , UNDEF_REG,  GPR26,  UNDEF_REG,   GPR28,  UNDEF_REG,  GPR30,  UNDEF_REG,   UNDEF_REG 

  -- Artificial Registers (APRs)
  , SPILLED_REG  -- register spilled into memory
  , UNDEF_REG    -- temporarily undefined register
   
  -- Floating-Point Registers (FPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
    
  -- Special-Purpose Registers (SPRs)
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG
   
  -- Fields of 'CR' and 'FPSCR' registers
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  , UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG, UNDEF_REG  
  }; -- NextDualRegUnit  

  --------------------------------------------------------------------------------
  DUAL_REG_UNITS *= RegSetArray{  
  -- General Purpose Registers (GPRs)
    {UNDEF_REG, UNDEF_REG}
  , {GPR1, GPR2},    {GPR1, GPR2},    {GPR3, GPR4},    {GPR3, GPR4}   
  , {GPR5, GPR6},    {GPR5, GPR6},    {GPR7, GPR8},    {GPR7, GPR8}   
  , {GPR9, GPR10},   {GPR9, GPR10},   {GPR11, GPR12},  {GPR11, GPR12} 
  , {GPR13, GPR14},  {GPR13, GPR14},  {GPR15, GPR16},  {GPR15, GPR16}  
  , {GPR17, GPR18},  {GPR17, GPR18},  {GPR19, GPR20},  {GPR19, GPR20} 
  , {GPR21, GPR22},  {GPR21, GPR22},  {GPR23, GPR24},  {GPR23, GPR24} 
  , {GPR25, GPR26},  {GPR25, GPR26},  {GPR27, GPR28},  {GPR27, GPR28} 
  , {GPR29, GPR30},  {GPR29, GPR30},  {UNDEF_REG, UNDEF_REG} 
  
  -- Artificial Registers (APRs)
  , {UNDEF_REG, UNDEF_REG}  -- register spilled into memory 
  , {UNDEF_REG, UNDEF_REG}  -- temporarily undefined register 
  
  -- Floating-Point Registers (FPRs)
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}

  -- Special-Purpose Registers (SPRs)
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}
   
  -- Fields of 'CR' and 'FPSCR' registers
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  , {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}, {UNDEF_REG, UNDEF_REG}
  }; -- Reg

--------------------------------------------------------------------------------
TYPE
  SizeType = LONGINT;  -- NOTE: usage of ir.SizeType leads to dependence cycle in import
  
--------------------------------------------------------------------------------
PROCEDURE isDualRegSize *(sz: SizeType; whole_type: BOOLEAN): BOOLEAN;
BEGIN
  RETURN whole_type & (sz = DUAL_GPR_SIZE);   
END isDualRegSize;  

--------------------------------------------------------------------------------
PROCEDURE isRegGPR *(r: Reg): BOOLEAN;
BEGIN
  RETURN (r IN GPRs);
END isRegGPR;  

--------------------------------------------------------------------------------
PROCEDURE isRegSetGPR *(mask: RegSet): BOOLEAN;
BEGIN
  RETURN ((mask * GPRs) # RegSet0);
END isRegSetGPR;  

--------------------------------------------------------------------------------
PROCEDURE PrevReg *(r: Reg): Reg;
VAR res: Reg;
BEGIN
  res := PREV_REG[r];
  ASSERT( res # UNDEF_REG );
  RETURN res;
END PrevReg;

--------------------------------------------------------------------------------
PROCEDURE NextReg *(r: Reg): Reg;
VAR res: Reg;
BEGIN
  res := NEXT_REG[r];
  ASSERT( res # UNDEF_REG );
  RETURN res;
END NextReg;

--------------------------------------------------------------------------------
PROCEDURE PairReg *(r: Reg): Reg;
VAR res: Reg;
BEGIN
  res := PAIR_REG[r];
  ASSERT( res # UNDEF_REG );
  RETURN res;
END PairReg;

--------------------------------------------------------------------------------
-- Returns next register in corresponding regiter pair (dual register).
-- Attempt to get next register for the last register in the pair rouses an exception.
PROCEDURE NextDualRegUnit *(r: Reg): Reg;
VAR res: Reg;
BEGIN
  res := NEXT_DUAL_REG_UNIT[r];
  ASSERT( res # UNDEF_REG );
  RETURN res;
END NextDualRegUnit;

--------------------------------------------------------------------------------
PROCEDURE DualRegUnits *(r: Reg): RegSet;
VAR rset: RegSet;
BEGIN
  rset := DUAL_REG_UNITS[r];
  ASSERT( rset # RegSet0 );
  RETURN rset;
END DualRegUnits;
  
--------------------------------------------------------------------------------
PROCEDURE GetRegSet *(r: Reg; sz: SizeType): RegSet;
VAR rset: RegSet; 
BEGIN 
  IF isDualRegSize(sz, isRegGPR(r)) THEN
    rset := DualRegUnits(r);
  ELSE 
    rset := RegSet{r};
  END;
  RETURN rset;
END GetRegSet; 

--------------------------------------------------------------------------------
PROCEDURE isRegSetEmpty *(rset-: RegSet; sz: SizeType): BOOLEAN;  
VAR r: Reg;
    is_gpr: BOOLEAN;
BEGIN
  is_gpr := isRegSetGPR(rset); 
  IF (rset = RegSet0) THEN
     RETURN TRUE;
  ELSIF ~isDualRegSize(sz, is_gpr) THEN
    RETURN FALSE;
  ELSIF is_gpr THEN
    FOR r := FIRST_GPR TO LAST_GPR DO
      IF (r IN DUAL_GPRs) & (r IN rset) & (NEXT_DUAL_REG_UNIT[r] IN rset) THEN
        RETURN FALSE;
      END;
    END;
  END;  
  RETURN TRUE;
END isRegSetEmpty; 

--------------------------------------------------------------------------------
PROCEDURE RegSetIN *(rset-: RegSet; r: Reg; sz: SizeType): BOOLEAN;
BEGIN
  IF NOT (r IN rset) THEN RETURN FALSE; END;

  IF isDualRegSize(sz, isRegGPR(r)) THEN                       -- special treatment for dual regs
    IF NOT (r IN DUAL_REGs)             THEN RETURN FALSE; END;
    IF NOT (NextDualRegUnit(r) IN rset) THEN RETURN FALSE; END;
  END;

  RETURN TRUE;
END RegSetIN;  

--------------------------------------------------------------------------------
PROCEDURE RegSetIntersect *(rset-: RegSet; r: Reg; sz: SizeType): BOOLEAN;
BEGIN
  RETURN (r IN rset) 
      OR ( isDualRegSize(sz, isRegGPR(r)) & (r IN DUAL_REGs) & (NextDualRegUnit(r) IN rset) );
END RegSetIntersect;  

--------------------------------------------------------------------------------
PROCEDURE RegSetINCL *(VAR rset: RegSet; r: Reg; sz: SizeType); 
BEGIN 
  INCL(rset, r);
  IF isDualRegSize(sz, isRegGPR(r)) THEN
    INCL(rset, NextReg(r));
  END; 
END RegSetINCL; 

--------------------------------------------------------------------------------
PROCEDURE RegSetEXCL *(VAR rset: RegSet; r: Reg; sz: SizeType); 
BEGIN 
  EXCL(rset, r);
  IF isDualRegSize(sz, isRegGPR(r)) THEN
    EXCL(rset, NextReg(r));
  END; 
END RegSetEXCL; 


--------------------------------------------------------------------------------
--                       Condition Register (CR)                              --
--------------------------------------------------------------------------------
TYPE
  CRFArray *= ARRAY CRF OF Reg;

CONST
  CRF_Reg *= CRFArray { -- can't be changed!
     CR,    CR,    CR,    CR,     CR,    CR,    CR,    CR
  ,  FPSCR, FPSCR, FPSCR, FPSCR,  FPSCR, FPSCR, FPSCR, FPSCR
  };

--------------------------------------------------------------------------------
TYPE
  CRbit *= ( -- can't be changed!
  -- CR0 Field Bits Resulting from Fixed-Point
  --         Compare Operation  |  Operation with Record Bit
    CR_LT  -- Less Than         |  -- Negatitive (Less That Zero)
  , CR_GT  -- Greater Than      |  -- Positive (Greater Than Zero)
  , CR_EQ  -- Equal             |  -- Zero (Equal Zero)
  , CR_SO  -- Symmary Overflow  |  -- Symmary Overflow
  -- CR1 Field Bits Resulting from Floating-Point Operation with Record Bit
  , CR_FX                          -- FP Exception Summary
  , CR_FEX                         -- FP Enable Exception
  , CR_VX                          -- FP Invalid Operation Exception Summary
  , CR_OX                          -- FP Overflow Exception
  -- CR2 Field Bits                   
  , CR_8,   CR_9,   CR_10,  CR_11
  -- CR3 Field Bits    
  , CR_12,  CR_13,  CR_14,  CR_15
  -- CR4 Field Bits                   
  , CR_16,  CR_17,  CR_18,  CR_19
  -- CR5 Field Bits    
  , CR_20,  CR_21,  CR_22,  CR_23
  -- CR6 Field Bits                   
  , CR_24,  CR_25,  CR_26,  CR_27
  -- CR7 Field Bits      
  , CR_28,  CR_29,  CR_30,  CR_31
  );

  CRbitSet *= PACKEDSET OF CRbit;

CONST
  ZERO_CR *= CR_LT;

CONST
  -- CR Field Bits Resulting from Floating-Point Compare Operation
  CR_FL *= CR_LT;  -- FP Less Than
  CR_FG *= CR_GT;  -- FP Greater Than
  CR_FE *= CR_EQ;  -- FP Equal
  CR_FU *= CR_SO;  -- FP Unordered


--------------------------------------------------------------------------------
TYPE
  CRbitArray *= ARRAY CRbit OF CRF;

CONST
  CRbit_CRF *= CRbitArray { -- can't be changed!
    CR0, CR0, CR0, CR0,   CR1, CR1, CR1, CR1
  , CR2, CR2, CR2, CR2,   CR3, CR3, CR3, CR3
  , CR4, CR4, CR4, CR4,   CR5, CR5, CR5, CR5
  , CR6, CR6, CR6, CR6,   CR7, CR7, CR7, CR7
  };

--------------------------------------------------------------------------------
TYPE
  CR0bitArray *= ARRAY CRbit OF CRbit;

CONST
  CRbit_CR0bit *= CR0bitArray { -- can't be changed!
    CR_LT, CR_GT, CR_EQ, CR_SO,   CR_LT, CR_GT, CR_EQ, CR_SO
  , CR_LT, CR_GT, CR_EQ, CR_SO,   CR_LT, CR_GT, CR_EQ, CR_SO
  , CR_LT, CR_GT, CR_EQ, CR_SO,   CR_LT, CR_GT, CR_EQ, CR_SO
  , CR_LT, CR_GT, CR_EQ, CR_SO,   CR_LT, CR_GT, CR_EQ, CR_SO
  };


--------------------------------------------------------------------------------
--                 Fixed-Point Exception Register (XER)                       --
--------------------------------------------------------------------------------
TYPE
  XERbit *= ( -- can't be changed!
      XER_SO,  -- Summary Overflow
      XER_OV,  -- Overflow
      XER_CA   -- Carry
  );

  XER_CRbitArray *= ARRAY XERbit OF CRbit;

CONST
  XER_CRbit *= XER_CRbitArray { -- can't be changed!
      CR_LT,  -- XER_SO
      CR_GT,  -- XER_OV
      CR_EQ   -- XER_CA
  };

(*
  XERbitSet *= PACKEDSET OF XERbit;

CONST
  XERs  *= XERbitSet {XER_SO..XER_CA};
*)

--------------------------------------------------------------------------------
--       Branch-On (BO) Parameter for Branch Conditional Instruction          --
--------------------------------------------------------------------------------
TYPE
  BO *= ( -- can't be changed!
  -- The other operation is encoded by this basic values
      BO_decCTRz,  -- Decrement the CTR, then branch if the decremented (CTR = 0)
      BO_decCTRnz, -- Decrement the CTR, then branch if the decremented (CTR # 0)
      BO_condT,    -- Branch if the condition is TRUE
      BO_condF,    -- Branch if the condition is FALSE
      BO_always,   -- Branch always
      BO_predict   -- Condition branch is likely to be taken
  );

  BOSet *= PACKEDSET OF BO;

CONST
  NoBOs *= BOSet {};

(*
  PhysBO *= SET;
  BOsArray = ARRAY BO OF PhysBO;

CONST
  BOVal *= BOsArray {
      {1,4}, -- BO_decCTRz
      {4},   -- BO_decCTRnz
      {2,3}, -- BO_condT
      {2},   -- BO_condF
      {2,4}, -- BO_always
      {0}    -- BO_predict
  };
*)

--------------------------------------------------------------------------------
--                   High-level Jump Condition Encode                         --
--------------------------------------------------------------------------------
TYPE
  JumpCond *= ( -- can't be changed!
      JC_EQ,  -- Equal
      JC_NE,  -- Not equal
      JC_LT,  -- Less than
      JC_LE,  -- Less than or equal
      JC_GT,  -- Greater than
      JC_GE,  -- Greater than or equal
      JC_In,  -- Include in set
      JC_NIn, -- Not include in set
      JC_DZ,  -- Decrement CTR; jump if (CTR = 0)
      JC_DNZ, -- Decrement CTR; jump if (CTR # 0)
      JC_No,  -- No jump
      JC_Un   -- Unconditional jump
  );

  JumpsCondArray = ARRAY JumpCond OF JumpCond;

CONST
  InverseCond *= JumpsCondArray {
      JC_NE,  -- Opposite to Equal
      JC_EQ,  -- Opposite to Not Equal
      JC_GE,  -- Opposite to Less than
      JC_GT,  -- Opposite to Less than or equal
      JC_LE,  -- Opposite to Greater than
      JC_LT,  -- Opposite to Greater than or equal
      JC_NIn, -- Opposite to Include in set
      JC_In,  -- Opposite to Not include in set
      JC_DNZ, -- Opposite to Decrement CTR; jump if (CTR = 0)
      JC_DZ,  -- Opposite to Decrement CTR; jump if (CTR # 0)
      JC_Un,  -- Opposite to No jump
      JC_No   -- Opposite to Unconditional jump
  };


--------------------------------------------------------------------------------
--             Trap-On (TO) Parameter of Trap Instruction                     --
--------------------------------------------------------------------------------
TYPE
  TOBit *= ( -- can't be changed!
  -- The other operation is encoded by this basic values
    TO_lt     -- Trap if Less Than
  , TO_gt     -- Trap if Greater Than
  , TO_eq     -- Trap if Equal
  , TO_llt    -- Trap if Logically (usigned) Less Than
  , TO_lgt    -- Trap if Logically (usigned) Greater Than
  );
  TOSet *= PACKEDSET OF TOBit;

CONST
  TO_always *= TOSet{MIN(TOBit)..MAX(TOBit)};  -- Trap Unconditionally

--------------------------------------------------------------------------------
--             High-level Condition Encode to Invoke Trap                     --
--------------------------------------------------------------------------------
TYPE
  TOcond *= ( -- can't be changed!
      TO_EQ  -- trap if equal
    , TO_NE  -- trap if not equal
    , TO_LE  -- trap if less than or equal
    , TO_GT  -- trap if greater than
    , TO_GE  -- trap if greater than or equal
    , TO_LT  -- trap if less than
    , TO_Un  -- unconditional trap
  );

--------------------------------------------------------------------------------
TYPE
  TOcondNameArray = ARRAY TOcond OF TextName;

CONST
  DbgTOcond *= TOcondNameArray {
    "TO_EQ"  -- trap if equal
  , "TO_NE"  -- trap if not equal
  , "TO_LE"  -- trap if less than or equal
  , "TO_GT"  -- trap if greater than
  , "TO_GE"  -- trap if greater than or equal
  , "TO_LT"  -- trap if less than
  , "TO_Un"  -- unconditional trap
  };

--------------------------------------------------------------------------------
-- Gets jump condition code corresponding to trap condition code.
PROCEDURE GetJC * (to_cond: TOcond): JumpCond;
BEGIN
  CASE to_cond OF
  | TO_Un:  RETURN JC_Un;
  | TO_LT:  RETURN JC_LT;
  | TO_LE:  RETURN JC_LE;
  | TO_GT:  RETURN JC_GT;
  | TO_GE:  RETURN JC_GE;
  | TO_EQ:  RETURN JC_EQ;
  | TO_NE:  RETURN JC_NE;
  END;
END GetJC;

END PPCreg.
