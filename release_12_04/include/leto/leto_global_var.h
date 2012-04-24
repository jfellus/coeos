#ifndef GLOBAL_VAR_H
#define GLOBAL_VAR_H

#define LONGUEUR_NOM 255        /* defini dans leto.h habituellement */

#define LINK_CREATE_NO_ACTION 0
#define LINK_CREATE_CHOOSE_START 1
#define LINK_CREATE_CHOOSE_END 2

#define LINK_ALGO 0
#define LINK_ONE_TO_ONE 1

#define	nbPoint_max	100
#define nb_max_planes 2000

/* bords des boites representant les differentes echelles temporelles */
#define limite_nbre_max_ech 200

typedef struct donnees_script
{
  int premiere_lecture;

#ifndef AVEUGLE
  TxPoint point_courant_leto;
  TxPoint pointer_selection_center;

  int flag_rectangle_selection;  
  int couleur_fond;
  int couleur_texte;

  /* bords des boites representant les differentes echelles temporelles */
  int boite_x_min[limite_nbre_max_ech];
  int boite_y_min[limite_nbre_max_ech];
  int boite_x_max[limite_nbre_max_ech];
  int boite_y_max[limite_nbre_max_ech];

  int display_plane[nb_max_planes];
  
  int xmin, ymin;
  int xmax, ymax;

  int modified;

  int flag_create_link;  /* 0 ou 1 utilise le flag modify_link */
  int type_create_link;  /* 0 ou 1 utilise le flag modify_link */

  int flag_drag_group;              /* pour gere deplacement d'un groupe  */

  int first_time;             /* affichage au demarrage du reseau lu */
  int show_sub_networks;

  /* numero de l'onglet pour metaleto */
  int num_onglet;

  /* pour find_and_manage groupes */
  GtkWidget *fm_window;
  GtkTreeModel *fm_group_list_store;

  selected_group *groupes_courants;
#endif

  type_liaison *liaison_courante;
  t_polyline *coude_courant;

  int flag_symb;              /* pour gerer les versions symboliques */
  int flag_drag_coude;


  /*--------------------------------------------------------------*/

  char groupe_depart_name[SIZE_NO_NAME], groupe_arrivee_name[SIZE_NO_NAME];
  
  /*--------------------------------------------------------------*/
  /*       structure de donnee pour le reseau de neurones         */
  /*--------------------------------------------------------------*/

  type_tableau neurone;    /*tableau contenant l'ens du reseau   */

  int nbre_neurone;        /* nombre de neurones dans le reseau  */
  int last_groupe_number;         /* no seulement incremente a partir du dernier groupe lu */
  int nbre_entree;         /* nbre de vecteurs utilises en entree */
  int nbre_sortie;         /* nbre de neurones utilises en sortie */
  int nbre_couche;         /* nbre de couches dans le reseau     */
  float ca, ck;            /* constantes pour la sigmoide        */

  char nom_groupe[200][256];

  int nbre_max_ech;

  type_vecteur_entier t;
  type_vecteur_entier c;   /* type du groupe pour apprentissage  */
  type_vecteur_entier n;   /* nbre de neurones dans un groupe    */
  type_vecteur_entier ngc; /* nbre de groupes par couche         */

   char nomfich1[LONGUEUR_NOM], freseau[LONGUEUR_NOM], draw[LONGUEUR_NOM], fvar[LONGUEUR_NOM];

  int nbre_groupe, nbre_liaison;

  type_groupe *deb_groupe, *fin_groupe;
  type_liaison *deb_liaison, *fin_liaison;

  /* script.c */
  type_noeud_comment *first_comment_group;
  type_noeud_comment *first_comment_link;

  int nbre_macro_lues;
  int nbre_groupes_lus;

  long int seed;
} donnees_script;


#define NB_MAX_SCRIPTS 255 /* ca devrait suffire pour quelques annees, j'espere */
extern donnees_script les_scripts[NB_MAX_SCRIPTS];

extern donnees_script *sc; /* le script courant : celui a l'affichage */

extern void init_global_var_un_script(donnees_script *le_script);

#endif
