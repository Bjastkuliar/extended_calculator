%option noyywrap
%{
#include <stdlib.h>
#include <string.h>
%}

DIGIT    [0-9]
INT	{DIGIT}+
DOUBLE	{INT}\.{DIGIT}+


LETTER   [a-zA-Z]
ID       {LETTER}({LETTER}|{DIGIT}|\_)*
SPECIAL [\|\?\:\\\'\,\@]
STR    \"([ a-zA-Z0-9]*{SPECIAL}*_*)*\"

%%

[ ]     { /* skip blanks */ }

quit        {return QUIT;}
print       {return PRINT;}

if          {return IF;}
then        {return THEN;}

type        {return TYPE;}
double		{ return DOUBLE; }
int			{ return INTEGER; }
string		{ return STRING; }

{INT}   {yylval.integer_val = atoi(yytext);
          return INTEGER_VAL;}
{DOUBLE}   {yylval.double_val = atof(yytext);
            return DOUBLE_VAL;}
{STR}  {yylval.lexeme = strdup(yytext);
            return STRING_VAL;}
{ID}    {yylval.lexeme = strdup(yytext);
          return ID;}
"*="    {return MULTASS;}
"/="    {return DIVASS;}
"-="    {return SUBASS;}
"+="    {return ADDASS;}

"+"     {return '+';}
"-"     {return '-';}
"*"     {return '*';}
"/"     {return '/';}
"="     {return '=';}
"<"     {return '<';}
">"     {return '>';}
"<="    {return LEQ;}
">="    {return GEQ;}
"=="    {return EQ;}
"!="    {return NEQ;}
"&&"    {return AND;}
"||"    {return OR;}
"++"    {return INC;}
"--"    {return DEC;}
"!"     {return '!';}
";"     {return ';';}
","     {return ',';}
"("     {return '(';}
")"     {return ')';}
"{"     {return '{';}
"}"     {return '}';}
\n      {return '\n';}

%%