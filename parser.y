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
%token WHILE

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
%type <variable_val> string
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
stmt : expr		{if($1.fromID==1)printf("--ERROR--\nPlease use PRINT function for variables\n"); else{printResult($1);}}
	| PRINT		{printTable();}
	| PRINT ID	{printNode(findOrAdd($2));}
	| WHILE		{printf("While detected");}
     	| ass
     	| cond		{printf("Result: %s\n", $1 ? "true" : "false"); }
     	| ifstmt
     	;
/* Definition and/or assignment of a variable.
// The code aims to handle all cases possible when defining a variable, in this way it would be possible to define a variable without having to
// explicitly define its type and/or value, which could be defined in a second occasion. Assigning a value to the variable infers also the type
// intended for the variable itself. Once the variable has received its type, it is no longer possible to overwrite it.*/
ass : type ID  '=' expr			{//complete assignment
			symbol_table *node = findOrAdd($2);
			if(node->type_declared==false){
				if(node->initialised==false){ //node stores no value
					node->initialised=true;
                                	node->type_declared=true;
					if($1=="integer") {
						node->value.type=INTEGER_TYPE;
						if($4.type==INTEGER_TYPE){
							node->value.integer_val=$4.integer_val;
						} else if ($4.type==DOUBLE_TYPE){
							printf("Warning: casting double to integer, approximation may occur!\n");
							node->value.integer_val=(int)$4.double_val;
						} else {
							printf("Error: could not recognise the type of the expression!\n");
						}
					} else if($1=="double") {
						node->value.type=DOUBLE_TYPE;
						if($4.type==DOUBLE_TYPE){
							node->value.double_val=$4.double_val;
						} else if ($4.type==INTEGER_TYPE){
							node->value.double_val=(double)$4.integer_val;
						} else {
							printf("Error: could not recognise the type of the expression!\n");
						}
					} else {
						printf("Error: could not recognise the type declared!\n");
					}
				} else{//node already stores a value
					//node has no type defined but it stores a value, this should be an impossible case
					printf("ERROR: The node %s currently stores a value, but has no type defined!\n",node->id);
					exit(1);
				}
			} else {//node has a type
				if(node->initialised==false){//node stores no value
					if($1=="integer"){
						if(node->value.type==INTEGER_TYPE){
							if($4.type==INTEGER_TYPE){ //everything is integer, assign the value
								node->initialised=true;
								node->value.integer_val=$4.integer_val;
							} else if($4.type==DOUBLE_TYPE){ //cast double value to int with warning
								node->initialised=true;
								printf("Warning: casting double to integer, approximation may occur!\n");
                                                                node->value.integer_val=(int)$4.double_val;
							} else {
								printf("Error: could not recognise the type of the expression!\n");
							}
						} else {
							printf("Error: the type of the node does not match the type declared!\n");
						}
					} else if($1=="double"){
						if(node->value.type==DOUBLE_TYPE){
							if($4.type==DOUBLE_TYPE){//everything is double, assign the value
								node->initialised=true;
								node->value.double_val=$4.double_val;
							} else if($4.type==INTEGER_TYPE){
								node->initialised=true;
                                                                node->value.double_val=(double)$4.integer_val;
							} else {
								printf("Error: could not recognise the type of the expression!\n");
							}
						} else {
							printf("Error: the type of the node does not match the type declared!\n");
						}
					} else {
						printf("ERROR: couldn't recognise the specified type declaration!\n");
						exit(1);
					}
				} else {//node already stores a value
					if($1=="integer"){
						if(node->value.type==INTEGER_TYPE){
							if($4.type==INTEGER_TYPE){ //everything is integer, assign the value
								node->initialised=true;
								node->value.integer_val=$4.integer_val;
							} else if($4.type==DOUBLE_TYPE){
								node->initialised=true;
								node->value.integer_val=(int)$4.double_val;
							} else {
								printf("Error: could not recognise the type of the expression!\n");
							}
						} else {
							printf("Error: the type of the node does not match the type declared!\n");
						}
					} else if($1=="double"){
						if(node->value.type==DOUBLE_TYPE){
							if($4.type==DOUBLE_TYPE){//everything is double, assign the value
								node->initialised=true;
								node->value.double_val=$4.double_val;
							} else if ($4.type == INTEGER_TYPE){
								node->initialised=true;
								node->value.double_val=(double)$4.integer_val;
							} else {
								printf("Error: could not recognise the type of the expression!\n");
							}
						} else {
							printf("Error: the type of the node does not match the type declared!\n");
						}
					} else {
						printf("ERROR: couldn't recognise the specified type declaration!\n");
						exit(1);
					}
				}
			}
		}
	| type ID shorthand expr 	{//complete assignment
                                 			symbol_table *node = findOrAdd($2);
                                 			if(node->type_declared==false){
                                 				if(node->initialised==false){ //node stores no value
                                 					node->initialised=true;
                                                                 	node->type_declared=true;
                                 					if($1=="integer") {
                                 						node->value.type=INTEGER_TYPE;
                                 						if($4.type==INTEGER_TYPE){
                                 							node->value.integer_val=$4.integer_val;
                                 						} else if ($4.type==DOUBLE_TYPE){
                                 							printf("Warning: casting double to integer, approximation may occur!\n");
                                 							node->value.integer_val=(int)$4.double_val;
                                 						} else {
                                 							printf("Error: could not recognise the type of the expression!\n");
                                 						}
                                 					} else if($1=="double") {
                                 						node->value.type=DOUBLE_TYPE;
                                 						if($4.type==DOUBLE_TYPE){
                                 							node->value.double_val=$4.double_val;
                                 						} else if ($4.type==INTEGER_TYPE){
                                 							node->value.double_val=(double)$4.integer_val;
                                 						} else {
                                 							printf("Error: could not recognise the type of the expression!\n");
                                 						}
                                 					} else {
                                 						printf("Error: could not recognise the type declared!\n");
                                 					}
                                 					printf("Warning: the variable you declared was not holding any value! Assigning the value to the variable itself\n");
                                 				} else{//node already stores a value
                                 					//node has no type defined but it stores a value, this should be an impossible case
                                 					printf("ERROR: The node %s currently stores a value, but has no type defined!\n",node->id);
                                 					exit(1);
                                 				}
                                 			} else {//node has a type
                                 				if(node->initialised==false){//node stores no value
                                 					if($1=="integer"){
                                 						if(node->value.type==INTEGER_TYPE){
                                 							if($4.type==INTEGER_TYPE){ //everything is integer, assign the value
                                 								node->initialised=true;
                                 								node->value.integer_val=$4.integer_val;
                                 							} else if($4.type==DOUBLE_TYPE){ //cast double value to int with warning
                                 								node->initialised=true;
                                 								printf("Warning: casting double to integer, approximation may occur!\n");
                                                                                                 node->value.integer_val=(int)$4.double_val;
                                 							} else {
                                 								printf("Error: could not recognise the type of the expression!\n");
                                 							}
                                 						} else {
                                 							printf("Error: the type of the node does not match the type declared!\n");
                                 						}
                                 					} else if($1=="double"){
                                 						if(node->value.type==DOUBLE_TYPE){
                                 							if($4.type==DOUBLE_TYPE){//everything is double, assign the value
                                 								node->initialised=true;
                                 								node->value.double_val=$4.double_val;
                                 							} else if($4.type==INTEGER_TYPE){
                                 								node->initialised=true;
                                                                                                 node->value.double_val=(double)$4.integer_val;
                                 							} else {
                                 								printf("Error: could not recognise the type of the expression!\n");
                                 							}
                                 						} else {
                                 							printf("Error: the type of the node does not match the type declared!\n");
                                 						}
                                 					} else {
                                 						printf("ERROR: couldn't recognise the specified type declaration!\n");
                                 						exit(1);
                                 					}
                                 					printf("Warning: the variable you declared was not holding any value! Assigning the value to the variable itself\n");
                                 				} else {//node already stores a value
                                 					if($1=="integer"){
                                 						if(node->value.type==INTEGER_TYPE){
                                 							node->initialised=true;
                                 							if($4.type==INTEGER_TYPE){ //everything is integer, assign the value
                                 								int tmp = (int)node->value.integer_val;
                                 								if($3=="multass"){
                                 									node->value.integer_val=tmp * $4.integer_val;
                                 								} else if($3=="addass"){
                                 									node->value.integer_val=tmp + $4.integer_val;
                                 								} else if($3=="subass"){
                                 									node->value.integer_val=tmp - $4.integer_val;
                                 								} else if($3=="divass"){
                                 									node->value.integer_val=tmp / $4.integer_val;
                                 								} else {
                                 									printf("Error: Could not recognise the shorthand operation!\n");
                                 								}
                                 							} else if($4.type==DOUBLE_TYPE){
                                 								printf("Warning: casting double to integer, approximation may occur!\n");
                                 								int tmp = (int)node->value.integer_val;
                                 								if($3=="multass"){
													node->value.integer_val=(int)tmp * $4.double_val;
												} else if($3=="addass"){
													node->value.integer_val=(int)tmp + $4.double_val;
												} else if($3=="subass"){
													node->value.integer_val=(int)tmp - $4.double_val;
												} else if($3=="divass"){
													node->value.integer_val=(int)tmp / $4.double_val;
												} else {
													printf("Error: Could not recognise the shorthand operation!\n");
												}
                                 							} else {
                                 								node->initialised = false;
                                 								printf("Error: could not recognise the type of the expression!\n");
                                 							}
                                 						} else {
                                 							printf("Error: the type of the node does not match the type declared!\n");
                                 						}
                                 					} else if($1=="double"){
                                 						if(node->value.type==DOUBLE_TYPE){
                                 							if($4.type==DOUBLE_TYPE){//everything is double, assign the value
                                 								node->initialised=true;
                                 								double tmp =node->value.double_val;
                                 								if($3=="multass"){
													node->value.double_val=tmp * $4.double_val;
												} else if($3=="addass"){
													node->value.double_val=tmp + $4.double_val;
												} else if($3=="subass"){
													node->value.double_val=tmp - $4.double_val;
												} else if($3=="divass"){
													node->value.double_val=tmp / $4.double_val;
												} else {
													printf("Error: Could not recognise the shorthand operation!\n");
												}
                                 							} else if ($4.type == INTEGER_TYPE){
                                 								node->initialised=true;
                                 								double tmp =node->value.double_val;
                                 								if($3=="multass"){
													node->value.double_val=(double)tmp * $4.integer_val;
												} else if($3=="addass"){
													node->value.double_val=(double)tmp + $4.integer_val;
												} else if($3=="subass"){
													node->value.double_val=(double)tmp - $4.integer_val;
												} else if($3=="divass"){
													node->value.double_val=(double)tmp / $4.integer_val;
												} else {
													printf("Error: Could not recognise the shorthand operation!\n");
												}
                                 							} else {
                                 								printf("Error: could not recognise the type of the expression!\n");
                                 							}
                                 						} else {
                                 							printf("Error: the type of the node does not match the type declared!\n");
                                 						}
                                 					} else {
                                 						printf("ERROR: couldn't recognise the specified type declaration!\n");
                                 						exit(1);
                                 					}
                                 				}
                                 			}
                                 		}
	| ID '=' expr   		{//untyped assignment, no type specified
			symbol_table *node = findOrAdd($1);
			if(node->type_declared==false){
				if(node->initialised==false){
					//node has neither type defined nor it stores a value
					if($3.type==INTEGER_TYPE){
						node->initialised=true;
						node->value.type=INTEGER_TYPE;
						node->value.integer_val=$3.integer_val;
						node->type_declared=true;
					} else if($3.type == DOUBLE_TYPE){
						node->initialised=true;
                                                node->value.type=DOUBLE_TYPE;
                                                node->value.double_val=$3.double_val;
                                                node->type_declared=true;
					} else {
						printf("Error: the type of the expression could not be recognised!\n");
						exit(1);
					}
				} else {
					//node has no type defined but it stores a value, this should be an impossible case
					printf("Error: The node %s currently stores a value, but has no type defined!\n",node->id);
					exit(1);
				}
			} else {
				if(node->initialised==0){
        				//node has type defined but it stores no value
					if(node->value.type==INTEGER_TYPE){
						if($3.type==INTEGER_TYPE){
							node->initialised=true;
                                                        node->value.integer_val=$3.integer_val;
						} else if ($3.type == DOUBLE_TYPE){
							node->initialised = true;
							node->value.integer_val=(int)$3.double_val;
							printf("Warning: casting the double result to an integer!");
						} else {
							printf("Error: type mismatch! Node %s has type %i (integer), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
						}
					} else if(node->value.type == DOUBLE_TYPE){
						if($3.type==INTEGER_TYPE){
                                                	node->initialised=true;
                                                        node->value.double_val=(double)$3.integer_val;
        					} else if($3.type == DOUBLE_TYPE){
        						node->initialised=true;
        						node->value.double_val= $3.double_val;
        					} {
                                                	printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
                                                }
					} else {
						printf("Error: the type of node %s could not be recognised!\n", node->id);
						exit(1);
					}
        			} else {
        				//node has type defined and it stores a value
        				if(node->value.type==$3.type){
        					if(node->value.type==INTEGER_TYPE){
        						node->value.integer_val=$3.integer_val;
        						printf("Info: Updated variable %s to the new value %i",node->id,$3.integer_val);
        					} else if(node->value.type==DOUBLE_TYPE){
        						node->value.double_val=$3.double_val;
        						printf("Info: Updated variable %s to the new value %f",node->id,$3.double_val);
        					} else {
        						printf("Error: the type of node %s could not be recognised!\n", node->id);
                                                        exit(1);
        					}
        				} else {
        					printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
        					exit(1);
        				}
        			}
			}
			}
	| ID shorthand expr 		{//untyped assignment, no type specified
                                        			symbol_table *node = findOrAdd($1);
                                        			if(node->type_declared==false){
                                        				if(node->initialised==false){
                                        					//node has neither type defined nor it stores a value
                                        					printf("Warning: the variable declared has no value stored, assigning the result instead!");
                                        					if($3.type==INTEGER_TYPE){
                                        						node->initialised=true;
                                        						node->value.type=INTEGER_TYPE;
                                        						node->value.integer_val=$3.integer_val;
                                        						node->type_declared=true;
                                        					} else if($3.type == DOUBLE_TYPE){
                                        						node->initialised=true;
                                                                                        node->value.type=DOUBLE_TYPE;
                                                                                        node->value.double_val=$3.double_val;
                                                                                        node->type_declared=true;
                                        					} else {
                                        						printf("Error: the type of the expression could not be recognised!\n");
                                        						exit(1);
                                        					}
                                        				} else {
                                        					//node has no type defined but it stores a value, this should be an impossible case
                                        					printf("Error: The node %s currently stores a value, but has no type defined!\n",node->id);
                                        					exit(1);
                                        				}
                                        			} else {
                                        				if(node->initialised==false){
                                                				//node has type defined but it stores no value
                                        					if(node->value.type==INTEGER_TYPE){
                                        						if($3.type==INTEGER_TYPE){
                                        							node->initialised=true;
                                                                                                node->value.integer_val=$3.integer_val;
                                        						} else {
                                        							printf("Error: type mismatch! Node %s has type %i (integer), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
                                        						}
                                        					} else if(node->value.type == DOUBLE_TYPE){
                                        						if($3.type==INTEGER_TYPE){
                                                                                        	node->initialised=true;
                                                                                                node->value.double_val=$3.double_val;
                                                					} else {
                                                                                        	printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
                                                                                        }
                                        					} else {
                                        						printf("Error: the type of node %s could not be recognised!\n", node->id);
                                        						exit(1);
                                        					}
                                                			} else {
                                                				//node has type defined and it stores a value
                                                				if(node->value.type==$3.type){
                                                					if(node->value.type==INTEGER_TYPE){
                                                						node->value.integer_val=$3.integer_val;
                                                						printf("Info: Updated variable %s to the new value %i",node->id,$3.integer_val);
                                                					} else if(node->value.type==DOUBLE_TYPE){
                                                						node->value.double_val=$3.double_val;
                                                						printf("Info: Updated variable %s to the new value %f",node->id,$3.double_val);
                                                					} else {
                                                						printf("Error: the type of node %s could not be recognised!\n", node->id);
                                                                                                exit(1);
                                                					}
                                                				} else {
                                                					printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",node->id,node->value.type,$3.type);
                                                					exit(1);
                                                				}
                                                			}
                                        			}
                                        			}
	| type ID 			{//typed uninitialized assignment, add to table and specify type
			symbol_table *node = findOrAdd($2);
			if(node->type_declared==0){
				if($1=="integer"){
					printf("Set the variable type to integer\n");
					node->value.type=INTEGER_TYPE;
				} else if($1=="double"){
					node->value.type=DOUBLE_TYPE;
					printf("Set the variable type to double\n");
				}
				node->type_declared=1;
			} else {
				char* node_type = varType(node->value);
				if(strcmp(node_type,$1)==0){
					printf("Error: the variable you specified is already defined with type %s!\n",node_type);
				} else {
					printf("Info: the variable you specified is already defined with the same type!\n");
				}
			}
			//if var has already type specified (and is a different one) print error, else do nothing
			}
	| ID          			{//uninitialized assignment, simply add to the table
          			symbol_table *node = findOrAdd($1);
          			}
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
expr  : expr '+' expr  	{$$ = sumOrConcat($1,$3);
                         $$.fromID=0;}
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
      			 data.fromID=0;
      			 data.integer_val = $1;
      			 $$ = data;}
      | DOUBLE_VAL	{struct variable data;
      			 data.type = DOUBLE_TYPE;
      			 data.fromID=0;
      			 data.double_val = $1;
      			 $$ = data;}
      | string
      | ID		{symbol_table *node = findOrAdd($1);
      			struct variable data;
      			data.fromID=1;
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

string	  : STRING_VAL		{struct variable data;
 				data.type = STRING_TYPE;
 				data.string_val = ++$1;
 				data.string_val[strlen(data.string_val)-1] = '\0';
 				$$ = data;}
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