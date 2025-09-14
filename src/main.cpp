#include "config.h"
#include "application.h"

#include <glib/gi18n.h>
#include <adwaita.h>

int main(int argc, char *argv[])
{
    g_autoptr(WallyApplication) app = NULL;
    int ret;

    // Initialize internationalization
    bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
    bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
    textdomain(GETTEXT_PACKAGE);

    // Initialize Adwaita
    adw_init();

    // Create application
    app = wally_application_new(APP_ID, G_APPLICATION_DEFAULT_FLAGS);
    ret = g_application_run(G_APPLICATION(app), argc, argv);

    return ret;
}
