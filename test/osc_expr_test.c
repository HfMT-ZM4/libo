#include <stdio.h>
#include <string.h>
#include "osc.h"
#include "osc_bundle_u.h"
#include "osc_atom_array_u.h"
#include "osc_parser.h"
#include "osc_expr.h"
#include "osc_expr_parser.h"

int main(int ac, char **av)
{
	//char *expr = "/foo./bar./bloo[[3]] = 20";
	//char *expr = "/a = /foo./bar./bloo";
	//char *expr = "/foo = assigntobundlemember({}, /bar, 20)";
	char *exprs[] = {
		"/x = /a./b./c, /y = apply(lambda(a, a./b./c), /a)",
		"/x = /a./b, /y = apply(lambda(a, a./b), /a)",
	};
	char *bndls[] = {
		"/a : { /b : { /c : 33 } }",
		"/a : { /b : 33 }",
	};
	int n = sizeof(exprs) / sizeof(char*);
	printf("n = %d\n", n);
	for(int i = 0; i < n; i++){
		printf("********************\n");
		char *expr = exprs[i];
		char *bndl = bndls[i];
		printf("parsing expr: \n%s\n", expr);
		t_osc_expr *e = NULL;
		t_osc_err err = osc_expr_parser_parseExpr(expr, &e, NULL);
		if(err){
			printf("error parsing expression: %llu\n", err);
			continue;
		}
		t_osc_bndl_u *b = NULL;
		printf("parsing bndl: \n%s\n", bndl);
		err = osc_parser_parseString(strlen(bndl), bndl, &b);
		if(err){
			printf("error parsing bndl\n");
			continue;
		}
		t_osc_bndl_s *bs = osc_bundle_u_serialize(b);
		long len = osc_bundle_s_getLen(bs);
		char *ptr = osc_bundle_s_getPtr(bs);
		t_osc_atom_ar_u *out = NULL;
		printf("applying expr to bundle\n");
		while(e){
			int i = osc_expr_eval(e, &len, &ptr, &out, NULL);
			if(i){
				printf("got %d while evaluating expr\n", i);
				continue;
			}
			e = osc_expr_next(e);
		}
		char *pp = osc_bundle_s_format(len, ptr);
		printf("%s\n", pp);
	}
}
