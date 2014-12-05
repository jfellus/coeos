/*
Copyright  ETIS — ENSEA, Université de Cergy-Pontoise, CNRS (1991-2014)
promethe@ensea.fr

Authors: P. Andry, J.C. Baccon, D. Bailly, A. Blanchard, S. Boucena, A. Chatty, N. Cuperlier, P. Delarboulas, P. Gaussier, 
C. Giovannangeli, C. Grand, L. Hafemeister, C. Hasson, S.K. Hasnain, S. Hanoune, J. Hirel, A. Jauffret, C. Joulain, A. Karaouzène,  
M. Lagarde, S. Leprêtre, M. Maillard, B. Miramond, S. Moga, G. Mostafaoui, A. Pitti, K. Prepin, M. Quoy, A. de Rengervé, A. Revel ...

See more details and updates in the file AUTHORS 

This software is a computer program whose purpose is to simulate neural networks and control robots or simulations.
This software is governed by the CeCILL v2.1 license under French law and abiding by the rules of distribution of free software. 
You can use, modify and/ or redistribute the software under the terms of the CeCILL v2.1 license as circulated by CEA, CNRS and INRIA at the following URL "http://www.cecill.info".
As a counterpart to the access to the source code and  rights to copy, modify and redistribute granted by the license, 
users are provided only with a limited warranty and the software's author, the holder of the economic rights,  and the successive licensors have only limited liability. 
In this respect, the user's attention is drawn to the risks associated with loading, using, modifying and/or developing or reproducing the software by the user in light of its specific status of free software, 
that may mean  that it is complicated to manipulate, and that also therefore means that it is reserved for developers and experienced professionals having in-depth computer knowledge. 
Users are therefore encouraged to load and test the software's suitability as regards their requirements in conditions enabling the security of their systems and/or data to be ensured 
and, more generally, to use and operate it in the same conditions as regards security. 
The fact that you are presently reading this means that you have had knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

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
        while (pt != NULL)       /* compte le nombre d'entrees      */
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
        while (pt != NULL)       /* ecrit tous les coefficients     */
        {
            fwrite((type_coeff *) pt, sizeof(type_coeff), 1, f1);
            pt = pt->s;         /* inscrit coeff suivant           */
        }
    }
    fclose(f1);
}
