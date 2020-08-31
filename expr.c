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
    initialise deux structs tokenList et fait pointer les pointeurs
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
    Switch cmd pour l'union.
    Initialise deux tokenLists pour les dependencies et les commandes. Fait pointer
    une struct cmd sur ces deux tokenLists.
    Idéalement, la structure devrait être revue pour accommoder plusieurs commandes.
*/

struct value * createCmdValue(struct tokenList * dependencies, struct tokenList * callableCmds) {
    struct value * value = (struct value*) malloc(sizeof(struct value));
    value->variableOrCmdSwitch = cmd;
    struct cmd * cmd = (struct cmd*) malloc(sizeof(struct cmd));
    cmd->dependencies = dependencies;
    cmd->callableCmds = callableCmds;
    value->cmd = cmd;
    return value;
}


/*
    Initialise une struct value, alloue la mémoire nécessaire,
    Switch variable pour l'union.
    Initialise deux tokenLists pour les dependencies et les commandes.
    Idéalement, la structure devrait être revue pour accommoder plusieurs commandes shell.
*/

struct value * createVariableValue(struct tokenList * tokenList) {
    struct value * value = (struct value*) malloc(sizeof(struct value));
    value->variableOrCmdSwitch = variable;
    value->tokensInVariable = tokenList;
    return value;
}


/*
    Résoud entièrement toutes les variables imbriquées, applatit la liste au fur et à mesure.
*/

char ** getVariablesResolved(struct tokenList * tokensInVariable) {
    char ** tokensParsedInVariable = (char **) calloc(tokensInVariable->count + 1, sizeof(char*));

    char ** resolvedNestedVariable;

    int f = 0; // Mémoire de l'offset entre toutes les itérations

    for(int i=tokensInVariable->count-1; i >= 0 ; i--) {

        char * variableToken = tokensInVariable->tokens[i]->value;
        int variableTokenLevel = tokensInVariable->tokens[i]->variableLevel;

        if (variableTokenLevel) {
            struct value * nestedResult = search(variableToken, variablesHash);

            if(nestedResult) {
                struct tokenList * nestedTokensInVariable = nestedResult->tokensInVariable;
                resolvedNestedVariable = getVariablesResolved(nestedTokensInVariable);

                int j = 0;
                while (resolvedNestedVariable[j]) {
                    if(j>0) {
                        tokensParsedInVariable = (char **) realloc(tokensParsedInVariable, (tokensInVariable->count + 1 + j - f)* sizeof(char*));
                    }
                    tokensParsedInVariable[tokensInVariable->count - 1 - i - f] = resolvedNestedVariable[j];
                    j++;
                    f--;
                }
                if(j){
                    f++;
                }

            }
            else { //truncate array si variable vide
                f++;
            }

        }
        else {
            //No realloc pour f++ car pas grave si on perd de l'espace.
            tokensParsedInVariable[tokensInVariable->count - 1 - i - f] = variableToken;
        }
    }
    return tokensParsedInVariable;
}


/*
    "Proof of concept" de règles implicites en insérant directement dans nos
    structures des règles quand elles ne sont pas présentes à partir des "dependencies".
*/

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


/*
  Fonction récursive qui indique si elle a appelé la commande.

*/
enum Bool callCommmand(char * target, char* cwd) {
    struct value *result = search(target, cmdsHash);
    enum Bool hasLowerDependencyRebuilt = False;

    /* Injecte la règle implicite dans la structure de données */
    if (!result || result->cmd->callableCmds->count == 0) {
        insertImplictRule(target, result);
        if (!result) {
            result = search(target, cmdsHash);
        }
    }

    /* Si la commande existe */
    if (result) {
        char fullpath[2000] = {'\0'};
        struct tokenList *dependencies = result->cmd->dependencies;
        char **dependenciesResolved = getVariablesResolved(dependencies);

        struct stat attrib1;
        struct stat attrib2;
        sprintf(fullpath, "%s/%s", cwd, target);
        stat(fullpath, &attrib1);
        int doesTargetExist = access(fullpath, F_OK);
        char **command;

        /* Appel récursif sur toutes les dependencies */
        int i = 0;
        while (dependenciesResolved[i]) {
            memset(fullpath, '\0', 2000);
            sprintf(fullpath, "%s/%s", cwd, dependenciesResolved[i]);
            stat(fullpath, &attrib2); //If previous recursion rebuilt it

            /* Si une dependency force l'appel d'une commande, on le note. */
            if (callCommmand(dependenciesResolved[i], cwd) || doesTargetExist == -1 ||
                access(fullpath, F_OK) == -1 || difftime(attrib2.st_mtime, attrib1.st_mtime) > 0) {
                hasLowerDependencyRebuilt = True;

            }
            i++;

        }

        /* On appelle la commande une fois itéré sur toutes les dependencies */
        if (hasLowerDependencyRebuilt || dependencies->count == 0) {
            command = getVariablesResolved(result->cmd->callableCmds);

            char buffer[SIZE] = {'\0'};


            int i = 0;
            while (command[i]) {
                sprintf(buffer + strlen(buffer), "%s ", command[i]);
                i++;
            }

            printf("command: %s\n", buffer);


            FILE *p = popen(buffer, "w");
            pclose(p);
        }

    }
    return hasLowerDependencyRebuilt;

}

/*
  Nettoie la mémoire, sûrement perfectible, premier jet.
*/
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

