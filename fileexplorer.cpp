#include "functions.h"
#include <gtk/gtk.h>
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <unistd.h>


int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Crie uma janela GTK
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    // Crie um GtkTreeView para exibir os diretórios
    GtkWidget *view = gtk_tree_view_new();
    //gtk_box_pack_start(GTK_BOX(box), view, TRUE, TRUE, 0);

    // Crie um GtkListStore para armazenar os diretórios
    GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
    // Crie um GtkCellRenderer para renderizar os diretórios
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1, "Name", renderer, "text", 0, NULL);
    GtkTreeViewColumn *column = gtk_tree_view_get_column(GTK_TREE_VIEW(view), 0);
    gtk_tree_view_column_set_cell_data_func(column, renderer, color_cell_data_func, NULL, NULL);
    gtk_tree_view_set_model(GTK_TREE_VIEW(view), GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkWidget *button = gtk_button_new_with_label("Back");
    g_signal_connect(button, "clicked", G_CALLBACK(goBack), store);
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    GtkWidget *search_entry = gtk_search_entry_new();
    g_signal_connect(search_entry, "search-changed", G_CALLBACK(on_search_changed), store);
    gtk_box_pack_start(GTK_BOX(box), search_entry, FALSE, FALSE, 0);

    g_signal_connect(view, "row-activated", G_CALLBACK(on_row_activated), NULL);

    // Create a GtkScrolledWindow
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);

    // Add the GtkTreeView to the GtkScrolledWindow
    gtk_container_add(GTK_CONTAINER(scrolled_window), view);

    // Add the GtkScrolledWindow to the box

    // Crie um GtkCellRenderer para renderizar os diretórios

    // Obtenha a seleção do GtkTreeView
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));

    // Conecte o sinal "changed" ao callback on_changed
    g_signal_connect(G_OBJECT(selection), "changed", G_CALLBACK(on_changed), store);

    // Adicione os diretórios ao GtkListStore
    std::vector<std::string> directories = listDirectories(".");
    for (const std::string &directory : directories) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, directory.c_str(), -1);
    }

    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    // Adicione o GtkTreeView à janela
    gtk_container_add(GTK_CONTAINER(window), box);

    // Exiba a janela
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
