#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>

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
 * each node holds a pointer to the next one, as well as the actual variable values.*/
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

const char INTEGER_TYPE = 1;
const char DOUBLE_TYPE = 2;
const char STRING_TYPE = 3;

/*Symbol-table management functions*/
symbol_table *findOrAdd(char *string);
void setHead(symbol_table *node);
symbol_table *addNode(char *str, symbol_table *lastNode);
void printID(symbol_table *str);
void printTable();
char* varType(struct variable data);

/* Comparison and equality functions */
bool greaterNum(struct variable, struct variable);
bool lesserNum(struct variable, struct variable);
bool equal(struct variable n1, struct variable n2);
bool neqNum(struct variable, struct variable);
bool geqNum(struct variable, struct variable);
bool leqNum(struct variable, struct variable);

/* looks for a node with the given string as ID,
 * if the symbol-table is yet to be initialised, it initialises it and returns the head,
 * if it finds a match in the table, returns that node
 * if no match is found, the table is extended with a new node which is returned*/
symbol_table *findOrAdd(char *string){

    //the symbol-table is yet to be initialised
    if (head == NULL) {
        table_init = true;
        printf("Info: Initialized a new symbol table!\n");
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
        declared = "yes";

        //checking if the node stores a value, and appending that value accordingly
        if(nodeToPrint->initialised){
            init = "yes";
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
            val = "NULL";
            init = "no";
        }
    } else {
        val = "NULL";
        declared = "no";
        init = "no";
    }

    if(nodeToPrint->next!= NULL){
        nextNodeId = nodeToPrint->next->id;
    } else {
        nextNodeId = "NULL";
    }
    printf("Node ID: %s\n"
           "Type Declared: %s\n"
           "Value initialised: %s\n"
           "Next node: %s\n"
           "Value: %s\n",nodeToPrint->id,declared,init,nextNodeId,val);

}

void recPrintTable(symbol_table *node,int nodeNo);

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
    } else {
        printf("Error while trying to print variable!\n");
        exit(1);
    }
}

/* ARITHMETIC FUNCTIONS
 * as well as some other features like string concatenation*/
struct variable sumOrConcat(struct variable n1, struct variable n2){
    struct variable result;

    //if the two variables are strings, concatenate them
    if (n1.type == STRING_TYPE && n2.type == STRING_TYPE){
        result.string_val = n1.string_val;
        strcat(result.string_val, n2.string_val);
        result.type = STRING_TYPE;
    }
    else if (n1.type == INTEGER_TYPE && n2.type == INTEGER_TYPE){
        result.integer_val = n1.integer_val + n2.integer_val;
        result.type = INTEGER_TYPE;
    }
    else if (n1.type == INTEGER_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.integer_val + n2.double_val;
        result.type = DOUBLE_TYPE;
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == INTEGER_TYPE){
        result.double_val = n1.double_val + n2.integer_val;
        result.type = DOUBLE_TYPE;
    }
    else if (n1.type == DOUBLE_TYPE && n2.type == DOUBLE_TYPE){
        result.double_val = n1.double_val + n2.double_val;
        result.type = DOUBLE_TYPE;
    }
    else{
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

struct variable mult(struct variable n1, struct variable n2){

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