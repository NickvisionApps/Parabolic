#ifndef LISTITEMFACTORY_H
#define LISTITEMFACTORY_H

#include <adwaita.h>

// TODO include docstring

void setup_listitem_cb(GtkListItemFactory *factory, GtkListItem *item);

void bind_listitem_cb(GtkListItemFactory *factory, GtkListItem *item, GtkStringList *list);


#endif
