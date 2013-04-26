/*
  ce fichier contient les fonctions d'export vers du PS, PNG, jpeg, Xfig,
  tout ce machin utilise la libg2 (g2.sourceforge.net)


  il suffit d'appeler la fonction export(format,nom_fichier,liste_chainee_groupe,liste_chainee_liaison)
*/
#ifdef EXPORT

#include "export.h"

#include <g2.h>
#include <g2_FIG.h>

#define PLEIN 1
#define VIDE 0


int test_dessiner()
{

    export(0, "export_script.fig", deb_groupe, deb_liaison);
    return 0;
}



/* code proche de regenere */
int export(const int format, const char *nom,
           const type_groupe * liste_groupe,
           const type_liaison * liste_liaison)
{

    type_groupe *gpe, *groupe1, *groupe2;
    type_liaison *link;
    int i, x, y, id, j, hauteur, largeur;
    TxPoint point;



    /*
       verification du format
     */
    /* pour le moment uniquement du XFIG */

    /*
       determiner la taille de l'image
       inutile pour les FIG si un jour il ya support d'autre chose ...
     */
    /*
       minx = miny = 999999;
       maxx = maxy = 0;
       gpe=deb_groupe;
       while(gpe!=NULL){
       x=gpe->posx;
       y=gpe->posy;
       if(x>maxx) maxx=x;
       if(x<minx) minx=x;
       if(maxy>y) maxy=y;
       if(miny<y) miny=y;
       gpe=gpe->s;
       } */

    /*
       ouvrir l'image
     */
    id = g2_open_FIG(nom);
    g2_set_auto_flush(id, 0);
    g2_allocate_basic_colors(id);
    g2_pen(id, 1);
    g2_set_coordinate_system(id, 0., 0., 1., -1.);
    g2_set_font_size(id, 16.);

    /*
       dessiner le reseau :
       1. les groupes
       2. les liens
       3. les echelles de temps
     */
    /* 1. les groupes */
    gpe = liste_groupe;
    while (gpe != NULL)
    {
        DessinerGroupe(id, gpe);
        gpe = gpe->s;
    }





    /* 2. les liaisons */
    link = liste_liaison;
    while (link != NULL)
    {
        if (link->depart >= 0)
        {
            groupe1 = trouver_groupe(link->depart);
            groupe2 = trouver_groupe(link->arrivee);
            if (groupe1 != NULL && groupe2 != NULL
                && display_plane[abs(groupe1->reverse)] == 1
                && display_plane[abs(groupe2->reverse)] == 1)
            {

                DessinerLiaison(id, link, groupe1, groupe2);
            }
        }
        link = link->s;
    }


    /* 3. les echelles */

    for (i = 0; i < limite_nbre_max_ech; i++)
    {
        boite_x_min[i] = 999999;
        boite_y_min[i] = 999999;
        boite_x_max[i] = 0;
        boite_y_max[i] = 0;
    }

    nbre_max_ech = -1;
    gpe = liste_groupe;
    while (gpe != NULL)
    {
        point.x = gpe->posx;
        point.y = gpe->posy;
        if (gpe->ech_temps > nbre_max_ech)
            nbre_max_ech = gpe->ech_temps;
        if (boite_x_min[gpe->ech_temps] > point.x)
            boite_x_min[gpe->ech_temps] = point.x;
        if (boite_y_min[gpe->ech_temps] > point.y)
            boite_y_min[gpe->ech_temps] = point.y;
        if (boite_x_max[gpe->ech_temps] < point.x)
            boite_x_max[gpe->ech_temps] = point.x;
        if (boite_y_max[gpe->ech_temps] < point.y)
            boite_y_max[gpe->ech_temps] = point.y;
        gpe = gpe->s;
    }

    for (i = 0; i <= nbre_max_ech; i++)
        for (j = i + 1; j <= nbre_max_ech; j++)
        {
            if (boite_x_min[j] > boite_x_min[i])
                boite_x_min[j] = boite_x_min[i];
            if (boite_y_min[j] > boite_y_min[i])
                boite_y_min[j] = boite_y_min[i];
            if (boite_x_max[j] < boite_x_max[i])
                boite_x_max[j] = boite_x_max[i];
            if (boite_y_max[j] < boite_y_max[i])
                boite_y_max[j] = boite_y_max[i];
        }

    for (i = 0; i <= nbre_max_ech; i++)
    {
        if (boite_x_min[i] <= boite_x_max[i]
            && boite_y_min[i] <= boite_y_max[i])
        {
            point.x = boite_x_min[i] - distc - 2 * deltax;
            point.y = boite_y_min[i] - (2 * deltay + i * 5);
            largeur = boite_x_max[i] - point.x + distc + 2 * deltax + 5 * i;
            hauteur = boite_y_max[i] - point.y + 4 * deltay + 5 * i;
            DessinerRectangle(id, 0, VIDE, point, largeur, hauteur, 1);
        }
    }



    /*
       fermer le fichier
     */
    g2_flush(id);
    g2_close(id);
    /*
       si tout c'est bien passe
     */
    return 0;
}








void DessinerGroupe(int id, type_groupe * groupe)
{

    TxPoint point, point2, point3, point4, point5, point6;


    point.x = groupe->posx;
    point.y = groupe->posy;

    point.x = point.x - deltax;
    point.y = point.y - deltay;

    point2.x = point.x + deltax / 2;    /* position de l'ecriture du no du gpe */
    point2.y = point.y + 3 * deltay / 2;

    point3.x = point.x - distc;
    point3.y = point.y + 2 * deltay;

    point4.x = point.x + 2 * deltax + distc;
    point4.y = point.y + deltay;

    point5.x = point.x + 2 * deltax + distc;
    point5.y = point.y + 3 * deltay;

    DessinerRectangle(id, lut_g[groupe->type % 30], VIDE, point, 2 * deltax,
                      2 * deltay, 1);

    point.y = point.y + 2 * deltay;
    DessinerRectangle(id, lut_g[groupe->type % 30], VIDE, point, 2 * deltax,
                      2 * deltay, 1);

    point.y = point3.y;

    if (groupe->reverse <= 0)
    {
        if (groupe->type != No_Entree)
        {
            DessinerCercle(id, lut_g[groupe->type % 30], PLEIN, point3, 4, 1);
            DessinerSegment(id, lut_g[groupe->type % 30], point3, point, 1);
        }

        DessinerFlecheHorizontale(id, lut_g[groupe->type % 30], point4, 6, 2);
        point.x = point.x + 2 * deltax;
        point.y = point4.y;
        DessinerSegment(id, lut_g[groupe->type % 30], point4, point, 1);

        DessinerFlecheHorizontale(id, lut_g[groupe->type % 30], point5, 6, 2);
        point.y = point5.y;
        DessinerSegment(id, lut_g[groupe->type % 30], point5, point, 1);

    }
    else
    {
        if (groupe->reverse > 0)
        {

            point3.y = point3.y - deltay;
            DessinerFlecheHorizontale(id, lut_g[groupe->type % 30], point3,
                                      -6, 2);
            point.x = point3.x + distc;
            point.y = point3.y;
            DessinerSegment(id, lut_g[groupe->type % 30], point3, point, 1);

            point3.y = point3.y + 2 * deltay;
            DessinerFlecheHorizontale(id, lut_g[groupe->type % 30], point3,
                                      -6, 2);
            point.y = point3.y;
            DessinerSegment(id, lut_g[groupe->type % 30], point3, point, 1);


            if (groupe->type != No_Entree)
            {
                point4.y = point4.y + deltax;
                point.y = point4.y;
                point.x = point4.x - distc;
                DessinerCercle(id, lut_g[groupe->type % 30], PLEIN, point4, 4,
                               1);
                DessinerSegment(id, lut_g[groupe->type % 30], point4, point,
                                1);
            }
        }
    }

    EcrireChaine(id, couleur_texte, point2, Int2Str(groupe->no), NULL);

    point3.x = point3.x + deltax;
    point3.y = point3.y + 5 * deltay;
    EcrireChaine(id, couleur_texte, point3, groupe->nom, NULL);
}




void DessinerLiaison(int id, type_liaison * liaison, type_groupe * groupe1,
                     type_groupe * groupe2)
{
    TxPoint point1, point2;
    TxPoint pointa, pointb, pointc;
    char s[100];
    int type;
    int sens1, sens2, prop;
    int l;

    if (liaison->mode < 2)
    {
        point1.x = groupe1->posx;
        point1.y = groupe1->posy;
    }
    else
    {
        point1.x = groupe1->posx;
        point1.y = groupe1->posy + 2 * deltay;
    }

    point2.x = groupe2->posx;
    point2.y = groupe2->posy + deltay;

    type = liaison->type;

    if (groupe1->reverse <= 0)
        sens1 = 1;
    else
        sens1 = -1;
    if (groupe2->reverse <= 0)
        sens2 = 1;
    else
        sens2 = -1;


    if (groupe1 != groupe2)
    {                           /* liaisons entre 2 gpes differents */
        if (point2.x - sens2 * (deltax + distc) <
            point1.x + sens1 * (deltax + distc))
        {
            l = point2.y - point1.y;
            l = -l / 4;
            point1.x = point1.x + sens1 * (deltax + distc);
            point2.x = point2.x - sens2 * (deltax + distc);
            pointa.x = point1.x;
            pointb.x = point2.x;
            pointa.y = point1.y - l;
            pointb.y = point2.y + l;
            DessinerSegment(id, lut_l[type], point1, pointa, 1);
            DessinerSegment(id, lut_l[type], pointa, pointb, 1);
            DessinerSegment(id, lut_l[type], pointb, point2, 1);
        }
        else
        {
            point1.x = point1.x + sens1 * (deltax + distc);
            point2.x = point2.x - sens2 * (deltax + distc);
            DessinerSegment(id, lut_l[type], point1, point2, 1);
        }
    }
    else
    {                           /* liaisons reentrantes sur le meme gpe */
        point1.x = point1.x + sens1 * (deltax + distc);
        point2.x = point2.x - sens2 * (deltax + distc);
        pointa.x = point1.x;
        pointa.y = point1.y - 3 * deltay * sens1;
        pointc.x = point2.x;
        pointc.y = point2.y - 4 * deltay * sens2;
        pointb.y = pointc.y;
        pointb.x = pointc.x + 2 * (deltax + distc);
        DessinerSegment(id, lut_l[type], point1, pointa, 1);
        DessinerSegment(id, lut_l[type], pointa, pointb, 1);
        DessinerSegment(id, lut_l[type], pointb, pointc, 1);
        DessinerSegment(id, lut_l[type], pointc, point2, 1);
        point1.y = pointa.y;
        point2.y = pointa.y;
    }


    point2.x = (point2.x + point1.x) / 2;
    point2.y = (point2.y + point1.y) / 2;
    point1.x = point2.x;
    point1.y = point2.y;

    if (liaison->type == No_l_algorithmique
        || liaison->type == No_l_neuro_mod)
        EcrireChaine(id, couleur_texte, point1, liaison->nom, NULL);

    point1.x = point2.x + 14;
    if (liaison->type != No_l_algorithmique
        && liaison->type != No_l_neuro_mod)
    {
        if (liaison->mode == 0 || liaison->mode == 2)
            EcrireChaine(id, couleur_texte, point1, "X", NULL);
        else
            EcrireChaine(id, couleur_texte, point1, "d", NULL);
        point1.x = point1.x + 5;
    }
    point1.x = point1.x + 5;
    if (liaison->secondaire == 0)
        EcrireChaine(id, couleur_texte, point1, ".", NULL);

    if (liaison->type == No_l_1_a)
        EcrireChaine(id, couleur_texte, point2, "A", NULL);
    else if (liaison->type == No_l_1_t)
        EcrireChaine(id, couleur_texte, point2, "//", NULL);
    else if (liaison->type == No_l_1_1_non_modif)
        EcrireChaine(id, couleur_texte, point2, "/", NULL);
    else if (liaison->type == No_l_1_1_modif)
        EcrireChaine(id, couleur_texte, point2, "\\", NULL);
    else if (liaison->type == No_l_1_v)
        EcrireChaine(id, couleur_texte, point2, "V", NULL);


    point1.x = point1.x + 10;
    if (liaison->type != No_l_algorithmique
        && liaison->type != No_l_neuro_mod)
        EcrireChaine(id, couleur_texte, point1, liaison->nom, NULL);


}














/* fonctions de dessin */

/*
  dessine un rectangle en point de largeur dx et hauteur dy
  
  TODO :
  color
*/
void DessinerRectangle(int id, int couleur, int rempli, TxPoint point, int dx,
                       int dy, int what)
{
    double x1, y1, x2, y2;


    ChangerPinceau(id, couleur);

    x1 = point.x;
    x2 = point.x + dx;

    y1 = point.y;
    y2 = point.y + dy;

    if (rempli == VIDE)
        g2_rectangle(id, x1, y1, x2, y2);
    else
        g2_filled_rectangle(id, x1, y1, x2, y2);
}


void DessinerSegment(int id, int couleur, TxPoint deb, TxPoint fin, int what)
{

    double x1, y1, x2, y2;
    ChangerPinceau(id, couleur);

    x1 = deb.x;
    y1 = deb.y;

    x2 = fin.x;
    y2 = fin.y;

    g2_line(id, x1, y1, x2, y2);
}


void DessinerCercle(int id, int couleur, int rempli, TxPoint centre,
                    int diametre, int epaisseur)
{
    double x, y, r;
    ChangerPinceau(id, couleur);
    x = (double) centre.x;
    y = (double) centre.y;
    r = (double) diametre;

    if (rempli == VIDE)
        g2_circle(id, x, y, r);
    else
        g2_filled_circle(id, x, y, r);

}


void EcrireChaine(int id, int couleur, TxPoint position, char *chaine,
                  char *font)
{
    double x, y;
    ChangerPinceau(id, couleur);
    x = (double) position.x;
    y = (double) position.y;

    g2_string(id, x, y, chaine);


}



void DessinerFlecheHorizontale(int id, int couleur, TxPoint position,
                               int largeur, int epaisseur)
{
    TxPoint triangle[3], extremite;

    ChangerPinceau(id, couleur);

    extremite.x = position.x + largeur;
    extremite.y = position.y;
    triangle[2].x = extremite.x;
    triangle[2].y = extremite.y;
    if (largeur > 0)
        extremite.x -= 8;
    else
        extremite.x += 8;
    DessinerSegment(id, couleur, position, extremite, epaisseur);
    triangle[0].y = extremite.y - (3 + epaisseur);
    triangle[1].y = extremite.y + (3 + epaisseur);
    triangle[0].x = extremite.x;
    triangle[1].x = extremite.x;
    DessinerPolygone(id, couleur, TxPlein, triangle, 3, epaisseur);
}




void DessinerPolygone(int id, int couleur, int rempli, TxPoint * points,
                      int taille, int epaisseur)
{
    int i;
    double *pts;

    ChangerPinceau(id, couleur);

    pts = (double *) malloc(taille * 2 * sizeof(double));
    if (pts == NULL)
    {
        printf
            ("%s(%s:%d) : unable to alloc memory !\n\tno draw of this polygone\n",
             __FUNCTION__, __FILE__, __LINE__);
    }
    else
    {
        for (i = 0; i < taille; i++)
        {
            pts[i * 2] = points[i].x;
            pts[i * 2 + 1] = points[i].y;
        }
        if (rempli == VIDE)
        {
            g2_polygon(id, taille, pts);
        }
        else
        {
            g2_filled_polygon(id, taille, pts);
        }
        free(pts);
    }

}





void ChangerPinceau(int id, int couleur)
{
    switch (couleur)
    {
    case 0:                    /*noir */
        g2_pen(id, 1);
        break;
    case 8:                    /*gris */
        g2_pen(id, 14);
        break;
    case 16:                   /*blanc */
        g2_pen(id, 0);
        break;
    case 17:                   /*rouge */
        g2_pen(id, 19);
        break;
    case 18:                   /*vert */
        g2_pen(id, 7);
        break;
    case 19:                   /*bleu */
        g2_pen(id, 3);
        break;
    case 20:                   /*bleu clair */
        g2_pen(id, 8);
        break;
    case 21:                   /*kaki */
        g2_pen(id, 13);
        break;
    case 22:                   /*violet */
        g2_pen(id, 12);
        break;
    case 28:                   /*bleu fonce */
        g2_pen(id, 2);
        break;
    case 29:                   /*vert */
        g2_pen(id, 4);
        break;
    case 15:                   /*gris */
        g2_pen(id, 15);
        break;
    case 25:                   /*???? */
        g2_pen(id, 10);
        break;
    case 30:                   /*vert */
        g2_pen(id, 16);
        break;
    default:
        printf("couleur inconnue : %d, utilisation du noir", couleur);
        g2_pen(id, 1);
        break;
    }


}

#endif
