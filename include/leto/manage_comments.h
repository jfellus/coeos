extern GtkWidget *creer_buffer_texte(GtkWidget * window, TxDonneesFenetre *onglet_leto);

extern void entry_group_callback_comment(GtkWidget * widget, GtkWidget * entry,  void* user_data);
extern void entry_link_callback_comment(GtkWidget * widget, GtkWidget * entry,  void* user_data);

extern GtkWidget *InsereCommentdansFormulaire(GtkWidget * fenetre, GtkWidget * table, TxChampsFormulaire * champs);

extern void update_champs_comment(TxChampsFormulaire * champs, type_noeud_comment *comment);
