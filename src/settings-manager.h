#pragma once

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

#define WALLY_TYPE_SETTINGS_MANAGER (wally_settings_manager_get_type())

G_DECLARE_FINAL_TYPE(WallySettingsManager, wally_settings_manager, WALLY, SETTINGS_MANAGER, GObject)

WallySettingsManager *wally_settings_manager_new(void);

GSettings *wally_settings_manager_get_settings(WallySettingsManager *self);

gboolean wally_settings_manager_is_dark_theme(WallySettingsManager *self);

void wally_settings_manager_monitor_theme_changes(WallySettingsManager *self,
                                                  GCallback callback,
                                                  gpointer user_data);

G_END_DECLS
