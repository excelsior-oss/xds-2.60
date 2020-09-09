import com.excelsior.xFunction.*;

class sample {

    public static void main(String[] args) throws xFunctionException {

        xFunction registerWindow = new xFunction("DLL1", "void RegisterWindow(PASSPORT*)");


        PASSPORT pass = new PASSPORT();
        pass.time = 12345;

        Pointer arg = Pointer.createPointerTo(pass);

	registerWindow.invoke(arg);

	registerWindow.invoke(arg);
        PASSPORT res = (PASSPORT)arg.deref();


        System.out.println(res.time);

	System.out.println("OK");

        xFunction iterID = new xFunction("DLL1", "int IterId(PASSPORT, int*)");
        
        Pointer  arg1 = Pointer.create("int*");
        int res1  = ((Integer)(iterID.invoke(new Argument(pass), arg1))).intValue();
        int ID = ((Integer)arg1.deref()).intValue();
        System.out.println(res1);
        System.out.println(ID);

        xFunction iterData1 = new xFunction("DLL1", "void IterData1(PASSPORT,int,CSTRING)");

        Argument argstr = new Argument("a",Argument.CSTRING); 
        iterData1.invoke(new Argument(pass),new Argument(17), argstr);
	String str2 = (String)(argstr.getValue());
        System.out.println(str2);
        

    }



}
/*
-- по ID выдает строчку - целое число
PROCEDURE IterData1 (p:PASSPORT; id:CARDINAL; VAR str: STRING); */


//PROCEDURE RegisterWindow(VAR p:PASSPORT);
/*
 PASSPORT = RECORD
   time: CARDINAL;
 END; 
*/


class PASSPORT extends Structure {

    public PASSPORT() {}

    int time;

    public String defineLayout(){
        return "int time";
    }

}



/*-- выдает случайно некоторое количество ID  - от 0 до 10 штук
PROCEDURE IterId(p: PASSPORT; VAR id: CARDINAL): BOOLEAN;*/




