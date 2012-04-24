#ifndef OUTILS_EXTERNES_H
#define OUTILS_EXTERNES_H

/*---------------------------------------------------------------*/
/* fonctions utilisees dans leto_cle.o                           */
/*---------------------------------------------------------------*/

extern void ecrit_reseau();


extern void creer_liaisons_entre_groupe(int e_deb, int e_fin, int s_deb,
                                        int s_fin, int gpe, int gpe_entree,
                                        type_liaison * liaison, int no_voie,
                                        int pas, int no_gpe_liaison);
extern void creer_micro_liens(int s_deb, int s_fin, int nbre_voie, int gpe,
                              float val);

/*---------------------------------------------------------------*/
/* fabrique les liaisons a partir des parametres suivants:       */
/*      . e_deb   = numero du premier neurone du gpe d'entree    */
/*      . e_fin   = numero du dernier neurone du groupe d'entree */
/*      . s_deb   = numero du premier neurone du groupe de sortie*/
/*      . s_fin   = idem pour le dernier                         */
/*      . gpe     = numero du groupe de sortie (pourloi utilisee)*/
/*      . liaison = pt versdefinition du type de liaisons a creer*/
/*      . no_voie = numero de la voie a creer                    */
/*                                                               */
/*  void creer_liaisons_entre_groupe(e_deb,e_fin,s_deb,s_fin,    */
/*                                         gpe,liaison,no_voie)  */
/*  int e_deb,e_fin,s_deb,s_fin,gpe,no_voie;                     */
/*  type_liaison *liaison;                                       */
/*    {   ...  }                                                 */
/*---------------------------------------------------------------*/

/*---------------------------------------------------------------*/



#endif
