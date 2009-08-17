#ifndef OUTILS_H
#define OUTILS_H
extern type_tableau creer_reseau(int );
extern void free_reseau(type_tableau t2);

extern type_noeud **creer_groupe(int );
extern void free_groupe(type_noeud** t2);

extern type_coeff *creer_coeff();
extern void free_coeff(type_coeff *coeff);

extern type_noeud *creer_noeud();
extern void free_noeud(type_noeud *t);

extern type_matrice creer_matrice(int n, int m);
extern void free_matrice(type_matrice *t, int n);

extern type_vecteur_entier creer_vecteur(int n);
extern void free_vecteur(type_vecteur_entier *t);

extern type_groupe *creer_groupeb(type_groupe * prec);
extern void free_liste_groupes(type_groupe *groupe);


extern type_groupe *trouver_groupe_ante_par_nom(char *nom);
extern type_groupe *trouver_groupe(int n);
extern type_groupe *trouver_groupe_par_nom(char *nom, TxDonneesFenetre *onglet_leto);
extern type_liaison *trouver_liaison_par_nom(char *no_groupe_depart_name,char *no_groupe_arrivee_name);
extern type_groupe *creer_groupeb(type_groupe * prec);

extern type_liaison *creer_liaison(type_liaison * prec);
extern void free_liaison(type_liaison * liaison);

extern type_coeff *pointe_vers_dernier(int j);
extern int compte_neurone();
extern float alea(float x);
extern int sorte_liaison(int No_liaison);

extern void prepare_affichage();
extern void replace_tout();

extern int selected_link(type_liaison *liaison, TxDonneesFenetre *onglet_leto);
extern int plane_used(int plan);
extern int find_new_plane_for_new_macro(int *no_macro_used);

extern void affiche_liste_groupes();

void initialise_liaisons();

/* fonction a moi */
extern void get_base_name(char *filename);
extern float bound(float value, float min, float max);

#endif
