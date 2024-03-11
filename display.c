#include <gtk/gtk.h>

// Function to set up and show the window
static void activate(GtkApplication* app, gpointer user_data) {
    // Create a new window
    GtkWidget *window = gtk_application_window_new(app);

    // Set the window title and default size
    gtk_window_set_title(GTK_WINDOW(window), "Hello, GTK!");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);

    // Create a label widget with a message
    GtkWidget *label = gtk_label_new("Hello, GTK World!");

    // Add the label to the window
    gtk_container_add(GTK_CONTAINER(window), label);

    // Show all widgets in the window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    // Create a new GTK application
    GtkApplication *app = gtk_application_new("com.example.MyApplication", G_APPLICATION_FLAGS_NONE);

    // Connect the activate signal to the activate function
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Release resources
    g_object_unref(app);

    return status;
}
