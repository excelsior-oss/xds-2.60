
IMPLEMENTATION MODULE RegEdt ;

IMPORT Windows, SYSTEM, JNI, Storage;

CONST MAX_VALUE_SIZE = 4096;

--- JNIEXPORT jint JNICALL Java_RegistryKey_RegCreateKey
---   (JNIEnv *, jclass, jobject, jstring, jobject);

PROCEDURE ["StdCall"] Java_RegistryKey_RegCreateKey(PJNIENV : JNI.PJNIEnv; This : JNI.jclass;
        ParentKey : JNI.jobject; KeyName : JNI.jstring; HKey : JNI.jobject) : JNI.jint;
VAR Disp : Windows.REG_DISPOSITION;
    Fid1, Fid2 : JNI.jfieldID;
    KeyN : JNI.PCHAR;
    HKey0 : Windows.HKEY;
BEGIN
  Fid1 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,ParentKey), "HKey", "I");
  KeyN := PJNIENV^^.GetStringUTFChars(PJNIENV, KeyName, NIL);
  IF Windows.RegCreateKeyEx(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, ParentKey, Fid1)),
      KeyN,0,NIL,Windows.REG_OPTION_NON_VOLATILE,Windows.KEY_ALL_ACCESS, NIL,
      HKey0,Disp) # Windows.ERROR_SUCCESS THEN
      PJNIENV^^.ReleaseStringUTFChars(PJNIENV, KeyName, KeyN);
    RETURN -1;
  ELSE
    PJNIENV^^.ReleaseStringUTFChars(PJNIENV, KeyName, KeyN);
    Fid2 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,HKey), "HKey", "I");
    PJNIENV^^.SetIntField(PJNIENV, HKey, Fid2, SYSTEM.CAST(JNI.jint, HKey0));
    IF Disp = Windows.REG_CREATED_NEW_KEY THEN
      RETURN 0;
    ELSE
      RETURN 1;
    END;
  END;
END Java_RegistryKey_RegCreateKey;

--- JNIEXPORT jint JNICALL Java_RegistryKey_RegOpenKey
---   (JNIEnv *, jclass, jobject, jstring, jobject);
PROCEDURE ["StdCall"] Java_RegistryKey_RegOpenKey(PJNIENV : JNI.PJNIEnv; This : JNI.jclass;
        ParentKey : JNI.jobject; KeyName : JNI.jstring; HKey : JNI.jobject) : JNI.jint;
VAR
    Fid1, Fid2 : JNI.jfieldID;
    KeyN : JNI.PCHAR;
    HKey0 : Windows.HKEY;
BEGIN
  Fid1 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,ParentKey), "HKey", "I");
  Fid2 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,HKey), "HKey", "I");
  KeyN := PJNIENV^^.GetStringUTFChars(PJNIENV, KeyName, NIL);
  IF Windows.RegOpenKeyEx(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, ParentKey, Fid1)),
      KeyN,0,Windows.KEY_READ + Windows.KEY_CREATE_SUB_KEY, HKey0) # Windows.ERROR_SUCCESS THEN
    PJNIENV^^.ReleaseStringUTFChars(PJNIENV, KeyName, KeyN);
    RETURN -1;
  ELSE
    PJNIENV^^.SetIntField(PJNIENV, HKey, Fid2, SYSTEM.CAST(JNI.jint, HKey0));
    PJNIENV^^.ReleaseStringUTFChars(PJNIENV, KeyName, KeyN);
    RETURN 0;
  END;
END Java_RegistryKey_RegOpenKey;


--- JNIEXPORT void JNICALL Java_RegistryKey_RegCloseKey
---   (JNIEnv *, jclass, jobject);

PROCEDURE ["StdCall"] Java_RegistryKey_RegCloseKey(PJNIENV : JNI.PJNIEnv; This : JNI.jclass; HKey : JNI.jobject);
VAR Fid : JNI.jfieldID;
BEGIN
  Fid := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,HKey), "HKey", "I");
  Windows.RegCloseKey(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, HKey, Fid)));
END Java_RegistryKey_RegCloseKey;

--- JNIEXPORT void JNICALL Java_RegistryKey_RegSetValue
---   (JNIEnv *, jclass, jobject, jstring, jobject, jobject);

PROCEDURE ["StdCall"] Java_RegistryKey_RegSetValue(PJNIENV : JNI.PJNIEnv; This : JNI.jclass;
        HKey : JNI.jobject; ValName : JNI.jstring;
                             ValType : JNI.jobject; Data : JNI.jobject);
VAR Fid1, Fid2, Fid3 : JNI.jfieldID;
    ValN : JNI.PCHAR;
    Dat  : JNI.PCHAR;
    DLen : CARDINAL;
BEGIN
  Fid1 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,HKey), "HKey", "I");
  Fid2 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,ValType), "RegType", "I");
  Fid3 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,Data), "Data", "[B");
  ValN := PJNIENV^^.GetStringUTFChars(PJNIENV, ValName, NIL);
  DLen := SYSTEM.CAST(CARDINAL, PJNIENV^^.GetArrayLength(PJNIENV, PJNIENV^^.GetObjectField(PJNIENV, Data, Fid3)));
  Dat := PJNIENV^^.GetByteArrayElements(PJNIENV, PJNIENV^^.GetObjectField(PJNIENV, Data, Fid3), NIL);
  Windows.RegSetValueEx(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, HKey, Fid1)),
   ValN,0,SYSTEM.CAST(Windows.REG_TYPE, PJNIENV^^.GetIntField(PJNIENV, ValType, Fid2)),
   Dat, DLen);
  PJNIENV^^.ReleaseStringUTFChars(PJNIENV, ValName, ValN);
END Java_RegistryKey_RegSetValue;

--- JNIEXPORT void JNICALL Java_RegistryKey_RegGetValue
---   (JNIEnv *, jclass, jobject, jstring, jobject, jobject);

PROCEDURE ["StdCall"] Java_RegistryKey_RegGetValue(PJNIENV : JNI.PJNIEnv; This : JNI.jclass;
    HKey : JNI.jobject; ValName : JNI.jstring; ValType : JNI.jobject; Data : JNI.jobject);
VAR Fid1, Fid2, Fid3 : JNI.jfieldID;
    ValN : JNI.PCHAR;
    VType : Windows.REG_TYPE;
    Dat  : ARRAY[1..MAX_VALUE_SIZE] OF JNI.jbyte;
    DLen : CARDINAL;
    DatArr : JNI.jbyteArray;
BEGIN
  Fid1 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,HKey), "HKey", "I");
  Fid2 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,ValType), "RegType", "I");
  Fid3 := PJNIENV^^.GetFieldID(PJNIENV, PJNIENV^^.GetObjectClass(PJNIENV,Data), "Data", "[B");
  ValN := PJNIENV^^.GetStringUTFChars(PJNIENV, ValName, NIL);
  Windows.RegQueryValueEx(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, HKey, Fid1)),
     ValN, NIL, VType, NIL, DLen);

  Windows.RegQueryValueEx(SYSTEM.CAST(Windows.HKEY, PJNIENV^^.GetIntField(PJNIENV, HKey, Fid1)),
     ValN, NIL, VType, SYSTEM.ADR(Dat), DLen);
  PJNIENV^^.ReleaseStringUTFChars(PJNIENV, ValName, ValN);
  PJNIENV^^.SetIntField(PJNIENV, ValType, Fid2, SYSTEM.CAST(JNI.jint, VType));
  DatArr := PJNIENV^^.NewByteArray(PJNIENV,SYSTEM.CAST(JNI.jsize, DLen));
  PJNIENV^^.SetByteArrayRegion(PJNIENV, DatArr ,0, DLen, SYSTEM.ADR(Dat));
  PJNIENV^^.SetObjectField(PJNIENV, Data, Fid3, SYSTEM.CAST(JNI.jobject, DatArr));
END Java_RegistryKey_RegGetValue;

END RegEdt.