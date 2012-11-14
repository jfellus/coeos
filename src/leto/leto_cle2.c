#include <stdio.h>
#include <math.h>

/*#define DEBUG*/

#include "public_leto.h"

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
    printf("\n Erreur lors de l'ecriture du fichier contenant le reseau \n");
    exit(1);
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
    while (pt != nil) /* compte le nombre d'entrees      */
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
      while (pt != nil) /* ecrit tous les coefficients     */
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

  debug_printf("Write .res to disk \n");

  fprintf(f1, "Reseau de neurone\n");
  fprintf(f1, "Copyright Philippe GAUSSIER oct 1991\n");

  fprintf(f1, "Version %i\n\n", RESEAU_VERSION);
  debug_printf("Version %i\n\n", RESEAU_VERSION);

  fprintf(f1, "script associe : %s\n", sc->nomfich1);
  debug_printf("script associe : %s\n", sc->nomfich1);

  fprintf(f1, "%i\n", sc->nbre_couche);
  debug_printf("nbre_couche = %i\n", sc->nbre_couche);

  fprintf(f1, "%i\n", sc->nbre_groupe);
  debug_printf("nbre_groupe = %i\n", sc->nbre_groupe);

  fprintf(f1, "%i\n", sc->nbre_neurone);
  debug_printf("nbre_neurone = %i\n", sc->nbre_neurone);

  for (i = 0; i < sc->nbre_couche; i++)
  {
    fprintf(f1, "%i\n", sc->t[i]);
    debug_printf("couche %i, val = %i\n", i, sc->t[i]);
  }

  fwrite(sc->neurone, sizeof(type_neurone), sc->nbre_neurone, f1);
  debug_printf("neurons written to res file\n");

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
    while (pt != nil) /* ecrit tous les coefficients     */
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

      /*debug_printf("ecrit_reseau: neuron %i, coeff %p => (val = %f, entree = %i, type = %i, evolution = %i, proba = %f, gpe_liaison = %i, Nbre_E = %i, Nbre_S = %i, Nbre ES = %i\n", i, (void *) pt, pt->val, pt->entree, pt->type, pt->evolution, pt->proba, pt->gpe_liaison, pt->Nbre_E, pt->Nbre_S, pt->Nbre_ES);*/

      pt = pt->s; /* inscrit coeff suivant           */

      if (pt == nil)
      {
       fwrite(&statut_fin, taille_char, 1, f1);
      }
      else
      {
        fwrite(&statut_continuer, taille_char, 1, f1);
      }
    }
  }
  debug_printf("coeff written to res file\n");

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
    debug_printf("liaison de 1 vers tous sur une colonne ou une ligne \n");
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
    pt->val = norme + 0.001 + alea(0.001);

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
    pt->val = norme + 0.001 + alea(0.001);

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
    pt->val = norme + 0.001 + alea(0.001);

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

void creer_liaisons_entre_groupe(int e_deb, int e_fin, int s_deb, int s_fin, int gpe, int gpe_entree, type_liaison * liaison, int no_voie, int pas, int no_gpe_liaison)
{
  int x, y, z;
  int dx, dy; /* taille du groupe de sortie */
  int j;
  int pos1; /* position entree prise pour 1 vers 1       */

  type_groupe *groupe, *groupe_entree;
  int pas_entree, nbre = 0;
  int decalage;

  int taillex, tailley, entree_nbre, groupe_taillex, groupe_tailley;

  int cluster_manage = 0;
  int vrai_pas = 0;

  debug_printf("creation des liaisons\n");
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
  debug_printf("pas pour trouver les macro neurones du gpe d'entree = %d \n", pas_entree);

  no_voie = no_voie * 2; /* pair   = produit     */
  /* impair = distance    */
  groupe = trouver_groupe(gpe);

  groupe_taillex = MY_Int2Int(groupe->taillex);
  groupe_tailley = MY_Int2Int(groupe->tailley);

  dx = groupe_taillex;
  dy = groupe_tailley;

  debug_printf("taille du groupe de sortie en x : %d , en y : %d \n", dx, dy);

  pos1 = e_deb + pas_entree - 1;

  if (groupe->type == No_Winner_Colonne || groupe->type == No_PTM || groupe->type == No_Winner_Macro || groupe->type == No_PLG || groupe->type == No_But || groupe->type == No_Pyramidal || groupe->type == No_Pyramidal_plan || groupe->type == No_Sigma_PI
      || groupe->type == No_Macro_Colonne || groupe->type == No_KO_Discret || groupe->type == No_KO_Continu || groupe->type == No_Selverston) decalage = no_voie / 2;
  else decalage = 0;

  /** neuromod -> pas de micro neurone */
  if (liaison->mode >= NEUROMOD) decalage = 0;

  if (pas > 1) debug_printf("creation d'un lien pour une categorie de micro neurones %d\n", decalage);

  if (pas < 0)
  {
    cluster_manage = 1;
    pas = 1;
    vrai_pas = MY_Int2Int(groupe->nbre) / (dx * dy);
  }

  x = y = z = 0;
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
  debug_printf("fin creation liaison\n");
}

