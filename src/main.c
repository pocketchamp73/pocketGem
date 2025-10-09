#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *splash_label;
    GtkWidget *main_container;
    GtkTextBuffer *response_buffer;
    GtkWidget *input_entry;
} AppWidgets;

// Forward declarations
static void show_main_ui(AppWidgets *widgets);
static void on_submit_clicked(GtkWidget *button, gpointer user_data);
static void on_entry_activate(GtkWidget *entry, gpointer user_data);
static gboolean hide_splash(gpointer user_data);

// TODO: Implement actual Gemini API call
static char* send_to_gemini(const char *question) {
    // Placeholder - will be replaced with actual API call
    char *response = g_strdup_printf("You asked: %s\n\nThis is a placeholder response. "
                                     "Gemini API integration will be added here.", question);
    return response;
}

static void on_submit_clicked(GtkWidget *button, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const char *question = gtk_entry_get_text(GTK_ENTRY(widgets->input_entry));

    if (strlen(question) > 0) {
        // Get response from Gemini
        char *response = send_to_gemini(question);

        // Append to text buffer
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(widgets->response_buffer, &end);

        // Add question
        gtk_text_buffer_insert(widgets->response_buffer, &end, "\n> ", -1);
        gtk_text_buffer_insert(widgets->response_buffer, &end, question, -1);
        gtk_text_buffer_insert(widgets->response_buffer, &end, "\n\n", -1);

        // Add response
        gtk_text_buffer_insert(widgets->response_buffer, &end, response, -1);
        gtk_text_buffer_insert(widgets->response_buffer, &end, "\n", -1);

        g_free(response);

        // Clear input
        gtk_entry_set_text(GTK_ENTRY(widgets->input_entry), "");
    }
}

static void on_entry_activate(GtkWidget *entry, gpointer user_data) {
    on_submit_clicked(NULL, user_data);
}

static void show_main_ui(AppWidgets *widgets) {
    GtkWidget *vbox, *text_scroll, *text_view, *input_box, *submit_button;

    // Create main vertical box
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 5);

    // Create scrollable text view for responses (top half)
    text_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);

    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    widgets->response_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    gtk_text_buffer_set_text(widgets->response_buffer,
                            "Welcome to pocketGem!\nAsk Gemini a question below.", -1);

    gtk_container_add(GTK_CONTAINER(text_scroll), text_view);
    gtk_box_pack_start(GTK_BOX(vbox), text_scroll, TRUE, TRUE, 0);

    // Create input box (bottom part)
    input_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    widgets->input_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->input_entry), "Ask a question...");
    g_signal_connect(widgets->input_entry, "activate", G_CALLBACK(on_entry_activate), widgets);
    gtk_box_pack_start(GTK_BOX(input_box), widgets->input_entry, TRUE, TRUE, 0);

    submit_button = gtk_button_new_with_label("Send");
    g_signal_connect(submit_button, "clicked", G_CALLBACK(on_submit_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(input_box), submit_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), input_box, FALSE, FALSE, 0);

    // Store main container
    widgets->main_container = vbox;

    gtk_container_add(GTK_CONTAINER(widgets->window), vbox);
    gtk_widget_show_all(widgets->window);
}

static gboolean hide_splash(gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    // Remove splash screen
    gtk_widget_destroy(widgets->splash_label);

    // Show main UI
    show_main_ui(widgets);

    // Focus on input field
    gtk_widget_grab_focus(widgets->input_entry);

    return G_SOURCE_REMOVE;
}

static void activate(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = g_new0(AppWidgets, 1);

    widgets->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->window), "pocketGem");
    gtk_window_set_default_size(GTK_WINDOW(widgets->window), 480, 272);

    // Show splash screen
    widgets->splash_label = gtk_label_new("pocketGem - Gemini for pocketCHIP");
    gtk_container_add(GTK_CONTAINER(widgets->window), widgets->splash_label);
    gtk_widget_show_all(widgets->window);

    // Schedule transition to main UI after 2 seconds
    g_timeout_add_seconds(2, hide_splash, widgets);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("com.pocketchip.pocketgem", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
