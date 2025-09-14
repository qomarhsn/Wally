#include "settings-manager.h"
#include "config.h"

#include <glib/gi18n.h>

struct _WallySettingsManager
{
    GObject parent_instance;
    
    GSettings *app_settings;
    GSettings *interface_settings;
};

G_DEFINE_FINAL_TYPE(WallySettingsManager, wally_settings_manager, G_TYPE_OBJECT)

static void
wally_settings_manager_dispose(GObject *object)
{
    WallySettingsManager *self = WALLY_SETTINGS_MANAGER(object);
    
    g_clear_object(&self->app_settings);
    g_clear_object(&self->interface_settings);
    
    G_OBJECT_CLASS(wally_settings_manager_parent_class)->dispose(object);
}

static void
wally_settings_manager_class_init(WallySettingsManagerClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    
    object_class->dispose = wally_settings_manager_dispose;
}

static void
wally_settings_manager_init(WallySettingsManager *self)
{
    // Initialize app settings
    self->app_settings = g_settings_new(APP_ID);
    
    // Initialize GNOME interface settings for theme detection
    self->interface_settings = g_settings_new("org.gnome.desktop.interface");
}

WallySettingsManager *
wally_settings_manager_new(void)
{
    return static_cast<WallySettingsManager*>(g_object_new(WALLY_TYPE_SETTINGS_MANAGER, NULL));
}

GSettings *
wally_settings_manager_get_settings(WallySettingsManager *self)
{
    g_return_val_if_fail(WALLY_IS_SETTINGS_MANAGER(self), NULL);
    
    return self->app_settings;
}

gboolean
wally_settings_manager_is_dark_theme(WallySettingsManager *self)
{
    g_return_val_if_fail(WALLY_IS_SETTINGS_MANAGER(self), FALSE);
    
    g_autofree char *color_scheme = g_settings_get_string(self->interface_settings, "color-scheme");
    
    // Check if the color scheme indicates dark theme
    return g_strcmp0(color_scheme, "prefer-dark") == 0;
}

void
wally_settings_manager_monitor_theme_changes(WallySettingsManager *self,
                                             GCallback callback,
                                             gpointer user_data)
{
    g_return_if_fail(WALLY_IS_SETTINGS_MANAGER(self));
    g_return_if_fail(callback != NULL);
    
    // Monitor changes to the color scheme
    g_signal_connect(self->interface_settings, "changed::color-scheme",
                     callback, user_data);
}
