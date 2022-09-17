0000
0001 illegal character
0002 comment not closed; started at line %d
0003 illegal number
0004 string literal not closed or too long
0005 unexpected end of file
0006 identifier too long
0007 identifier expected
0008 expected symbol %s
0009 illegal keyword usage
0010 source text read error
0011 can not determine source text language
0012 character constant too large (377C or 0FFX is maximum)

0020 undeclared identifier "%s"
0021 type identifier "%s" shall not be used in declaring itself
0022 identifier "%s" was already defined at %s[%d.%d]
0023 procedure with forward declaration cannot be a code procedure
0024 recursive import not allowed
0025 unsatisfied exported object "%s"
0026 identifier "%s" is used in its own declaration, see %s[%d.%d]
0027 illegal usage of module identifier "%s"
0028 identifier "%s" was already defined in other module
0029 incompatible types:\n    "%s"\n    "%s"
0030 incompatible types
0031 identifier does not denote a type
0032 scalar type expected
0033 ordinal type expected
0034 illegal type conversion
0035 NEW: "%s" not known in this scope
0036 DISPOSE: "%s" not known in this scope
0037 procedure "%s" should be a proper procedure
0038 illegal number of parameters "%s"
0039 procedure "%s": %s parameter expected for "%s"
0040 procedure "%s": type of parameter "%s" mismatch
0041 guard or test type is not an extension of variable type

0043 illegal result type of procedure
0044 incompatible result types

0046 illegal usage of open array type
0047 fewer actual than formal parameters
0048 more actual than formal parameters
0049 sequence parameter should be of SYSTEM.BYTE or SYSTEM.LOC type
0050 object is not array
0051 object is not record
0052 object is not pointer
0053 object is not set
0054 object is not variable
0055 object is not procedure: %s
0056 the value of constant in sequence parameter out of bounds

0057 a call of super method is valid in method redefinition only
0058 type-bound procedure is not defined for the base type
0059 object is neither a pointer nor a VAR-parameter record
0060 pointer not bound to record or array type
0061 dimension too large or negative
0062 pointer not bound to record

0064 base type of open array aggregate should be a simple type
0065 the record type is from another module

0067 receiver type should be exported %s
0068 this type-bound procedure cannot be called from a record
0069 wrong mode of receiver type

0071 non-Oberon type cannot be used in specific Oberon-2 construct
0072 illegal order of redefinition of type-bound procedures

0074 redefined type-bound procedure should be exported
0075 function procedure without RETURN statement
0076 value is required

0078 SIZE (TSIZE) cannot be applied to an open array
0079 HIGH or LEN cannot be applied to a foreign open array

0081 expected start of factor
0082 expected start of declaration
0083 expected start of type
0085 expected expression
0086 expected start of statement
0087 expression should be constant
0088 identifier does not match block name
0089 procedure not implemented "%s"
0090 proper procedure is expected
0091 call of proper procedure in expression
0092 code procedure is not allowed in definition module
0093 not allowed in definition module
0094 allowed only in definition module
0095 allowed only in global scope
0096 unsatisfied opaque type "%s"
0097 unsatisfied forward type "%s"
0098 allowed only for value parameter
0099 RETURN allowed only in procedure body
0100 illegal order of declarations

0102 language extension is not allowed %s
0103 INTERNAL ERROR(ME): value expected
0104 INTERNAL ERROR(ME): designator expected
0105 INTERNAL ERROR(ME): statement expected
0106 INTERNAL ERROR(ME): node type = NIL
0107 shall not be less than 0
0108 CASE in record constructor is not allowed

0109 forward type cannot be opaque
0110 illegal length, %d was expected
0111 repetition counter must be an expression of a whole number type
0112 expression for field "%s" was expected
0113 no variant is associated with the value of the expression
0114 object "%s" is a field
0115 expression of "%s" type was expected

0116 field "%s" is not exported
0117 type of "%s" is not defined yet
0118 base type is allowed only for Oberon record
0119 variant fields are not allowed in Oberon record
0120 field of Oberon type is not allowed in non-Oberon record
0121 illegal use of type designator "%s"
0122 expression out of bounds
0123 designator is read-only
0124 low bound greater than high bound
0125 EXIT not within LOOP statement
0126 case label defined more than once

0128 FOR-loop control variable must be declared in the local scope
0129 more expressions than fields in a record type

0131 zero step in FOR statement
0132 shall be an open array designator
0133 implementation limit exceeded for set base type (length > %d)
0134 must be value of unsigned type
0135 must be value of pointer type
0136 must be type designator
0137 numeric constant does not have a defined storage size
0138 string literals are not allowed here

0139 must be (qualified) identifier that denotes variable
0140 interrupt procedures are not implemented yet
0141 opaque type can not be defined as Oberon pointer
0142 INTERNAL ERROR(ME): can not generate code
0143 not allowed in Oberon
0144 pointer and record types are mixed in type test
0145 control variable must not be a formal parameter
0146 control variable cannot be exported
0147 threatened variable is not allowed here
0148 finalization is allowed only in module block
0149 RETRY is allowed only in exceptional part of block
0150 invalid system flag value
0151 must be of integer type
0152 incompatible calling conventions:\n    "%s"\n    "%s"
0153 incompatible calling conventions
0154 procedure "%s" does not match previous definition:\n  was: %s\n  now: %s
0155 procedure "%s" does not match previous definition
0156 procedure designator is expected
0157 object must be exported
0158 control variable "%s" is threatened here
0159 type of aggregate is not set or array or record
0160 invalid parameter specification: expected NIL
0161 VAR [NIL] parameter expected
0162 %s value should be in %{} (not "%s")
0163 control variable can not be volatile
0164 Language is not valid for external procedure
0165 cannot take the address of a numerical constant

0171 illegal structure of conditional compilation options
0172 conditional compilation option starts with incorrect symbol
0173 pragma not closed; started at line %d
0174 unexpected end of file while skipping; see at %d
0175 invalid pragma syntax
0176 "%s" illegal value for equation %s

0190 incorrect header in symbol file "%s"
0191 incorrect version of symbol file "%s" (%d instead of %d)
0192 key inconsistency of imported module "%s"
0193 generation of new symbol file not allowed
0194 module name does not match symbol file name "%s"
0195 cannot read symbol file "%s" generated by %s
0196 INTERNAL ERROR: incorrect sym ident %d while reading symbol file "%s"
0197 INTERNAL ASSERT(%d) while reading symbol file "%s"

0200 not yet implemented
0201 real overflow or underflow in constant expression
0202 integer overflow in constant expression
0203 division by zero

0206 array length is too large or less than zero

0208 CASE statement always fails
0209 implicit usage of SYSTEM.CAST
0210 there are no assignments to VAR-parameter "%s"
0211 variable "%s" optimized away

0219 too many dimensions (implementation limit %d)
0220 heirarchy of record extensions too high (implementation limit %d)
0221 procedure declaration nesting limit (%d) has been exceeded

0281 type-bound procedure is not valid as procedure value
0282 local procedure is not valid as procedure value "%s"
0283 code (or external) procedure is not valid as procedure value

0285 label declared but not used
0286 object is not a label
0287 label is not declared in the current scope
0288 label expected
0289 label already defined

0290 Foreign module may not have a body 
0291 Address of this expression cannot be used as constant in this compiler version

% ------- WARNINGS

0300 variable "%s" declared but never used
0301 parameter "%s" is never used
0302 value was assigned but never used
0303 procedure "%s" declared but never used
0304 possibly used before definition "%s"
0305 constant declared but never used
0306 import of "%s" is never used

0310 infinite loop
0311 unreachable code
0312 loop is executed exactly once

0314 variable "%s" has compile time defined value here
0315 NIL dereference
0316 this SYSTEM procedure is not described in Modula-2 ISO standard
0317 VAR parameter is used here, check that it is not threatened inside WITH
0318 redundant FOR statement
0319 CASE statement without ELSE
0320 undeclared option "%s"
0321 option "%s" is already defined
0322 undeclared equation "%s"
0323 equation "%s" is already defined
0324 field %s alignment (%d) is greater than record alignment (%d)
0325 object %s alignment (%d) is greater then global alignment (%d)
0326 possible side effect in removed ASSERT argument

0330 function %s inlined
0331 external function %s inlined

0333 Mixed integer and cardinal computations
0334 dangerous assignment to a pointer to "%d" of a pointer to "%d" bytes

0341 Import of module with body to module without body
0342 Unsupported MSC function "%s"

0348 type cast to type of other size
0349 dangerous type cast
0350 non portable type cast: size is undefined

% ------- C backend warnings
0351 option NOHEADER is allowed only in C-modules
0352 option NOCODE is allowed only in C-modules
0353 dependence cycle in C code
0354 incorrect usage of C keyword "%s"
0355 local VOLATILE variable declared: possible de-optimization

0390 obsolete pragma setting

% ------- make errors

0400 #definition or Oberon module is not found for module "%s"
0401 #implementation is not found for module "%s"
0402 #compile "%s" ("%s" not found)
0403 #compile "%s" ("%s" was modified after "%s")
0404 #compile "%s" ("%s" was modified)
0405 #compile "%s" (was modified after "%s")
0406 #undefined file extension "%s"
0407 #module not found "%s"
0408 #file "%s": definition module expected
0409 #file "%s": main module expected
0410 #file "%s": Oberon module expected
0411 #file "%s": implementation or main module expected
0412 #file "%s": module name "%s" expected, got "%s"
0413 #no modules in the project
0414 #file "%s" (line %d): %s
0415 #unknown operation mode "%s"
0416 #inconsistent set of operation modes
0417 #file "%s": module name "%s" expected (import from "%s")
0418 string expected
0419 unknown extension "%s"
0420 unknown template argument "%s"
0421 only one level of "{}" brackets is allowed
0422 inconsistent set of module kind
0423 #more than one source file for module:\n  %s\n  %s
0424 file creating error: %s
0425 file opening error: %s
0426 #unexpected module kind: "%s"
0427 #file "%s": error in module header or import section
0428 syntax error
0429 line too long
0430 #invalid argument (%s): %s
0431 #ERRFMT equation: wrong syntax (position %d)
0432 file closing error: %s
0433 #error in template file "%s", line %d.%d: %s
0434 #wrong value of equation "%s": "%s" is used
0435 #project is up to date
0436 #dependence cycle in import (See "%s", "%s")
0437 #too many errors
0438 is allowed only inside "{}" brackets
0439 external command fault %d:\n%s
0440 #error in template string, line %d.%d: %s\n%s
0441 #file "%s" (line %d.%02d): %s
0442 incompatible types
0443 undeclared identifier
0444 #file "%s": implementation module expected
0445 #file "%s": read error
0446 "end" missed
0447 could not execute external command:\n%s
0448 file writing error: %s
0449 object file limit (length > %d) is exceeded for name\n    "%s"

0450 compilation aborted: %s
0451 Invalid TESTCOVERAGEMASK '%s'
0452 Test Coverage System does not support multithreading

0499 module has not neither code or date sections
0500 can not output debug info
0501 can not output object file

0502 Compiling "%s"\n
0503 New "%s" is generated using template "%s"\n
0504 no errors
0505 errors %2d
0506 , no warnings
0507 , warnings %2d
0508 , lines %4d, time %2d.%02d
0509 , new symfile
0510 , (new def)
0513 WARNING
0514 ERROR 
0515 FAULT 
0520 %s\nConfiguration file is not opened: %s\n
0523 compilation aborted: %s
0524 files: %d  errors: %d(%d)  lines %d  time: %d:%02d  speed %d l/m\n
0525 \n***************************** Done *****************************\n
0526 XDEBUG option is ON !
0527 Cannot select back-end %s
0528 Generate makefile for project "%s"\n
0529 Linking...
0530 Checking dependencies
0531 Make project "%s"\n
0532 Browser, "%s"\n
0533 Cannot select target CPU %s

0544 There are no back-ends in compiler configuration.\n
0545 There are no front-ends in compiler configuration.\n

% ------ MODULE xmISOFM items.
0600 wrong name format
0601 wrong flags
0602 too many open
0603 out of channels
0604 wrong permissions
0605 no room on device
0606 no such file
0607 file exists
0608 wrong file type
0609 no text operations
0610 no raw operations
0611 no mixed operations
0612 already opened
0613 other problem (no memory)
0614 unknown result

0620 Invalid ALIGNMENT value specified
0621 ident or string
0622 ident

% ------ 102 extensions
0701 (extended compatibility with byte)
0702 (string concatenation)
0703 (procedure comparison with NIL)
0704 (string comparison)
0705 (DISPOSE function)
0706 (array or record or simple aggregate)
0707 (obsolete type cast)
0708 (not implemented yet)
0709 (TopSpeed structured values - use curly braces instead of parentheses)
0710 (direct language specification)
0711 (read-only tag)
0712 (special kinds of parameters)
0713 (read-only parameters)
0714 (parameter value by default)
0715 (function result is array or record)
0716 (SEQ parameter)
0717 (ARRAY OF type)
0718 (renaming in import)
0719 (exponentiation operator)
0720 (ISO pragma syntax)
0721 , use CPPCOMMENTS option
0722 , use // instead of --
0723 , use /* instead of *)

0800 Configuration:\n
0801    Front end:  %s\n
0802    Back end:   %s\n
0803    Test coverage system:  %s RTS v%d\n
0804    Data coverage system:  %s RTS v%d\n

0820 \nUsage:\n  Make project:\n
0821   Make project database:\n
0822   Check dependencies and recompile:\n
0823   Extract definition from symbol file:\n
0824   Generate make file:\n
0825   Compile:\n
0826 \nOptional operation modes:\n
0827   =all        Recompile all modules\n
0828   =batch      Prepare batch file\n
0829   =equations  Show equations\n
0830   =options    Show options\n
0831   =help       Print this text\n

% ------- compiler warnings
0900 redundant code eliminated
0901 redundant code not eliminated - can raise exception
0902 constant condition eliminated
0903 function result is not used
0904 inctruction 'movf' for probably undefined values of variable(s):\n    %s

0910 realValueException will be raised here
0911 wholeValueException will be raised here
0912 wholeDivException will be raised here
0913 indexException will be raised here
0914 rangeException will be raised here
0915 invalidLocation exception will be raised here
0916 realDivException will be raised here
0917 functionException (function without RETURN statement) will be raised here

% ------- x86 compiler errors
0920 INTERNAL ERROR: cardinal overflow
0921 invalid code procedure
0922 code procedures not supported

% ------- x86 compiler fatal errors
0950 out of memory
0951 too complex expression 
0952 that type conversion is not implemented

0960 INTERNAL ERROR: invalid constant value
0961 INTERNAL ERROR: invalid type mode (%d)
0962 INTERNAL ERROR: invalid object mode (%d)

% ------- C backend errors
1000 can not gen. overlapped field, GENCPP is OFF
1001 parameter "%s" is not declared
1002 can not generate recursive type definition
1003 external names conflict:\n    "%s.%s" and "%s.%s"
1004 external name "%s.%s" conflicts with standard name (xm.kwd)
1005 unimplemented system procedure
1006 undefined array length for dimension %d
1007 undefined array size for dimension %d
1008 can not generate expression
1009 can not generate l-value type cast
1010 can not generate type conversion
1011 can not generate aggregate
1012 can not generate statement
1013 cannot generate constant aggregate of this type
1014 size of %s exceeds the limit
1015 too many parameters (implementation limit)
1016 can not generate type designator
1017 can not generate type declaration
1018 can not generate recursive type declaration
1019 can not generate object declaration
1020 object %s will be initialized at run-time
1021 module has a body while option NOMODULEINIT enabled
1022 NOMODULEINIT option is allowed only within definition module
1023 cannot generate initial value for %s
1024 cannot get alien module object address 
1025 undefined equation %s  
1026 procedure "%s" contains assemler code and will be ignoreg by Test Coverage System

% ------- SPARC compiler messages
1100 impossible to avoid register window shift (SAVE\RESTORE instructions)


2000 usage of %s keyword is disabled by K26 option

% ---- warning 2001 is used for implementation module only
2001 VERSION equation must be specified when K26 option is enabled

2002 Equation MAINNAME ignored under GHS
2003 CRCVERSION option must be enabled when K26 option is enabled
2004 SOURCEID equation must be specified when K26 option is enabled

% ---- warning 2005 is used for definition module only
2005 VERSION equation must be specified when K26 option is enabled
