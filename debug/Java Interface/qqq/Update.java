package qqq;

import com.excelsior.xFunction.*;


public class Update extends Callback {


    private static void someFunction(){
        System.out.println("someFunction called");
    }

    public void invokeUpdate(){
        try{
            xFunction regUp = new xFunction("DLL1", "void RegisterUpdate(qqq.Update)");

            regUp.invoke(new Argument(this));

            System.out.println("OK");

        } 
        catch(xFunctionException e) {
            e.printStackTrace();
        }

    }

    public String defineSignature() {
        return "void someFunction()";
    }

}
