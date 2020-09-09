 
package qqq;
import qqq.jFrame1;
import javax.swing.*;
import qqq.myFrame;
import qqq.Update;

public class prog {


  public prog() { 
   Update update = new Update();
   update.invokeUpdate();
   mainFrame = new jFrame1 ();
   mainFrame.setTitle("Heloooooooo");
/*   for(int i = 0; i < 10; i++){
     mainFrame.add(Frase[i]);
   }*/
 }
  
 
  public static void main (String args[]) {
    new prog();
    mainFrame.show();
 
  }
  static jFrame1 mainFrame ;
  private static String[] Frase = {"Hello","My","Darling","!","Sono",
  "molto","contenta","di","vederti","Ciao","See you later"};

}