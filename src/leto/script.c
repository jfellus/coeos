
/*---------------------------------------------------------------*/
/*                   GESTION DES SCRIPTS                         */
/* 1991 version de base                                          */
/* sept 2004 fin ajout gestion commentaires                      */
/*---------------------------------------------------------------*/

#define SYMBOLIQUE_EXTENSION ".symb"

/*#define DEBUG*/
#include "public_leto.h"
#include "outils_script.h"
#include "gere_coudes.h"

#include <search.h>  /* pour la table de hachage */

/* variables globales pour la gestion des commentaires non attaches a un groupe */
/* Il s'agit des commentaires precedent le debut des groupes et des commentaires */
/* precedent le debut des liens (si c'est possible! / correspond fin d'un groupe non?) */

type_liaison *script_liaison_scan_arrivee(type_liaison *liaison, int arrivee)
{	
  if (liaison == NULL)
    {
      fprintf(stderr, "(%s) : invalid liaison pointer\n", __FUNCTION__);
      exit(1);
    }
  
  if (arrivee < -1)
    {
      fprintf(stderr, "(%s) : invalid depart value\n", __FUNCTION__);
      exit(1);
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
      fprintf(stderr, "(%s) : invalid liaison pointer\n", __FUNCTION__);
      exit(1);
    }
  
  if (depart < -1)
    {
      fprintf(stderr, "(%s) : invalid depart value\n", __FUNCTION__);
      exit(1);
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
  mxml_node_t *tree, * mxml_node = NULL;

  debug_printf("XML leto : debut script_load_file_xml  %s \n",path);
  
  tree = loadFile(path); /* voir basic_parser_5/src/libparserxml.c */
  if (tree == NULL)
  {
     fprintf(stderr, "(%s) : loading liaison tree failed\n", __FUNCTION__);
     return 1;
  }

  debug_printf("%s \n", tree->value.element.name);
  mxml_node = mxmlFindElement(tree, tree, "liaison_list" , NULL, NULL, MXML_DESCEND_FIRST ); /*entete xml */
  if (mxml_node == NULL) 
  {
     fprintf(stderr, "noeud xml liaison_list  non trouve\n"); 
     return -1;
  }
  debug_printf("%s \n", mxml_node->value.element.name);
  
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

  groupe = sc->deb_groupe;
  while (groupe != NULL)
  {
     if (groupe->deja_active == 0)
     {
	nbre_groupe_propre_au_script++; /*  groupe en direct de ce script, non un groupe insere */
     }
     groupe = groupe->s;
  }
  debug_printf("nbre_groupe_propre_au_script = %d \n", nbre_groupe_propre_au_script);
  return nbre_groupe_propre_au_script;
}

/* si selection=1 on se limite aux groupes dans le rectangle */
int compte_nbre_liaison_propre_au_script(int selection, TxDonneesFenetre *onglet_leto)
{
  int nbre_liaison_propre_au_script=0;
  type_liaison *liaison;
#ifndef AVEUGLE
  if (selection == 1 && sc->groupes_courants == NULL) return 0;
#endif

  liaison = sc->deb_liaison;
  while (liaison != NULL)
  {
     if(liaison->deja_active == 0 && (selection == 0 || selected_link(liaison, onglet_leto))) 
     {
	nbre_liaison_propre_au_script++;
     }
     liaison = liaison->s;
  }
  debug_printf("nbre_liaison_propre_au_script = %d \n", nbre_liaison_propre_au_script);
  return nbre_liaison_propre_au_script;
}

/* read the input script */
/* la lecture suppose que rien n'a ete charge avant. 
   Si l'on relit un script les anciens groupes ne sont pour l'instant pas efface mais perdus! */
/* si recursive_load=1  les sous reseaux sont lus et rajoutes, sinon pas de lecture */

void lecture(int recursive_load,TxDonneesFenetre *onglet_leto)
{
    int i;
    type_groupe *groupe;
    type_liaison *liaison;
    FILE *f1;
    int nbre_liaison_provisoire;
    int reverse,selected_plane;
    char *pt;

    static char ligne[TAILLE_CHAINE];

#ifndef AVEUGLE
    int j;
#else
    hdestroy();       /* on detruit la table precedente si elle existait */
    hcreate(10000);  /* table de hachage pour les no de groupes symboliques */
#endif
    nbre_groupes_lus = 0; /* on initialise le nombre de groupes lus */

    /* on place le flag_symb a 1 si le fichier est un .symb
     * (en esperant que l'utilisateur ne met pas de variable dans un .script !) */
    if((pt = strstr(sc->nomfich1, SYMBOLIQUE_EXTENSION)) != NULL)
      sc->flag_symb = 1;
    else 
       sc->flag_symb=0;

    liaison = sc->deb_liaison = sc->fin_liaison =NULL;
    groupe = sc->deb_groupe = sc->fin_groupe =NULL;

    debug_printf("(lecture) load script = %s \n", sc->nomfich1);
    f1 = fopen(sc->nomfich1, "r");

    if (f1 == NULL)
    {
       fprintf(stderr, "\n Error while opening the script file \n");
       fprintf(stderr, "\n Leto supposes it is a new script file \n");
       return;
    }

    sc->nbre_neurone = 0; 
    sc->nbre_liaison = 0; 
    sc->last_groupe_number = 0;
    sc->first_comment_group = (type_noeud_comment *) read_line_with_comment(f1, NULL, ligne);

    sscanf(ligne, "nombre de groupes = %d\n", &sc->nbre_groupe);
    if (sc->first_comment_group != NULL && strstr(sc->first_comment_group->chaine,"!ignore_include") != NULL)
    {
       debug_printf("script avec la directive : %s \n",sc->first_comment_group->chaine);
       debug_printf("Les sous reseaux ne seront pas charges (ils doivent etre a plat dans le script) \n");
       if(recursive_load!=0) 
       {
	  recursive_load=0;
	  debug_printf("L'option recursive_load est ignoree \n");
       }
    }

    for (i = 0; i < sc->nbre_groupe; i++)
    {
       debug_printf("lecture() groupe %d nbre_groupe = %d \n",i,sc->nbre_groupe);
        if (i != 0)
	  sc->fin_groupe = groupe = creer_groupeb(sc->fin_groupe);
	else
	   groupe = sc->deb_groupe = sc->fin_groupe = creer_groupeb(NULL);

	if (onglet_leto == NULL)
	  sc->nbre_neurone += read_one_group_leto(f1, groupe, NULL, (void **)NULL);
	else
	  sc->nbre_neurone += read_one_group_leto(f1, groupe, NULL, (void **)&onglet_leto->hashtab);
	/* gestion de la lecture d'un sous reseau */
	if(groupe->type==No_Sub_Network && recursive_load==1)
	  {
	    debug_printf("lecture() groupe %d insert the sub network %s\n",i,groupe->nom);
	    /* le nbre de groupes a ete modifie dans read_macro */
	    i=i+read_macro(groupe->no_name,groupe->nom,groupe->posx,groupe->posy,1,1,&selected_plane,onglet_leto);
	    if(groupe->reverse>0) reverse=1; else reverse=-1;
	    groupe->reverse=reverse*selected_plane; /* plan >100 pour tout bouger ensemble */
	    debug_printf("lecture() index no groupe = %d \n",i);
	  }

	groupe->deja_active=0; /* c'est un groupe en direct de ce script, non un groupe insere */
	/* printf("groupe->no = %d last = %d\n",groupe->no,sc->last_groupe_number); */
	debug_printf("-------------------\n");
    }
    sc->nbre_groupes_lus = nbre_groupes_lus = sc->nbre_groupe;
    debug_printf("Fin de lecture des groupes last_group_number=%d\n",sc->last_groupe_number);
    debug_printf("(lecture) Nombre groupes lus = %d\n",nbre_groupes_lus);

    sc->first_comment_link = (type_noeud_comment *) read_line_with_comment(f1, NULL, ligne);

    /*   printf("message :: nombre de liaisons --> %s \n",ligne);*/

    sscanf(ligne, "nombre de liaisons = %d\n", &nbre_liaison_provisoire);
    debug_printf("(lecture) nombre de liaisons = %d \n", nbre_liaison_provisoire);
    liaison = sc->fin_liaison;

    for (i = 0; i < nbre_liaison_provisoire; i++)
    {
      if(sc->deb_liaison==NULL) 
	{
	  liaison = sc->deb_liaison = sc->fin_liaison = (type_liaison *) creer_liaison(NULL);
	}
      else liaison = (type_liaison *) creer_liaison(liaison);

      if (onglet_leto == NULL)
	read_one_link(f1, liaison, NULL, NULL);
      else
	read_one_link(f1, liaison, NULL, (void **)&onglet_leto->hashtab);

      liaison->deja_active=0; /* liaison propre au script */
    }
    sc->nbre_liaison=sc->nbre_liaison+nbre_liaison_provisoire; /* tenir compte des scripts inclus */
    debug_printf("nbre liaisons = %d \n",sc->nbre_liaison);
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

int read_macro(char *base_base, char *nom,int px, int py,int relatif,
                int mode_inclusion, int *selected_plane, TxDonneesFenetre *onglet_leto)
{
    int i;
    int nbre_groupe_macro,nbre_gpes_inclus, nbre_liaison_macro;
    static char ligne[TAILLE_CHAINE];
    char base_nom[SIZE_NO_NAME];
    char base_nom_complet[SIZE_NO_NAME];

    type_groupe *groupe_local,*debut_macro;
    type_liaison *liaison, *liaison1 = NULL;
    FILE *f1;
    int reverse;
    int mx,my;
    float zoom;
    int no_macro_used;

    /*printf("LISTE des groupes debut: \n"); affiche_liste_groupes();*/

    nbre_groupe_macro=0;
    debug_printf("Lecture macro %s, base_base %s \n",nom,base_base);
    debug_printf("Nombre total macros lues = %d \n",sc->nbre_macro_lues);
    f1 = fopen(nom, "r");
    if (f1 == NULL)
    {
       fprintf(stderr, "\n Error while opening the macro script file %s \n", nom);
        return (0);
    }

    no_macro_used=sc->nbre_macro_lues;
    *selected_plane=(100+sc->nbre_macro_lues*7);

    if(1/*mode_inclusion==0*/)
      {
	   while(plane_used(*selected_plane)==1)
	   {
	     debug_printf("Attention plan %d deja utilise j'en cherche un autre \n",*selected_plane);
	     no_macro_used++;
	     (*selected_plane)=(100+no_macro_used*7);
	   }
      }

    memcpy(base_nom,nom, (strlen(nom)+1) * sizeof(char));
    get_base_name(base_nom);

    if(strcmp(base_nom,"copy_buffer")==0)
      {
		printf("PASTE \n");
		memcpy(base_nom,"c",(strlen("c")+1) * sizeof(char));
      }
 /* interprete nom2[numero] en nom2 et numero*/
    /*  scanf("%s",nom2);
    pt=rindex(nom2,']');*pt='\0';pt=rindex(nom2,'[');
    sscanf((pt+1),"%d",&numero);(*pt)='\0';
    printf("nom2 = %s , numero = %d \n",nom2,numero);*/

    if(base_base==NULL) sprintf(base_nom_complet,"%s[%d]",base_nom,no_macro_used);
    /* else sprintf(base_nom_complet,"%s_%s[%d]",base_base,base_nom,no_macro_used);*/
    else memcpy(base_nom_complet,base_base, (strlen(base_base)+1) * sizeof(char));

    debug_printf("base = %s \n",base_nom_complet);

    groupe_local = sc->fin_groupe;

    sc->first_comment_group = (type_noeud_comment *) read_line_with_comment(f1, sc->first_comment_group,ligne);

    /* printf("1ere ligne sub network (non comment) : %s \n",ligne);*/
    sscanf(ligne, "nombre de groupes = %d\n", &nbre_groupe_macro);
    debug_printf("nbre groupes deja lu %d, nombre de groupes macro = %d\n",  sc->nbre_groupe, nbre_groupe_macro);
    if(nbre_groupe_macro==0)
      {
	printf("ATTENTION no group in this script: ABORT reading !!!\n");
	return 0; /* aucun groupe inclus*/
      }

    mx=my=0; /* centre de gravite des groupes lus */
    debut_macro=NULL;
    nbre_gpes_inclus=0;
    for (i = 0; i < nbre_groupe_macro; i++)
    {
      groupe_local = (type_groupe *) creer_groupeb(groupe_local);
      if(sc->deb_groupe==NULL) sc->deb_groupe=groupe_local; /* c'est le 1er groupe lu */

	if (onglet_leto == NULL)
	  sc->nbre_neurone += read_one_group_leto(f1, groupe_local,base_nom_complet, (void **)NULL);
	else
	  sc->nbre_neurone += read_one_group_leto(f1, groupe_local,base_nom_complet, (void **)&onglet_leto->hashtab);

	debug_printf("read macro %s group %s no=%d\n",base_nom,groupe_local->no_name,groupe_local->no);
	if(debut_macro==NULL) debut_macro=groupe_local;
	sc->fin_groupe=groupe_local ;

	/* gestion de la lecture d'un sous reseau, PB ICI ............. */
	if(groupe_local->type==No_Sub_Network)
	  {
	    debug_printf("insert the sub network %s no_name=%s\n",groupe_local->nom,groupe_local->no_name);
	    if(groupe_local->reverse>0) reverse=1; else reverse=-1;
	    groupe_local->reverse=reverse*(100+sc->nbre_macro_lues*7); /* plan >100 pour tout bouger ensemble */
	    /*  i=i+read_macro(base_nom_complet,groupe_local->nom,groupe_local->posx,groupe_local->posy,1,1);*/ 
	    /* le nbre de groupes a ete modifie dans read_macro */
	    nbre_gpes_inclus=nbre_gpes_inclus+
	                              read_macro(groupe_local->no_name,groupe_local->nom,groupe_local->posx,groupe_local->posy,1,1,selected_plane,onglet_leto);
	    debug_printf("read_macro index no groupe = %d \n",i);
	  }

	groupe_local->deja_active=mode_inclusion; /* c'est un groupe insere (a ne pas resauvegarder) */
	mx=mx+groupe_local->posx;
	my=my+groupe_local->posy;
	if(relatif==0)
	  {
	    groupe_local->posx = groupe_local->posx+50;
	    groupe_local->posy = groupe_local->posy+50;
	  }
        groupe_local->p_posx += 50;
        groupe_local->p_posy += 50;
	if(groupe_local->reverse>0) reverse=1; else reverse=-1;
	groupe_local->reverse=reverse*(*selected_plane); /* plan >100 pour tout bouger ensemble */

    }
    /*sc->fin_groupe = groupe_local;*/

    if(relatif==1)
      {
	     if(mode_inclusion==1) zoom=0.1; else zoom=0.75;
	     mx=mx/nbre_groupe_macro;  my=my/nbre_groupe_macro ;
	     groupe_local=debut_macro;
		for (i = 0; i < nbre_groupe_macro; i++)  /* tres laid PG !!! */
	  	  {
	    	groupe_local->posx = 2+px+(int)(((float)(groupe_local->posx-mx))*zoom);
	    	groupe_local->posy = 2+py+(int)(((float)(groupe_local->posy-my))*zoom);
	    	groupe_local=groupe_local->s;
	  	  }
      }


    sc->first_comment_link = (type_noeud_comment *) read_line_with_comment(f1, sc->first_comment_link,ligne);
    sscanf(ligne, "nombre de liaisons = %d\n", &nbre_liaison_macro);

    debug_printf("read_macro lecture de %d liaisons \n", nbre_liaison_macro);
    
    liaison = sc->fin_liaison;
    for (i = sc->nbre_liaison; i < sc->nbre_liaison + nbre_liaison_macro; i++)
    {
      if(sc->deb_liaison==NULL) 
	{
	  liaison = sc->deb_liaison = sc->fin_liaison = (type_liaison *) creer_liaison(NULL);
	}
      else liaison = creer_liaison(liaison);

      if (onglet_leto == NULL)
	read_one_link(f1, liaison, base_nom_complet, NULL);
      else
	read_one_link(f1, liaison,base_nom_complet, (void **)&onglet_leto->hashtab);

      debug_printf("liaison de %s a %s \n",liaison->depart_name,liaison->arrivee_name);
      liaison->deja_active=mode_inclusion;
      /* liaison->depart = liaison->depart + sc->nbre_groupe;  */  /* offset to change the connected groups (inutile maintenant)*/
      /* liaison->arrivee = liaison->arrivee + sc->nbre_groupe; */ /* c'est le nom compose qui evite le pb */
      if (i == sc->nbre_liaison)  liaison1 = liaison;
#ifndef AVEUGLE
      initialise_coudes_liaison(liaison); /*initialise les coudes de la liaison - par defaut on ne reprend pas le graphisme */
#endif
    }
    sc->fin_liaison = liaison;

    /* cette solution n'est valable que si le lien micro macro est toujours cree en premier: faux en ce moment ex: kohonen 
       ce lien n'existe pas */
    /*  detruit_liaison(liaison1); */ /* only a single link between micro and macro neuron */

    sc->nbre_liaison = sc->nbre_liaison + nbre_liaison_macro;
    sc->nbre_groupe = nbre_groupe_macro + sc->nbre_groupe;

    fclose(f1);
    if(no_macro_used>sc->nbre_macro_lues) sc->nbre_macro_lues=no_macro_used+1; 
    else  sc->nbre_macro_lues=sc->nbre_macro_lues+1;
    /* printf("LISTE des groupes fin: \n"); affiche_liste_groupes();*/

    debug_printf("debug_Fin lecture macro\n");
#ifndef AVEUGLE
    if (sc->premiere_lecture == 0 && onglet_leto->window != NULL)
      regenerer_test(onglet_leto);
#endif
    return ( nbre_groupe_macro+nbre_gpes_inclus);

}

/* if comment=1 then save the comments else don't save comments */
/* si save_sub_networks=1 alors on met tout a plat et on sauvegarde tout sinon seules les creations faites a ce niveau
   du reseau sont sauvegardees */

void save_script(int comment,int save_sub_networks, TxDonneesFenetre *onglet_leto)
{
    type_groupe *groupe;
    type_liaison *liaison;
    FILE *f1;
    int nbre_groupe_a_sauvegarder,nbre_liaison_a_sauvegarder;
    char nom_du_script[1024];

    if(save_sub_networks==1)   /* on applatit tout le reseau pour pouvoir le simuler --> compilation */
      {
	nbre_groupe_a_sauvegarder=sc->nbre_groupe;
	nbre_liaison_a_sauvegarder=sc->nbre_liaison;
	sprintf(nom_du_script,"%s_o",sc->nomfich1);
	printf("Sauvegarde du script avec mise a plat des sous reseau dans %s \n",nom_du_script);
      }
    else
      {
	nbre_groupe_a_sauvegarder=compte_nbre_groupe_propre_au_script(0);
	nbre_liaison_a_sauvegarder=compte_nbre_liaison_propre_au_script(0,onglet_leto);

	memset(nom_du_script,0,sizeof(char)*1024);
	memcpy(nom_du_script,sc->nomfich1, (strlen(sc->nomfich1)+1) * sizeof(char));
      }

    prepare_affichage();

    fprintf(stdout,"SCRIPT %s\n",nom_du_script);

    f1 = fopen(nom_du_script, "w");
    if (f1 == NULL)
    {
       fprintf(stderr, "\n Error while opening the network script \n");
        exit(0);
    }

    if(save_sub_networks==1) fprintf(f1,"%%!ignore_include \n"); /* lors de la relecture il ne faudra pas en plus relire les includes*/

    if (comment == 1 && sc->first_comment_group != NULL)
        save_comment(f1, sc->first_comment_group);

    fprintf(f1, "nombre de groupes = %d\n", nbre_groupe_a_sauvegarder);
    groupe = sc->deb_groupe;
    while (groupe != NULL)
    {
       if(save_sub_networks==1 || groupe->deja_active==0)
	 write_one_group(f1, groupe, comment);
       groupe = groupe->s;
    }

    if (comment == 1 && sc->first_comment_link != NULL)
        save_comment(f1, sc->first_comment_link);

    fprintf(f1, "nombre de liaisons = %d\n", nbre_liaison_a_sauvegarder);

    liaison = sc->deb_liaison;
    while (liaison != NULL)
    {
		if (save_sub_networks==1 || liaison->deja_active==0)
		{
			write_one_link(f1, liaison, comment);
		}
        liaison = liaison->s;
    }

    fclose(f1);
#ifndef AVEUGLE
    if(sc->draw[0]!='\0') xml_saveLiaisonFile(sc->deb_liaison, sc->draw); 
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
void save_script_selected(int comment,int save_sub_networks, char *nom, TxDonneesFenetre *onglet_leto)
{
#ifndef AVEUGLE
   selected_group *sel_group;
   type_liaison *liaison;
   FILE *f1;
   int nbre_groupe_a_sauvegarder,nbre_liaison_a_sauvegarder,res;
   char nom_du_script[1024];

   debug_printf("save script selected \n");
   if (nom == NULL)
   {
      printf("nom du fichier pour la sauvegarde : ");
      res = scanf("%s",nom_du_script);
   }
   else 
      memcpy(nom_du_script,nom, (strlen(nom)+1) * sizeof(char));

   nbre_groupe_a_sauvegarder = compte_nbre_groupe_propre_au_script(1);
   debug_printf("nbre_groupe_a_sauvegarder=%d\n", nbre_groupe_a_sauvegarder);

   nbre_liaison_a_sauvegarder = compte_nbre_liaison_propre_au_script(1, onglet_leto);


   prepare_affichage();   /* position des groupes pour promethe,..., pb difficile! */
   f1 = fopen(nom_du_script, "w");

   if (f1 == NULL)
   {
      fprintf(stderr, "\n save_script_selected: Error while opening the network script %s\n",nom_du_script);
      exit(0);
   }
	
   /* lors de la relecture il ne faudra pas en plus relire les includes*/
   if (save_sub_networks == 1) 
      fprintf(f1,"%%!ignore_include \n");

   if (comment == 1 && sc->first_comment_group != NULL)
      save_comment(f1, sc->first_comment_group);

   fprintf(f1, "nombre de groupes = %d\n", nbre_groupe_a_sauvegarder);
	 
   for (sel_group = sc->groupes_courants; sel_group != NULL; sel_group = sel_group->next)
   {
      if (save_sub_networks == 1 || sel_group->group->deja_active == 0) 
	 write_one_group(f1, sel_group->group, comment);
   }
    
   if (comment == 1 && sc->first_comment_link != NULL)
      save_comment(f1, sc->first_comment_link);

   fprintf(f1, "nombre de liaisons = %d\n", nbre_liaison_a_sauvegarder);

   liaison = sc->deb_liaison;
   while (liaison != NULL)
   {
      if(save_sub_networks==1 || liaison->deja_active==0) 
      {
	 if (selected_link(liaison,onglet_leto)==1)
	 {
	    write_one_link(f1, liaison, comment);
	 }
      }
      liaison = liaison->s;
   }

   fclose(f1);
   debug_printf("fin sauvegarde selection %s\n",nom_du_script);
#endif
}

