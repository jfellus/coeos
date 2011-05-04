#include "public_leto.h"
#include "gere_coudes.h"


/* supprime les 0 a la fin d'un nombre a virgule */
void simplifie_nom(char *nom)
{
  int i, j, size;

  size = strlen(nom);
  i = size - 1;

  if (nom[i] != '0')
  {
     return;
  }

  while (nom[i] == '0' && i > 1)
  {
     i--;
  }
  
  j = i;
  while (nom[j] != '.' && j > 1)
  {
     j--;
  }

  if (j == 1 && nom[j] != '.')
  {
     return;  /* pas de . , on ne supprime pas les 0*/
  }
  nom[i+1] = (char) 0;  
}

/*-------------------------------------------------------------*/
/*         AFFICHAGE des groupes et des liaisons               */
/*-------------------------------------------------------------*/

void affiche_macro(TxPoint point, type_groupe * groupe,TxDonneesFenetre *onglet_leto)
{
    TxPoint point2;
    TxDessinerCercle(onglet_leto,bleu,TxPlein,point,60,1);
    point2.x=point.x- 3*deltax ; 
    point2.y=point.y+70+deltay/2;
    
    TxEcrireChaine(onglet_leto, bleu, point2, groupe->nom,NULL);
}




void affiche_groupe(TxPoint point, type_groupe * groupe,TxDonneesFenetre *onglet_leto)
{
   TxPoint point2, point3, point4, point5, point6, point7;
   int x,y,type, text_color;
   char *font_weight = NULL;

   x = point.x;
   y = point.y;

   point.x = point.x - deltax;
   point.y = point.y - deltay;

   point2.x = point.x + deltax / 3;    /* position de l'ecriture du no du gpe */
   point2.y = point.y + 3 * deltay / 2;

   point3.x = point.x - distc;
   point3.y = point.y + 2 * deltay;

   point4.x = point.x + 2 * deltax + distc;
   point4.y = point.y + deltay;

   point5.x = point.x + 2 * deltax + distc;
   point5.y = point.y + 3 * deltay;

   type = groupe->type % 30;

   if (is_selected(groupe) != NULL)
   {
      font_weight = "bold";
      text_color = blanc;
      TxDessinerRectangle(onglet_leto, lut_g[type], TRUE, point, 2 * deltax + 1, 2 * deltay, 1);
   }
   else
   {
      text_color = lut_g[type];
      TxDessinerRectangle(onglet_leto, lut_g[type], FALSE, point, 2 * deltax, 2 * deltay, 1);
   }


   point.y = point.y + 2 * deltay;
   TxDessinerRectangle(onglet_leto, lut_g[type], 0, point, 2 * deltax, 2 * deltay, 1);

   point.y = point3.y;

   if (groupe->reverse <= 0)
   {
      if (groupe->type != No_Entree)
      {
	 TxDessinerCercle(onglet_leto, lut_g[type], TxPlein, point3, 4, 1);
	 TxDessinerSegment(onglet_leto, lut_g[type], point3, point, 1);
      }

      FlecheHorizontale(onglet_leto, lut_g[type], point4, 6, 2);
      point.x = point.x + 2 * deltax;
      point.y = point4.y;
      TxDessinerSegment(onglet_leto, lut_g[type], point4, point, 1);

      FlecheHorizontale(onglet_leto, lut_g[type], point5, 6, 2);
      point.y = point5.y;
      TxDessinerSegment(onglet_leto, lut_g[type], point5, point, 1);

   }
   else if (groupe->reverse > 0)
   {

      point3.y = point3.y - deltay;
      FlecheHorizontale(onglet_leto, lut_g[type], point3, -6, 2);
      point.x = point3.x + distc;
      point.y = point3.y;
      TxDessinerSegment(onglet_leto, lut_g[type], point3, point, 1);

      point3.y = point3.y + 2 * deltay;
      FlecheHorizontale(onglet_leto, lut_g[type], point3, -6, 2);
      point.y = point3.y;
      TxDessinerSegment(onglet_leto, lut_g[type], point3, point, 1);


      if (groupe->type != No_Entree)
      {
	 point4.y = point4.y + deltax;
	 point.y = point4.y;
	 point.x = point4.x - distc;
	 TxDessinerCercle(onglet_leto, lut_g[type], TxPlein, point4, 4, 1);
	 TxDessinerSegment(onglet_leto, lut_g[type], point4, point, 1);
      }
   }

   TxEcrireChaine(onglet_leto, text_color, point2, groupe->no_name, NULL);

#ifndef SYMBOLIQUE_VERSION
   if (groupe->taillex > 1 || groupe->tailley > 1) 
   {
      point6.x = point2.x;
      point6.y = point2.y + 2 * deltax + 1;
	
      if (groupe->taillex > 1)
      {
	 point7.x = point6.x + 7;
	 point7.y = point6.y;
	 TxDessinerSegment(onglet_leto, lut_g[type], point6, point7, 1);
	 point7.x = point7.x + 7;
	 FlecheHorizontale(onglet_leto, lut_g[type], point7, 1, 1);
	
      }

      if (groupe->tailley > 1)
      {
	 point7.x = point6.x;
	 point7.y = point6.y - 7;
	 TxDessinerSegment(onglet_leto, lut_g[type], point6, point7, 1);
	 point7.y = point7.y - 7;
	 FlecheVerticale(onglet_leto, lut_g[type], point7, -1, 1);
      }
   }
   else
   {
      point6.x = point2.x + 5;
      point6.y = point2.y + 15;
      TxDessinerCercle(onglet_leto, lut_g[type], TxPlein, point6, 2, 1);
   }
#else
   if(!verify_if_int(groupe->taillex) || !verify_if_int(groupe->tailley) || (MY_Int2Int(groupe->taillex) > 1) || (MY_Int2Int(groupe->tailley) > 1)) 
   {
      point6.x = point2.x;
      point6.y = point2.y + 2 * deltax + 1;
      
      if(!verify_if_int(groupe->taillex) || (MY_Int2Int(groupe->taillex) > 1))
      {
	 point7.x = point6.x + 7;
	 point7.y = point6.y;
	 TxDessinerSegment(onglet_leto, lut_g[type], point6, point7, 1);
	 point7.x = point7.x + 7;
	 FlecheHorizontale(onglet_leto, lut_g[type], point7, 1, 1);
	  
      }
      if(!verify_if_int(groupe->tailley) || (MY_Int2Int(groupe->tailley) > 1))
      {
	 point7.x = point6.x;
	 point7.y = point6.y - 7;
	 TxDessinerSegment(onglet_leto, lut_g[type], point6, point7, 1);
	 point7.y = point7.y - 7;
	 FlecheVerticale(onglet_leto, lut_g[type], point7, -1, 1);
      }
   }
   else
   {
      point6.x = point2.x + 5;
      point6.y = point2.y + 15;
      TxDessinerCercle(onglet_leto, lut_g[type], TxPlein, point6, 2, 1);
   }
#endif
   point3.x = point3.x /*+ deltax*/;
   point3.y = point3.y + 3 * (deltay+1);
   TxEcrireChaine(onglet_leto, lut_g[type]/*sc->couleur_texte*/, point3, groupe->nom, font_weight); 
}



void affiche_liaison_b(TxDonneesFenetre * onglet_leto, TxPoint point1,
                       TxPoint point2, int type)
{
    TxPoint pointa, pointb, pointc;
    if (point1.x < point2.x)
    {
        point1.x = point1.x + deltax + distc;
        point1.y = point1.y;
        point2.x = point2.x - deltax - distc;
        point2.y = point2.y;
        TxDessinerSegment(onglet_leto, lut_l[type], point1, point2, 1);
    }
    else
    {
        point1.x = point1.x - deltax - distc;
        point2.x = point2.x + deltax + distc;
        pointa.x = point1.x;
        pointa.y = point1.y - 2 * deltay;
        pointb.y = pointa.y;
        pointb.x = pointa.x + 2 * (deltax + distc);
        pointc.x = point2.x;
        pointc.y = point2.y - 2 * deltay;
        TxDessinerSegment(onglet_leto, lut_l[type], point1, pointa, 1);
        TxDessinerSegment(onglet_leto, lut_l[type], pointa, pointb, 1);
        TxDessinerSegment(onglet_leto, lut_l[type], pointb, pointc, 1);
        TxDessinerSegment(onglet_leto, lut_l[type], pointc, point2, 1);
    }
}

/*
 * fonction affiche_liaison dessine la liaison indiquee
 */

void  affiche_parametres_liaison(type_liaison *liaison, TxPoint point2,TxDonneesFenetre *onglet_leto)  
{      
   TxPoint point1;
   char nom[256];
   char *font_weight = NULL;
   int type = liaison->type;

   point1.x = point2.x;
   point1.y = point2.y;

   if (liaison == sc->liaison_courante)
   {
      font_weight = "bold";
   }
   
   if (liaison->type == No_l_algorithmique || liaison->type == No_l_neuro_mod)
   {
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point1, liaison->nom, font_weight);
   }
   
   point1.x = point2.x + 14;
   point1.y = point1.y + 3;

   if (liaison->type != No_l_algorithmique && liaison->type != No_l_neuro_mod)
   {
      if (liaison->mode == 0 || liaison->mode == 2)
      {
	 TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point1, "X", font_weight);
      }
      else
      {
	 TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point1, "d", font_weight);
      }
      point1.x = point1.x + 5;
   }

   point1.x = point1.x /*+ 5*/;
   point2.y = point2.y + 3;
   
   /*if (liaison->secondaire == 0)
   {
      TxEcrireChaine(onglet_leto, sc->couleur_texte, point1, ".", font_weight);
      }*/
    
   if (liaison->type == No_l_1_a)
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point2, "A", font_weight);
   else if (liaison->type == No_l_1_t)
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point2, "//", font_weight);
   else if (liaison->type == No_l_1_1_non_modif)
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point2, "/", font_weight);
   else if (liaison->type == No_l_1_1_modif)
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point2, "\\", font_weight);
   else if (liaison->type == No_l_1_v)
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point2, "V", font_weight);
   
   point1.x = point1.x - 40;
   point1.y = point1.y + 10;
   if (liaison->type != No_l_algorithmique && liaison->type != No_l_neuro_mod)
   {
      strcpy(nom, MY_Float2Str(liaison->norme));
      simplifie_nom(nom);
      TxEcrireChaine(onglet_leto, lut_l[type]/*sc->couleur_texte*/, point1, nom, font_weight);
   }
}

void affiche_liaison(type_liaison *liaison, type_groupe * groupe1, type_groupe * groupe2,TxDonneesFenetre *onglet_leto)
{
  int type;
  int secondaire,style;
  int nbPoint;
  int thickness = 1;
  TxPoint TxTabPoint[nbPoint_max];
  TxPoint point2;
	
  type = liaison->type;
  secondaire = liaison->secondaire;
  style = liaison->style;
	
  nbPoint = get_link_points(liaison, TxTabPoint);

  if (liaison == sc->liaison_courante)
  {
     thickness = 3;
     fill_coudes(onglet_leto);
  }
  
  if (secondaire == 0 && style == 1)
  {
     TxDessinerCourbeComplexe(onglet_leto, lut_l[type], TxTabPoint, nbPoint, thickness);
  }
  else
  {
    if (secondaire == 1) 
    {
       TxDessinerSegmentPointilleBrise(onglet_leto, lut_l[type], TxTabPoint, nbPoint, thickness);
    }
    else 
    {
       TxDessinerSegmentBrise(onglet_leto, lut_l[type], TxTabPoint, nbPoint, thickness);
    }
  }
  
  get_link_caption_coordinates(liaison, &(point2.x), &(point2.y));
  if (point2.x >= sc->xmin && point2.x <= sc->xmax && point2.y >= sc->ymin && point2.y <= sc->ymax)
  {
     affiche_parametres_liaison(liaison, point2, onglet_leto);
  }
}

void draw_selection(GtkWidget * widget, gdouble x, gdouble y, gpointer data, int ctrl_mask)
{
   int xmin, xmax, ymin, ymax;
   TxPoint point;
   
   if (sc->point_courant_leto.x > x)
   {
      xmin = x;
      xmax = sc->point_courant_leto.x;
   }
   else
   {
      xmax = x;
      xmin = sc->point_courant_leto.x;
   }

   if (sc->point_courant_leto.y > y)
   {
      ymin = y;
      ymax = sc->point_courant_leto.y;
   }
   else
   {
      ymax = y;
      ymin = sc->point_courant_leto.y;
   }

   point.x = x;
   point.y = y;

   update_selection_rectangle(xmin, ymin, xmax, ymax, ctrl_mask);
   scroll(point, (TxDonneesFenetre *) data);
   regenerer_test(((TxDonneesFenetre *) data));


   /* Paint to the pixmap, where we store our state */
   gdk_draw_rectangle(((TxDonneesFenetre *) data)->pixmap, widget->style->black_gc, FALSE,
		      xmin, ymin, xmax - xmin, ymax - ymin);

   /* Now invalidate the affected region of the drawing area. */
   /*gdk_window_invalidate_rect(widget->window, &sc->selection_update_rect, FALSE);*/
}

void show_hide_sub_networks(GtkWidget * widget, gpointer data)
{
#ifndef LETO
    /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
    if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto)==0) return;
#endif

    sc->show_sub_networks=1-sc->show_sub_networks;
    /*TxEffacerAireDessin2(&fenetre1, fenetre1.width, fenetre1.height);*/
    TxEffacerAireDessin(((t_gennet_script *) data)->onglet_leto,sc->couleur_fond);
    TxFlushDessin(((t_gennet_script *) data)->onglet_leto, 0, 0, ((t_gennet_script *) data)->onglet_leto->width, ((t_gennet_script *) data)->onglet_leto->height);
    regenerer_test(((t_gennet_script *) data)->onglet_leto);
}


/******************************************************************************************/

void regenerer_test(TxDonneesFenetre *onglet_leto)
{
  type_groupe *groupe1, *groupe2, *groupe;
  type_liaison *liaison;
  TxPoint point;
  int i, j, largeur = 0, hauteur = 0;

  debug_printf("regenerer_test: onglet = %p, pixmap = %p\n", (void *) onglet_leto, (void *) onglet_leto->pixmap);
  
  if (!GDK_IS_DRAWABLE(onglet_leto->pixmap))
  {
     return;
  }

  debug_printf("regenerer_test 2\n");

  init_cairo(onglet_leto);


  groupe = sc->deb_groupe;
  liaison = sc->deb_liaison;
  
  point.x = sc->xmin;
  point.y = sc->ymin;
  TxDessinerRectangle(onglet_leto, sc->couleur_fond, TxPlein, point,
		      sc->xmax - sc->xmin, sc->ymax - sc->ymin, 0);/* background */
  for (i = 0; i < limite_nbre_max_ech; i++)
    {
      sc->boite_x_min[i] = 999999;
      sc->boite_y_min[i] = 999999;
      sc->boite_x_max[i] = 0;
      sc->boite_y_max[i] = 0;
    }
  
  sc->nbre_max_ech = -1;
  while (groupe != NULL)
    {
      point.x = groupe->posx;
      point.y = groupe->posy;
 
      if (groupe->posx > sc->xmin && groupe->posy > sc->ymin && groupe->posx < sc->xmax && groupe->posy < sc->ymax)
	 if (sc->display_plane[abs(groupe->reverse)] == 1 && (groupe->deja_active==0 || sc->show_sub_networks==1))
	  {
	    if (groupe->type != No_Sub_Network )
	       affiche_groupe(point, groupe,onglet_leto);
	    else
	       affiche_macro(point, groupe,onglet_leto);
	  }
      
      if (groupe->ech_temps > sc->nbre_max_ech)
	sc->nbre_max_ech = groupe->ech_temps;
      if (sc->boite_x_min[groupe->ech_temps] > point.x)
	sc->boite_x_min[groupe->ech_temps] = point.x;
      if (sc->boite_y_min[groupe->ech_temps] > point.y)
	sc->boite_y_min[groupe->ech_temps] = point.y;
      if (sc->boite_x_max[groupe->ech_temps] < point.x)
	sc->boite_x_max[groupe->ech_temps] = point.x;
      if (sc->boite_y_max[groupe->ech_temps] < point.y)
	sc->boite_y_max[groupe->ech_temps] = point.y;
      groupe = groupe->s;
    }
  
  liaison = sc->deb_liaison;
  while (liaison != NULL)
    {
      if (liaison->depart >= 0)   /* ne pas s'occuper de la premiere liaison qui lie micro-neurone et macro-neurone */
	/* typage general, pas vraie connexion */
        {
	   groupe1 = trouver_groupe(liaison->depart);
	  if(groupe1==NULL) 
	    {
	      printf("(fct regenerer_test(onglet_leto)) Le gpe1 %d %s de la liaison n'a pas ete trouve liaison->depart \n",liaison->depart, liaison->depart_name);
	      printf("\nListe des groupes existants = \n\n"); affiche_liste_groupes();
	      exit(1);
	    }
	  groupe2 = trouver_groupe(liaison->arrivee);
	  if(groupe2==NULL) 
	    {
	      printf("(fct regenerer_test(onglet_leto)) Le gpe2 %d %s de la liaison n'a pas ete trouve liaison->depart \n",liaison->arrivee,liaison->arrivee_name);
	      printf("\nListe des groupes existants = \n\n"); affiche_liste_groupes();
	      exit(1);
	    }
	  
	  if ((groupe1->posx < sc->xmin && groupe2->posx < sc->xmin) ||
	      (groupe1->posx > sc->xmax && groupe2->posx > sc->xmax) ||
	      (groupe1->posy < sc->ymin && groupe2->posy < sc->ymin) ||
	      (groupe1->posy > sc->ymax && groupe2->posy > sc->ymax))
            {
	      liaison = liaison->s;
	      continue;
            }
	  if (sc->display_plane[abs(groupe1->reverse)] == 1
	      && sc->display_plane[abs(groupe2->reverse)] == 1
	      && ( sc->show_sub_networks==1 || liaison->deja_active==0))
	    
	    affiche_liaison(liaison, groupe1, groupe2, onglet_leto);
        }
      liaison = liaison->s;
    }
  
  for (i = 0; i <= sc->nbre_max_ech; i++)
    for (j = i + 1; j <= sc->nbre_max_ech; j++)
      {
	
	if (sc->boite_x_min[j] > sc->boite_x_min[i])
	  sc->boite_x_min[j] = sc->boite_x_min[i];
	if (sc->boite_y_min[j] > sc->boite_y_min[i])
	  sc->boite_y_min[j] = sc->boite_y_min[i];
	if (sc->boite_x_max[j] < sc->boite_x_max[i])
	  sc->boite_x_max[j] = sc->boite_x_max[i];
	if (sc->boite_y_max[j] < sc->boite_y_max[i])
	  sc->boite_y_max[j] = sc->boite_y_max[i];
      }
  
  for (i = 0; i <= sc->nbre_max_ech; i++)
    {
      if (sc->boite_x_min[i] <= sc->boite_x_max[i]
	  && sc->boite_y_min[i] <= sc->boite_y_max[i])
        {
	  
	  point.x = sc->boite_x_min[i] - distc - 2 * deltax;
	  point.y = sc->boite_y_min[i] - (2 * deltay + i * 5);
	  largeur = sc->boite_x_max[i] - point.x + distc + 2 * deltax + 5 * i;
	  hauteur = sc->boite_y_max[i] - point.y + 4 * deltay + 5 * i;
	  TxDessinerRectangle(onglet_leto, lut_g[i], TxVide, point, largeur,
			      hauteur, 2);
        }
    }

  TxFlushDessin(onglet_leto, sc->xmin, sc->ymin, sc->xmax - sc->xmin, sc->ymax - sc->ymin);
  set_widgets_sensitivity(onglet_leto);
}



/*********************************************************************************************************************/


void aide()
{
    TxPoint point1, point2;
    char s[255];

    TxEffacerAireDessin(&fenetre2, sc->couleur_fond);


    point1.x = 15;
    point1.y = 50;
    point2.x = 50;
    point2.y = 50;
    affiche_liaison_b(&fenetre2, point1, point2, lut_l[No_l_1_1_modif]);
    memcpy(s, "link : 1 input per neuron", (strlen("link : 1 input per neuron")+1) * sizeof(char));
    point2.x = point2.x + 20;
    TxEcrireChaine(&fenetre2, sc->couleur_texte, point2, s, NULL);
    point1.x = 15;
    point1.y = 80;
    point2.x = 50;
    point2.y = 80;
    affiche_liaison_b(&fenetre2, point1, point2, lut_l[No_l_1_a]);
    memcpy(s, "link : neuron connected all the possible inputs", (strlen("link : neuron connected all the possible inputs")+1) * sizeof(char));
    point2.x = point2.x + 20;
    TxEcrireChaine(&fenetre2, sc->couleur_texte, point2, s, NULL);
    point1.x = 15;
    point1.y = 120;
    point2.x = 50;
    point2.y = 120;
    affiche_liaison_b(&fenetre2, point1, point2, lut_l[No_l_1_v]);
    memcpy(s, "link : neuron connected a neighborhood", (strlen("link : neuron connected a neighborhood")+1) * sizeof(char));
    point2.x = point2.x + 20;
    TxEcrireChaine(&fenetre2, sc->couleur_texte, point2, s, NULL);
    point1.x = 15;
    point1.y = 150;
    point2.x = 50;
    point2.y = 150;
    affiche_liaison_b(&fenetre2, point1, point2, lut_l[No_l_1_1_non_modif]);
    memcpy(s, "link: 1 to 1 inconditional link", (strlen("link: 1 to 1 inconditional link")+1) * sizeof(char));
    point2.x = point2.x + 20;
    TxEcrireChaine(&fenetre2, sc->couleur_texte, point2, s, NULL);
}

void invert_colors(GtkWidget * widget, gpointer data)
{
#ifndef LETO
    /* controle si on est dans un onglet Leto
    * ( l'onglet Metaleto etant le numero 0 )
    */
    if(tab_is_Metaleto(((t_gennet_script *) data)->onglet_leto)==0) return;

puts("desactive car genere des erreurs, il faudrait peut etre modifie TxEffacerAireDessin pour Metaleto");

#else
    int tmp;

    tmp = sc->couleur_fond;
    sc->couleur_fond = sc->couleur_texte;
    sc->couleur_texte = tmp;

    TxEffacerAireDessin(((t_gennet_script *) data)->onglet_leto, sc->couleur_fond);
    TxFlushDessin(((t_gennet_script *) data)->onglet_leto, 0, 0, ((t_gennet_script *) data)->onglet_leto->width, ((t_gennet_script *) data)->onglet_leto->height);
    regenerer_test(((t_gennet_script *) data)->onglet_leto);
#endif
}
