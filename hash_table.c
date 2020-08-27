/*  hash_table.c
    Table de hachage simple avec des listes liées pour
    la gestion des collisions ou la redéfinition de variables.

    Pour l'instant, les variables redéfinies ne sont pas accessibles.
    En modifiant search, elles le seront en utilisant le nombre de $
    qui correspond à la redéfinition comme dans GNU make.
*/

# include "make.h"

/*
    déclaration de la fonction de hachage.
*/
int hashCode(char *);

/*
    Fonction simple de hachage.
*/
int hashCode(char * key) {
    unsigned long hash = 0;
    for (int i = 0; i < strlen(key); i++) {
        hash = 31 * hash + key[i];
    }
    return hash%SIZE;
}

/*
    A terme, il faudra passer en argument le nombre de $ pour savoir
    à quelle variable on fait appel (lorsque redéfini).
    Renvoie un type value qui peut être tout aussi bien celui utilisé
    pour les variables que pour les cibles.
 */
struct value * search(char * key, struct linkedList * hashTable[]) {

    /*Obtient l'index et initialise premier noeud de liste*/
    struct linkedList * currentNode = hashTable[hashCode(key)];

    /*Renvoie première valeur qui a la même clé que la requête*/
    while(currentNode) {
        if (strcmp(currentNode->key, key) == 0) {
            return currentNode->value;
        }
        currentNode = currentNode->next; //Sinon continue d'itérer
    }
    return NULL; //En cas de non trouvaille, renvoie NULL
}

/*
    Fonction qui insère les données dans la table de hachage.
    Prend une struct value fais pointer le dernier noeud de la
    liste liée ou directement l'index de la table s'il n'y pas
    encore de noeud.
*/
void insert(char * key, struct value * value, struct linkedList * hashTable[]) {

    /*Initialise noeud de liste liée*/
    struct linkedList * item = (struct linkedList*) malloc(sizeof(struct linkedList));
    item->value = value;
    item->key = key;
    item->next = NULL;

    /*Obtient index dans la table*/
    int hashIndex = hashCode(key);

    /*Obtient noeud courant*/
    struct linkedList * currentNode = hashTable[hashIndex];

    /*Tant qu'on peut parcourir les noeuds et que ce n'est pas
      le dernier
     */
    while(currentNode) {
        /*Si dernier noeud, on lie le nouveau noeud
          et on sort
         */
        if(currentNode->next == NULL) {
            currentNode->next = item;
            return;
        }
        currentNode = currentNode->next;
    }
    /*Sinon on fait pointer la table sur le noeud*/
    hashTable[hashIndex] = item;
}