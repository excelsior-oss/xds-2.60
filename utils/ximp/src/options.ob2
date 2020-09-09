MODULE Options;

CONST

(* Option names *)
  genunused * = "genunused";
  genloops  * = "genloops";
  genimp    * = "genimp";

(* Options *)
  Options * = '-' + genunused + '-' + ';' +
              '-' + genloops  + '-' + ';' +
              '-' + genimp    + '=';


(* Default option values *)
  Values * = '-' + genunused + '+' + ';' +
             '-' + genloops  + '+' + ';' +
             '-' + genimp    + '=';
END Options.
