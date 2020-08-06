/* ppcm.h
 constantes arbitraires, variables globales, prototypes de fonctions
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

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



char * getVariablesResolved(char * token) {
    char buffer[SIZE];
    struct value * nestedResult = search(token, variablesHash);
    struct tokenList * tokensInVariable = nestedResult->tokensInVariable;
    for(int i=0; i<tokensInVariable->count; i++) {
        char * variableToken = tokensInVariable->tokens[i]->value;
        int variableTokenLevel = tokensInVariable->tokens[i]->variableLevel;
        if(variableTokenLevel){
            printf("TEST%d", variableTokenLevel);
            variableToken = getVariablesResolved(variableToken);
        }
        sprintf(buffer, " %s", variableToken);
    }

    return strdup(buffer);
}

//string array for all dependencies
void getDependenciesResolved(struct tokenList * dependencies, char ** buffer) {
    for(int i=0; i < dependencies->count; i++) {
        char * token = dependencies->tokens[i]->value;
        int isVar = dependencies->tokens[i]->variableLevel;
        if(isVar){
            printf("TEST%d", isVar);
            token = getVariablesResolved(token);
        }
        buffer[i] = token;
    }
}

char * getcmdsResolved(struct tokenList * callableCmds) {
    char buffer[SIZE];
    for(int i=0; i < callableCmds->count; i++) {
        char * token = callableCmds->tokens[i]->value;
        int isVar = callableCmds->tokens[i]->variableLevel;
        if(isVar){
            token = getVariablesResolved(token);
        }
        sprintf(buffer, " %s", token);
    }
    return strdup(buffer);
}

/*
void callCommmands(dependencies needed resolved, cmds resolved) {

}
 */

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