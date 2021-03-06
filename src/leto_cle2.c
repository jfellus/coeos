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
#include <math.h>

/*#define DEBUG*/

#include "graphic_Tx.h"
#include "script.h"
#include "reseau.h"
#include "outils.h"
#include <limits.h>

int liste[100000]; /* tableau contenant la liste des liens tires au hasard lors de la creation de liens aleatoires*/
/* pour eviter de retirer plusieurs fois le meme lien */

/*-----------------------------------------------------------------------------*/

void ecrit_reseau_ascii()
{
  int i, j;
  type_coeff *pt;
  type_groupe *groupe;
  FILE *f1;

  f1 = fopen(sc->freseau, "w");
  if (f1 == NULL)
  {
    EXIT_ON_ERROR("Erreur lors de l'ecriture du fichier contenant le reseau \n");
  }

  printf("ecriture du reseau\n");
  fprintf(f1, "Reseau de neurone\n");
  fprintf(f1, "Copyright Philippe GAUSSIER oct 1991\n");
  fprintf(f1, "Version %d\n\n", RESEAU_VERSION); /* AR : VERSION -> RESEAU_VERSION 17/01/2004 */
  fprintf(f1, "script associe : %s\n", sc->nomfich1);
  fprintf(f1, "%d\n", sc->nbre_couche);
  fprintf(f1, "%d\n", sc->nbre_groupe);
  fprintf(f1, "%d\n", sc->nbre_neurone);
  fprintf(f1, "%d\n", sc->nbre_entree);
  fprintf(f1, "%d\n", sc->nbre_sortie);

  /*ecrit le nbre de neurones par couches */
  for (i = 0; i < sc->nbre_couche; i++)
    fprintf(f1, "%d\n", sc->t[i]);

  /*ecrit type de chaque groupe */
  groupe = sc->deb_groupe;
  while (groupe != NULL)
  {
    fprintf(f1, "%d\n", groupe->type);
    fprintf(f1, "nom du groupe : %s\n", groupe->nom);
    groupe = groupe->s;
  }

  for (i = 0; i < sc->nbre_neurone; i++)
  {
    j = 0; /* nbre d'entrees                  */
    pt = sc->neurone[i].coeff;
    while (pt != NULL) /* compte le nombre d'entrees      */
    {
      j++;
      pt = pt->s;
    }

    fprintf(f1, "%d\n", sc->neurone[i].groupe);
    if (trouver_groupe(sc->neurone[i].groupe)->type == No_Fonction_Algo)
    {
      for (j = i + 1; j < sc->nbre_neurone; j++)
        if (sc->neurone[i].groupe != sc->neurone[j].groupe)
        {
          fprintf(f1, "taille = %d \n", j - i);
          i = j - 1;
          break;
        }
      if (j == sc->nbre_neurone)
      {
        fprintf(f1, "taille = %d \n", j - i);
        i = j - 1;
      }
    }
    else
    {
      fprintf(f1, "%d\n", j);
      fprintf(f1, "%f\n", sc->neurone[i].seuil);

      if (trouver_groupe(sc->neurone[i].groupe)->type == No_Winner_Colonne || trouver_groupe(sc->neurone[i].groupe)->type == No_PTM) fprintf(f1, "%f\n", sc->neurone[i].d);

      fprintf(f1, "%f %f %f\n", sc->neurone[i].posx, sc->neurone[i].posy, sc->neurone[i].posz);
      pt = (sc->neurone[i]).coeff;
      while (pt != NULL) /* ecrit tous les coefficients     */
      {
        fprintf(f1, "%f ", pt->val);
        fprintf(f1, "%d ", pt->entree);
        fprintf(f1, "%d ", pt->type);
        fprintf(f1, "%d ", pt->evolution);
        /*     fprintf(f1,"%d ",pt->temps);
         fprintf(f1,"%d\n",pt->stemps);  */
        pt = pt->s; /* inscrit coeff suivant           */
      }
    } /* pour les fonctions pas besoin de les stocker */
  }
  fclose(f1);
}

void ecrit_reseau()
{
  int i;
  int taille_int, taille_float;
  type_coeff *pt;
  char statut_continuer = 1;
  char statut_fin = 0;
  int taille_char;
  FILE *f1;

  f1 = fopen(sc->freseau, "w");
  if (f1 == NULL)
  {
    printf("Erreur lors de l'ecriture du fichier contenant le reseau");
    exit(1);
  }

  fprintf(f1, "Reseau de neurone\n");
  fprintf(f1, "Copyright Philippe GAUSSIER oct 1991\n");

  fprintf(f1, "Version %i\n\n", RESEAU_VERSION);

  fprintf(f1, "script associe : %s\n", sc->nomfich1);

  fprintf(f1, "%i\n", sc->nbre_couche);

  fprintf(f1, "%i\n", sc->nbre_groupe);

  fprintf(f1, "%i\n", sc->nbre_neurone);

  for (i = 0; i < sc->nbre_couche; i++)
  {
    fprintf(f1, "%i\n", sc->t[i]);
  }

  fwrite(sc->neurone, sizeof(type_neurone), sc->nbre_neurone, f1);

  /*   for (i = 0; i < sc->nbre_neurone; i++)
   {
   printf("neuron %i : seuil = %f, s = %f, s1 = %f, s2 = %f, flag = %i, d = %f, last_activation = %f, cste = %f, groupe = %i, nbre_voie = %i, nbre_coeff = %i, max = %c, posx = %f, posy = %f, posz = %f, coeff = %p, nsor = %p\n\n", i, sc->neurone[i].seuil, sc->neurone[i].s, sc->neurone[i].s1, sc->neurone[i].s2, sc->neurone[i].flag, sc->neurone[i].d, sc->neurone[i].last_activation, sc->neurone[i].cste, sc->neurone[i].groupe, sc->neurone[i].nbre_voie, sc->neurone[i].nbre_coeff, sc->neurone[i].max, sc->neurone[i].posx, sc->neurone[i].posy, sc->neurone[i].posz, (void *)sc->neurone[i].coeff, (void *)sc->neurone[i].nsor);
   } */

  taille_int = sizeof(int);
  taille_float = sizeof(float);
  taille_char = sizeof(char);

  for (i = 0; i < sc->nbre_neurone; i++)
  {
    pt = sc->neurone[i].coeff;
    while (pt != NULL) /* ecrit tous les coefficients     */
    {
      fwrite(&(pt->val), taille_float, 1, f1);
      fwrite(&(pt->entree), taille_int, 1, f1);
      fwrite(&(pt->type), taille_int, 1, f1);
      fwrite(&(pt->evolution), taille_int, 1, f1);
      fwrite(&(pt->proba), taille_float, 1, f1);
      fwrite(&(pt->gpe_liaison), taille_int, 1, f1);
      fwrite(&(pt->Nbre_E), taille_float, 1, f1);
      fwrite(&(pt->Nbre_S), taille_float, 1, f1);
      fwrite(&(pt->Nbre_ES), taille_float, 1, f1);

      pt = pt->s; /* inscrit coeff suivant           */

      if (pt == NULL)
      {
        fwrite(&statut_fin, taille_char, 1, f1);
      }
      else
      {
        fwrite(&statut_continuer, taille_char, 1, f1);
      }
    }
  }

  fclose(f1);
}

/*-------------------------------------------------------------------------*/
/*    choix des liaisons 1 vers X au hasard                                */
/* choisi une liaison au hasard                                            */
/* teste si val deja attribuee a une liaison                               */
/* si pas utilisee la rajoute dans le tableau                              */
/*-------------------------------------------------------------------------*/

int teste_si_deja_relie(int val, int taille)
{
  int i;
  for (i = 0; i < taille; i++)
  {
    if (liste[i] == val) return (1);
  }
  return (0);
}

/*--------------------------------------------------------------------------*/
/* retourne le nombre de liaisons creees                                    */

int choix_liaisons_hasard(type_liaison * liaison, int neurone_deb, /* numero du 1er neurone du groupe d'entree             */
int neurone_fin, /* numero du dernier neurone ....                       */
int pas_entree /* pour ne tenir compte que des macro neurones          */)
{
  int i, val, liaison_nbre;
  liaison_nbre = MY_Int2Int(liaison->nbre);

  for (i = 0; i < liaison_nbre; i++) /* (int)(liaison->nbre) */
  {
    do
    {
      val = neurone_deb + (int) ((((float) ((neurone_fin - neurone_deb) / pas_entree)) * ((float) rand())) / ((float) modulo));
    } while (teste_si_deja_relie(val * pas_entree - 1, i) == 1);
    liste[i] = val * pas_entree - 1;
  }
  return (i);
}

/*--------------------------------------------------------------------------*/

/*
 int choix_liaisons_voisinage(liaison,neurone_deb,neurone_fin,x,y,taille_arrivee,pas_entree,gpe_entree)
 type_liaison *liaison;
 int neurone_deb;   numero du 1er neurone du groupe d'entree
 int neurone_fin;   numero du dernier neurone ....
 int x,y;           position du neurone
 int taille_arrivee;  taille de la matrice neurones sortie
 int pas_entree;     pour ne tenir compte que des macro neurones
 type_groupe *gpe_entree;
 */

int choix_liaisons_voisinage_core(type_liaison * liaison, int neurone_deb, /* numero du 1er neurone du groupe d'entree             */
int neurone_fin, /* numero du dernier neurone ....                       */
type_groupe * gpe, int x, int y, /* position du neurone                                  */
int pas_entree, /* pour ne tenir compte que des macro neurones          */
type_groupe * gpe_entree)
{
  int i, p, u, v;
  int nbre;
  float echx, echy;
  float dvx, dvy, dvx2, dvy2;
  int offset_x = 0;
  int offset_y = 0;

  int entree_taillex, entree_tailley; /* correspond a gpe_entree->taillex et gpe_entree->tailley */
  int taillex, tailley; /* correspond a gpe->taillex et gpe->tailley */
  float dv_x, dv_y, proba; /* correspond a liaison->dv_x, liaison->_y et liaison->proba */

  /* on convertis si necessaire les variables des groupes et des liaisons */

  entree_taillex = MY_Int2Int(gpe_entree->taillex);
  entree_tailley = MY_Int2Int(gpe_entree->tailley);

  taillex = MY_Int2Int(gpe->taillex);
  tailley = MY_Int2Int(gpe->tailley);

  dv_x = MY_Float2Float(liaison->dv_x);
  dv_y = MY_Float2Float(liaison->dv_y);
  proba = MY_Float2Float(liaison->proba);

  nbre = neurone_fin - neurone_deb + 1;
  echx = ((float) entree_taillex) / ((float) taillex);
  echy = ((float) entree_tailley) / ((float) tailley);
  if (taillex == 1)
  {
    x = (int) (((float) (x + 1)) / 2. * echx); /* offset_x=1; */
  }
  else x = (int) (((float) (x)) * echx);
  if (tailley == 1)
  {
    y = (int) (((float) (y + 1)) / 2. * echy); /*offset_y=1; */
  }
  else y = (int) (((float) (y)) * echy);
  /*dvx=dvy=liaison->dv; */
  dvx = dvx2 = dv_x;
  dvy = dvy2 = dv_y;

  if (dvx > 0. && dvx < echx) /* pour gerer les cas ou des liens ne seraient pas connectes */
  {
    /*        dvx=echx;
     printf("nouvelle taille du voisinage dvx = %f \n",dvx); */
  }
  if (dvy > 0. && dvy < echy)
  {
    /*     dvy=echy;
     printf("nouvelle taille du voisinage dvy = %f \n",dvy); */
  }

  if (dvx > (float) (entree_taillex / 2)) dvx = (float) (entree_taillex / 2);
  if (dvy > (float) (entree_tailley / 2)) dvy = (float) (entree_tailley / 2);

  if (dvx < 0. || dvy < 0.) /*liens de 1 vers tous sur une ligne ou une colonne */
  {
    if (dvx < 0.)
    {
      x = (float) (entree_taillex / 2);
      dvx = (float) (entree_taillex / 2);
      if (entree_taillex % 2 == 0) offset_x = -1.;
    }
    if (dvy < 0.)
    {
      y = (float) (entree_tailley / 2);
      dvy = (float) (entree_tailley / 2);
      if (entree_tailley % 2 == 0) offset_y = -1.;
    }
  }

  if (isequal(dvx, 0) && isequal(dvy, 0))
  {
    offset_x = 0.;
    offset_y = 0.;
  }

  i = 0;

  for (u = -(int) dvx - offset_x; u <= (int) dvx; u++)
    for (v = -(int) dvy - offset_y; v <= (int) dvy; v++)
    {
      p = -1;
      /*      printf("u = %d , v=%d \n",u,v); */
      if (x + u >= 0 && x + u < entree_taillex) p = x + u;
      else if (x + u < 0 && entree_taillex > 1) p = x + u + entree_taillex;
      else if (x + u >= entree_taillex && entree_taillex > 1) p = x + u - entree_taillex;

      if (v + y >= 0 && v + y < entree_tailley) p = p + (v + y) * entree_taillex;
      else if (v + y < 0 && entree_tailley > 1) p = p + (entree_tailley + v + y) * entree_taillex;
      else if (v + y >= entree_tailley && entree_tailley > 1) p = p + (v + y - entree_tailley) * entree_taillex;

      if (p >= 0 && p < nbre)
      {
        /* d=sqrt(((float)(u))*((float)(u))+((float)(v))*((float)(v))); */
        /* if(d<liaison->dv && ((float)rand()/(float)modulo)<=liaison->proba) */
        if (fabs(u) <= dvx && fabs(v) <= dvy && ((float) rand() / (float) modulo) <= proba)
        {
          liste[i] = p * pas_entree + pas_entree - 1 + neurone_deb;
          i++;
        }
      }
    }
  liste[i] = -1;
  return (i);
}

/* test que l'on a bien au moins une liaison si la proba est sup a 0 */
int choix_liaisons_voisinage(type_liaison * liaison, int neurone_deb, /* numero du 1er neurone du groupe d'entree             */
int neurone_fin, /* numero du dernier neurone ....                       */
type_groupe * gpe, int x, int y, /* position du neurone                                  */
int pas_entree, /* pour ne tenir compte que des macro neurones          */
type_groupe * gpe_entree)
{
  int nbre_liaisons;
  float proba;

  proba = MY_Float2Float(liaison->proba);

  nbre_liaisons = 0;
  while (nbre_liaisons <= 0 && proba > 0.)
    nbre_liaisons = choix_liaisons_voisinage_core(liaison, neurone_deb, neurone_fin, gpe, x, y, pas_entree, gpe_entree);
  return nbre_liaisons;
}

/*--------------------------------------------------------------------------*/

void conditionne_liaison(type_groupe * groupe, type_liaison * liaison, type_coeff * pt, int no_voie, int no_gpe_liaison)
{
  float norme;

  norme = MY_Float2Float(liaison->norme);

  pt->type = no_voie + liaison->mode;

  pt->gpe_liaison = no_gpe_liaison;
  pt->proba = 0.5;

  if (groupe->type == No_Ou)
  {
    pt->val = 1.;
    pt->evolution = 0;
  }
  else if (groupe->type == No_Et)
  {
    pt->val = 1. / ((float) groupe->nbre_voie);
    pt->evolution = 0;
  }
  else
  {
    pt->val = norme + 1e-20 + alea(1e-20);
    pt->evolution = sorte_liaison(liaison->type);
  }
}

/*-------------------------------------------------------------------------*/
/* liens entre le corps du macro neurone et ses micro neurones             */
/*-------------------------------------------------------------------------*/

void creer_micro_liens(int s_deb, int s_fin, int gpe, int nbre_voie, float val)
{
  int i, j, dx;
  int x, y;
  type_groupe *groupe;
  type_coeff *pt, *pt1;

  int taillex;
  float seuil;

  printf("creation des micro liens du groupe  %d \n", gpe);
  printf("%d voies de liaisons \n", nbre_voie);
  x = y = 0;
  groupe = trouver_groupe(gpe);

  taillex = MY_Int2Int(groupe->taillex);
  seuil = MY_Float2Float(groupe->seuil);

  dx = taillex;

  for (i = s_deb + nbre_voie; i <= s_fin; i += nbre_voie + 1)
  {
    pt = creer_coeff(); /* cree la premiere liaison du neurone */
    sc->neurone[i].coeff = pt;
    pt->entree = i - nbre_voie;
    /*  pt->temps=0;
     pt->stemps=0; */
    pt->evolution = 0 /*sorte_liaison(No_l_1_t) */;
    pt->type = 0; /* produit */
    pt->gpe_liaison = 0;
    pt->val = val;
    pt->proba = 1.;
    sc->neurone[i].seuil = seuil; /* au lieu de 0 modif du 21 janv 98 XXXXXXXXXXXXXXXXXXXXXXX */
    for (j = i - nbre_voie + 1; j < i; j++)
    {
      pt1 = pt;
      pt = creer_coeff();
      pt1->s = pt;
      pt->entree = j;
      /*    pt->temps=0;
       pt->stemps=0;  */
      pt->evolution = 0 /*sorte_liaison(No_l_1_t) */;
      pt->type = 0; /* produit */
      pt->gpe_liaison = 0;
      pt->val = val;
      pt->proba = 1.; /* les liens sont sur! */
      sc->neurone[i].posx = (float) x;
      sc->neurone[i].posy = (float) y;
      sc->neurone[i].posz = (float) gpe; /* les micro sont mis a la position du macro */
      /* pourrait etre facilement modifie pour plus de detail sur la position des micro neurones */
    }
    sc->neurone[i].posx = (float) x;
    sc->neurone[i].posy = (float) y;
    sc->neurone[i].posz = (float) gpe;
    x++;
    if (x >= dx)
    {
      x = 0;
      y++;
    };
  }
}

/* Mathias Quoy 01/03/2005 */
/* genere une variable aleatoire gaussienne */

float alea_gauss(float moyenne, float ecart_type)
{
  float Xt, r1, r2;

  r1 = (float) drand48();
  r2 = (float) drand48();

  Xt = (float) sqrt(-2.0 * ecart_type * ecart_type * log(r1)) * cos(2.0 * pi * r2) + moyenne;
  return (Xt);
}

/* Mathias Quoy 01/03/2005 */
/* initialisation aleatoire des poids suivant une loi normale de moyenne "J_barre"
 et d'ecart-type "J" */
/* Pour fonctionnement chaotique, J_barre=0 et J=1/sqrt(nb de neurones) */

void init_poids_alea_gauss(type_coeff * pt, float moyenne, float ecart_type)
{

  pt->val = alea_gauss(moyenne, ecart_type);

  /*printf("init poids gaussiens %f\n", pt->val);*/
}

void init_seuils_alea_gauss(int numero, float moyenne, float ecart_type)
{

  sc->neurone[numero].seuil = alea_gauss(moyenne, ecart_type);

  /* printf("init seuils gaussiens %f\n",sc->neurone[numero].seuil); */
}

/*----------------------------------------------------------------*/
void init_corps_neurone(type_groupe * groupe, int j)
{
  float seuil, tolerance;

  seuil = MY_Float2Float(groupe->seuil);
  tolerance = MY_Float2Float(groupe->tolerance);

  sc->neurone[j].seuil = seuil;
  sc->neurone[j].d = tolerance;

  if (groupe->type == No_Kohonen)
  {
    sc->neurone[j].seuil = 0.;
  } /* PG ce n'est pas tres propre utilser le groupe !!! */
  if (groupe->type == No_SAW)
  {
    sc->neurone[j].seuil = 0.;
  } /* PG ce n'est pas tres propre utilser le groupe !!! */
  if (groupe->type == No_Special)
  {
    sc->neurone[j].seuil = 0.;
  }
  if (groupe->type == No_Winner_Special)
  {
    sc->neurone[j].seuil = 0.;
  }

  if (groupe->type == No_Ou)
  {
    sc->neurone[j].seuil = 1. - 1.e-6;
  } /* PG ce n'est pas tres propre utilser le groupe !!! */
  if (groupe->type == No_Et)
  {
    sc->neurone[j].seuil = 0.99;
  }

  /* pas de liaison d'un neurone vers lui-meme */
  if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
  {
    init_seuils_alea_gauss(j, seuil, tolerance);
  }
}




void creer_liaison_a_b(donnees_script *sc, type_liaison * liaison, float val, int no_neurone_depart, int no_neurone_arrive, int no_voie)
{
  type_coeff *pt, *pt1;
  if (sc->neurone[no_neurone_arrive].coeff == NULL)
    {
    pt = creer_coeff();
    sc->neurone[no_neurone_arrive].coeff = pt;
    }
  else
  {
    pt1 = pointe_vers_dernier(no_neurone_arrive);
     pt = creer_coeff();
     pt1->s = pt;
  }
  if (liaison->type == No_l_1_patern_non_modif)
    {
      pt->type = no_voie + liaison->mode;
      pt->evolution = 0;
    }
  else if (liaison->type == No_l_1_patern_modif)
    {
      pt->type = no_voie + 1;
      pt->evolution = 1;
    }
  //printf("CREATION LIENS ENTRE NEURO no_neurone_depart = %d et no_neurone_arrive=%d\n",no_neurone_depart,no_neurone_arrive);
  pt->entree = no_neurone_depart;
  pt->val = val;
  pt->proba = 0.5;
}

/*--------------------------------------------------*/
/*                 liaisons 1 vers 1                */
/*--------------------------------------------------*/

/* pos1 numero du neurone dans le groupe d'entree a associer (doit etre global car son increment est global) */
/* le numero du neurone peut etre modifie par la fonction: cas moins de neurone en sortie qu'en entree */

void creer_liaison_1_vers_patern_neurone(donnees_script *sc, type_groupe * groupe, type_liaison * liaison, int no_premier_neurone, int pas, int pas_entree, int no_voie, int no_gpe_liaison, int s_deb, int s_fin, int e_deb, int e_fin, int *pos1) //argument à nettoyer, peu d'utile
{
#define BUFF_MAX 1024
  char nom_fichier[NAME_MAX];
  char path_fichier[PATH_MAX];
  char* buffer_matrice=NULL;
  char buffer_matrice2[BUFF_MAX];
  char buffer_cara[10];
  float norme,valeur;
  FILE* fichier = NULL;
  char cara, cara2;
  int no_neurone_vise_maintenant_x=0,no_neurone_vise_maintenant_y=0,no_neurone_vise_maintenant=0,stop=0;
  int no_neurone_origine_first, no_neurone_origine_last, centre_x_patern, centre_y_patern, no_neuro_vise, i;
  float posi_relative_entree_x=0,posi_relative_entree_y=0;
  int ligne = 1, ret2, emplacement, ancien_emplacement, nbre_coeff_x_traite, nbre_coeff_y_traite,no_neurone_vise_y=-1,no_neurone_vise_x=-1,decalagex=0,decalagey=0;
  int inverse=0;
  type_groupe * groupe_entree_actuel;
  type_groupe * groupe_actuel;

  type_groupe* groupe_entree=trouver_groupe(liaison->depart);


  norme = MY_Float2Float(liaison->norme);
  strcpy(path_fichier, sc->directory);

 // printf("%s\n",liaison->nom);
  if (sscanf(liaison->nom, "%[^'.'].patern", nom_fichier) != 1)
  {
    EXIT_ON_ERROR("Verifiez que le .patern est bien definis en nom du liens patern en entree du groupe %s", groupe->no_name);
  }
 // printf("%s\n",nom_fichier);
  strcat(nom_fichier, ".patern");
 // strcat(path_fichier, nom_fichier);

  //printf("%s\n",nom_fichier);
  fichier = fopen(nom_fichier, "r");
  if (fichier == NULL)
  {
    EXIT_ON_ERROR("Fichier fournis pour le groupe %s non-trouve", groupe->no_name);
  }

// rajouter ici init du groupe de sortie (groupe) init_corps_neurone(groupe, j);!

  groupe_entree_actuel=groupe_entree;
  groupe_actuel=groupe;

  while ((cara = fgetc(fichier)) != EOF)
  {
    inverse = 0;
    if(cara == 'i')
    {
      cara = fgetc(fichier); // pour avoir le ; suivant
      if (cara!=';') EXIT_ON_ERROR("format incorect, ; attendu après le i ligne %d", ligne);
      cara = fgetc(fichier);
    //  printf("carac = %c\n",cara);
      inverse = 1;
    }
    //printf("inverse = %d\n",inverse);
    if (inverse==1)
    {
      groupe_actuel=groupe_entree;
      groupe_entree_actuel=groupe;
    }
    else
    {
      groupe_entree_actuel=groupe_entree;
      groupe_actuel=groupe;
    }

    if (cara == '[')
    {
      ret2 = fscanf(fichier, "%d;%d];xc=%d;yc=%d", &no_neurone_origine_first, &no_neurone_origine_last, &centre_x_patern, &centre_y_patern);
      if (ret2 != 4) EXIT_ON_ERROR("caractere incorect dans le .patern ligne %d, error no 1", ligne);
      if(no_neurone_origine_first<0 || no_neurone_origine_last<0 || centre_x_patern<0 || centre_y_patern<0) EXIT_ON_ERROR("nombre negatif dans les no de neurones ligne %d , error no 1", ligne);
    }
    else if (sscanf(&cara, "%[0-9]", &cara2) == 1)
    {
      ungetc(cara, fichier);
      ret2 = fscanf(fichier, "%d;xc=%d;yc=%d", &no_neurone_origine_first, &centre_x_patern, &centre_y_patern);
      if (ret2 != 3) EXIT_ON_ERROR("caractere incorect dans le .patern ligne %d avec un ret2 de %d, error no 2", ligne,ret2);
      if(no_neurone_origine_first<0 || centre_x_patern<0 || centre_y_patern<0) EXIT_ON_ERROR("nombre negatif dans les no de neurones ligne %d , error no 2", ligne);
      no_neurone_origine_last = no_neurone_origine_first;
    }
    else
    {
      EXIT_ON_ERROR("caractere %c incorect dans le .patern , error no 3", cara);
    }

    cara2 = fgetc(fichier);
    if (cara2 == ';')
    {
      ret2 = fscanf(fichier, "v=%d\n", &no_neuro_vise);
      if (ret2 != 1) EXIT_ON_ERROR("caractere incorect dans le .patern ligne %d , error no 4", ligne);
      if(no_neuro_vise>(MY_Int2Int(groupe_actuel->taillex)*MY_Int2Int(groupe_actuel->tailley))) EXIT_ON_ERROR("caractere incorect dans le .patern ligne %d , error no 5", ligne);
      ligne++;
    }
    else if (cara2 == '\n')
    {
      no_neuro_vise = -1; //a calculer en dynamique proportionnel
      ligne++;
    }
    else
    {
      EXIT_ON_ERROR("fin de ligne dans l'une des matrices chargee du .patern non valide (ni un numero ni un \n) : ligne %d", ligne);
    }

    //printf("no neuro_vise lecture= %d\n",no_neuro_vise);
    if (no_neuro_vise!=-1)
    {
      no_neurone_vise_x=no_neuro_vise % MY_Int2Int(groupe_actuel->taillex);
      //printf("no_neurone_vise_x_init = %d\n",no_neurone_vise_x);
      no_neurone_vise_y=no_neuro_vise / MY_Int2Int(groupe_actuel->taillex);
      //printf("no_neurone_vise_y_init = %d\n",no_neurone_vise_y);
    }


    nbre_coeff_x_traite = 0;
    nbre_coeff_y_traite = 0;
    buffer_matrice = fgets(buffer_matrice2, 1024, fichier);

    while ((buffer_matrice != NULL))
    {
      nbre_coeff_x_traite=0;
      //printf("buffer matrice = %s\n",buffer_matrice);
      if (buffer_matrice[0] == ' ' || buffer_matrice[0] == '\n' || buffer_matrice[0] == EOF) break;

      ret2 = sscanf(buffer_matrice, "%s %n", buffer_cara, &emplacement);
      //printf("buffer cara_init = %s emplacement_init = %d\n",buffer_cara,emplacement);

      while (ret2 == 1)
      {
        if (buffer_cara[0] == '#')
        {
          nbre_coeff_x_traite++;
        }
        else
        {
          valeur = strtof(buffer_cara, NULL);
        //  printf("valeur = %f\n",valeur);
          valeur=valeur*norme;

        //  printf("nbre_coeff_x_traite = %d nbre_coeff_y_traite = %d\n",nbre_coeff_x_traite,nbre_coeff_y_traite);
        //  printf("valeur*norme = %f\n",valeur);

          decalagex=nbre_coeff_x_traite-centre_x_patern;
          decalagey=nbre_coeff_y_traite-centre_y_patern;
          //printf("decalagex = %d, centre_x_patern=%d decalagey = %d centre_y_patern=%d emplacement=%d\n",decalagex,centre_x_patern,decalagey,centre_y_patern,emplacement);

          nbre_coeff_x_traite++; // on le met ici et non à la fin de la boucle pour qu'en cas de break de la boucle ce soit bien pris en compte


          for(i=no_neurone_origine_first;i<=no_neurone_origine_last;i+=pas)
          {
            stop=0;
              if(no_neuro_vise==-1)
              {
                if(MY_Int2Int(groupe_entree_actuel->taillex)>1)
                posi_relative_entree_x=((float)(i%MY_Int2Int(groupe_entree_actuel->taillex)))/(float)((float)MY_Int2Int(groupe_entree_actuel->taillex)-1.0);
                else
                posi_relative_entree_x=0.0; // si le groupe est de taille 1 en x, on est forcement à 0.0 ou à 1.0 (indiferement) de la taille totale

                if(MY_Int2Int(groupe_entree_actuel->tailley)>1)
                posi_relative_entree_y=((float)(i/MY_Int2Int(groupe_entree_actuel->taillex)))/(float)((float)MY_Int2Int(groupe_entree_actuel->tailley)-1.0);
                else
                posi_relative_entree_y=0.0;

            //    printf("neuro vise = -1 : i = %d,MY_Int2Int(groupe_entree->taillex) = %d MY_Int2Int(groupe_entree->tailley)=%d\n",i,MY_Int2Int(groupe_entree_actuel->taillex),MY_Int2Int(groupe_entree_actuel->tailley));
                no_neurone_vise_x=(int)round(posi_relative_entree_x*((float)MY_Int2Int(groupe_actuel->taillex)-1.0));
                no_neurone_vise_y=(int)round(posi_relative_entree_y*((float)MY_Int2Int(groupe_actuel->tailley)-1.0));
              //  printf("neuro vise = -1 posi_relative_entree_x = %f,posi_relative_entree_y = %f no_neurone_vise_x=%d no_neurone_vise_y = %d\n",posi_relative_entree_x,posi_relative_entree_y,no_neurone_vise_x,no_neurone_vise_y);
              }

            if(no_neurone_vise_x<0) EXIT_ON_ERROR("no_neurone_vise_x < 0 dans leto_cle 2, probleme dans le link one to patern ligne %d du fichier .patern",ligne);
            if(no_neurone_vise_y<0) EXIT_ON_ERROR("no_neurone_vise_y < 0 dans leto_cle 2, probleme dans le link one to patern ligne %d du fichier .patern",ligne);

            no_neurone_vise_maintenant_x=no_neurone_vise_x+decalagex;
            no_neurone_vise_maintenant_y=no_neurone_vise_y+decalagey;

            //creation_lien depuis le neurone no_depart jusqu'au neurone
            if (no_neurone_vise_maintenant_x>=MY_Int2Int(groupe_actuel->taillex) || no_neurone_vise_maintenant_y >= MY_Int2Int(groupe_actuel->tailley)) stop=1;

            // printf("no_neurone_vise_maintenant_x avant break =%d no_neurone_vise_maintenant_y avant break = %d\n", no_neurone_vise_maintenant_x,no_neurone_vise_maintenant_y);
            if(no_neurone_vise_maintenant_x<0) stop=1; // on ne construit pas le liens si on déborde avec le patern matriciel (comme une conv non circulaire)
            if(no_neurone_vise_maintenant_y<0) stop=1; // on ne construit pas le liens si on déborde avec le patern matriciel (comme une conv non circulaire)

           // printf("no_neurone_vise_maintenant_x = %d no_neurone_vise_maintenant_y = %d\n",no_neurone_vise_maintenant_x,no_neurone_vise_maintenant_y);

            no_neurone_vise_maintenant=no_neurone_vise_maintenant_y*MY_Int2Int(groupe_actuel->taillex)+no_neurone_vise_maintenant_x;
            //printf("no_neurone_vise_maintenant = %d \n",no_neurone_vise_maintenant);
            if (stop!=1)
              {


          //    printf("inverse=%d valeur= %f no_neurone_vise_maintenant+e_deb=%d, i+s_deb=%d no_voie=%d\n",inverse,valeur,no_neurone_vise_maintenant+e_deb,i+s_deb,no_voie);

                if (inverse==0) creer_liaison_a_b(sc, liaison,valeur, i+e_deb, no_neurone_vise_maintenant+s_deb, no_voie);
                else creer_liaison_a_b(sc, liaison,valeur,no_neurone_vise_maintenant+e_deb, i+s_deb, no_voie);

              }
          }

        }
      //  printf("nbre_coeff_x_traite =%d\n\n\n",nbre_coeff_x_traite);
        if(nbre_coeff_x_traite>MY_Int2Int(groupe_actuel->taillex)) EXIT_ON_ERROR("L'une des matrice donnee n'est pas compatible avec la taille en x du groupe visee ligne %d\n", ligne);

        //nettoyage buffer_cara
        ancien_emplacement = emplacement;

        ret2 = sscanf(&(buffer_matrice[ancien_emplacement]), "%s %n", buffer_cara, &emplacement);
        emplacement+=ancien_emplacement;

       // printf("buffer cara2 = %s emplacement2 = %d\n",buffer_cara,emplacement);
      }
     // printf("sortie ! \n");

      nbre_coeff_y_traite++;
   //   printf("nbre_coeff_y_traite =%d\n\n\n\n",nbre_coeff_y_traite);
      if(nbre_coeff_y_traite>MY_Int2Int(groupe_actuel->tailley)) EXIT_ON_ERROR("L'une des matrice donnee n'est pas compatible avec la taille en y du groupe visee ligne %d\n", ligne);

      //nettoyage buffer_matrice
     // printf("segfault1 ? ! \n");
      buffer_matrice=NULL;
      buffer_matrice = fgets(buffer_matrice2, 1024, fichier);
     // printf("buffer matrice=%s bas buffer_matrice2= %s\n",buffer_matrice,buffer_matrice2);
    }

  }

  /*
   if (sc->neurone[j].coeff == NULL)
   {
   pt = creer_coeff();
   sc->neurone[j].coeff = pt;
   }
   else
   {
   pt1 = pointe_vers_dernier(j);
   pt = creer_coeff();
   pt1->s = pt;
   }

   pt->entree = *pos1;
   if (groupe->type == No_Ou)
   {
   pt->val = 1.;
   pt->evolution = 0;
   }
   else if (groupe->type == No_Et)
   {
   pt->val = 1. / ((float) groupe->nbre_voie);
   pt->evolution = 0;
   }
   else
   {
   if (liaison->type == No_l_1_1_non_modif) pt->type = no_voie + liaison->mode;
   else if (liaison->type == No_l_1_1_modif) pt->type = no_voie + 1;
   else pt->type = -1;
   pt->val = norme;
   pt->evolution = sorte_liaison(liaison->type);
   }

   pt->proba = 0.5;

   pt->gpe_liaison = no_gpe_liaison;
   *pos1 = *pos1 + pas_entree;

   if (j == s_fin && *pos1 <= e_fin) *no_neurone = s_deb - 1;
   else if (*pos1 > e_fin) *pos1 = e_deb + pas_entree - 1;
   */
}

/*--------------------------------------------------*/
/*             liaisons 1 vers tous                 */
/*--------------------------------------------------*/
/* pointeur sur le groupe du neurone a traiter */
/* j numero du neurone a traiter */
/* e_deb numero du premier neurone du groupe d'entree */
void creer_liaison_1_vers_tous_neurone(type_groupe * groupe, type_liaison * liaison, int j, int pas_entree, int no_voie, int no_gpe_liaison, int e_deb, int e_fin)
{
  type_coeff *pt, *pt1;
  int i;

  float proba, norme;
  float seuil, tolerance;

  seuil = MY_Float2Float(groupe->seuil);
  tolerance = MY_Float2Float(groupe->tolerance);

  proba = MY_Float2Float(liaison->proba);
  norme = MY_Float2Float(liaison->norme);

  if (sc->neurone[j].coeff == NULL)
  {
    pt = creer_coeff(); /* cree la premiere liaison du neurone */
    sc->neurone[j].coeff = pt;
    pt->entree = e_deb + pas_entree - 1;
    pt->evolution = sorte_liaison(liaison->type);

    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
    /* entree recurrente */
    pt->proba = proba; /*1./sqrt(nbre); */
    else pt->proba = 0.5;

    pt->type = no_voie + liaison->mode;
    pt->gpe_liaison = no_gpe_liaison;
    pt->val = norme + 0.0001 + alea(0.0001);

    if (groupe->type == No_Ou)
    {
      pt->val = 1.;
      pt->evolution = 0;
    }
    if (groupe->type == No_Et)
    {
      pt->val = 1. / ((float) groupe->nbre_voie);
      pt->evolution = 0;
    }

    /* pas de liaison d'un neurone vers lui-meme */
    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
    {
      init_seuils_alea_gauss(j, seuil, tolerance);
      if (j != (pt->entree - pas_entree + 1)) init_poids_alea_gauss(pt, norme, pt->proba);
      else pt->val = 0.;
    }

    if (groupe->type == No_PCR)
    {
      if (rand() > RAND_MAX / 2) pt->val = 1;
      else pt->val = 0;
    }
  } /* traitement des coefficients suivants */
  else
  {
    pt1 = pointe_vers_dernier(j);
    pt = creer_coeff();
    pt->evolution = sorte_liaison(liaison->type);

    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
    /* entree recurrente */
    pt->proba = proba; /*1./sqrt(nbre); */
    else pt->proba = 0.5;

    pt1->s = pt;
    pt->entree = e_deb + pas_entree - 1;
    pt->type = no_voie + liaison->mode;
    pt->gpe_liaison = no_gpe_liaison;
    pt->val = norme + 0.0001 + alea(0.0001);

    if (groupe->type == No_Ou)
    {
      pt->val = 1.;
      pt->evolution = 0;
    }

    if (groupe->type == No_Et)
    {
      pt->val = 1. / ((float) groupe->nbre_voie);
      pt->evolution = 0;
    }

    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
    {
      init_seuils_alea_gauss(j, seuil, tolerance);
      if (j != (pt->entree - pas_entree + 1)) init_poids_alea_gauss(pt, norme, pt->proba);
      else pt->val = 0.;
    }

    if (groupe->type == No_PCR)
    {
      if (rand() > RAND_MAX / 2) pt->val = 1;
      else pt->val = 0;
    }

  }

  for (i = e_deb + 2 * pas_entree - 1; i <= e_fin; i += pas_entree) /*autres liaisons      *//* 2* ??? */
  {
    pt1 = pt;
    pt = creer_coeff();
    pt->evolution = sorte_liaison(liaison->type);

    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu) pt->proba = proba; /* 1./sqrt(nbre); */
    else pt->proba = 0.5;

    pt1->s = pt;
    pt->entree = i; /* pointe vers entree couche -1  */
    pt->type = no_voie + liaison->mode;
    pt->gpe_liaison = no_gpe_liaison;
    pt->val = norme + 0.0001 + alea(0.0001);

    if (groupe->type == No_KO_Discret || groupe->type == No_KO_Continu)
    {
      init_seuils_alea_gauss(j, seuil, tolerance);
      if (j != (pt->entree - pas_entree + 1)) init_poids_alea_gauss(pt, norme, pt->proba);
      else pt->val = 0.;
    }

    if (groupe->type == No_PCR)
    {
      if (rand() > RAND_MAX / 2) pt->val = 1;
      else pt->val = 0;
    }
  }
}

/*--------------------------------------------------*/
/*                 liaisons 1 vers 1                */
/*--------------------------------------------------*/

/* pos1 numero du neurone dans le groupe d'entree a associer (doit etre global car son increment est global) */
/* le numero du neurone peut etre modifie par la fonction: cas moins de neurone en sortie qu'en entree */

void creer_liaison_1_vers_1_neurone(type_groupe * groupe, type_liaison * liaison, int *no_neurone, int pas_entree, int no_voie, int no_gpe_liaison, int s_deb, int s_fin, int e_deb, int e_fin, int *pos1)
{
  type_coeff *pt, *pt1;
  int j;

  float norme;

  norme = MY_Float2Float(liaison->norme);

  j = *no_neurone;
  if (sc->neurone[j].coeff == NULL)
  {
    pt = creer_coeff(); /* cree la premiere liaison du neurone */
    sc->neurone[j].coeff = pt;
  }
  else
  {
    pt1 = pointe_vers_dernier(j);
    pt = creer_coeff();
    pt1->s = pt;
  }

  pt->entree = *pos1;
  if (groupe->type == No_Ou)
  {
    pt->val = 1.;
    pt->evolution = 0;
  }
  else if (groupe->type == No_Et)
  {
    pt->val = 1. / ((float) groupe->nbre_voie);
    pt->evolution = 0;
  }
  else
  {
    if (liaison->type == No_l_1_1_non_modif) pt->type = no_voie + liaison->mode;
    else if (liaison->type == No_l_1_1_modif) pt->type = no_voie + 1;
    else pt->type = -1;
    pt->val = norme;
    pt->evolution = sorte_liaison(liaison->type);
  }

  pt->proba = 0.5;

  pt->gpe_liaison = no_gpe_liaison;
  *pos1 = *pos1 + pas_entree;
  /*  printf("- entree = %d sortie = %d , e_fin = %d s_fin = %d\n",*pos1,j,e_fin,s_fin); */

  if (j == s_fin && *pos1 <= e_fin) *no_neurone = s_deb - 1; /*  dangereux si pas proportionnel */
  else if (*pos1 > e_fin) *pos1 = e_deb + pas_entree - 1;
  /*  printf("+ entree = %d sortie = %d \n",*pos1,*no_neurone); */
}

/*---------------------------------------------------*/
/*            liasons 1 vers X au hasard             */
/*         ou liaisons vers un voisinage < dv        */
/*---------------------------------------------------*/

/* cree les liaison vers un voisinage pour le neurone j */
void creer_liaison_1_vers_voisinage_neurone(type_groupe * groupe, type_liaison * liaison, type_groupe * groupe_entree, int j, int x, int y, int pas_entree, int no_voie, int no_gpe_liaison, int e_deb, int e_fin)
{
  type_coeff *pt, *pt1;
  int i;
  int nbre_liaisons; /* lorsqu'il est lie au hasard = variable */

  nbre_liaisons = MY_Int2Int(liaison->nbre);

  if (liaison->type == No_l_1_a) nbre_liaisons = choix_liaisons_hasard(liaison, e_deb, e_fin, pas_entree);
  if (liaison->type == No_l_1_v || liaison->type == No_l_1_v_non_modif)
  {
    nbre_liaisons = choix_liaisons_voisinage(liaison, e_deb, e_fin, groupe, x, y, pas_entree, groupe_entree);
  }
  /*                 nbre_liaisons=choix_liaisons_voisinage(liaison,e_deb,e_fin,x,y,dx,pas_entree,groupe_entree);*/
  if (nbre_liaisons > 0)
  {
    if (sc->neurone[j].coeff == NULL)
    {
      pt = creer_coeff(); /* cree la premiere liaison du neurone */
      sc->neurone[j].coeff = pt;
      if (groupe->type == No_Ou)
      {
        pt->val = 1.;
        pt->evolution = 0;
      }
      else if (groupe->type == No_Et)
      {
        pt->val = 1. / ((float) groupe->nbre_voie);
        pt->evolution = 0;
      }
    }
    else
    {
      pt1 = pointe_vers_dernier(j);
      pt = creer_coeff();
      pt1->s = pt;
    }

    conditionne_liaison(groupe, liaison, pt, no_voie, no_gpe_liaison);
    pt->entree = liste[0];

    for (i = 1; i < nbre_liaisons; i++) /*autres liaisons      */
    {
      if (liste[i] == -1) break;
      pt1 = pt;
      pt = creer_coeff();
      pt->val = 1e-10;
      pt1->s = pt;
      conditionne_liaison(groupe, liaison, pt, no_voie, no_gpe_liaison);
      pt->entree = liste[i]; /* pointe vers entree couche -1  */
    }

  }
  else
  {
    printf("attention liaisons vers un voisinage non creees !!! \n");
  }
}

/*-------------------------------------------------------------------------*/
/* creation d'une voie de liaisons entre deux ensemble de neurones         */
/* si la liaison est non modifiable : le type est pair = Produit           */
/* sinon tous les autres types sont fixes par la nature des neurones       */
/* ou directement par l'utilisateur s'il s'agit du groupe winner_colonne   */
/* ou du groupe Kohonen_Gaussier                                           */
/* pas correspond a la taille en nbre de micro neurones d'un macro neurone */
/*-------------------------------------------------------------------------*/

void creer_liaisons_entre_groupe(donnees_script *sc, int e_deb, int e_fin, int s_deb, int s_fin, int gpe, int gpe_entree, type_liaison * liaison, int no_voie, int pas, int no_gpe_liaison)
{
  int x, y, z;
  int dx, dy; /* taille du groupe de sortie */
  int j;
  int pos1; /* position entree prise pour 1 vers 1       */
  int j_deb;

  type_groupe *groupe, *groupe_entree;
  int pas_entree, nbre = 0;
  int decalage;

  int taillex, tailley, entree_nbre, groupe_taillex, groupe_tailley;

  int cluster_manage = 0;
  int vrai_pas = 0;

  groupe_entree = trouver_groupe(gpe_entree);

  /* conversion si necessaire */
  taillex = MY_Int2Int(groupe_entree->taillex);
  tailley = MY_Int2Int(groupe_entree->tailley);
  entree_nbre = MY_Int2Int(groupe_entree->nbre);

  nbre = taillex * tailley;
  if (nbre == 0)
  {
    fprintf(stdout, "ERROR : creer_liaisons_entre_groupe : nombre de neurones = 0 !\n");
    return;
  }
  pas_entree = entree_nbre / nbre;

  no_voie = no_voie * 2; /* pair   = produit     */
  /* impair = distance    */
  groupe = trouver_groupe(gpe);

  groupe_taillex = MY_Int2Int(groupe->taillex);
  groupe_tailley = MY_Int2Int(groupe->tailley);

  dx = groupe_taillex;
  dy = groupe_tailley;

  pos1 = e_deb + pas_entree - 1;

  if (groupe->type == No_Winner_Colonne || groupe->type == No_PTM || groupe->type == No_Winner_Macro || groupe->type == No_PLG || groupe->type == No_But || groupe->type == No_Pyramidal || groupe->type == No_Pyramidal_plan || groupe->type == No_Sigma_PI
      || groupe->type == No_Macro_Colonne || groupe->type == No_KO_Discret || groupe->type == No_KO_Continu || groupe->type == No_Selverston || groupe->type == No_selective_winner_modulated) decalage = no_voie / 2;
  else decalage = 0;

  /** neuromod -> pas de micro neurone */
  if (liaison->mode >= NEUROMOD) decalage = 0;

  if (pas < 0)
  {
    cluster_manage = 1;
    pas = 1;
    vrai_pas = MY_Int2Int(groupe->nbre) / (dx * dy);
  }

  x = y = z = 0;

  j_deb = s_deb + decalage;
//  if (liaison->type == No_l_1_patern) creer_liaison_1_vers_patern_neurone(sc,groupe, liaison, j_deb, pas, s_fin, pas_entree, no_voie, no_gpe_liaison, e_deb, e_fin);

  for (j = s_deb + decalage; j <= s_fin; j += pas)
  {
    init_corps_neurone(groupe, j);

    /*       printf("j: %d // %d \n",j-s_deb,pos1-e_deb); */

    if (liaison->type == No_l_1_t) creer_liaison_1_vers_tous_neurone(groupe, liaison, j, pas_entree, no_voie, no_gpe_liaison, e_deb, e_fin);

    if (liaison->type == No_l_1_1_modif || liaison->type == No_l_1_1_non_modif || liaison->type == No_l_1_1_non_modif_bloqueur)
    {
      if (cluster_manage)
      {
        if ((j - s_deb) % vrai_pas == vrai_pas - 1)
        {
          /* 	    printf("modulo %d\n", j-s_deb+vrai_pas-1); */
          /** macro - dernier du cluster - on incremente */
          creer_liaison_1_vers_1_neurone(groupe, liaison, &j, pas_entree, no_voie, no_gpe_liaison, s_deb, s_fin, e_deb, e_fin, &pos1);
        }
        else
        /** micro pas d'increment (pas_entree = 0) */
        creer_liaison_1_vers_1_neurone(groupe, liaison, &j, 0, no_voie, no_gpe_liaison, s_deb, s_fin, e_deb, e_fin, &pos1);
      }
      else creer_liaison_1_vers_1_neurone(groupe, liaison, &j, pas_entree, no_voie, no_gpe_liaison, s_deb, s_fin, e_deb, e_fin, &pos1);
    }

    else if (liaison->type == No_l_1_a || liaison->type == No_l_1_v || liaison->type == No_l_1_v_non_modif) creer_liaison_1_vers_voisinage_neurone(groupe, liaison, groupe_entree, j, x, y, pas_entree, no_voie, no_gpe_liaison, e_deb, e_fin);
    x++;
    if (x >= dx)
    {
      x = 0;
      y++;
    }
  }

  if (liaison->type == No_l_1_patern_modif || liaison->type == No_l_1_patern_non_modif)
    {
    creer_liaison_1_vers_patern_neurone(sc,groupe,liaison,j_deb,pas,pas_entree,no_voie, no_gpe_liaison,s_deb,s_fin, e_deb, e_fin,&pos1);
    }
}

