#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*main structure for variable handling: a unique object that can have one of three values*/
struct variable{
    union{
        int integer_val;
        double double_val;
        char *string_val;

    };
    char fromID;
    char type;
};

/* SYMBOL-TABLE IMPLEMENTATION: The table is implemented as a linked list of nodes,
 * each node holds a pointer to the next one, as well as the actual variable values.
 * It also holds two boolean variables in order to check whether the variable wrapped
 * in the node is initialised or not without risking of incurring into errors when evaluating
 * uninitialised variables.*/
struct table_node{
    char *id;
    bool type_declared;    // specifies whether the variable type has been declared or not
    bool initialised; // specifies whether the variable has a value defined or not
    struct table_node *next;
    struct variable value;
};

/*Initialisation of global variables*/
typedef struct table_node symbol_table;
symbol_table *head = (symbol_table *)0;

const int MAX_SIZE_SYMBOL_TABLE = 64; // max number of symbols in the symbol table
bool table_init = false;
int numberOfNodes = 0;

const char UNDEFINED_TYPE = 0;
const char INTEGER_TYPE = 1;
const char DOUBLE_TYPE = 2;
const char STRING_TYPE = 3;

/*Symbol-table management function prototypes*/
symbol_table *findOrAdd(char *string);
void setHead(symbol_table *node);
symbol_table *addNode(char *str, symbol_table *lastNode);
void printID(symbol_table *string);
void printTable();
char *varType(struct variable data);
void recPrintTable(symbol_table *node,int nodeNo);

/* Assignment functions
 * depending on the type of assignments (i.e. the arguments passed) the compiler
 * should behave differently based on if the user is trying to declare a new
 * undefined variable or if it already defines one or more fields of it*/
symbol_table *completeTypedAssign(char *type, char *id, struct variable expression);
symbol_table *completeTypedShorthand(char *type, char *id, char* shorthand, struct variable expression);
symbol_table *completeUntypedAssign(char *id, struct variable expression);
symbol_table *completeUntypedShorthand(char *id, char *shorthand, struct variable expression);
symbol_table *typedAssign(char *type, char *id);

/* Comparison and equality functions */
bool greaterNum(struct variable, struct variable);
bool lesserNum(struct variable, struct variable);
bool equal(struct variable n1, struct variable n2);
bool neqNum(struct variable, struct variable);
bool geqNum(struct variable, struct variable);
bool leqNum(struct variable, struct variable);

/*SYMBOL-TABLE IMPLEMENTATION FUNCTIONS*/

/* looks for a node with the given string as ID,
 * if the symbol-table is yet to be initialised, it initialises it and returns the head,
 * if it finds a match in the table, returns that node
 * if no match is found, the table is extended with a new node which is returned*/
symbol_table *findOrAdd(char *string){

    //the symbol-table is yet to be initialised
    if (head == NULL) {
        table_init = true;
        printf("Initialising the symbol table\n");
        head = (symbol_table *)malloc(sizeof(symbol_table));
        head->id = strdup(string);
        head->type_declared = false;
        head->initialised = false;
        head->next = NULL;
        numberOfNodes++;
        return head;
    } else {
        symbol_table *node = head;
        //search the symbol table for a match
        while (strcmp(node->id,string) != 0){
            //the current node is the last one of the table, no match was found
            if(node->next==NULL && numberOfNodes< MAX_SIZE_SYMBOL_TABLE){
                printf("Info: Match not found, adding %s to the symbol table.\n",string);
                return addNode(string,node);
            }
            node = node->next;
        }
        printf("Info: Found a match for node %s\n",node->id);
        return node;
    }
}

/*Appends a new node with the given string as ID to the last node, extending the table by 1*/
symbol_table *addNode(char *str, symbol_table *lastNode){
    symbol_table *addedNode = (symbol_table *)malloc(sizeof(symbol_table));
    addedNode->id = strdup(str);
    addedNode->next = NULL;
    addedNode->type_declared = false;
    addedNode->initialised=false;
    lastNode->next = addedNode;

    numberOfNodes++;

    return addedNode;
}

/* Prints the content of the specified node in a format of enhanced readability.
 * Including the actual value stored*/
void printNode(symbol_table *nodeToPrint){
    /* all values to be printed are stored in strings in that way
     * even if the value itself is not initialised or is NULL,
     * the method does not cause an error*/
    char* declared;
    char* init;
    char* nextNodeId; //the next node field may be empty
    char* val; //the variable value may be not initialised
    char v[256] = {0}; //needed in order to append the actual value to the final string to be printed

    //checking if the node has a type specified
    if(nodeToPrint->type_declared){
        declared = (char *)"yes";

        //checking if the node stores a value, and appending that value accordingly
        if(nodeToPrint->initialised){
            init = (char *)"yes";
            if(nodeToPrint->value.type==INTEGER_TYPE){
                snprintf(v, 255,"(Integer value) %i",nodeToPrint->value.integer_val);
                val = (char *) &v;
            } else if(nodeToPrint->value.type==DOUBLE_TYPE){
                snprintf(v, 255,"(Double value) %f",nodeToPrint->value.double_val);
                val = (char *) &v;
            } else if(nodeToPrint->value.type==STRING_TYPE){
                val = strcat("(String value) %s",nodeToPrint->value.string_val);
            } else {
                printf("Error: error while trying to access the value stored in node %s", nodeToPrint->id);
                exit(1);
            }
        } else {
            val = (char *)"NULL";
            init = (char *)"no";
        }
    } else {
        val = (char *)"NULL";
        declared = (char *)"no";
        init = (char *)"no";
    }

    if(nodeToPrint->next!= NULL){
        nextNodeId = nodeToPrint->next->id;
    } else {
        nextNodeId = (char *)"NULL";
    }
    printf("-----------------------------------------------\n");
    printf("Node ID: %s\n"
           "Type Declared: %s\n"
           "Value initialised: %s\n"
           "Next node: %s\n"
           "Value: %s\n",nodeToPrint->id,declared,init,nextNodeId,val);
    printf("-----------------------------------------------\n\n");

}

/* Recursive method composed of two parts:
 * printTable()     which is in charge of starting the recursion and printing the header of the print statement
 * recPrintTable()  which is in charge of printing the node separators and print each node*/
void printTable(){
    if(table_init){
        int nodeNo = 0;
        printf("PRINTING THE WHOLE SYMBOL_TABLE\n for single nodes use print ID\n");
        symbol_table *ptr = head;
        recPrintTable(ptr,nodeNo);
    } else {
        printf("Error: Please initialise the symbol table first by declaring one variable at least!\n");
    }

}
void recPrintTable(symbol_table *nodeToPrint, int nodeNo){
    nodeNo++;
    printf("##########################################\n");
    printf("Printing node number %i\n",nodeNo);
    printf("##########################################\n");
    printNode(nodeToPrint);
    if(nodeNo<MAX_SIZE_SYMBOL_TABLE&&nodeToPrint->next!=NULL){
        recPrintTable(nodeToPrint->next,nodeNo);
    }
}

// returns type of data of the specified variable
char* varType(struct variable data){

    char* type;

    switch(data.type) {

        case 1:
            type="int";
            break;
        case 2:
            type="double";
            break;
        case 3:
            type="string";
            break;
        case 0:
            type="none";
            break;
    }

    return type;
}

/*Prints the result of arithmetic operations (or string concatenations) accordingly to the type*/
void printResult(struct variable var){
    if(var.type==STRING_TYPE){
        printf("Result: %s\n",var.string_val);
    } else if(var.type == INTEGER_TYPE){
        printf("Result: %d\n",var.integer_val);
    } else if(var.type == DOUBLE_TYPE){
        printf("Result: %f\n",var.double_val);
    } else if (var.type == UNDEFINED_TYPE){
        printf("Result is uninitialised!\nUse the print ID command to print the information about a specific ID");
    } else {
        printf("Error while trying to print variable of type %i!\n",var.type);
        exit(1);
    }
}

/* Methods for handling variable initialisation, which runs differently based on the inputs provided
 * and if the declared variable already exists/contains some values*/
symbol_table * completeTypedAssign (char* type, char* id, struct  variable expression){
    symbol_table *node = findOrAdd(id);
    if (node->type_declared) {//node has a type
        if (node->initialised) {//node already stores a value
            if (strcmp("integer", type) == 0) {
                if (node->value.type == INTEGER_TYPE) {
                    if (expression.type == INTEGER_TYPE) { //everything is integer, assign the value
                        node->initialised = true;
                        node->value.integer_val = expression.integer_val;
                    } else if (expression.type == DOUBLE_TYPE) {
                        node->initialised = true;
                        node->value.integer_val = (int) expression.double_val;
                    } else {
                        printf("Error: could not recognise the type of the expression!\n");
                    }
                } else {
                    printf("Error: the type of the node does not match the type declared!\n");
                }
            } else if (strcmp("double", type) == 0) {
                if (node->value.type == DOUBLE_TYPE) {
                    if (expression.type == DOUBLE_TYPE) {//everything is double, assign the value
                        node->initialised = true;
                        node->value.double_val = expression.double_val;
                    } else if (expression.type == INTEGER_TYPE) {
                        node->initialised = true;
                        node->value.double_val = (double) expression.integer_val;
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
        } else {//node stores no value
            if (strcmp("integer", type) == 0) {
                if (node->value.type == INTEGER_TYPE) {
                    if (expression.type == INTEGER_TYPE) { //everything is integer, assign the value
                        node->initialised = true;
                        node->value.integer_val = expression.integer_val;
                    } else if (expression.type == DOUBLE_TYPE) { //cast double value to int with warning
                        node->initialised = true;
                        printf("Warning: casting double to integer, approximation may occur!\n");
                        node->value.integer_val = (int) expression.double_val;
                    } else {
                        printf("Error: could not recognise the type of the expression!\n");
                    }
                } else {
                    printf("Error: the type of the node does not match the type declared!\n");
                }
            } else if (strcmp("double", type) == 0) {
                if (node->value.type == DOUBLE_TYPE) {
                    if (expression.type == DOUBLE_TYPE) {//everything is double, assign the value
                        node->initialised = true;
                        node->value.double_val = expression.double_val;
                    } else if (expression.type == INTEGER_TYPE) {
                        node->initialised = true;
                        node->value.double_val = (double) expression.integer_val;
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
    } else {
        if (node->initialised) {//node already stores a value
            //node has no type defined, but it stores a value, this should be an impossible case
            printf("ERROR: The node %s currently stores a value, but has no type defined!\n", node->id);
            exit(1);
        } else { //node stores no value
            node->initialised = true;
            node->type_declared = true;
            if (strcmp("integer", type) == 0) {
                node->value.type = INTEGER_TYPE;
                if (expression.type == INTEGER_TYPE) {
                    node->value.integer_val = expression.integer_val;
                } else if (expression.type == DOUBLE_TYPE) {
                    printf("Warning: casting double to integer, approximation may occur!\n");
                    node->value.integer_val = (int) expression.double_val;
                } else {
                    printf("Error: could not recognise the type of the expression!\n");
                }
            } else if (strcmp("double", type) == 0) {
                node->value.type = DOUBLE_TYPE;
                if (expression.type == DOUBLE_TYPE) {
                    node->value.double_val = expression.double_val;
                } else if (expression.type == INTEGER_TYPE) {
                    node->value.double_val = (double) expression.integer_val;
                } else {
                    printf("Error: could not recognise the type of the expression!\n");
                }
            } else {
                printf("Error: could not recognise the type declared!\n");
            }
        }
    }
    return node;

}
symbol_table * completeTypedShorthand(char *type, char *id, char* shorthand, struct variable expression){
    //complete assignment
    symbol_table *node = findOrAdd(id);
    if (node->type_declared) {//node has a type
        if (node->initialised) {//node already stores a value
            if (strcmp(type, "integer") == 0) {
                if (node->value.type == INTEGER_TYPE) {
                    node->initialised = true;
                    if (expression.type == INTEGER_TYPE) { //everything is integer, assign the value
                        int tmp = (int) node->value.integer_val;
                        if (strcmp(shorthand, "multi_ass") == 0) {
                            node->value.integer_val = tmp * expression.integer_val;
                        } else if (strcmp(shorthand, "add_ass") == 0) {
                            node->value.integer_val = tmp + expression.integer_val;
                        } else if (strcmp(shorthand, "sub_ass") == 0) {
                            node->value.integer_val = tmp - expression.integer_val;
                        } else if (strcmp(shorthand, "div_ass") == 0) {
                            node->value.integer_val = tmp / expression.integer_val;
                        } else {
                            printf("Error: Could not recognise the shorthand operation!\n");
                        }
                    } else if (expression.type == DOUBLE_TYPE) {
                        printf("Warning: casting double to integer, approximation may occur!\n");
                        int tmp = (int) node->value.integer_val;
                        if (strcmp(shorthand, "multi_ass") == 0) {
                            node->value.integer_val = (int) tmp * expression.double_val;
                        } else if (strcmp(shorthand, "add_ass") == 0) {
                            node->value.integer_val = (int) tmp + expression.double_val;
                        } else if (strcmp(shorthand, "sub_ass") == 0) {
                            node->value.integer_val = (int) tmp - expression.double_val;
                        } else if (strcmp(shorthand, "div_ass") == 0) {
                            node->value.integer_val = (int) tmp / expression.double_val;
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
            } else if (strcmp(type, "double") == 0) {
                if (node->value.type == DOUBLE_TYPE) {
                    if (expression.type == DOUBLE_TYPE) {//everything is double, assign the value
                        node->initialised = true;
                        double tmp = node->value.double_val;
                        if (strcmp(shorthand, "multi_ass") == 0) {
                            node->value.double_val = tmp * expression.double_val;
                        } else if (strcmp(shorthand, "add_ass") == 0) {
                            node->value.double_val = tmp + expression.double_val;
                        } else if (strcmp(shorthand, "sub_ass") == 0) {
                            node->value.double_val = tmp - expression.double_val;
                        } else if (strcmp(shorthand, "div_ass") == 0) {
                            node->value.double_val = tmp / expression.double_val;
                        } else {
                            printf("Error: Could not recognise the shorthand operation!\n");
                        }
                    } else if (expression.type == INTEGER_TYPE) {
                        node->initialised = true;
                        double tmp = node->value.double_val;
                        if (strcmp(shorthand, "multi_ass") == 0) {
                            node->value.double_val = (double) tmp * expression.integer_val;
                        } else if (strcmp(shorthand, "add_ass") == 0) {
                            node->value.double_val = (double) tmp + expression.integer_val;
                        } else if (strcmp(shorthand, "sub_ass") == 0) {
                            node->value.double_val = (double) tmp - expression.integer_val;
                        } else if (strcmp(shorthand, "div_ass") == 0) {
                            node->value.double_val = (double) tmp / expression.integer_val;
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
        } else {//node stores no value
            if (strcmp(type, "integer") == 0) {
                if (node->value.type == INTEGER_TYPE) {
                    node->initialised = true;
                    if (expression.type == INTEGER_TYPE) { //everything is integer, assign the value
                        node->value.integer_val = expression.integer_val;
                    } else if (expression.type == DOUBLE_TYPE) { //cast double value to int with warning
                        printf("Warning: casting double to integer, approximation may occur!\n");
                        node->value.integer_val = (int) expression.double_val;
                    } else {
                        node->initialised = false;
                        printf("Error: could not recognise the type of the expression!\n");
                    }
                } else {
                    printf("Error: the type of the node does not match the type declared!\n");
                }
            } else if (strcmp(type, "double") == 0) {
                if (node->value.type == DOUBLE_TYPE) {
                    if (expression.type == DOUBLE_TYPE) {//everything is double, assign the value
                        node->initialised = true;
                        node->value.double_val = expression.double_val;
                    } else if (expression.type == INTEGER_TYPE) {
                        node->initialised = true;
                        node->value.double_val = (double) expression.integer_val;
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
        }
    } else {
        if (node->initialised) {//node already stores a value
            //node has no type defined, but it stores a value, this should be an impossible case
            printf("ERROR: The node %s currently stores a value, but has no type defined!\n", node->id);
            exit(1);
        } else { //node stores no value
            node->initialised = true;
            node->type_declared = true;
            if (strcmp(type, "integer") == 0) {
                node->value.type = INTEGER_TYPE;
                if (expression.type == INTEGER_TYPE) {
                    node->value.integer_val = expression.integer_val;
                } else if (expression.type == DOUBLE_TYPE) {
                    printf("Warning: casting double to integer, approximation may occur!\n");
                    node->value.integer_val = (int) expression.double_val;
                } else {
                    printf("Error: could not recognise the type of the expression!\n");
                }
            } else if (strcmp(type, "double") == 0) {
                node->value.type = DOUBLE_TYPE;
                if (expression.type == DOUBLE_TYPE) {
                    node->value.double_val = expression.double_val;
                } else if (expression.type == INTEGER_TYPE) {
                    node->value.double_val = (double) expression.integer_val;
                } else {
                    printf("Error: could not recognise the type of the expression!\n");
                }
            } else {
                printf("Error: could not recognise the type declared!\n");
            }
            printf("Warning: the variable you declared was not holding any value! Assigning the value to the variable itself\n");
        }
    }
    return node;
}
symbol_table * completeUntypedAssign(char *id, struct variable expression){
    symbol_table *node = findOrAdd(id);
    if (node->type_declared) {
        if (node->initialised == 0) {
            //node has type defined but it stores no value
            if (node->value.type == INTEGER_TYPE) {
                if (expression.type == INTEGER_TYPE) {
                    node->initialised = true;
                    node->value.integer_val = expression.integer_val;
                } else if (expression.type == DOUBLE_TYPE) {
                    node->initialised = true;
                    node->value.integer_val = (int) expression.double_val;
                    printf("Warning: casting the double result to an integer!");
                } else {
                    printf("Error: type mismatch! Node %s has type %i (integer), but the expression has type %i instead!\n",
                           node->id, node->value.type, expression.type);
                }
            } else if (node->value.type == DOUBLE_TYPE) {
                if (expression.type == INTEGER_TYPE) {
                    node->initialised = true;
                    node->value.double_val = (double) expression.integer_val;
                } else if (expression.type == DOUBLE_TYPE) {
                    node->initialised = true;
                    node->value.double_val = expression.double_val;
                }
                {
                    printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",
                           node->id, node->value.type, expression.type);
                }
            } else {
                printf("Error: the type of node %s could not be recognised!\n", node->id);
                exit(1);
            }
        } else {
            //node has type defined and it stores a value
            if (node->value.type == expression.type) {
                if (node->value.type == INTEGER_TYPE) {
                    node->value.integer_val = expression.integer_val;
                    printf("Info: Updated variable %s to the new value %i", node->id, expression.integer_val);
                } else if (node->value.type == DOUBLE_TYPE) {
                    node->value.double_val = expression.double_val;
                    printf("Info: Updated variable %s to the new value %f", node->id, expression.double_val);
                } else {
                    printf("Error: the type of node %s could not be recognised!\n", node->id);
                    exit(1);
                }
            } else {
                printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",
                       node->id, node->value.type, expression.type);
                exit(1);
            }
        }
    } else {
        if (node->initialised) {
            //node has no type defined, but it stores a value, this should be an impossible case
            printf("Error: The node %s currently stores a value, but has no type defined!\n", node->id);
            exit(1);
        } else {
            //node has neither type defined nor it stores a value
            if (expression.type == INTEGER_TYPE) {
                node->initialised = true;
                node->value.type = INTEGER_TYPE;
                node->value.integer_val = expression.integer_val;
                node->type_declared = true;
            } else if (expression.type == DOUBLE_TYPE) {
                node->initialised = true;
                node->value.type = DOUBLE_TYPE;
                node->value.double_val = expression.double_val;
                node->type_declared = true;
            } else {
                printf("ERROR: the type of the expression could not be recognised!\n");
                exit(1);
            }
        }
    }
    return node;
}
symbol_table * completeUntypedShorthand(char *id, char *shorthand, struct variable expression){
    //untyped assignment, no type specified
    symbol_table *node = findOrAdd(id);
    if (node->type_declared) {
        if (node->initialised) {
            //node has type defined and it stores a value
            if (node->value.type == INTEGER_TYPE) {
                node->initialised = true;
                if (expression.type == INTEGER_TYPE) { //everything is integer, assign the value
                    int tmp = (int) node->value.integer_val;
                    if (strcmp(shorthand, "multi_ass") == 0) {
                        node->value.integer_val = tmp * expression.integer_val;
                    } else if (strcmp(shorthand, "add_ass") == 0) {
                        node->value.integer_val = tmp + expression.integer_val;
                    } else if (strcmp(shorthand, "sub_ass") == 0) {
                        node->value.integer_val = tmp - expression.integer_val;
                    } else if (strcmp(shorthand, "div_ass") == 0) {
                        node->value.integer_val = tmp / expression.integer_val;
                    } else {
                        printf("Error: Could not recognise the shorthand operation!\n");
                    }
                } else if (expression.type == DOUBLE_TYPE) {
                    printf("Warning: casting double to integer, approximation may occur!\n");
                    int tmp = (int) node->value.integer_val;
                    if (strcmp(shorthand, "multi_ass") == 0) {
                        node->value.integer_val = (int) tmp * expression.double_val;
                    } else if (strcmp(shorthand, "add_ass") == 0) {
                        node->value.integer_val = (int) tmp + expression.double_val;
                    } else if (strcmp(shorthand, "sub_ass") == 0) {
                        node->value.integer_val = (int) tmp - expression.double_val;
                    } else if (strcmp(shorthand, "div_ass") == 0) {
                        node->value.integer_val = (int) tmp / expression.double_val;
                    } else {
                        printf("Error: Could not recognise the shorthand operation!\n");
                    }
                } else {
                    node->initialised = false;
                    printf("Error: could not recognise the type of the expression!\n");
                }
            } else if (node->value.type == DOUBLE_TYPE) {
                if (expression.type == DOUBLE_TYPE) {//everything is double, assign the value
                    node->initialised = true;
                    double tmp = node->value.double_val;
                    if (strcmp(shorthand, "multi_ass") == 0) {
                        node->value.double_val = tmp * expression.double_val;
                    } else if (strcmp(shorthand, "add_ass") == 0) {
                        node->value.double_val = tmp + expression.double_val;
                    } else if (strcmp(shorthand, "sub_ass") == 0) {
                        node->value.double_val = tmp - expression.double_val;
                    } else if (strcmp(shorthand, "div_ass") == 0) {
                        node->value.double_val = tmp / expression.double_val;
                    } else {
                        printf("Error: Could not recognise the shorthand operation!\n");
                    }
                } else if (expression.type == INTEGER_TYPE) {
                    node->initialised = true;
                    double tmp = node->value.double_val;
                    if (strcmp(shorthand, "multi_ass") == 0) {
                        node->value.double_val = (double) tmp * expression.integer_val;
                    } else if (strcmp(shorthand, "add_ass") == 0) {
                        node->value.double_val = (double) tmp + expression.integer_val;
                    } else if (strcmp(shorthand, "sub_ass") == 0) {
                        node->value.double_val = (double) tmp - expression.integer_val;
                    } else if (strcmp(shorthand, "div_ass") == 0) {
                        node->value.double_val = (double) tmp / expression.integer_val;
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
            //node has type defined but it stores no value
            if (node->value.type == INTEGER_TYPE) {
                if (expression.type == INTEGER_TYPE) {
                    node->initialised = true;
                    node->value.integer_val = expression.integer_val;
                } else {
                    printf("Error: type mismatch! Node %s has type %i (integer), but the expression has type %i instead!\n",
                           node->id, node->value.type, expression.type);
                }
            } else if (node->value.type == DOUBLE_TYPE) {
                if (expression.type == INTEGER_TYPE) {
                    node->initialised = true;
                    node->value.double_val = expression.double_val;
                } else {
                    printf("Error: type mismatch! Node %s has type %i (double), but the expression has type %i instead!\n",
                           node->id, node->value.type, expression.type);
                }
            } else {
                printf("Error: the type of node %s could not be recognised!\n", node->id);
                exit(1);
            }
        }
    } else {
        if (node->initialised) {
            //node has no type defined, but it stores a value, this should be an impossible case
            printf("Error: The node %s currently stores a value, but has no type defined!\n", node->id);
            exit(1);
        } else {
            //node has neither type defined nor it stores a value
            printf("Warning: the variable declared has no value stored, assigning the result instead!");
            node->initialised = true;
            if (expression.type == INTEGER_TYPE) {
                node->value.type = INTEGER_TYPE;
                node->value.integer_val = expression.integer_val;
                node->type_declared = true;
            } else if (expression.type == DOUBLE_TYPE) {
                node->value.type = DOUBLE_TYPE;
                node->value.double_val = expression.double_val;
                node->type_declared = true;
            } else {
                node->initialised = false;
                printf("Error: the type of the expression could not be recognised!\n");
                exit(1);
            }
        }
    }
    return node;
}
symbol_table * typedAssign(char *type, char *id){
    symbol_table *node = findOrAdd(id);
    if(node->type_declared==0){
        if(strcmp("integer",type)==0){
            printf("Set the variable type to integer\n");
            node->value.type=INTEGER_TYPE;
        } else if(strcmp("double",type)==0){
            node->value.type=DOUBLE_TYPE;
            printf("Set the variable type to double\n");
        }
        node->type_declared=1;
    } else {
        char* node_type = varType(node->value);
        if(strcmp(node_type,type)==0){
            printf("Error: the variable you specified is already defined with type %s!\n",node_type);
        } else {
            printf("Info: the variable you specified is already defined with the same type!\n");
        }
    }
    return node;
}

/* EXTENDED ARITHMETIC FUNCTIONS
 * implementation of the four basic operations as well as the increase/decrease operator and string concatenation*/
struct variable sumOrConcat(struct variable n1, struct variable n2){
    struct variable result;

    //if one of the two variables is a string, concatenate
    if(n1.type==STRING_TYPE || n2.type == STRING_TYPE){
        if (n1.type == STRING_TYPE && n2.type == STRING_TYPE){
            result.string_val = n1.string_val;
            strcat(result.string_val, n2.string_val);
            result.type = STRING_TYPE;
        } else if (n1.type == INTEGER_TYPE){
            char v [20];
            sprintf(v,"%i",n1.integer_val);
            result.string_val = strcat(v,n2.string_val);
            result.type = STRING_TYPE;
        } else if (n1.type == DOUBLE_TYPE){
            char v [20] = {0};
            sprintf(v,"%f",n1.double_val);
            result.string_val= strcat(v,n2.string_val);
            result.type = STRING_TYPE;
        } else if (n2.type == INTEGER_TYPE){
            result.string_val = n1.string_val;
            char v [20];
            sprintf(v,"%i",n2.integer_val);
            strcat(result.string_val, v);
            result.type = STRING_TYPE;
        } else if (n2.type == DOUBLE_TYPE){
            result.string_val = n1.string_val;
            char v [20];
            sprintf(v,"%f",n2.double_val);
            strcat(result.string_val, v);
            result.type = STRING_TYPE;
        } else {
            result.type = 8;
        }
    } else if (n1.type == UNDEFINED_TYPE || n2.type == UNDEFINED_TYPE){
        if (n1.type == INTEGER_TYPE) {
            result.integer_val = n1.integer_val;
            result.type = INTEGER_TYPE;
        } else if (n2.type == INTEGER_TYPE){
            result.integer_val = n2.integer_val;
            result.type = INTEGER_TYPE;
        } else if (n1.type == DOUBLE_TYPE){
            result.double_val = n1.double_val;
            result.type = DOUBLE_TYPE;
        } else if (n2.type == DOUBLE_TYPE) {
            result.double_val = n1.double_val;
            result.type = DOUBLE_TYPE;
        } else if (n1.type == STRING_TYPE) {
            result.string_val = n1.string_val;
            result.type = STRING_TYPE;
        } else if (n2.type == STRING_TYPE){
            result.string_val = n2.string_val;
            result.type = STRING_TYPE;
        } else {
            result.type = 8;
        }
    }
    else if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        result.integer_val = n1.integer_val + n2.integer_val;
        result.type = INTEGER_TYPE;
    } else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.integer_val + n2.double_val;
        result.type = DOUBLE_TYPE;
    } else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        result.double_val = n1.double_val + n2.integer_val;
        result.type = DOUBLE_TYPE;
    } else if (n1.type == DOUBLE_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.double_val + n2.double_val;
        result.type = DOUBLE_TYPE;
    } else{
        result.type = 9; // ERROR TYPE
    }

    return result;
}

struct variable sub(struct variable n1, struct variable n2){
    struct variable result;

    if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        result.integer_val = n1.integer_val - n2.integer_val;
        result.type = INTEGER_TYPE;
    }
    else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.integer_val - n2.double_val;
        result.type = DOUBLE_TYPE;
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        result.double_val = n1.double_val - n2.integer_val;
        result.type = DOUBLE_TYPE;
    }
    else{
        result.double_val = n1.double_val - n2.double_val;
        result.type = DOUBLE_TYPE;
    }

    return result;
}

struct variable multi(struct variable n1, struct variable n2){

    struct variable result;

    if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        result.integer_val = n1.integer_val * n2.integer_val;
        result.type = INTEGER_TYPE;
    }
    else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.integer_val * n2.double_val;
        result.type = DOUBLE_TYPE;
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        result.double_val = n1.double_val * n2.integer_val;
        result.type = DOUBLE_TYPE;
    }
    else{
        result.double_val = n1.double_val * n2.double_val;
        result.type = DOUBLE_TYPE;
    }

    return result;
}

struct variable divide(struct variable n1, struct variable n2){

    struct variable result;
    if(n2.double_val == 0.0 || n2.integer_val == 0){
        printf("cannot divide by 0");
        exit(0);
    }
    else if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        result.integer_val = n1.integer_val / n2.integer_val;
        result.type = INTEGER_TYPE;
    }
    else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.integer_val / n2.double_val;
        result.type = DOUBLE_TYPE;
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        result.double_val = n1.double_val / n2.integer_val;
        result.type = DOUBLE_TYPE;
    }
    else{
        result.double_val = n1.double_val / n2.double_val;
        result.type = DOUBLE_TYPE;
    }

    return result;
}

struct variable inc(struct variable n){
    struct variable result;
    if(n.type==STRING_TYPE){
        printf("cannot increment a string!");
        exit(0);
    } else if (n.type == INTEGER_TYPE){
        result.integer_val = n.integer_val+1;
        result.type = INTEGER_TYPE;
    } else {
        result.double_val = n.double_val+1;
        result.type = DOUBLE_TYPE;
    }
    return result;
}

struct variable dec(struct variable n){
    struct variable result;
    if(n.type==STRING_TYPE){
        printf("cannot decrement a string!");
        exit(0);
    } else if (n.type == INTEGER_TYPE){
        result.integer_val = n.integer_val-1;
        result.type = INTEGER_TYPE;
    } else {
        result.double_val = n.double_val-1;
        result.type = DOUBLE_TYPE;
    }
    return result;
}


/*COMPARISON/LOGIC FUNCTIONS */
bool greaterNum(struct variable n1, struct variable n2){
    if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        if (n1.integer_val > n2.integer_val){
            return true;
        }
    } else if (n1.type == DOUBLE_TYPE && n2.type == DOUBLE_TYPE){
        if (n1.double_val > n2.double_val){
            return true;
        }
    } else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        if (n1.double_val > n2.integer_val){
            return true;
        }
    } else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        if (n1.integer_val > n2.double_val){
            return true;
        }
    }
    return false;
}

bool equal(struct variable n1, struct variable n2){
    if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        if (n1.integer_val == n2.integer_val){
            return true;
        }
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == DOUBLE_TYPE){
        if (n1.double_val == n2.double_val){
            return true;
        }
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        if (n1.double_val == n2.integer_val){
            return true;
        }
    }
    else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        if (n1.integer_val == n2.double_val){
            return true;
        }
    } else if (n1.type == STRING_TYPE && n2.type == STRING_TYPE){
        if(strcmp(n1.string_val,n2.string_val) == 0){
            return true;
        }
    }
    return false;
}

bool lesserNum(struct variable n1, struct variable n2){
    return !equal(n1, n2) && !greaterNum(n1, n2);
}

bool neqNum(struct variable n1, struct variable n2){
    return !equal(n1, n2);
}

bool geqNum(struct variable n1, struct variable n2){
    return greaterNum(n1,n2) || equal(n1, n2);
}

bool leqNum(struct variable n1, struct variable n2){
    return lesserNum(n1,n2) || equal(n1, n2);
}