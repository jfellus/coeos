
#include <stdio.h>

#include "graphic_Tx.h"
#include "script.h"
#include "reseau.h"


void ecriture_reseau2( char freseau[100], char nomfich1[100])
{
   int i, j, items;
    type_coeff *pt;
    FILE *f1;

    f1 = fopen(freseau, "w");
    if (f1 == NULL)
    {
        printf
            ("\n Erreur lors de l'ecriture du fichier contenant le reseau \n");
        exit(0);
    }
    printf("ecriture du reseau\n");
    fprintf(f1, "script associe : %s\n", nomfich1);
    fprintf(f1, "%d\n", sc->nbre_couche);
    fprintf(f1, "%d\n", sc->nbre_groupe);
    fprintf(f1, "%d\n", sc->nbre_neurone);
    fprintf(f1, "%d\n", sc->nbre_entree);
    fprintf(f1, "%d\n", sc->nbre_sortie);
    sc->ca = 0;
    sc->ck = 0;                     /*on peut les demander a utilisat  */
    fprintf(f1, "%f\n", sc->ca);    /* constantes de la sigmoide       */
    fprintf(f1, "%f\n", sc->ck);
    /*ecrit le nbre de neurones par couches */
    for (i = 0; i < sc->nbre_couche; i++)
        fprintf(f1, "%d\n", sc->t[i]);

    /*ecrit type de chaque groupe           */

    for (i = 0; i < sc->nbre_groupe; i++)
    {
        fprintf(f1, "%d\n", (trouver_groupe(i))->type);
        fprintf(f1, "nom du groupe : %s\n", (trouver_groupe(i))->nom);
    }


    for (i = 0; i < sc->nbre_neurone; i++)
    {
        j = 0;                  /* nbre d'entrees                  */
        pt = sc->neurone[i].coeff;
        while (pt != nil)       /* compte le nombre d'entrees      */
        {
            j++;
            pt = pt->s;
        }
        sc->neurone[i].nbre_coeff = j;
    }
    printf("taille = %d \n", (int)sizeof(sc->neurone[0]));
    items = fwrite((type_neurone *) sc->neurone, sizeof(sc->neurone[0]), sc->nbre_neurone, f1);
    printf("items = %d \n", items);
    if (items != sc->nbre_neurone)
        printf("ERREUR ecriture fichier \n");

    for (i = 0; i < sc->nbre_neurone; i++)
    {
        pt = (sc->neurone[i]).coeff;
        while (pt != nil)       /* ecrit tous les coefficients     */
        {
            fwrite((type_coeff *) pt, sizeof(type_coeff), 1, f1);
            pt = pt->s;         /* inscrit coeff suivant           */
        }
    }
    fclose(f1);
}
