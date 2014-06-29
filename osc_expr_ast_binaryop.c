/*
  Written by John MacCallum, The Center for New Music and Audio Technologies,
  University of California, Berkeley.  Copyright (c) 2011-13, The Regents of
  the University of California (Regents). 
  Permission to use, copy, modify, distribute, and distribute modified versions
  of this software and its documentation without fee and without a signed
  licensing agreement, is hereby granted, provided that the above copyright
  notice, this paragraph and the following two paragraphs appear in all copies,
  modifications, and distributions.

  IN NO EVENT SHALL REGENTS BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT,
  SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOST PROFITS, ARISING
  OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF REGENTS HAS
  BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  REGENTS SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE. THE SOFTWARE AND ACCOMPANYING DOCUMENTATION, IF ANY, PROVIDED
  HEREUNDER IS PROVIDED "AS IS". REGENTS HAS NO OBLIGATION TO PROVIDE
  MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
*/

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "osc.h"
#include "osc_mem.h"
#include "osc_expr_builtin.h"
#include "osc_expr_oprec.h"
#include "osc_expr_ast_expr.h"
#include "osc_expr_ast_binaryop.h"
#include "osc_expr_ast_binaryop.r"
#include "osc_expr_ast_value.h"

int osc_expr_ast_binaryop_evalInLexEnv(t_osc_expr_ast_expr *ast,
				       t_osc_expr_lexenv *lexenv,
				       t_osc_bndl_u *oscbndl,
				       t_osc_atom_ar_u **out)
{
	return 0;
}

int osc_expr_ast_binaryop_evalLvalInLexEnv(t_osc_expr_ast_expr *ast,
					   t_osc_expr_lexenv *lexenv,
					   t_osc_bndl_u *oscbndl,
					   t_osc_msg_u **assign_target,
					   long *nlvals,
					   t_osc_atom_u ***lvals)
{
	return 0;
}

long osc_expr_ast_binaryop_format(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec((t_osc_expr_ast_binaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e);
	long offset = 0;
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " %s ", osc_expr_oprec_getName(r));
	offset += osc_expr_ast_expr_format(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	return offset;
}

long osc_expr_ast_binaryop_formatLisp(char *buf, long n, t_osc_expr_ast_expr *e)
{
	if(!e){
		return 0;
	}
	t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec((t_osc_expr_ast_binaryop *)e);
	if(!r){
		return 0;
	}
        t_osc_expr_ast_expr *left = osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e);
        t_osc_expr_ast_expr *right = osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e);
	long offset = 0;
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, "(%s ", osc_expr_oprec_getName(r));
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, left);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, " ");
	offset += osc_expr_ast_expr_formatLisp(buf ? buf + offset : NULL, buf ? n - offset : 0, right);
	offset += snprintf(buf ? buf + offset : NULL, buf ? n - offset : 0, ")");
	return offset;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_copy(t_osc_expr_ast_expr *ast)
{
	if(ast){
		t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)ast;
		t_osc_expr_oprec *r = osc_expr_ast_binaryop_getOpRec(b);
		t_osc_expr_ast_expr *left = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getLeftArg(b));
		t_osc_expr_ast_expr *right = osc_expr_ast_expr_copy(osc_expr_ast_binaryop_getRightArg(b));
		t_osc_expr_ast_binaryop *copy = osc_expr_ast_binaryop_alloc(r, left, right);
		return (t_osc_expr_ast_expr *)copy;
	}else{
		return NULL;
	}
}

void osc_expr_ast_binaryop_free(t_osc_expr_ast_expr *e)
{
	if(e){
		osc_expr_ast_expr_free(osc_expr_ast_binaryop_getLeftArg((t_osc_expr_ast_binaryop *)e));
		osc_expr_ast_expr_free(osc_expr_ast_binaryop_getRightArg((t_osc_expr_ast_binaryop *)e));
		osc_mem_free(e);
	}
}

t_osc_bndl_u *osc_expr_ast_binaryop_toBndl(t_osc_expr_ast_expr *e)
{
	if(!e){
		return NULL;
	}
	t_osc_expr_ast_binaryop *b = (t_osc_expr_ast_binaryop *)e;
	t_osc_msg_u *nodetype = osc_message_u_allocWithInt32("/nodetype", OSC_EXPR_AST_NODETYPE_BINARYOP);
	t_osc_msg_u *rec = osc_message_u_allocWithBndl_u("/record", osc_expr_oprec_toBndl(osc_expr_ast_binaryop_getOpRec(b)), 1);
	t_osc_msg_u *left = osc_message_u_allocWithBndl_u("/left", osc_expr_ast_expr_toBndl(osc_expr_ast_binaryop_getLeftArg(b)), 1);
	t_osc_msg_u *right = osc_message_u_allocWithBndl_u("/right", osc_expr_ast_expr_toBndl(osc_expr_ast_binaryop_getRightArg(b)), 1);
	t_osc_bndl_u *bndl = osc_bundle_u_alloc();
	osc_bundle_u_addMsg(bndl, nodetype);
	osc_bundle_u_addMsg(bndl, rec);
	osc_bundle_u_addMsg(bndl, left);
	osc_bundle_u_addMsg(bndl, right);
	return bndl;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_fromBndl(t_osc_bndl_u *b)
{
	if(!b){
		return NULL;
	}
	return NULL;
}

t_osc_expr_oprec *osc_expr_ast_binaryop_getOpRec(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->rec;
	}
	return NULL;
}

void osc_expr_ast_binaryop_setOpRec(t_osc_expr_ast_binaryop *e, t_osc_expr_oprec *r)
{
	if(e && r){
		e->rec = r;
	}
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_getLeftArg(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->left;
	}
	return NULL;
}

t_osc_expr_ast_expr *osc_expr_ast_binaryop_getRightArg(t_osc_expr_ast_binaryop *e)
{
	if(e){
		return e->right;
	}
	return NULL;
}

void osc_expr_ast_binaryop_setLeftArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *left)
{
	if(e){
		e->left = left;
	}
}

void osc_expr_ast_binaryop_setRightArg(t_osc_expr_ast_binaryop *e, t_osc_expr_ast_expr *right)
{
	if(e){
		e->right = right;
	}
}

t_osc_expr_ast_binaryop *osc_expr_ast_binaryop_alloc(t_osc_expr_oprec *rec, t_osc_expr_ast_expr *left, t_osc_expr_ast_expr *right)
{
	if(!rec || !left || !right){
		return NULL;
	}
	t_osc_expr_ast_binaryop *b = osc_mem_alloc(sizeof(t_osc_expr_ast_binaryop));
	if(!b){
		return NULL;
	}
	/*
	osc_expr_ast_expr_init((t_osc_expr_ast_expr *)b,
			       OSC_EXPR_AST_NODETYPE_BINARYOP,
			       NULL,
			       osc_expr_ast_binaryop_evalInLexEnv,
			       osc_expr_ast_binaryop_format,
			       osc_expr_ast_binaryop_formatLisp,
			       osc_expr_ast_binaryop_free,
			       osc_expr_ast_binaryop_copy,
			       osc_expr_ast_binaryop_serialize,
			       osc_expr_ast_binaryop_deserialize,
			       sizeof(t_osc_expr_ast_binaryop));
	*/
	t_osc_expr_funcrec *funcrec = osc_expr_builtin_lookupFunctionForOperator(rec);
	if(!funcrec){
		return NULL;
	}
	osc_expr_ast_funcall_init((t_osc_expr_ast_funcall *)b,
				  OSC_EXPR_AST_NODETYPE_BINARYOP,
				  NULL,
				  NULL,
				  NULL,
				  osc_expr_ast_binaryop_format,
				  NULL,
				  NULL,//osc_expr_ast_binaryop_free,
				  osc_expr_ast_binaryop_copy,
				  osc_expr_ast_binaryop_toBndl,
				  osc_expr_ast_binaryop_fromBndl,
				  sizeof(t_osc_expr_ast_binaryop),
				  funcrec,
				  2,
				  left,
				  right);
	osc_expr_ast_binaryop_setOpRec(b, rec);
	osc_expr_ast_binaryop_setLeftArg(b, left);
	osc_expr_ast_binaryop_setRightArg(b, right);
	t_osc_expr_ast_expr *args = osc_expr_ast_funcall_getArgs((t_osc_expr_ast_funcall *)b);

	return b;
}