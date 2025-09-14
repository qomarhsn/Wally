#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define WALLY_TYPE_APPLICATION (wally_application_get_type())

G_DECLARE_FINAL_TYPE(WallyApplication, wally_application, WALLY, APPLICATION, AdwApplication)

WallyApplication *wally_application_new(const char *application_id, GApplicationFlags flags);

G_END_DECLS
