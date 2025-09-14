#pragma once

#include <adwaita.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define WALLY_TYPE_PREFERENCES_WINDOW (wally_preferences_window_get_type())

typedef struct _WallyPreferencesWindow WallyPreferencesWindow;
typedef struct _WallyPreferencesWindowClass WallyPreferencesWindowClass;

struct _WallyPreferencesWindowClass
{
    AdwPreferencesWindowClass parent_class;
};

GType wally_preferences_window_get_type(void) G_GNUC_CONST;
WallyPreferencesWindow *wally_preferences_window_new(void);

G_END_DECLS
