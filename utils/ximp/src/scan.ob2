MODULE scan;
IMPORT
  SYSTEM,
  file:= H2DFile,
  lstr:= LongStrs,
  io:= Printf;

TYPE INT * = SYSTEM.INT32;

CONST
  (* Tokens *)
  ident     * = 0;
  period    * = 1;
  comma     * = 2;
  semicolon * = 3;
  becomes   * = 4;
  eof       * = 255;

VAR f: file.FILE;
    s: lstr.String;
    line, pos: INT;
    name: lstr.String;
    init: BOOLEAN;


PROCEDURE Error (fmt-: ARRAY OF CHAR; SEQ x: SYSTEM.BYTE);
BEGIN
  io.printf(fmt, x);
  HALT;
END Error;


PROCEDURE Open * (nm: ARRAY OF CHAR);
BEGIN
  IF ~file.Open(nm, file.rdmode, f) THEN
    Error("Can't open file %s", nm);
  END;
  lstr.Assign(nm, name);
  file.RdStr(f, s); pos:= 0; line:= 0;
  init:= TRUE;
END Open;

PROCEDURE RdStr();
BEGIN
  file.RdStr(f, s); pos:= 0; INC(line);
END RdStr;

PROCEDURE GetToken * (VAR text: lstr.String): INT;
VAR comments, comment_line, comment_pos: INT;
    i: INT;
    flag: BOOLEAN;
    border: CHAR;

BEGIN
  ASSERT(init);
  lstr.Assign('', text);
  LOOP
    IF s[pos] = 0X THEN
      RdStr();
    END;
    IF s = NIL THEN init:= FALSE; RETURN eof END;
    CASE s[pos] OF
       0X:
      |'"', "'":
            comment_line:= line; comment_pos:= pos;
            border:= s[pos]; INC(pos);
            WHILE (s[pos] # 0X) & (s[pos] # border) DO INC(pos) END;
            IF s[pos] = 0X THEN
              Error("Unclosed string constant: %s [%d:%d]\n", name^, comment_line+1, comment_pos+1);
            ELSE
              INC(pos);
            END;
      |'-': IF s[pos+1] = '-' THEN
              RdStr();
              IF s = NIL THEN init:= FALSE; RETURN eof END;
            ELSE
              INC(pos);
            END;
      |':': IF s[pos+1] = '=' THEN
              INC(pos, 2); RETURN becomes;
            ELSE
              INC(pos);
            END;
      |'(': IF s[pos+1] = '*' THEN
              comments:= 1;
              comment_line:= line; comment_pos:= pos;
              INC(pos, 2);
              WHILE (s # NIL) & (comments > 0) DO
                WHILE (s[pos] # 0X) & (s[pos] # '*') & (s[pos] # '(') DO INC(pos) END;
                IF s[pos] = 0X THEN
                  RdStr();
                  IF s = NIL THEN
                    Error("Unclosed comment: %s [%d:%d]\n", name^, comment_line+1, comment_pos+1);
                  END;
                ELSIF (s[pos] = '(') & (s[pos+1] = '*') THEN
                  INC(comments);
                  INC(pos, 2);
                ELSIF (s[pos] = '*') & (s[pos+1] = ')') THEN
                  INC(pos, 2); DEC(comments);
                ELSE
                  INC(pos);
                END;
              END;
            ELSE
              INC(pos);
            END;
      |'<': IF s[pos+1] = '*' THEN
              comment_line:= line; comment_pos:= pos;
              INC(pos, 2); flag:= TRUE;
              WHILE (s # NIL) & flag DO
                WHILE (s[pos] # 0X) & (s[pos] # '*') DO INC(pos) END;
                IF s[pos] = 0X THEN
                  RdStr();
                  IF s = NIL THEN
                    Error("Unclosed directive: %s [%d:%d]\n", name^, comment_line+1, comment_pos+1);
                  END;
                ELSIF (s[pos] = '*') & (s[pos+1] = '>') THEN
                  INC(pos, 2); flag:= FALSE;
                ELSE
                  INC(pos);
                END;
              END;
            ELSE
              INC(pos);
            END;
      |'A'..'Z', 'a'..'z', '_':
            lstr.AppendChar(s[pos], text);
            INC(pos);
            i:= 1;
            WHILE (s[pos] >= 'A') & (s[pos] <= 'Z') OR
                  (s[pos] >= 'a') & (s[pos] <= 'z') OR
                  (s[pos] >= '0') & (s[pos] <= '9') OR
                  (s[pos] = '_')
            DO
              lstr.AppendChar(s[pos], text);
              INC(pos);
            END;
            RETURN ident;
      |'0'..'9':
            WHILE (s[pos] >= 'A') & (s[pos] <= 'Z') OR
                  (s[pos] >= 'a') & (s[pos] <= 'z') OR
                  (s[pos] >= '0') & (s[pos] <= '9')
            DO INC(pos) END;
      |'.': INC(pos); RETURN period;
      |',': INC(pos); RETURN comma;
      |';': INC(pos); RETURN semicolon;
    ELSE
      INC(pos);
    END;
  END;
END GetToken;

BEGIN
  s:= NIL; pos:= 0; line:= 0;
  init:= FALSE;
END scan.
