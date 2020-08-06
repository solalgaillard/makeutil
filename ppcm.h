/* ppcm.h
 constantes arbitraires, variables globales, prototypes de fonctions
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

# define MAXNOM 3000            /* nbre max. de noms (variables+fonctions) */
# define MAXEXPR 1000           /* nbre max. d'expresions par fonction */

#define SIZE 4096

char firstCommand[1000];



int hashCode(char * key) {
    int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash%SIZE;
}

struct token {
    char * value;
    int variableLevel;
};


struct tokenList {
    int count;
    struct token ** tokens;
};



//Do same thing with dependencies
struct cmd{
    struct tokenList * dependencies; // ->Résoudre en checkant si commande
    struct tokenList * callableCmds;
};




struct value {
    enum {variable, cmd} variableOrCmdSwitch;
    union
    {
        struct tokenList * tokensInVariable;
        struct cmd * cmd ;
    };
};


struct tokenList * createTokenList() {
    struct tokenList * tokenList = (struct tokenList*) malloc(sizeof(struct tokenList));
    tokenList->count = 0;
    return tokenList;
}

struct cmd * createCmd() {
    struct cmd * cmd = (struct cmd*) malloc(sizeof(struct cmd));
    cmd->dependencies = createTokenList();
    cmd->callableCmds = createTokenList();
    return cmd;
}

struct value * createCmdValue(struct tokenList * dependencies, struct tokenList * callableCmds) {
    struct value * value = (struct value*) malloc(sizeof(struct value));
    value->variableOrCmdSwitch = cmd;
    struct cmd * cmd = (struct cmd*) malloc(sizeof(struct cmd));
    cmd->dependencies = dependencies;
    cmd->callableCmds = callableCmds;
    value->cmd = cmd;
    return value;
}

struct value * createVariableValue(struct tokenList * tokenList) {
    struct value * value = (struct value*) malloc(sizeof(struct value));
    value->variableOrCmdSwitch = variable;
    value->tokensInVariable = tokenList;
    return value;
}

void addToTokenList(char * data, struct tokenList * tokenList, int variableLevel) {
    struct token ** newTokenList = tokenList->tokens;
    newTokenList = (struct token **)realloc(newTokenList, (tokenList->count + 1) * sizeof(struct token *));
    if(newTokenList==NULL)
    {
        perror("Memory allocation failed");
        exit(1); // exit the program
    }
    struct token * token = (struct token*) malloc(sizeof(struct token));
    token->value = strdup(data);
    token->variableLevel = variableLevel;
    newTokenList[tokenList->count++] = token;
    tokenList->tokens = newTokenList;
}

struct linkedList {
    char * key;
    struct value * value;
    struct linkedList * next;
};

struct linkedList* variablesHash[SIZE];
struct linkedList* cmdsHash[SIZE];



struct value * search(char * key, struct linkedList * hashTable[]) {
    //get the hash
    int hashIndex = hashCode(key);
    struct linkedList * currentNode;

    //move in array until an empty
    if(hashTable[hashIndex]) {
        currentNode = hashTable[hashIndex];
        while(currentNode) {
            if (strcmp(currentNode->key, key) == 0) {
                return currentNode->value;

            }
            currentNode = currentNode->next;
        }
    }
    return NULL;
}


void insert(char * key, struct value * value, struct linkedList * hashTable[]) {

    struct linkedList * item = (struct linkedList*) malloc(sizeof(struct linkedList));

    item->value = value;
    item->key = strdup(key);
    item->next = NULL;

    struct linkedList * currentNode;

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty or deleted cell
    if(hashTable[hashIndex]) {
        currentNode = hashTable[hashIndex];
        while(currentNode) {
            if(currentNode->next == NULL) {
                currentNode->next = item;
                break;
            }
            currentNode = currentNode->next;
        }
    }
    else {
        hashTable[hashIndex] = item;
    }
}

/*

void display(struct linkedList * hashTable[]) {
    int i = 0;

    struct linkedList * currentNode;
    for(i = 0; i<SIZE; i++) {
        if(hashTable[i] != NULL) {
            currentNode = hashTable[i];
            while (currentNode) {
                printf(" (%s,%s, %d)", currentNode->key, currentNode->value->variableValue, i);
                currentNode = currentNode->next;
            }
        }
    }

    printf("\n");
}

*/