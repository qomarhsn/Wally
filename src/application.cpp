#include "config.h"
#include "application.h"
#include "preferences-window.h"

#include <glib/gi18n.h>

struct _WallyApplication
{
    AdwApplication parent_instance;
};

G_DEFINE_FINAL_TYPE(WallyApplication, wally_application, ADW_TYPE_APPLICATION)

static void
wally_application_activate(GApplication *app)
{
    GtkWindow *window;

    g_assert(WALLY_IS_APPLICATION(app));

    // Get the current window or create one if necessary
    window = gtk_application_get_active_window(GTK_APPLICATION(app));
    if (window == NULL)
        window = GTK_WINDOW(wally_preferences_window_new());

    gtk_application_add_window(GTK_APPLICATION(app), window);
    gtk_window_present(window);
}

static void
wally_application_class_init(WallyApplicationClass *klass)
{
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

    app_class->activate = wally_application_activate;
}

static void
wally_application_init(WallyApplication *self)
{
    // Set application properties
    g_object_set(self,
                 "application-id", APP_ID,
                 NULL);
}

WallyApplication *
wally_application_new(const char *application_id, GApplicationFlags flags)
{
    g_return_val_if_fail(application_id != NULL, NULL);

    return static_cast<WallyApplication*>(g_object_new(WALLY_TYPE_APPLICATION,
                                                       "application-id", application_id,
                                                       "flags", flags,
                                                       NULL));
}
