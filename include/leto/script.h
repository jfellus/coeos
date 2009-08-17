#ifndef SCRIPT_H
#define SCRIPT_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "script_parserxml.h"

extern int read_macro(char *base_base, char *nom_fichier,int px, int py, int relatif, int mode, int *selected_plane,TxDonneesFenetre *onglet_leto);

#include "outils_script.h"

#include <search.h>  /* pour la table de hachage */

/* variables globales pour la gestion des commentaires non attaches a un groupe */
/* Il s'agit des commentaires precedent le debut des groupes et des commentaires */
/* precedent le debut des liens (si c'est possible! / correspond fin d'un groupe non?) */

extern void save_script(int comment,int save_sub_networks, TxDonneesFenetre *onglet_leto);
extern void save_script_selected(int comment,int save_sub_networks,char *nom, TxDonneesFenetre *onglet_leto);
extern void sauvegarde();
extern void sauvegarde_avec_comment();
extern void lecture(int recursive_load,TxDonneesFenetre *onglet_leto);

#endif
