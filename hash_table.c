/* expr.c
 gestion des expressions, de la memoire et des registres de ppcm
 */
# include <stdio.h>
# include "make.h"

int hashCode(char * key) {
    int hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash%SIZE;
}


struct value * search(char * key, struct linkedList * hashTable[]) {
    //get the hash
    int hashIndex = hashCode(key);
    struct linkedList * currentNode;

    //move in array until an empty

    currentNode = hashTable[hashIndex];

    while(currentNode) {
        if (strcmp(currentNode->key, key) == 0) {
            return currentNode->value;

        }
        currentNode = currentNode->next;
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
