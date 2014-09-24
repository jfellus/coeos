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
/*---------------------------------------------------------------*/
/*                   GESTION DES SCRIPTS                         */
/* 1991 version de base                                          */
/* sept 2004 fin ajout gestion commentaires                      */
/*---------------------------------------------------------------*/

#define SYMBOLIQUE_EXTENSION ".symb"

/*#define DEBUG*/
#include "gere_coudes.h"
#include "draw_leto.h"
#include <search.h>  /* pour la table de hachage */
#include "script.h"
#include "outils.h"


/* si selection=1 on se limite aux groupes dans le rectangle */
int script_get_groups_nb(donnees_script *script)
{
  int groups_nb = 0;
  type_groupe *group;

  group = script->deb_groupe;
  while (group != NULL)
  {
    if (group->deja_active == 0) /* why  ?? AB*/
    {
      groups_nb++; /*  groupe en direct de ce script, non un groupe insere */
    }
    group = group->s;
  }
  return groups_nb;
}

void script_backup(donnees_script *script)
{
  char filename[PATH_MAX];
  FILE *file;
  int groups_nb, links_nb;
  type_groupe *group;
  type_liaison * link;

  groups_nb = script_get_groups_nb(script);
  links_nb = 0;
  for (link = script->deb_liaison; link != NULL; link = link->s)
  {
    links_nb++;
  };

  sprintf(filename, "%s.bak%i", script->nomfich1, script->backup_id);
  script->backup_id++;

  prepare_affichage(script);
  file = fopen(filename, "w");
  save_comment(file, script->first_comment_group);
  fprintf(file, "nombre de groupes = %d\n", groups_nb);
  for (group = script->deb_groupe; group != NULL; group = group->s)
  {
    write_one_group(file, group, 1);
  }
  save_comment(file, script->first_comment_link);
  fprintf(file, "nombre de liaisons = %d\n", links_nb);
  for (link = script->deb_liaison; link != NULL; link = link->s)
  {
    write_one_link(file, link, 1);
  }
  fclose(file);
}

#ifndef AVEUGLE
int script_set_modified(donnees_script *script)
{
  int has_changed;

  has_changed = !script->modified;
  script->modified = 1;
  script_backup(script);
  return has_changed;
}
#endif

void script_free_groups(donnees_script *script)
{
  type_groupe *old_groupe, *groupe;
  groupe = script->deb_groupe;

  while (groupe != NULL)
  {
    old_groupe = groupe;
    groupe = groupe->s;
    free(old_groupe);
  }
  script->deb_groupe = NULL;
  script->fin_groupe = NULL;
  script->last_groupe_number = 0;
  script->nbre_groupe = 0;
}

void script_free_links(donnees_script *script)
{
  type_liaison *link_old, *link;

  link = script->deb_liaison;

  while (link != NULL)
  {
    link_old = link;
    link = link->s;
    free(link_old);
  }

  script->deb_liaison = NULL;
  script->fin_liaison = NULL;
  script->nbre_liaison = 0;

}

void script_free_group_comments(donnees_script *script)
{
  type_noeud_comment *comment_old, *comments;

  comments = script->first_comment_group;
  while (comments != NULL)
  {
    comment_old = comments;
    comments = comments->suiv;
    free(comment_old);
  }
  comments = script->first_comment_group = NULL;

}

void script_free_link_comments(donnees_script *script)
{
  type_noeud_comment *comment_old, *comments;

  comments = script->first_comment_link;
  while (comments != NULL)
  {
    comment_old = comments;
    comments = comments->suiv;
    free(comment_old);
  }
  script->first_comment_link = NULL;
}

/* variables globales pour la gestion des commentaires non attaches a un groupe */
/* Il s'agit des commentaires precedent le debut des groupes et des commentaires */
/* precedent le debut des liens (si c'est possible! / correspond fin d'un groupe non?) */

type_liaison *script_liaison_scan_arrivee(type_liaison *liaison, int arrivee)
{
  if (liaison == NULL)
  {
    EXIT_ON_ERROR("(%s) : invalid liaison pointer\n", __FUNCTION__);
  }

  if (arrivee < -1)
  {
    EXIT_ON_ERROR("(%s) : invalid depart value\n", __FUNCTION__);
  }

  while (liaison->arrivee != arrivee && liaison != NULL)
  {
    liaison = liaison->s;
  }
  return liaison;
}

/* renvoie la premiere liaison ayant le groupe depart en entree */
type_liaison *script_liaison_scan_depart(type_liaison *liaison, int depart)
{
  if (liaison == NULL)
  {
    EXIT_ON_ERROR( "(%s) : invalid liaison pointer\n", __FUNCTION__);
  }

  if (depart < -1)
  {
    EXIT_ON_ERROR("(%s) : invalid depart value\n", __FUNCTION__);
  }

  while (liaison->depart != depart && liaison != NULL)
  {
    liaison = liaison->s;
  }

  return liaison;
}

/* lecture du fichier xml: renvoie -1 si pb, 1 si aucun echec */
int script_load_file_xml(char *path)
{
  mxml_node_t *tree, *mxml_node = NULL;

  tree = loadFile(path); /* voir basic_parser_5/src/libparserxml.c */
  if (tree == NULL)
  {
    fprintf(stderr, "(%s) : loading liaison tree failed\n", __FUNCTION__);
    return 1;
  }

  mxml_node = mxmlFindElement(tree, tree, "liaison_list", NULL, NULL, MXML_DESCEND_FIRST); /*entete xml */
  if (mxml_node == NULL)
  {
    fprintf(stderr, "noeud xml liaison_list  non trouve\n");
    return -1;
  }

  getLiaisons(sc->deb_liaison, mxml_node); /* dans basic_parser libparserxml.c */

  return 0;
}

/* si selection=1 on se limite aux groupes dans le rectangle */
int compte_nbre_groupe_propre_au_script(int selection)
{
  int nbre_groupe_propre_au_script = 0;
  type_groupe *groupe;
#ifndef AVEUGLE
  selected_group *sel_group = NULL;

  if (selection == 1)
  {
    for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
    {
      if (sel_group->group->deja_active == 0)
      {
        nbre_groupe_propre_au_script++; /*  groupe en direct de ce script, non un groupe insere */
      }
    }
    return nbre_groupe_propre_au_script;
  }
#endif

  (void) selection;

  groupe = sc->deb_groupe;
  while (groupe != NULL)
  {
    if (groupe->deja_active == 0)
    {
      nbre_groupe_propre_au_script++; /*  groupe en direct de ce script, non un groupe insere */
    }
    groupe = groupe->s;
  }
  return nbre_groupe_propre_au_script;
}

/* si selection=1 on se limite aux groupes dans le rectangle */
int compte_nbre_liaison_propre_au_script(int selection, TxDonneesFenetre *onglet_leto)
{
  int nbre_liaison_propre_au_script = 0;
  type_liaison * liaison;
#ifndef AVEUGLE
  if (selection == 1 && sc->groupes_courants == NULL) return 0;
#endif

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
    if (liaison->deja_active == 0 && (selection == 0 || selected_link(liaison, onglet_leto)))
    {
      nbre_liaison_propre_au_script++;
    }
    liaison = liaison->s;
  }
  return nbre_liaison_propre_au_script;
}

void script_load(donnees_script *script, char *filename, int recursive_load, struct hsearch_data *hash_table)
{
  FILE *file;
  int groups_nb, links_nb;
  int i, reverse, selected_plane;
  type_groupe *group;
  type_liaison *group_link;
  static char ligne[TAILLE_CHAINE];

  file = fopen(filename, "r");
  if (file == NULL) EXIT_ON_ERROR("Unable to open file %s", filename);

  script_free_groups(script);
  script_free_links(script);
  script_free_group_comments(script);
  script_free_link_comments(script);


  remove_hash_table(hash_table);
  init_hash_table(hash_table);

  script->first_comment_group = (type_noeud_comment *) read_line_with_comment(file, NULL, ligne);

  sscanf(ligne, "nombre de groupes = %d\n", &script->nbre_groupe);
  if (script->first_comment_group != NULL && strstr(script->first_comment_group->chaine, "!ignore_include") != NULL)
  {
    recursive_load = 0;
  }

  for (i = 0; i < script->nbre_groupe; i++)
  {
    if (i != 0) script->fin_groupe = group = creer_groupeb(script->fin_groupe);
    else group = script->deb_groupe = script->fin_groupe = creer_groupeb(NULL);

    script->nbre_neurone += read_one_group_leto(file, group, NULL, hash_table);

    /* gestion de la lecture d'un sous reseau */
    if (group->type == No_Sub_Network && recursive_load == 1)
    {
      /* le nbre de groupes a ete modifie dans read_macro */
      i = i + read_macro(group->no_name, group->nom, group->posx, group->posy, 1, 1, &selected_plane, hash_table);
      if (group->reverse > 0) reverse = 1;
      else reverse = -1;
      group->reverse = reverse * selected_plane; /* plan >100 pour tout bouger ensemble */
    }

    group->deja_active = 0; /* c'est un groupe en direct de ce script, non un groupe insere */
  }
  script->nbre_groupes_lus = groups_nb = script->nbre_groupe;
  script->first_comment_link = (type_noeud_comment *) read_line_with_comment(file, NULL, ligne);

  sscanf(ligne, "nombre de liaisons = %d\n", &links_nb);
  group_link = NULL;
  for (i = 0; i < links_nb; i++)
  {
    if (script->deb_liaison == NULL)
    {
      group_link = script->deb_liaison = script->fin_liaison = creer_liaison(NULL);
    }
    else group_link = creer_liaison(group_link);

    read_one_link(file, group_link, NULL, hash_table);

    group_link->deja_active = 0; /* liaison propre au script */
  }
  script->nbre_liaison = script->nbre_liaison + links_nb; /* tenir compte des scripts inclus */
  script->fin_liaison = group_link;
  fclose(file);

  initialise_liaisons(script);
}

/* read the input script */
/* la lecture suppose que rien n'a ete charge avant. 
 Si l'on relit un script les anciens groupes ne sont pour l'instant pas efface mais perdus! */
/* si recursive_load=1  les sous reseaux sont lus et rajoutes, sinon pas de lecture */

void lecture(int recursive_load, TxDonneesFenetre *onglet_leto)
{
  int i;
  type_groupe *groupe;
  type_liaison * liaison;
  FILE *f1;
  int nbre_liaison_provisoire;
  int reverse, selected_plane;
  char *pt;

  static char ligne[TAILLE_CHAINE];

#ifndef AVEUGLE
  int j;
  remove_hash_table(onglet_leto->hashtab);
  init_hash_table(onglet_leto->hashtab);
#else
  hdestroy(); /* on detruit la table precedente si elle existait */
  hcreate(10000); /* table de hachage pour les no de groupes symboliques */
#endif
  nbre_groupes_lus = 0; /* on initialise le nombre de groupes lus */

  /* on place le flag_symb a 1 si le fichier est un .symb
   * (en esperant que l'utilisateur ne met pas de variable dans un .script !) */
  if ((pt = strstr(sc->nomfich1, SYMBOLIQUE_EXTENSION)) != NULL) sc->flag_symb = 1;
  else sc->flag_symb = 0;

  f1 = fopen(sc->nomfich1, "r");

  if (f1 == NULL)
  {
    printf("Cannot open file %s\n", sc->nomfich1);
    printf("We suppose it is a new script file \n");
    return;
  }

  liaison = sc->deb_liaison = sc->fin_liaison = NULL;
  groupe = sc->deb_groupe = sc->fin_groupe = NULL;

  sc->nbre_neurone = 0;
  sc->nbre_liaison = 0;
  sc->last_groupe_number = 0;
  sc->first_comment_group = (type_noeud_comment *) read_line_with_comment(f1, NULL, ligne);

  sscanf(ligne, "nombre de groupes = %d\n", &sc->nbre_groupe);
  if (sc->first_comment_group != NULL && strstr(sc->first_comment_group->chaine, "!ignore_include") != NULL)
  {

    if (recursive_load != 0)
    {
      recursive_load = 0;
    }
  }

  for (i = 0; i < sc->nbre_groupe; i++)
  {
    if (i != 0) sc->fin_groupe = groupe = creer_groupeb(sc->fin_groupe);
    else groupe = sc->deb_groupe = sc->fin_groupe = creer_groupeb(NULL);

    sc->nbre_neurone += read_one_group_leto(f1, groupe, NULL, onglet_leto->hashtab);

    /* gestion de la lecture d'un sous reseau */
    if (groupe->type == No_Sub_Network && recursive_load == 1)
    {
      /* le nbre de groupes a ete modifie dans read_macro */
      i = i + read_macro(groupe->no_name, groupe->nom, groupe->posx, groupe->posy, 1, 1, &selected_plane, onglet_leto->hashtab);
      if (groupe->reverse > 0) reverse = 1;
      else reverse = -1;
      groupe->reverse = reverse * selected_plane; /* plan >100 pour tout bouger ensemble */
    }

    groupe->deja_active = 0; /* c'est un groupe en direct de ce script, non un groupe insere */
  }
  sc->nbre_groupes_lus = nbre_groupes_lus = sc->nbre_groupe;


  sc->first_comment_link = (type_noeud_comment *) read_line_with_comment(f1, NULL, ligne);

  sscanf(ligne, "nombre de liaisons = %d\n", &nbre_liaison_provisoire);
  liaison = sc->fin_liaison;

  for (i = 0; i < nbre_liaison_provisoire; i++)
  {
    if (sc->deb_liaison == NULL)
    {
      liaison = sc->deb_liaison = sc->fin_liaison = (type_liaison *) creer_liaison(NULL);
    }
    else liaison = (type_liaison *) creer_liaison(liaison);

    if (onglet_leto == NULL) read_one_link(f1, liaison, NULL, NULL);
    else read_one_link(f1, liaison, NULL, onglet_leto->hashtab);

    liaison->deja_active = 0; /* liaison propre au script */
  }

  sc->nbre_liaison = sc->nbre_liaison + nbre_liaison_provisoire; /* tenir compte des scripts inclus */
  sc->fin_liaison = liaison;
  fclose(f1);

  initialise_liaisons();

#ifndef AVEUGLE
  j = script_load_file_xml(sc->draw);
  if (j == 1)
  {
    fprintf(stderr, "pb chargement fichier xml \n");
  }
  else if (j == -1)
  {
    fprintf(stderr, "pb chargement fichier draw: no liaisons found\n");
  }

  if (sc->premiere_lecture == 0 && onglet_leto->window != NULL) regenerer_test(onglet_leto);
  script_backup(sc);
#endif
  sc->premiere_lecture = 0;
}

/* insert a script or a macro, return 0 if nothing can be read */
/* return n (number of groups) if the script can be read  */
/* px et py correspondent au decalage des coordonnees des groupes du sous reseau */

/* mode = 0 : paste direct dans le script , mode = 1 inclusion macro */
/* groupe->deja_active=mode si mode = 1 pas d'affichage ni de sauvegarde du groupe */

/* il faut se mettre sur un plan jamais utilise */

/*
 %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 pb lors de la copie, le prog oublie de copier le groupe en creant le sub network...
 probleme de denomination (nom1_nom2_nom3...)
 Il faudrait passer un nom contextuel en plus pour la lecture des macros.
 la lecture des groupes semble etre moins riche que la lecture des liens.
 */

int read_macro(char *base_base, char *nom, int px, int py, int relatif, int mode_inclusion, int *selected_plane, struct hsearch_data *hash_table)
{
  int i;
  int nbre_groupe_macro, nbre_gpes_inclus, nbre_liaison_macro;
  static char ligne[TAILLE_CHAINE];
  char base_nom[SIZE_NO_NAME];
  char base_nom_complet[SIZE_NO_NAME];

  type_groupe *groupe_local, *debut_macro;
  type_liaison * liaison;
  FILE *f1;
  char macro_script_path[PATH_MAX];
  int reverse;
  int mx, my;
  float zoom;
  int no_macro_used;
  int clipboard = 0;

  nbre_groupe_macro = 0;

  memcpy(base_nom, nom, (strlen(nom) + 1) * sizeof(char));
  get_base_name(base_nom);

  if (strcmp(base_nom, "copy_buffer") == 0)
  {
    clipboard = 1;
    snprintf(macro_script_path, PATH_MAX, "%s", nom);
  }
  else
  {
    snprintf(macro_script_path, PATH_MAX, "%s/%s", sc->directory, nom);
  }

  f1 = fopen(macro_script_path, "r");

  if (f1 == NULL) PRINT_WARNING("\n Error while opening the macro script file %s \n", macro_script_path);

  no_macro_used = sc->nbre_macro_lues;
  *selected_plane = (100 + sc->nbre_macro_lues * 7);

  if (1/*mode_inclusion==0*/)
  {
    while (plane_used(*selected_plane) == 1)
    {
      no_macro_used++;
      (*selected_plane) = (100 + no_macro_used * 7);
    }
  }

  if (clipboard)
  {
    memcpy(base_nom, "c", (strlen("c") + 1) * sizeof(char));
  }

  if (base_base == NULL) sprintf(base_nom_complet, "%s[%d]", base_nom, no_macro_used);
  else memcpy(base_nom_complet, base_base, (strlen(base_base) + 1) * sizeof(char));
  groupe_local = sc->fin_groupe;
  sc->first_comment_group = (type_noeud_comment *) read_line_with_comment(f1, sc->first_comment_group, ligne);
  sscanf(ligne, "nombre de groupes = %d\n", &nbre_groupe_macro);
  if (nbre_groupe_macro == 0)
  {
    printf("ATTENTION no group in this script: ABORT reading !!!\n");
    return 0; /* aucun groupe inclus*/
  }

  mx = my = 0; /* centre de gravite des groupes lus */
  debut_macro = NULL;
  nbre_gpes_inclus = 0;
  for (i = 0; i < nbre_groupe_macro; i++)
  {
    groupe_local = (type_groupe *) creer_groupeb(groupe_local);
    if (sc->deb_groupe == NULL) sc->deb_groupe = groupe_local; /* c'est le 1er groupe lu */

    sc->nbre_neurone += read_one_group_leto(f1, groupe_local, base_nom_complet, hash_table);

    if (debut_macro == NULL) debut_macro = groupe_local;
    sc->fin_groupe = groupe_local;

    /* gestion de la lecture d'un sous reseau, PB ICI ............. */
    if (groupe_local->type == No_Sub_Network)
    {
      if (groupe_local->reverse > 0) reverse = 1;
      else reverse = -1;
      groupe_local->reverse = reverse * (100 + sc->nbre_macro_lues * 7); /* plan >100 pour tout bouger ensemble */
      nbre_gpes_inclus = nbre_gpes_inclus + read_macro(groupe_local->no_name, groupe_local->nom, groupe_local->posx, groupe_local->posy, 1, 1, selected_plane, hash_table);
    }

    groupe_local->deja_active = mode_inclusion; /* c'est un groupe insere (a ne pas resauvegarder) */
    mx = mx + groupe_local->posx;
    my = my + groupe_local->posy;
    if (relatif == 0)
    {
      groupe_local->posx = groupe_local->posx + 50;
      groupe_local->posy = groupe_local->posy + 50;
    }
    groupe_local->p_posx += 50;
    groupe_local->p_posy += 50;
    if (groupe_local->reverse > 0) reverse = 1;
    else reverse = -1;
    groupe_local->reverse = reverse * (*selected_plane); /* plan >100 pour tout bouger ensemble */

  }
  /*sc->fin_groupe = groupe_local;*/

  if (relatif == 1)
  {
    if (mode_inclusion == 1) zoom = 0.1;
    else zoom = 0.75;
    mx = mx / nbre_groupe_macro;
    my = my / nbre_groupe_macro;
    groupe_local = debut_macro;
    for (i = 0; i < nbre_groupe_macro; i++) /* tres laid PG !!! */
    {
      groupe_local->posx = 2 + px + (int) (((float) (groupe_local->posx - mx)) * zoom);
      groupe_local->posy = 2 + py + (int) (((float) (groupe_local->posy - my)) * zoom);
      groupe_local = groupe_local->s;
    }
  }

  sc->first_comment_link = (type_noeud_comment *) read_line_with_comment(f1, sc->first_comment_link, ligne);
  sscanf(ligne, "nombre de liaisons = %d\n", &nbre_liaison_macro);


  liaison = sc->fin_liaison;
  for (i = sc->nbre_liaison; i < sc->nbre_liaison + nbre_liaison_macro; i++)
  {
    if (sc->deb_liaison == NULL)
    {
      liaison = sc->deb_liaison = sc->fin_liaison = (type_liaison *) creer_liaison(NULL);
    }
    else liaison = creer_liaison(liaison);

    read_one_link(f1, liaison, base_nom_complet, hash_table);

    liaison->deja_active = mode_inclusion;
    /* liaison->depart = liaison->depart + sc->nbre_groupe;  *//* offset to change the connected groups (inutile maintenant)*/
    /* liaison->arrivee = liaison->arrivee + sc->nbre_groupe; *//* c'est le nom compose qui evite le pb */
#ifndef AVEUGLE
    initialise_coudes_liaison(liaison); /*initialise les coudes de la liaison - par defaut on ne reprend pas le graphisme */
#endif
  }
  sc->fin_liaison = liaison;

  /* cette solution n'est valable que si le lien micro macro est toujours cree en premier: faux en ce moment ex: kohonen
   ce lien n'existe pas */
  /*  detruit_liaison(liaison1); *//* only a single link between micro and macro neuron */

  sc->nbre_liaison = sc->nbre_liaison + nbre_liaison_macro;
  sc->nbre_groupe = nbre_groupe_macro + sc->nbre_groupe;

  fclose(f1);
  if (no_macro_used > sc->nbre_macro_lues) sc->nbre_macro_lues = no_macro_used + 1;
  else sc->nbre_macro_lues = sc->nbre_macro_lues + 1;

  /*#ifndef AVEUGLE
   if (sc->premiere_lecture == 0 && onglet_leto->window != NULL) regenerer_test(sc->onglet_leto);
   #endif*/
  return (nbre_groupe_macro + nbre_gpes_inclus);

}

/* if comment=1 then save the comments else don't save comments */
/* si save_sub_networks=1 alors on met tout a plat et on sauvegarde tout sinon seules les creations faites a ce niveau
 du reseau sont sauvegardees */

void save_script(int comment, int save_sub_networks, TxDonneesFenetre *onglet_leto)
{
  type_groupe *groupe;
  type_liaison * liaison;
  FILE *f1;
  int nbre_groupe_a_sauvegarder, nbre_liaison_a_sauvegarder;
  char nom_du_script[1024];

  if (save_sub_networks == 1) /* on applatit tout le reseau pour pouvoir le simuler --> compilation */
  {
    nbre_groupe_a_sauvegarder = sc->nbre_groupe;
    nbre_liaison_a_sauvegarder = sc->nbre_liaison;
    sprintf(nom_du_script, "%s", sc->nomfich1);
  }
  else
  {
    nbre_groupe_a_sauvegarder = compte_nbre_groupe_propre_au_script(0);
    nbre_liaison_a_sauvegarder = compte_nbre_liaison_propre_au_script(0, onglet_leto);

    memset(nom_du_script, 0, sizeof(char) * 1024);
    memcpy(nom_du_script, sc->nomfich1, (strlen(sc->nomfich1) + 1) * sizeof(char));
  }

  prepare_affichage();

  fprintf(stdout, "\nScript generated:  %s\n", nom_du_script);

  f1 = fopen(nom_du_script, "w");
  if (f1 == NULL)
  {
    EXIT_ON_ERROR("\n Error while opening the network script \n");
  }

  if (save_sub_networks == 1) fprintf(f1, "%%!ignore_include \n"); /* lors de la relecture il ne faudra pas en plus relire les includes*/

  if (comment == 1 && sc->first_comment_group != NULL) save_comment(f1, sc->first_comment_group);

  fprintf(f1, "nombre de groupes = %d\n", nbre_groupe_a_sauvegarder);
  groupe = sc->deb_groupe;
  while (groupe != NULL)
  {
    if (save_sub_networks == 1 || groupe->deja_active == 0) write_one_group(f1, groupe, comment);
    groupe = groupe->s;
  }

  if (comment == 1 && sc->first_comment_link != NULL) save_comment(f1, sc->first_comment_link);

  fprintf(f1, "nombre de liaisons = %d\n", nbre_liaison_a_sauvegarder);

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
    if (save_sub_networks == 1 || liaison->deja_active == 0)
    {
      write_one_link(f1, liaison, comment);
    }
    liaison = liaison->s;
  }

  fclose(f1);
#ifndef AVEUGLE
  if (sc->draw[0] != '\0') xml_saveLiaisonFile(sc->deb_liaison, sc->draw);
  else
  {
    printf("Pour sauvegarder les coudes il faut un fichier .draw (sauvegarde dans provisoire.draw)\n");
    xml_saveLiaisonFile(sc->deb_liaison, "provisoire.draw");
  }
#endif
}

/* if comment=1 then save the comments else don't save comments */
/* si save_sub_networks=1 alors on met tout a plat et on sauvegarde tout sinon seules les creations faites a ce niveau
 du reseau sont sauvegardees */
/* nom est le nom du script, si NULL alors on le demande en direct. */
void save_script_selected(int comment, int save_sub_networks, char *nom, TxDonneesFenetre *onglet_leto)
{
#ifndef AVEUGLE
  selected_group *sel_group;
  type_liaison * liaison;
  FILE *f1;
  int nbre_groupe_a_sauvegarder, nbre_liaison_a_sauvegarder;
  char nom_du_script[1024];

  if (nom == NULL)
  {
    printf("nom du fichier pour la sauvegarde : ");
    if (scanf("%s", nom_du_script) != 1) PRINT_WARNING("Fail reading the filename");
  }
  else memcpy(nom_du_script, nom, (strlen(nom) + 1) * sizeof(char));

  nbre_groupe_a_sauvegarder = compte_nbre_groupe_propre_au_script(1);

  nbre_liaison_a_sauvegarder = compte_nbre_liaison_propre_au_script(1, onglet_leto);

  prepare_affichage(); /* position des groupes pour promethe,..., pb difficile! */
  f1 = fopen(nom_du_script, "w");

  if (f1 == NULL)
  {
    fprintf(stderr, "\n save_script_selected: Error while opening the network script %s\n", nom_du_script);
    exit(0);
  }

  /* lors de la relecture il ne faudra pas en plus relire les includes*/
  if (save_sub_networks == 1) fprintf(f1, "%%!ignore_include \n");

  if (comment == 1 && sc->first_comment_group != NULL) save_comment(f1, sc->first_comment_group);

  fprintf(f1, "nombre de groupes = %d\n", nbre_groupe_a_sauvegarder);

  for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
  {
    if (save_sub_networks == 1 || sel_group->group->deja_active == 0) write_one_group(f1, sel_group->group, comment);
  }

  if (comment == 1 && sc->first_comment_link != NULL) save_comment(f1, sc->first_comment_link);

  fprintf(f1, "nombre de liaisons = %d\n", nbre_liaison_a_sauvegarder);

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
    if (save_sub_networks == 1 || liaison->deja_active == 0)
    {
      if (selected_link(liaison, onglet_leto) == 1)
      {
        write_one_link(f1, liaison, comment);
      }
    }
    liaison = liaison->s;
  }

  fclose(f1);
#endif
  (void) comment;
  (void) save_sub_networks;
  (void) nom;
  (void) onglet_leto;
}

