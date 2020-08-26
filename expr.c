/*  expr.c
    gestion de la construction des différentes structures de données.
*/

# include "make.h"

char * getVariablesResolved(char *);
void getDependenciesResolved(struct tokenList *, char **);
char * getcmdsResolved(struct tokenList *);

/*
    Initialise une struct tokenList, alloue la mémoire nécessaire,
    initialise le compte à 0 et renvoie un pointeur sur la struct.
*/
struct tokenList * createTokenList() {
    struct tokenList * tokenList = (struct tokenList*) malloc(sizeof(struct tokenList));
    tokenList->count = 0;
    return tokenList;
}

/*
    Modifie en place une struct tokenList. Réalloue de la place sur le heap avec le
    compte actuel de tokens + 1. Alloue la place mémoire pour un nouveau token, assigne le
    niveau de variable (0,1,..) et fais pointer sur la chaîne de caractère.
    Réassigne le nouveau token sur le dernier index de la liste de tokens
*/
void addToTokenList(char * data, struct tokenList * tokenList, int variableLevel) {
    tokenList->tokens = (struct token **)realloc(tokenList->tokens, (tokenList->count + 1) * sizeof(struct token *));
    if(tokenList->tokens == NULL)
    {
        perror("Problème avec l'allocation de la mémoire");
        exit(1); // exit the program
    }
    struct token * token = (struct token*) malloc(sizeof(struct token));
    token->value = data;
    token->variableLevel = variableLevel;
    tokenList->tokens[tokenList->count++] = token;
}


/*
    Initialise une struct cmd, alloue la mémoire nécessaire,
    initialise deux structs tokenList et fais pointer les pointeurs
    dependencies et callableCmds vers celles-ci.
*/
struct cmd * createCmd() {
    struct cmd * cmd = (struct cmd*) malloc(sizeof(struct cmd));
    cmd->dependencies = createTokenList();
    cmd->callableCmds = createTokenList();
    return cmd;
}

/*
    Initialise une struct value, alloue la mémoire nécessaire,
    switch
    dependencies et callableCmds vers celles-ci.
*/

struct value * createCmdValue(struct tokenList * dependencies, struct tokenList * callableCmds) {
    struct value * value = (struct value*) malloc(sizeof(struct value));
    printf("*******%d*****\n", cmd);
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


char * getVariablesResolved(char * token) {
    char buffer[SIZE];
    struct value * nestedResult = search(token, variablesHash);
    struct tokenList * tokensInVariable = nestedResult->tokensInVariable;
    for(int i=tokensInVariable->count-1; i >= 0 ; i--) {
        char * variableToken = tokensInVariable->tokens[i]->value;
        int variableTokenLevel = tokensInVariable->tokens[i]->variableLevel;
        if(variableTokenLevel){
            variableToken = getVariablesResolved(variableToken);
        }
        if(i==tokensInVariable->count-1) {
            sprintf(buffer, "%s", variableToken);
        }
        else {
            sprintf(buffer + strlen(buffer), " %s", variableToken);
        }
    }
    return strdup(buffer);
}

//string array for all dependencies
void getDependenciesResolved(struct tokenList * dependencies, char ** buffer) {
    for(int i=0; i < dependencies->count; i++) {
        char * token = dependencies->tokens[i]->value;
        printf("\ntoken: %s\n", token);
        int isVar = dependencies->tokens[i]->variableLevel;
        if(isVar){
            token = getVariablesResolved(token);
        }
        buffer[i] = token;
    }
}

char * getcmdsResolved(struct tokenList * callableCmds) {
    char buffer[SIZE];
    for(int i=callableCmds->count-1; i >= 0 ; i--) {
        char * token = callableCmds->tokens[i]->value;
        printf("\n%s\n", token);
        int isVar = callableCmds->tokens[i]->variableLevel;
        printf("\n%d\n", isVar);
        if(isVar){
            token = getVariablesResolved(token);
        }
        if(i==callableCmds->count-1) {
            sprintf(buffer, "%s", token);
        }
        else {
            sprintf(buffer + strlen(buffer), " %s", token);
        }
    }
    return strdup(buffer);
}


void callCommmand(char * target, char* cwd) {
    struct value * result = search(target, cmdsHash);
    if(result) {
        char *dependenciesResolved[1024] = {NULL};
        char fullpath[2000] = {'\0'};
        struct tokenList *dependencies = result->cmd->dependencies;
        getDependenciesResolved(dependencies, dependenciesResolved);

        struct stat attrib1;
        struct stat attrib2;
        sprintf(fullpath, "%s/%s", cwd, target);
        int statsTarget;
        char * command;

        int i = 0;
        while (dependenciesResolved[i]) {
            memset(fullpath, '\0', 2000);
            sprintf(fullpath, "%s/%s", cwd, dependenciesResolved[i]);
            printf("\nfullpath: %s %d\n", fullpath, access( fullpath, F_OK ));
            statsTarget = stat(fullpath, &attrib2); //If previous recursion rebuilt it
            if (access( fullpath, F_OK ) == -1 || difftime(attrib2.st_mtime, attrib1.st_mtime) > 0) {
                printf("\nInside target: %s\n", target);
                //Check if terminal, don't take decision until we have parsed it all?
                callCommmand(dependenciesResolved[i], cwd);

                command = getcmdsResolved(result->cmd->callableCmds);
                printf("\ncommand: %s\n", command);
                system(command);
            }
            i++;
        }
    }
}

void cleanUpMemory() {
    struct linkedList * linkedList;

    for(int i = 0; i<SIZE; i++) {
        linkedList = variablesHash[i];
        while (linkedList) {
            free(variablesHash[i]->key);
            struct value * value = variablesHash[i]->value;
            struct tokenList * tokensInVariable = value->tokensInVariable;
            for(int j=0; j<tokensInVariable->count;j++) {
                free(tokensInVariable->tokens[j]->value);
                free(tokensInVariable->tokens[j]);
            }
            free(tokensInVariable);
            free(value);
            free(linkedList);
            linkedList = variablesHash[i]->next;
        }
        linkedList = cmdsHash[i];
        while (linkedList) {
            free(cmdsHash[i]->key);
            struct value * value = cmdsHash[i]->value;
            struct cmd * cmd = value->cmd;
            struct tokenList * dependencies = cmd->dependencies;
            for(int j=0; j<dependencies->count;j++) {
                free(dependencies->tokens[j]->value);
                free(dependencies->tokens[j]);
            }
            free(cmd);
            struct tokenList * callableCmds = cmd->callableCmds;
            for(int j=0; j<callableCmds->count;j++) {
                free(callableCmds->tokens[j]->value);
                free(callableCmds->tokens[j]);
            }
            free(dependencies);
            free(value);
            free(linkedList);
            linkedList = cmdsHash[i]->next;
        }
    }
}

