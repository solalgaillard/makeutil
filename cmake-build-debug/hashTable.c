#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SIZE 4096

struct data {
    char * value;
    char * key;
    struct data * next;
};




struct data* hashArray[SIZE];
struct data* dummyItem;
struct data* item;

int hashCode(char * key) {
    int hash = 0;

    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }

    return hash%SIZE;
}

struct DataItem *search(char * key) {
    //get the hash
    int hashIndex = hashCode(key);
    struct data * nextNode;

    //move in array until an empty
    if(hashArray[hashIndex]) {
        nextNode = hashArray[hashIndex];
        while(nextNode) {
            if (strcmp(nextNode->key, key) == 0) {
                return hashArray[hashIndex];
            }
            nextNode = nextNode->next;
        }
    }
    return NULL;
}

void insert(char * key,char * data) {

    struct data * item = (struct data*) malloc(sizeof(struct data));
    item->value = strdup(data);
    item->key = strdup(key);
    item->next = NULL;

    struct data * currentNode;

    //get the hash
    int hashIndex = hashCode(key);

    //move in array until an empty or deleted cell
    if(hashArray[hashIndex]) {
        currentNode = hashArray[hashIndex]
        while(currentNode) {
            if(currentNode->next == NULL) {
                currentNode->next = item;
                break;
            }
            currentNode = currentNode->next;
        }
    }
    else {
        hashArray[hashIndex] = item;
    }
}

void display() {
    int i = 0;

    struct data * currentNode;
    for(i = 0; i<SIZE; i++) {


        if(hashArray[i] != NULL)
            currentNode = hashArray[i];
            while(currentNode) {
                printf(" (%s,%s)",hashArray[i]->key,hashArray[i]->value);
                currentNode = currentNode->next;
            }
    }

    printf("\n");
}

int main() {
    dummyItem = (struct DataItem*) malloc(sizeof(struct DataItem));
    dummyItem->data = -1;
    dummyItem->key = -1;

    insert(1, 20);
    insert(2, 70);
    insert(42, 80);
    insert(4, 25);
    insert(12, 44);
    insert(14, 32);
    insert(17, 11);
    insert(13, 78);
    insert(37, 97);

    display();
    item = search(37);

    if(item != NULL) {
        printf("Element found: %d\n", item->data);
    } else {
        printf("Element not found\n");
    }

    delete(item);
    item = search(37);

    if(item != NULL) {
        printf("Element found: %d\n", item->data);
    } else {
        printf("Element not found\n");
    }