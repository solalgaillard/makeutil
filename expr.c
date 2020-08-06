/* expr.c
 gestion des expressions, de la memoire et des registres de ppcm
 */
# include <stdio.h>
# include "ppcm.h"

struct expr expr[MAXEXPR];	/* les expressions */
int nexpr;			/* le nombre d'expressions */

/* fairexpr  --  fabrique une expression (parametre, argument ou temporaire) */
struct expr *
fairexpr(char * nom){
  register struct expr * e;

  e = &expr[nexpr++];
  e->position = posexpr;
  e->nom = nom;
  posexpr += incr;
  return e;
}

/* exprvar  ---  renvoie l'expression qui designe la variable */
struct expr *
exprvar(char * s){
  register struct expr * e, * f;

  for(e = & expr[0], f = e + nexpr; e < f; e += 1)
    if (/* e->nom != NULL && */ e->nom == s)
      return e;
  fprintf(stderr, "Erreur, variable %s introuvable\n", s);
  return & expr[0];
}

/* reinitexpr  --  a la fin d'une fonction, ni expression ni registre */
void
reinitexpr(){
  nexpr = 0;
}
