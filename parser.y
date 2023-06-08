%{
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "symboltable-utils.h"

int yyerror (char const *message);
int yylex(void);
%}


%union {
       	char* lexeme;			//name of an identifier
       	char* type_var;			//the type of the identifier
       	/*following are the attributes of the variable depending on its type, i.e. if it is an
       	 *integer, then only the "integer" field is filled in, and so on and so forth*/
       	double double_val;			//double
       	int integer_val;			//integer
       	bool truth;			//boolean
       	struct variable variable_val;
       }

%token <integer_val> INTEGER_VAL
%token <double_val> DOUBLE_VAL
%token <lexeme> STRING_VAL
%token <lexeme> ID
%token TYPE
%token STRING
%token DOUBLE
%token INTEGER

%token IF
%token THEN

%token OR
%token AND
%token GEQ
%token NEQ
%token LEQ
%token EQ
%token INC
%token DEC

%token MULTASS
%token DIVASS
%token SUBASS
%token ADDASS

%token QUIT
%token PRINT

%type <variable_val> expr
%type <type_var> type
%type <truth> cond
%type <type_var> shorthand

%left OR
%left AND
%left '<' '>' '=' LEQ GEQ NEQ EQ
%nonassoc MULTASS SUBASS ADDASS DIVASS
%left '-' '+'
%left '*' '/' '%'
%nonassoc  INC DEC
%left '(' ')'
%start line

%%
/*The line production simply initiates a loop that enables the application to run more than one input,
// rather than closing automatically each time. It also allows to close the application safely without errors.*/
line  : '\n' stmt
	| stmt '\n' line
	| '\n' line
      	| QUIT			{exit(0);}
      	;

/*The stmt (shorthand for "statement") production is in charge of "determining" what the user is trying to do, whether
//to compute an expression, to assingn a (possibly typed) variable or to execute a loop or a conditional clause*/
stmt : expr		{printResult($1);}
	| PRINT		{printTable();}
	| PRINT ID	{printNode(findOrAdd($2));}
	| TYPE ID	{
			symbol_table *node = findOrAdd($2);
			printf("Type of %s: %s",node->id,varType(node->value));}
     	| ass
     	| cond		{printf("Result: %s\n", $1 ? "true" : "false"); }
     	| ifstmt
     	;
/* Definition and/or assignment of a variable.
// The code aims to handle all cases possible when defining a variable, in this way it would be possible to define a variable without having to
// explicitly define its type and/or value, which could be defined in a second occasion. Assigning a value to the variable infers also the type
// intended for the variable itself. Once the variable has received its type, it is no longer possible to overwrite it.*/
ass : type ID  '=' expr			{completeTypedAssign($1,$2,$4);	}
	| type ID shorthand expr 	{completeTypedShorthand($1,$2,$3,$4);}
	| ID '=' expr   		{completeUntypedAssign($1,$3);}
	| ID shorthand expr 		{completeUntypedShorthand($1,$2,$3);}
	| type ID 			{typedAssign($1,$2);}
	| ID          			{findOrAdd($1);}
	;

type : INTEGER	{$$ = "integer";}
	| DOUBLE {$$ = "double";}
	;

shorthand : MULTASS		{$$="multass";}
		| ADDASS	{$$="addass";}
		| SUBASS	{$$="subass";}
		| DIVASS	{$$="divass";}
		;

/*managing conditional statements*/
cond : expr '<' expr		{$$ = lesserNum($1,$3);}
	| expr '>' expr		{$$ = greaterNum($1,$3);}
	| expr LEQ expr		{$$ = leqNum($1,$3);}
	| expr GEQ expr		{$$ = geqNum($1,$3);}
	| expr EQ expr		{$$ = equal($1,$3);}
	| expr NEQ expr		{$$ = neqNum($1,$3);}
	| cond AND cond 	{$$ = $1 && $3;}
	| cond OR cond		{$$ = $1 || $3;}
	;

/*Arithmetic expressions*/
expr  : expr '+' expr  	{$$ = sumOrConcat($1,$3);}
      | expr '-' expr  	{if(!($1.type == STRING_TYPE || $3.type == STRING_TYPE)){
                        	$$ = sub($1,$3);
                        	} else {
                        		fprintf(stderr,"ERROR: it is not currently possible to subtract strings");
                        		}}
      | expr '*' expr  	{if(!($1.type == STRING_TYPE || $3.type == STRING_TYPE)){
                        	$$ = mult($1,$3);
                        	} else {
                        		fprintf(stderr,"ERROR: it is not currently possible to multiply strings");
                        		}}
      | expr '/' expr  	{if(!($1.type == STRING_TYPE || $3.type == STRING_TYPE)){
                        	$$ = divide($1,$3);
                        	} else {
                        		fprintf(stderr,"ERROR: it is not currently possible to divide strings");
                        		}}
      | expr INC	{if(!($1.type == STRING_TYPE)){
      				$$ = inc($1);
                                }else {
                                	fprintf(stderr,"ERROR: it is not currently possible to increment strings");
                                }}
      | expr DEC	{if(!($1.type == STRING_TYPE)){
                              	$$ = dec($1);
                                }else {
                                	fprintf(stderr,"ERROR: it is not currently possible to decrement strings");
                                        }}
      | INTEGER_VAL    	{struct variable data;
      			 data.type = INTEGER_TYPE;
      			 data.integer_val = $1;
      			 $$ = data;}
      | DOUBLE_VAL	{struct variable data;
      			 data.type = DOUBLE_TYPE;
      			 data.double_val = $1;
      			 $$ = data;}
      | STRING_VAL	{struct variable data;
			data.type = STRING_TYPE;
			data.string_val = $1;
			$$ = data;}
      | ID		{symbol_table *node = findOrAdd($1);
      			struct variable data;
      			if(node->type_declared==true){
      				if(node->initialised==true){
      					data = node->value;
      				} else {
      					printf("Warning: the variable %s in the arithmetic operation is not initialised!",node->id);
      				}
      			} else {
      				printf("Warning: the variable %s in the arithmetic operation has no type defined (and is not initialised)!",node->id);
      			}
      			$$=data;
      			}
      ;

ifstmt	: IF '(' cond ')' THEN '{' STRING_VAL '}' { if($3){printf("%s\n", $7);}; }
	;

%%

#include "lex.yy.c"

int yyerror (char const *message){
	return fprintf (stderr, "%s\n", message);
	fputs (message, stderr);
	fputc ('\n', stderr);
	return 0;
}

int main(void)
{
  return yyparse();
}