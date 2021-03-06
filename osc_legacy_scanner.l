%{
#include "osc_legacy_parser.h"
#include "osc_mem.h"
#include "osc_timetag.h"

int osc_legacy_scanner_wrap(yyscan_t scanner){
	return 1;
}

%}

dseq      ([[:digit:]]+)
dseq_opt  ([[:digit:]]*)
frac      (({dseq_opt}"."{dseq})|{dseq}".")
exp       ([eE][+-]?{dseq})
exp_opt   ({exp}?)
fsuff     [flFL]
fsuff_opt ({fsuff}?)
hpref     (0[xX])
hdseq     ([[:xdigit:]]+)
hdseq_opt ([[:xdigit:]]*)
hfrac     (({hdseq_opt}"."{hdseq})|({hdseq}"."))
bexp      ([pP][+-]?{dseq})
dfc       (({frac}{exp_opt}{fsuff_opt})|({dseq}{exp}{fsuff_opt}))
hfc       (({hpref}{hfrac}{bexp}{fsuff_opt})|({hpref}{hdseq}{bexp}{fsuff_opt}))
c99_floating_point_constant  ({dfc}|{hfc})

	//string [^ $\t\"\n\\]*
string [^ \t\"\n\\:\[\],\{\}]+
quoted_string [^\"\\]

%x quote escape

%option reentrant
%option bison-bridge
%option bison-locations
%option noinput
%option nounput

%%

\" {
	BEGIN(quote);
}

<quote>{quoted_string}+ {
	if(*buf){
		*buf = osc_mem_resize(*buf, *buflen + yyleng + 1);
		*buflen += yyleng + 1;
		strcat(*buf, yytext);
	}else{
		char *copy = osc_mem_alloc(yyleng + 1);
		strncpy(copy, yytext, yyleng + 1);
		yylval->string = copy;
		return STRING;
	}
	//return STRING;
}

<quote>\" {
	BEGIN(0);
	if(*buf){
		yylval->string = *buf;
		*buf = NULL;
		*buflen = 0;
		return STRING;
	}
}

<quote>{quoted_string}*\\. {
	if(*buf){
		*buf = osc_mem_resize(*buf, *buflen + yyleng);
		//strcat(*buf, yytext);
		memcpy(*buf + *buflen, yytext, yyleng - 1);
		*buflen += yyleng - 1;
		(*buf)[*buflen - 1] = yytext[yyleng - 1];
		//(*buf)[*buflen - 3] = (*buf)[*buflen - 2];
		(*buf)[*buflen] = '\0';
	}else{
		*buf = osc_mem_alloc(yyleng);
		memcpy(*buf, yytext, yyleng - 1);
		*buflen = yyleng - 1;
		//(*buf)[*buflen - 2] = (*buf)[*buflen - 1];
		//(*buf)[*buflen - 1] = '\0';
		(*buf)[*buflen - 1] = yytext[yyleng - 1];
		(*buf)[*buflen] = '\0';
	}
}

-?{c99_floating_point_constant}	{
					char *endp = NULL;
					double d = strtod(yytext, &endp); 
					//printf("float: %f\n", d);
					yylval->f = d;
					return OSCFLOAT;
				}

(true|True|TRUE)			{
						yylval->b = 1;
						return OSCBOOL;
					}
(false|False|FALSE)			{
						yylval->b = 0;
						return OSCBOOL;
					}

\{\}\[|\]:	{
		return *yytext;
	}

(\n[[:space:]]*)+	{
		return '\n';
	}

\'.\' {
	//printf("char: %c\n", yytext[1]);
	yylval->c = yytext[1];
	return OSCCHAR;
}

(-?([[:digit:]]{-}[0])[[:digit:]]*|0) 	{
						char *endp = NULL;
					        long i = strtol(yytext, &endp, 0);
						//printf("%s: int: %ld\n", __func__, i);
						yylval->i = i;
						return OSCINT32;
					}

\/[^ \t\n#\":]+	{
			//printf("OSC address: %s\n", yytext);
			int len = strlen(yytext) + 1;
			char *copy = osc_mem_alloc(len);
			memcpy(copy, yytext, len);
			yylval->string = copy;
			return OSCADDRESS;
		}

(-?(?:[1-9][0-9]*)?[0-9]{4})-(1[0-2]|0[1-9])-(3[0-1]|0[1-9]|[1-2][0-9])?T(2[0-3]|[0-1][0-9]):([0-5][0-9]):([0-5][0-9])(\.[0-9]+)??(Z|[+-](?:2[0-3]|[0-1][0-9]):[0-5][0-9])? { /* ISO 8601 from Regular Expressions Cookbook */
	t_osc_timetag t;
	osc_timetag_fromISO8601(yytext, &t);
	yylval->t = t;
	return OSCTIMETAG;
}

	/*{string}|$0	{*/
{string}	{
				int len = strlen(yytext) + 1;
				char *copy = osc_mem_alloc(len);
				memcpy(copy, yytext, len);
				yylval->string = copy;
				return STRING;
			}

{string}*\\.	{
			if(*buf){
				*buflen += yyleng + 1; // make room for the escaped char
				*buf = osc_mem_resize(*buf, *buflen);
				strcat(*buf, yytext);
				(*buf)[*buflen - 1] = '\0';
			}else{
				*buf = osc_mem_alloc(yyleng + 1);
				memcpy(*buf, yytext, yyleng);
				*buflen = yyleng;
				//(*buf)[*buflen - 2] = (*buf)[*buflen - 1];
				(*buf)[*buflen - 1] = '\0';
			}
			BEGIN(escape);
			yyless(1);
		}

<escape>.	{
			if(*buf){
				(*buf)[*buflen - 1] = *yytext;
			}else{
				//this is bad news
			}
			BEGIN(0);
			yylval->string = *buf;
			*buf = NULL;
			*buflen = 0;
			return STRING;
		}


[ \t]+

. {return *yytext;}

%%
//\xc2\xac\n
/*
int main(int argc, char **argv){
	yyscan_t scanner;
	char buf[128];
	yylex_init(&scanner);
	YY_BUFFER_STATE buf_state = yy_scan_string("/foo bar\n /bar foo\n", scanner);
	yyset_out(NULL, scanner);
	int ret = 1;
	//while(ret){
		//ret = yylex(scanner);
		//printf("foo: %s\n", yyget_text(scanner));
	//}

	//buf_state = yy_scan_string("/foo bar\n /bar foo\n", scanner);
	while(yylex(scanner)){
		//printf("foo: %s\n", yyget_text(scanner));
	}
	yy_delete_buffer(buf_state, scanner);
	yylex_destroy(scanner);
}
*/


