
IMPLEMENTATION MODULE dll1;
FROM  WholeStr IMPORT CardToStr;
FROM  Printf   IMPORT sprintf;

VAR count: CARDINAL;
    n    : CARDINAL;
-- заносить текущее время в паспорт
PROCEDURE ["StdCall"] RegisterWindow(VAR p:PASSPORT);
BEGIN
   p.time := count;
   INC(count);
END RegisterWindow;

-- выдает случайно некоторое количество ID  - от 0 до 10 штук
PROCEDURE ["StdCall"] IterId(p-: PASSPORT; VAR id: CARDINAL): CARDINAL;
BEGIN
  IF n <= 100 THEN
 --   id := p.time + n;
      id := n;
    RETURN 1;
  ELSE
    RETURN 0;
  END;
END IterId;


-- по ID выдает строчку - целое число
PROCEDURE ["StdCall"] IterData1 (p-: PASSPORT; id: CARDINAL; VAR str: STRING);
BEGIN
  CardToStr( id, str);
END IterData1;


-- по ID выдает строчку - целое число в 16 с/с
PROCEDURE ["StdCall"] IterData2 (p-: PASSPORT; id: CARDINAL; VAR str: STRING);
BEGIN
   sprintf(str, "%o",id);
END IterData2;

BEGIN
 count := 0;
 n     := 10;
END dll1.