/***  Copyright (c) xTech 1997.  All Rights Reserved ***/
/***  Utility library: regular expressions           ***/

#ifndef __RCTHUNK__
#define __RCTHUNK__

extern "C"
{
  typedef LHANDLE  HEXPR;
  typedef HEXPR   *PHEXPR;
  void RCThunk_Compile (PSZ szExpr, PHEXPR phExpr, PLONG pRes);
  /** res <= 0 -- error in position ABS(res);
      res >  0 -- done.
  */

  void _System RCThunk_FreeExpr (HEXPR hExpr);

  BOOL _System RCThunk_Const (HEXPR hExpr);
  /** Returns TRUE, if expression does not contain wildcards */

  BOOL _System RCThunk_Match (HEXPR hExpr, PSZ sz);
  /** Returns TRUE, iff expression matches with string "sz"
  */

  BOOL _System RCThunk_FastMatch (PSZ expr, PSZ s);
  /** Returns TRUE, iff expression matches with string "s" or the expression is empty string */

  void RCThunk_Substitute (HEXPR hExpr, PSZ s, PSZ m, PSZ d, LONG dlen);
  /** Substitutes  the substrings of "s" matched with "hExpr" instead
    of "$digit" in the "m" and copies the resulting string into "d" (up to "dlen" charachers).
  */

  void _System RCThunk_Substitute (HEXPR hExpr, PSZ s, PSZ m, PSZ d, LONG dlen);
  /** Substitutes  the substrings of "s" matched with "re" instead
    of "$digit" in the "m" and copies the resulting string into "d" (up to "dlen" symbols),
  */

  LONG _System RCThunk_Len (HEXPR hExpr, LONG n);
  /** Returns the length of  the  substring matched to "$n"
    at last call of match procedure with parameter "re".
  */

  LONG _System RCThunk_Pos (HEXPR hExpr, LONG n);
  /** Returns the position of the  beginning  of  the  substring
    matched to "$n" at last call of match procedure with
    parameter "re".
  */

}

#endif /* ifndef __RCTHUNK__ */

