
import java.awt.*;
import DATA;

class CloseAction implements java.awt.event.ActionListener {
	public void actionPerformed(java.awt.event.ActionEvent e) {
		System.exit(0);
	}
}

class SaveAction implements java.awt.event.ActionListener {
	public void actionPerformed(java.awt.event.ActionEvent e) {
		DATA d = new DATA();
		RegKey.StrData = RegKey.f.StrT.getText();
		try {
			RegKey.IntData = new Integer(RegKey.f.IntT.getText()).intValue();
		} catch(NumberFormatException ex) {}
		d.Data = RegKey.StrData.getBytes();
		RegistryKey.RegSetValue(RegKey.HK,"StrData",RegistryKey.REG_SZ,d);
		d.Data = new byte[4];
		d.Data[0] = (byte)(RegKey.IntData % 256);
		d.Data[1] = (byte)((RegKey.IntData>>8) % 256);
		d.Data[2] = (byte)((RegKey.IntData>>16) % 256);
		d.Data[3] = (byte)((RegKey.IntData>>24) % 256);
		RegistryKey.RegSetValue(RegKey.HK,"IntData",RegistryKey.REG_DWORD,d);
		RegistryKey.RegCloseKey(RegKey.HK);
		System.exit(0);
	}
}

class MyFrame extends Frame{
	static int W = 200;
	static int H = 100;
	TextField StrT;
	TextField IntT;

	public MyFrame(String s) {
		super(s);
	}

        public void go(){
		Button SaveB, CloseB;
		Label StrL = new Label("StringValue ",Label.CENTER);
		Label IntL = new Label("IntValue ", Label.CENTER);
		StrT = new TextField(RegKey.StrData);
		IntT = new TextField("" + RegKey.IntData);
		SaveB = new Button("Save & Close");
		SaveB.addActionListener(new SaveAction());
		CloseB = new Button("Close w/o save");
		CloseB.addActionListener(new CloseAction());
		setLayout(new GridLayout(3,2));
		reshape(W,H,W,H);
		setResizable(false);
		add(StrL);
		add(StrT);
		add(IntL);
		add(IntT);
		add(SaveB);
		add(CloseB);
		show();
	}
}
	

class RegKey {
	static public MyFrame f;
	static String StrData;
	static int IntData;
	static HKEY HK;
	static public void main(String p[]) {
		HKEY HK1 = new HKEY(0), HK2 = new HKEY(0);
		HK = new HKEY(0);
		DATA d = new DATA();
		int i;
		if(RegistryKey.RegOpenKey(RegistryKey.HKEY_LOCAL_MACHINE,"Software", HK1)==-1)
			throw new Error("Can't open HKEY_LOCAL_MACHINE\\Software");
		if((RegistryKey.RegOpenKey(HK1, "XDS", HK2))==-1)
			if((RegistryKey.RegCreateKey(HK1, "XDS", HK2))==-1)
				throw new Error("Can't open/create XDS subkey");
		if((i=RegistryKey.RegCreateKey(HK2, "Java Native Sample", HK))==-1)
			throw new Error("Can't create Sample subkey");
		if(i==0) {
			System.out.println("Sample key was created");
			StrData = "";
			IntData = 0;
			d.Data = StrData.getBytes();
			RegistryKey.RegSetValue(HK,"StrData",RegistryKey.REG_SZ,d);
			d.Data = new byte[4]; d.Data[0]=d.Data[1]=d.Data[2]=d.Data[3]=0;
			RegistryKey.RegSetValue(HK,"IntData",RegistryKey.REG_DWORD,d);
		} else {
			System.out.println("Sample key was opened");
			RegistryKey.RegGetValue(HK, "StrData", RegistryKey.REG_SZ,d);
			StrData = new String(d.Data,0);
			RegistryKey.RegGetValue(HK, "IntData", RegistryKey.REG_DWORD,d);
			IntData = (d.Data[3] >= 0) ? d.Data[3] : 256+d.Data[3]; IntData *= 256;
			IntData += (d.Data[2] >= 0) ? d.Data[2] : 256+d.Data[2]; IntData *= 256;
			IntData += (d.Data[1] >= 0) ? d.Data[1] : 256+d.Data[1]; IntData *= 256;
			IntData += (d.Data[0] >= 0) ? d.Data[0] : 256+d.Data[0];
		}
		System.out.println("StrData = "+StrData);
		System.out.println("IntData = "+IntData);
		f = new MyFrame("Registry Key Editor");
		f.go();
	}
}