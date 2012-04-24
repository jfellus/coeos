#ifndef EXPORT_H
#define EXPORT_H

int export(const int format, const char *nom,
           const type_groupe * liste_groupe,
           const type_liaison * liste_liaison);
void DessinerGroupe(int id, type_groupe * gpe);
void DessinerLiaison(int id, type_liaison * liaison, type_groupe * groupe1,
                     type_groupe * groupe2);
void DessinerRectangle(int id, int couleur, int rempli, TxPoint point, int dx,
                       int dy, int epaisseur);
void DessinerSegment(int id, int couleur, TxPoint deb, TxPoint fin,
                     int epaisseur);
void DessinerCercle(int id, int couleur, int rempli, TxPoint centre,
                    int diametre, int epaisseur);
void EcrireChaine(int id, int couleur, TxPoint position, char *chaine,
                  char *font);
void DessinerFlecheHorizontale(int id, int couleur, TxPoint position,
                               int largeur, int epaisseur);
void DessinerPolygone(int id, int couleur, int rempli, TxPoint * points,
                      int taille, int epaisseur);
void ChangerPinceau(int id, int couleur);
#endif
