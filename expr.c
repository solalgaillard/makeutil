/* expr.c
 gestion des expressions, de la memoire et des registres de ppcm
 */
# include <stdio.h>
# include "make.h"


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
            printf("testtest");
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
        int statsTarget = stat(fullpath, &attrib1);

        int mustBeRebuilt = 0;
        if (statsTarget != -1) {
            int i = 0;
            while (dependenciesResolved[i]) {

                memset(fullpath, '\0', 2000);
                sprintf(fullpath, "%s/%s", cwd, dependenciesResolved[i]);
                statsTarget = stat(fullpath, &attrib2);
                if (statsTarget != -1) {

                    //Check if terminal, don't take decision until we have parsed it all?

                    callCommmand(dependenciesResolved[i], cwd);

                    statsTarget = stat(fullpath, &attrib2); //If previous recursion rebuilt it
                    if (statsTarget == -1) {
                        perror("Problème après reconstruction");
                    }

                    if (difftime(attrib2.st_mtime, attrib1.st_mtime) > 0) {
                        mustBeRebuilt = 1;
                    }
                }
                i++;
            }
        }

        if(mustBeRebuilt || statsTarget == -1) {
            char * command = getcmdsResolved(result->cmd->callableCmds);
            printf("\n%s\n", command);
            system(command);
        }

    }

}