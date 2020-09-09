/* OS2RTL.H - extra macros for XDS-C OS/2 API support */

#define MPFROMUSHORT(s)             ((MPARAM)(USHORT)(s))
#define MPFROM2USHORT(s1, s2)       ((MPARAM)MAKEULONG(s1, s2))
#define MPFROMULONG(l)              ((MPARAM)(ULONG)(l))

#define USHORT1FROMMP(mp)           ((USHORT)(ULONG)(mp))
#define USHORT2FROMMP(mp)           ((USHORT)((ULONG)mp >> 16))
#define ULONGFROMMP(mp)             ((ULONG)(mp))

#define MRFROMUSHORT(s)             ((MRESULT)(USHORT)(s))
#define MRFROM2USHORT(s1, s2)       ((MRESULT)MAKEULONG(s1, s2))
#define MRFROMULONG(l)              ((MRESULT)(ULONG)(l))

#define USHORT1FROMMR(mr)           ((USHORT)((ULONG)mr))
#define USHORT2FROMMR(mr)           ((USHORT)((ULONG)mr >> 16))
#define ULONGFROMMR(mr)             ((ULONG)(mr))
