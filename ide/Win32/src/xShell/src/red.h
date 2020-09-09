typedef struct _Expr Expr;
typedef struct _Redir Redir;

/** result <= 0 -- error in position ABS (result);
    result >  0 -- done.
*/

int	compile_expr (unsigned char * _expr, Expr ** reg);
int	Match (Expr * reg, unsigned char *s);
void	FreeExpr (Expr * reg);

void	Add_redir (Redir ** p, Redir * q);
Redir *	ParseRedLine (char * p);
void	FreeRedir (Redir * r);

int	Search_with_red (char * name, char * res, Redir * red);
int	Create_with_red (char * name, char * res, Redir * red);