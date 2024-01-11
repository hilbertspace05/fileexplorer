// functions.h
#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <gtk/gtk.h>
#include <string>
#include <vector>

std::vector<std::string> listDirectories(const std::string &directory);
void on_changed(GtkTreeSelection *selection, GtkListStore *store);
void on_search_changed(GtkSearchEntry *entry, GtkListStore *store);
void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data);
void goBack(GtkButton *button, GtkListStore *store);
void color_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data);

#endif
