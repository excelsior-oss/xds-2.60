<* MAIN+ *>
MODULE lists;
TYPE Stack     = POINTER TO StackDesc;
     StackDesc = RECORD val:INTEGER; next:Stack END;

VAR stack : Stack; x,y : REAL;

PROCEDURE Push(val:INTEGER);
VAR st:Stack;
BEGIN  NEW(st); st^.val:=val; st^.next:=stack; stack:=st; y:=val+1.0;
END Push;

PROCEDURE Top():INTEGER;
BEGIN RETURN stack^.val;
END Top;

PROCEDURE Normalize(div:INTEGER);
VAR st:Stack;
BEGIN st:=stack;
      WHILE st#NIL DO st^.val:=st^.val DIV div; st:=st^.next; END;
END Normalize;

PROCEDURE Normal();
BEGIN IF Top()#0 THEN Normalize(Top()); ELSE Normalize(10); END;
END Normal;

PROCEDURE Iterate();
VAR x:Stack;
BEGIN
  NEW(x); x^.val:=1;
  WHILE x^.val>0 DO  x:=NIL; END;
END Iterate;

PROCEDURE Find(v:INTEGER):REAL;
VAR arr : POINTER TO ARRAY OF INTEGER;
    j : INTEGER;
BEGIN
  NEW(arr,5);
  FOR j:=0 TO 5 DO arr[j]:=j+j; END;
  RETURN arr[v];
END Find;

PROCEDURE Make();
VAR l:LONGINT;
    s:SHORTINT;
    i:INTEGER;
BEGIN
  l:=512;   i:=SHORT(l);   s:=SHORT(i);
END Make;

BEGIN
  IF x>2.0 THEN Iterate() END;
  IF x>3.0 THEN x:=Find(10) END;
  IF x>4.0 THEN Make() END;
  IF    x>1.0 THEN IF Top()>0 THEN Push(3); END;
                   Normalize(Top());
  ELSIF x<0.0 THEN Push(2); Normal(); Push(0); 
                   Normalize(Top());  x:=y;
  ELSE             Normal();
  END;
  ASSERT(stack^.val<0);
  stack:=NIL;
END lists.
