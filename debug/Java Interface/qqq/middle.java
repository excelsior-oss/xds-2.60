package qqq;

 import com.excelsior.xFunction.*;
 import qqq.passport;
 import java.util.Vector;

public class middle extends Object {

     
  public middle(){
    initPassport();
  }
 
  public Vector getData1(){
    Vector Data = new Vector();
    isNext = 1;
    int ID = iterID();
    while (isNext == 1){
      Data.add(iterData1(ID));
      ID = iterID();
    }    
    return Data;
  }

  public Vector getData2() {
    Vector Data = new Vector();
    isNext = 1;
    while (isNext == 1){
      int ID = iterID();
      Data.add(iterData2(ID));
    }
    return Data;
  }
  
  private int time;  
  private int isNext;


  private passport pass;

  private void initPassport(){
    pass = new passport();
    try{
      xFunction registerWindow = new xFunction("DLL1", "void RegisterWindow(qqq.passport*)");
      Pointer arg = Pointer.createPointerTo(pass);
      registerWindow.invoke(arg);
      pass = (qqq.passport)arg.deref();
    } 
    catch(xFunctionException e) {
      e.printStackTrace();
     
    }
  }

  private int iterID() { 
    int ID = 0;
    try{
      xFunction iterID = new xFunction("DLL1", "int IterId(qqq.passport*, int*)");
      Pointer  arg = Pointer.create("int*");
      isNext = ((Integer)(iterID.invoke(Pointer.createPointerTo(pass), arg))).intValue();
      ID = ((Integer)arg.deref()).intValue();
    }
    catch(xFunctionException e) {
      e.printStackTrace();
    }
   return ID;
  }


  private String iterData1(int ID) {
    String str = null;
    try{
      xFunction iterData1 = new xFunction("DLL1", "void IterData1(qqq.passport*,int,CSTRING)");
      Argument arg = new Argument("",Argument.CSTRING); 
      iterData1.invoke(Pointer.createPointerTo(pass),new Argument(ID), arg);
      str = (String)(arg.getValue());
    }
    catch(xFunctionException e) {
      e.printStackTrace();
    }
   return str;
  }
        
   private String iterData2(int ID) {
    String str = null;
     try{
       xFunction iterData2 = new xFunction("DLL1", "void IterData2(qqq.passport*,int,CSTRING)");
       Argument arg = new Argument("",Argument.CSTRING); 
       iterData2.invoke(Pointer.createPointerTo(pass),new Argument(ID), arg);
       str = (String)(arg.getValue());
     }
    catch(xFunctionException e) {
      e.printStackTrace();
    }
   return str;
  }

}

 