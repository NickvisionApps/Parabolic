#include <adwaita.h>
#include "helpers/listitemfactory.h"

void setup_listitem_cb(GtkListItemFactory *factory, GtkListItem *item)
{
  GtkWidget *label_widget = gtk_label_new("");
  gtk_label_set_ellipsize(GTK_LABEL (label_widget), PANGO_ELLIPSIZE_NONE);

  gtk_list_item_set_child(item, label_widget);
}

void bind_listitem_cb(GtkListItemFactory *factory, GtkListItem *item, GtkStringList *list)
{
  GtkWidget *label = gtk_list_item_get_child(item);
  guint pos = gtk_list_item_get_position(item);

  const char *str = gtk_string_list_get_string(list, pos);

  gtk_label_set_label(GTK_LABEL (label), str);
}
