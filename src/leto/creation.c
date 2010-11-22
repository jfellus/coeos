/* fonction principale de leto permettant de generer un fichier .res */
/* a partir d'un fichier .script */
/* la gestion detaillee de la creation est faite dans leto_cle.c     */

#include "public_leto.h"
#include <stdio.h>
#include <stdlib.h>

#include "leto_global_var.h"
#include "outils.h"




/* detruit tous les neurones et les liens crees pour la creation et l'enregistrement du .res */
/* evite les pbs de debordement de memoire si l'on fait plusieurs creation de reseau sans quitter leto */

void destruction()
{
  type_groupe *groupe;
  type_liaison *liaison;

  printf("Destruction/liberation des donnees liees au .res \n");
  
  groupe = sc->deb_groupe;
  liaison = sc->deb_liaison;
  
  /*  free_liaison(liaison);
      free_liste_groupes(groupe);*/
  free_reseau(sc->neurone);
  printf("fin de la destruction\n");
}


/*-------------------------------------------------------------------------*/

void creation(TxDonneesFenetre *onglet_leto)
{
    int i, j, k;
    int s_deb, s_fin, e_deb, e_fin;
    int x, y, dx;
    type_groupe *groupe;
    type_liaison *liaison;
    int nbre_voie;
    int pas;                    /* taille du macro neurones pour trouver */
    /* les micron neurones par modulo        */
    float val;
    int no_gpe_liaison;

    int groupe_nbre,taillex;

    sc->nbre_neurone = compte_neurone();
    printf("\nLe reseau comprend %d neurones \n", sc->nbre_neurone);
    sc->neurone = creer_reseau(sc->nbre_neurone);

    /* numerotation de neurones en fontion des groupes              */
    
    k = 0;
    groupe = sc->deb_groupe;
    while (groupe != NULL)
      {
	groupe_nbre=MY_Int2Int(groupe->nbre); /* on convertit dans au cas ou on serait en version symbolique */
      for (j = 0; j < groupe_nbre; j++)
        {
	  sc->neurone[k].groupe = groupe->no;
	  k++;
        }
      groupe = groupe->s;
    }
    printf("fin initialisation des groupes pour chaque neurone\n");

/*Creation des liaisons entre les neurones des different groupes*/

/*    for (i = 0; i < sc->nbre_groupe; i++)
    {
    groupe = trouver_groupe(i);*/

    groupe = sc->deb_groupe;
    while (groupe != NULL)
      {
        i = groupe->no;
        debug_printf("etude du groupe %d\n", i);
        nbre_voie = 0;
        if (groupe->nbre_voie > 1
            && (groupe->type == No_PTM || groupe->type == No_Winner_Macro
                || groupe->type == No_PLG || groupe->type == No_Winner_Colonne
                || groupe->type == No_But || groupe->type == No_Pyramidal
                || groupe->type == No_Pyramidal_plan
                || groupe->type == No_Sigma_PI
                || groupe->type == No_Macro_Colonne
		|| groupe->type == No_KO_Discret
		|| groupe->type == No_KO_Continu))
            pas = groupe->nbre_voie + 1;
        else
	  pas = 1;
	
	taillex = MY_Int2Int(groupe->taillex); /* on convertit au cas ou on serait en version symbolique */
	
        dx = taillex;
	
        for (j = 0; j < sc->nbre_neurone; j++)
	  {
	    if (sc->neurone[j].groupe == i)
	      break;
	  }
	s_deb = j;
        x = y = 0;
	for (j = s_deb; j < sc->nbre_neurone; j++)
	  
	  {
	    if (sc->neurone[j].groupe != i)
	      break;
	    sc->neurone[j].posx = (float) x;
	    sc->neurone[j].posy = (float) y;
	    sc->neurone[j].posz = (float) i;
	    x++;
	    if (x >= dx)
	      {
                y++;
                x = 0;
	      }
	  }
        s_fin = j - 1;
	debug_printf("s_deb = %d , s_fin = %d \n", s_deb, s_fin);
        liaison = sc->deb_liaison;
        no_gpe_liaison = 1;     /* le 0 correspond aux liens entre micro et macro neurones */
        while (liaison != NULL)
	  {
            if (liaison->arrivee == i && liaison->type != No_l_algorithmique
                && liaison->type != No_l_neuro_mod)
	      {
                k = liaison->depart;
                nbre_voie++;
                for (j = 0; j < sc->nbre_neurone; j++)
		  {
		    if (sc->neurone[j].groupe == k)
		      break;
		  }
		e_deb = j;
		for (j = e_deb; j < sc->nbre_neurone; j++)
		  {
		    if (sc->neurone[j].groupe != k)
		      break;
		  }
                e_fin = j - 1;
	        debug_printf("e_deb = %d ,e_fin = %d \n", e_deb, e_fin);
                creer_liaisons_entre_groupe(e_deb, e_fin, s_deb, s_fin, i, k,
                                            liaison, nbre_voie - 1, pas,
                                            no_gpe_liaison - 1);
	      }
            liaison = liaison->s;
            no_gpe_liaison++;
	  }
        if (nbre_voie > 1
            && (groupe->type == No_PTM || groupe->type == No_Winner_Macro
                || groupe->type == No_PLG || groupe->type == No_Winner_Colonne
                || groupe->type == No_But || groupe->type == No_Pyramidal
                || groupe->type == No_Pyramidal_plan
                || groupe->type == No_Sigma_PI
                || groupe->type == No_Macro_Colonne
		|| groupe->type == No_KO_Discret
		|| groupe->type == No_KO_Continu))
	  {
            if (groupe->type == No_But || groupe->type == No_Ou)
	      val = 1.;
            else
	      val = 1. / ((float) nbre_voie);
            debug_printf("creation w=%f %d \n", val, nbre_voie);
            creer_micro_liens(s_deb, s_fin, i, nbre_voie, val);
	  }
	groupe = groupe->s;
      }                           /*       etude du groupe suivant       */
    printf("fin creation\n");
    /*  ecriture_reseau2(freseau,nomfich1); */
    ecrit_reseau();
    save_script(1,0,onglet_leto); /*CG: sauvegarde  le .script*/
    save_script(1,1,onglet_leto);  /*PG: sauve le script et met a plat les sous reseau utile avec le .res mais pb denomination*/ /*sauvegarde(); */
    /*     destruction(); */
}

