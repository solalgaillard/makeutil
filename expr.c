/*  expr.c
    gestion de la construction des différentes structures de données.
*/

# include "make.h"

char ** getVariablesResolved(struct tokenList * );
char ** getDependenciesResolved(struct tokenList *);
char ** getcmdsResolved(struct tokenList *);

/*
    Initialise une struct tokenList, alloue la mémoire nécessaire,
    initialise le compte à 0 et renvoie un pointeur sur la struct.
*/
struct tokenList * createTokenList() {
    struct tokenList * tokenList = (struct tokenList*) malloc(sizeof(struct tokenList));
    tokenList->tokens = (struct token **) calloc(tokenList->count + 1, sizeof(struct token *));
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




char ** getVariablesResolved(struct tokenList * tokensInVariable) {
    char ** tokensParsedInVariable = (char **) calloc(tokensInVariable->count + 1, sizeof(char*));

    char ** resolvedNestedVariable;


    int j = 0;
    int f = 0;

    for(int i=tokensInVariable->count-1; i >= 0 ; i--) {

        char * variableToken = tokensInVariable->tokens[i]->value;
        int variableTokenLevel = tokensInVariable->tokens[i]->variableLevel;

        if (variableTokenLevel) {
            struct value * nestedResult = search(variableToken, variablesHash);

            if(nestedResult) {
                struct tokenList * nestedTokensInVariable = nestedResult->tokensInVariable;
                resolvedNestedVariable = getVariablesResolved(nestedTokensInVariable);

                j = 0;
                while (resolvedNestedVariable[j]) {
                    tokensParsedInVariable = (char **) realloc(tokensParsedInVariable, (tokensInVariable->count + 1 + j )* sizeof(char*));
                    tokensParsedInVariable[tokensInVariable->count - 1 - i - f + j] = resolvedNestedVariable[j];
                    j++;
                }
                j--;

            }
            else { //TRUNCATE ARRAY HERE DO IN OTHER DIRECTION
                /*for(int f=tokensInVariable->count - 1 - i + j;  f<tokensInVariable->count; f++) {

                    printf("truncated %s %s\n", tokensParsedInVariable[f], tokensParsedInVariable[f - 1]);

                    tokensParsedInVariable[f] = tokensParsedInVariable[f - 1];
                }*/
                f++;

                //tokensParsedInVariable = (char **) realloc(tokensParsedInVariable, (tokensInVariable->count + 1 + j - 1 )* sizeof(char*));

            }


        }
        else {
            //No realloc pour f++ car pas grave si on perd de l'espace.
            tokensParsedInVariable[tokensInVariable->count - 1 - i - f + j] = variableToken;
        }
    }

    return tokensParsedInVariable;
}


void insertImplictRule(char * targetName, struct value * result) {

    int index = -1;
    int i=strlen(targetName)-1;
    while(i>=0) {
        if (targetName[i] == '.') {
            index = i;
            break;
        }
        i--;
    }


    if (index != -1) {

        if(strcmp(targetName+index, ".o") == 0){

            char buffer[200] = {'\0'};


            int i = 0;
            while(i<index){
                buffer[i] = targetName[i];
                i++;
            }

            sprintf(buffer + strlen(buffer), "%s", ".c");

            struct tokenList * callableCmds;

            if(result) {
                callableCmds = result->cmd->callableCmds;
            }
            else {
                struct tokenList * dependencies = createTokenList();
                addToTokenList(strdup(buffer), dependencies, 0);
                callableCmds = createTokenList();
                struct value * cmdValue = createCmdValue(dependencies, callableCmds);
                insert(strdup(targetName), cmdValue, cmdsHash);
            }


            addToTokenList(strdup(buffer), callableCmds, 0);
            addToTokenList(strdup("-c"), callableCmds, 0);
            addToTokenList(strdup("CFLAGS"), callableCmds, 1);
            addToTokenList(strdup("CPPFLAGS"), callableCmds, 1); //CREATED SEGFAULT WHEN VAR NOT PRES
            addToTokenList(strdup("CC"), callableCmds, 1);





        }
        }

    }



enum Bool callCommmand(char * target, char* cwd) {
    struct value * result = search(target, cmdsHash);
    enum Bool hasLowerDependencyRebuilt = False;


    if (!result || result->cmd->callableCmds->count == 0) {
        insertImplictRule(target, result);
        if(!result) {
            result = search(target, cmdsHash);
        }




/*
        if (!result) {
            printf(".c should not be parsed");
        }
        else {
            char ** command = getVariablesResolved(result->cmd->callableCmds);


            char buffer[SIZE]={'\0'};


            int i = 0;
            while(command[i]) {
                printf( "\n%s \n", command[i]);
                i++;
            }
        }*/

    }


/*
    result = search("expr.o", cmdsHash);

    if(result) {

    }
    printf("buffer: %s", buffers);
*/
    /*
     *
     * - Get Extension of target
     *
     * struct tokenList * toklist = createTokenList();
     * addToTokenList($1, $2, 0);
     * struct value * cmdValue = createCmdValue($3, $5);
        printf("CMD TARGET %s\n",$1);
        insert($1, cmdValue, cmdsHash);
     *
     *
     * */




    if(result) {
        char fullpath[2000] = {'\0'};
        struct tokenList * dependencies = result->cmd->dependencies;
        char ** dependenciesResolved = getVariablesResolved(dependencies);


        //printf("%d\n\n", dependencies->count);

        struct stat attrib1;
        struct stat attrib2;
        sprintf(fullpath, "%s/%s", cwd, target);
        stat(fullpath, &attrib1);
        int doesTargetExist = access( fullpath, F_OK );
        char ** command;

        int i = 0;
        while (dependenciesResolved[i]) {
            memset(fullpath, '\0', 2000);
            sprintf(fullpath, "%s/%s", cwd, dependenciesResolved[i]);
            //printf("\nfullpath: %s %d\n", fullpath, access( fullpath, F_OK ));
            stat(fullpath, &attrib2); //If previous recursion rebuilt it

            //printf("\nInside target: %s %d %d %f \n",  dependenciesResolved[i], access( fullpath, F_OK ), doesTargetExist, difftime(attrib2.st_mtime, attrib1.st_mtime));

            if (callCommmand(dependenciesResolved[i], cwd) || doesTargetExist == -1 || access( fullpath, F_OK ) == - 1 || difftime(attrib2.st_mtime, attrib1.st_mtime) > 0) {


                //printf("Inside: %s", dependenciesResolved[i]);

/*
               command = getVariablesResolved(result->cmd->callableCmds);


                char buffer[SIZE]={'\0'};


                int i = 0;
                while(command[i]) {
                    sprintf(buffer + strlen(buffer), "%s ", command[i]);
                    i++;
                }

                printf("command: %s\n", buffer);


                FILE * p = popen (buffer, "w");
                pclose(p);*/
                hasLowerDependencyRebuilt = True;

            }
            i++;
        }

        if(hasLowerDependencyRebuilt) {
            command = getVariablesResolved(result->cmd->callableCmds);


            char buffer[SIZE]={'\0'};


            int i = 0;
            while(command[i]) {
                sprintf(buffer + strlen(buffer), "%s ", command[i]);
                i++;
            }

            printf("command: %s\n", buffer);


            FILE * p = popen (buffer, "w");
            pclose(p);
        }

    }
    return hasLowerDependencyRebuilt;
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

