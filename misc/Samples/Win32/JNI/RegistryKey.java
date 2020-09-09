
class HKEY {
	int HKey;
	public HKEY(int K) { HKey = K; }
}

class REG_TYPE {
	int RegType;
	public REG_TYPE(int R) { RegType = R; }
}

class DATA {
	byte Data[];
}

class RegistryKey {

	final static HKEY HKEY_CLASSES_ROOT	= new HKEY(0x80000000);
	final static HKEY HKEY_CURRENT_USER	= new HKEY(0x80000001);
	final static HKEY HKEY_LOCAL_MACHINE	= new HKEY(0x80000002);
	final static HKEY HKEY_USERS		= new HKEY(0x80000003);
	final static HKEY HKEY_PERFORMANCE_DATA	= new HKEY(0x80000004);
	final static HKEY HKEY_CURRENT_CONFIG	= new HKEY(0x80000005);
	final static HKEY HKEY_DYN_DATA		= new HKEY(0x80000006);

	final static REG_TYPE REG_NONE			= new REG_TYPE(0);
	final static REG_TYPE REG_SZ			= new REG_TYPE(1);
	final static REG_TYPE REG_EXPAND_SZ		= new REG_TYPE(2);
	final static REG_TYPE REG_BINARY		= new REG_TYPE(3);
	final static REG_TYPE REG_DWORD_LITTLE_ENDIAN	= new REG_TYPE(4);
	final static REG_TYPE REG_DWORD_BIG_ENDIAN	= new REG_TYPE(5);
	final static REG_TYPE REG_LINK			= new REG_TYPE(6);
	final static REG_TYPE REG_MULTI_SZ		= new REG_TYPE(7);
	final static REG_TYPE REG_RESOURCE_LIST		= new REG_TYPE(8);
	final static REG_TYPE REG_FULL_RESOURCE_DESCRIPTOR = new REG_TYPE(9);
	final static REG_TYPE REG_RESOURCE_REQUIREMENTS_LIST = new REG_TYPE(10);

	final static REG_TYPE REG_DWORD			= REG_DWORD_LITTLE_ENDIAN;


	static public native int RegCreateKey(HKEY ParentKey, String KeyName, HKEY HKey);

	static public native void RegCloseKey(HKEY HKey);

	static public native void RegSetValue(HKEY HKey, String ValName,
                             REG_TYPE ValType, DATA Data);
			
	static public native void RegGetValue(HKEY HKey, String ValName,
                             REG_TYPE ValType, DATA Data);

	static public native int RegOpenKey(HKEY ParentKey, String KeyName, HKEY HKey);

	static {
             System.loadLibrary("RegEdt");
	}

}

