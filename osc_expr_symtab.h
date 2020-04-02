#ifndef __OSC_EXPR_SYMTAB_H__
#define __OSC_EXPR_SYMTAB_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "osc_expr.h"

struct _osc_expr_rec *
osc_expr_symtab_lookup (register const char *str, register size_t len);
int osc_expr_symtab_num_functions(void);
char *osc_expr_symtab_get_ith_function_name(int i);
#ifdef __cplusplus
}
#endif

#endif
