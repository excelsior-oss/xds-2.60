 
package qqq;

import java.util.Locale;
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import qqq.dialog1;
import java.util.Vector;
import qqq.middle;
 
public class myFrame extends JFrame {
        
private myFrame Parent = null;
  
  public myFrame() {
    Locale.setDefault(Locale.US);
    initComponents ();
    pack ();
  }
  public myFrame(myFrame parent) {
    Parent = parent;
    Locale.setDefault(Locale.US);
    initComponents ();
    pack ();
  }

    
 private void initComponents () {  
    JScrollPane jScrollPane1 = new JScrollPane ();
    JButton     b            = new JButton();
    jScrollPane1.setCorner(JScrollPane.LOWER_RIGHT_CORNER, b);
    getContentPane ().add (jScrollPane1, BorderLayout.CENTER);
  
    
    Data = new  DefaultListModel();
    Data.clear();
    mid  = new middle();
    initData();
    DefaultListSelectionModel SelectModel = new DefaultListSelectionModel();
    jList1 = new JList ();
    jList1.setModel (Data);
    jList1.setSelectionModel (SelectModel);
    jList1.setSelectedIndex (0);
    jList1.setSelectionMode (javax.swing.ListSelectionModel.SINGLE_SELECTION);
    jList1.setForeground (Color.blue);
    jScrollPane1.setViewportView (jList1);

      jList1.addMouseListener (new java.awt.event.MouseAdapter () {
         public void mouseClicked (java.awt.event.MouseEvent evt) {
            mouseDbl (evt);
        }
        public void mousePressed(MouseEvent evt){
          maybeShowPopup(evt);
        }
        public void mouseReleased(MouseEvent evt){
          maybeShowPopup(evt);
        }  
      }
      );
     setPopupMenu();
  }
 
public void add(Object s){
  Data.addElement(s);
}


private void setPopupMenu(){
    jPopupMenu = new JPopupMenu ();


    JMenuItem  jMenuItem1  = new JMenuItem ();
    JMenuItem  jMenuItem2  = new JMenuItem ();
    //jPopupMenu1.setInvoker (jList1);

    final myItemAction temAction1 = new myItemAction ("hello 1");
    final myItemAction temAction2 = new myItemAction ("hello 2");

    jMenuItem1.setText ("hello 1");
    jMenuItem1.addActionListener (new java.awt.event.ActionListener () {
        public void actionPerformed (java.awt.event.ActionEvent evt) {
          temAction1.ItemAction (evt);
        }
      }
   );
  
    jPopupMenu.add (jMenuItem1);
    

    jMenuItem2.setText ("hello 2");
    jMenuItem2.addActionListener (new java.awt.event.ActionListener () {
        public void actionPerformed (java.awt.event.ActionEvent evt) {
          temAction2.ItemAction (evt);
        }
      }
   );
  
    jPopupMenu.add (jMenuItem2);
    

}
private void maybeShowPopup (java.awt.event.MouseEvent evt) {
        if (evt.isPopupTrigger()){ 
          Point point = new Point(evt.getX(),evt.getY());
          int index  = jList1.locationToIndex(point);
          if(index != -1){
            jList1.setSelectedIndex(index);
            jPopupMenu.show(evt.getComponent(),
                           evt.getX(), evt.getY());
          } else{
            jList1.clearSelection();
          }
        }  
  }

private middle mid;
private void initData(){
  Vector vdata1 = new Vector();
  Vector vdata2 = new Vector();
  vdata1 = mid.getData1();
  vdata2 = mid.getData2();
  int size = vdata1.size();
  for(int i = 0; i < size ; i++){
   Data.addElement(vdata1.get(i));
  }
}
 
private void mouseDbl (java.awt.event.MouseEvent evt) {
  int count = evt.getClickCount();
  if (count != 2)
   return;
  String input = (String)jList1.getSelectedValue();
  dialog1 dialog = new dialog1(this,input,true);
  dialog.setLocationRelativeTo(this);
  dialog.setVisible(true);
  input = dialog.getValidatedText();
  if(input != null){
    Point point = new Point(evt.getX(),evt.getY());
    final int index  = jList1.locationToIndex(point);
    if(index != -1){
      jList1.setSelectedIndex(index);
    qqq.myFrame frame = new qqq.myFrame();
    frame.setTitle("New Window");
    frame.add(input);  
    frame.show();   
   }  else
     jList1.clearSelection();
  }
}

  
  public  DefaultListModel Data;
  private JList            jList1;
  private JPopupMenu       jPopupMenu;
       
  


class myItemAction extends Object
{
  private String ItemName;

  public myItemAction (String itemname)
  {
    ItemName = itemname;
  }

  void ItemAction (java.awt.event.ActionEvent evt) {
    int index = jList1.getSelectedIndex();
    if ( index != -1 ){
      Data.addElement(jList1.getModel().getElementAt(index));
      index = Data.getSize() - 1;
      jList1.ensureIndexIsVisible(index);
    }
    jList1.clearSelection();
    Data.addElement(ItemName);
    jList1.ensureIndexIsVisible(Data.lastIndexOf(ItemName));
  }

}



}