#include "functions.h"
#include <boost/filesystem.hpp>
#include <algorithm>
#include <stack>

std::string currentPath = "."; // Variável global para armazenar o caminho atual

// Função para listar os diretórios em um diretório
std::vector<std::string> listDirectories(const std::string &directory) {
    std::vector<std::string> contents;
    if (boost::filesystem::exists(directory) && boost::filesystem::is_directory(directory)) {
        for (auto& entry : boost::filesystem::directory_iterator(directory)) {
            // Adiciona tanto diretórios quanto arquivos
            contents.push_back(entry.path().filename().string());
        }
    }
    return contents;
}


std::stack<std::string> directoryStack;

void on_changed(GtkTreeSelection *selection, GtkListStore *store) {
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *value;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, 0, &value, -1);

        if (value != NULL) { // Check if value is not NULL
            std::string selectedDirectory(value);
            boost::filesystem::path selectedPath = boost::filesystem::absolute(currentPath + "/" + selectedDirectory);

            if (boost::filesystem::exists(selectedPath) && boost::filesystem::is_directory(selectedPath)) {
                // Push the current path onto the stack before changing it
                directoryStack.push(currentPath);

                currentPath = selectedPath.string();

                gtk_list_store_clear(store);
                std::vector<std::string> directories = listDirectories(currentPath);
                for (const auto &directory : directories) {
                    gtk_list_store_append(store, &iter);
                    gtk_list_store_set(store, &iter, 0, directory.c_str(), -1);
                }
            }

            g_free(value);
        }
    }
}

//void on_changed(GtkTreeSelection *selection, GtkListStore *store) {
//    GtkTreeIter iter;
//    GtkTreeModel *model;
//    char *value;
//
//    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
//        gtk_tree_model_get(model, &iter, 0, &value, -1);
//
//        if (value != NULL) {
//            std::string selectedDirectory(value);
//            boost::filesystem::path selectedPath = boost::filesystem::absolute(boost::filesystem::path(currentPath) / selectedDirectory);
//
//            if (boost::filesystem::exists(selectedPath) && boost::filesystem::is_directory(selectedPath)) {
//                directoryStack.push(currentPath);
//                currentPath = selectedPath.string();
//
//                // Temporarily block the "changed" signal to prevent recursive calls
//                g_signal_handlers_block_by_func(G_OBJECT(selection), (gpointer)on_changed, store);
//
//                gtk_list_store_clear(store);
//                std::vector<std::string> directories = listDirectories(currentPath);
//                for (const auto &directory : directories) {
//                    gtk_list_store_append(store, &iter);
//                    gtk_list_store_set(store, &iter, 0, directory.c_str(), -1);
//                }
//
//                // Reconnect the "changed" signal after updating the list store
//                g_signal_handlers_unblock_by_func(G_OBJECT(selection), (gpointer)on_changed, store);
//            }
//
//            g_free(value);
//        }
//    }
//}


void on_search_changed(GtkSearchEntry *entry, GtkListStore *store) {
    const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

    // Convert the search text to lower case
    std::string lowerText(text);
    std::transform(lowerText.begin(), lowerText.end(), lowerText.begin(), ::tolower);

    // Clear the GtkListStore
    gtk_list_store_clear(store);

    // Fill the GtkListStore with the directories that match the search text
    std::vector<std::string> directories = listDirectories(currentPath);
    for (const std::string &directory : directories) {
        // Convert the directory name to lower case
        std::string lowerDirectory(directory);
        std::transform(lowerDirectory.begin(), lowerDirectory.end(), lowerDirectory.begin(), ::tolower);

        if (lowerDirectory.find(lowerText) != std::string::npos) { // Check if the directory name contains the search text
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, directory.c_str(), -1);
        }
    }
}

void on_row_activated(GtkTreeView *tree_view, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data) {
    GtkTreeModel *model = gtk_tree_view_get_model(tree_view);
    GtkTreeIter iter;

    if (gtk_tree_model_get_iter(model, &iter, path)) {
        char *value;
        gtk_tree_model_get(model, &iter, 0, &value, -1);

        if (value != NULL) {
            std::string fullPath = currentPath + "/" + std::string(value);
            if (!boost::filesystem::is_directory(fullPath)) { // Check if the selected item is a file
                GError *error = NULL;
                gchar *uri = g_filename_to_uri(fullPath.c_str(), NULL, &error);

                if (uri != NULL) {
                    g_app_info_launch_default_for_uri(uri, NULL, &error);
                    g_free(uri);
                }

                if (error != NULL) {
                    g_printerr("Failed to open file: %s\n", error->message);
                    g_error_free(error);
                }

                g_free(value);
            }
        }
    }
}


// Função para retornar ao diretório anterior
void goBack(GtkButton *button, GtkListStore *store) {
    // Convert the current path to an absolute path
    boost::filesystem::path absoluteCurrentPath = boost::filesystem::absolute(currentPath);

    // Get the parent directory path
    boost::filesystem::path parentPath = absoluteCurrentPath.parent_path();

    // Check if the parent directory is not empty (this would happen if currentPath was the root directory)
    if (!parentPath.empty()) {
        currentPath = parentPath.string();

        // Clear the GtkListStore
        gtk_list_store_clear(store);

        // Fill the GtkListStore with the contents of the parent directory
        std::vector<std::string> directories = listDirectories(currentPath);
        for (const std::string &directory : directories) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, directory.c_str(), -1);
        }
    }
}

void color_cell_data_func(GtkTreeViewColumn *col, GtkCellRenderer *renderer, GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data) {
    char *value;
    gtk_tree_model_get(model, iter, 0, &value, -1);

    if (value != NULL) {
        std::string fullPath = currentPath + "/" + std::string(value);
        std::string color = boost::filesystem::is_directory(fullPath) ? "red" : "blue";
        int weight = boost::filesystem::is_directory(fullPath) ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL;

        g_object_set(renderer, "foreground", color.c_str(), "weight", weight, NULL);
        g_free(value);
    }
}
