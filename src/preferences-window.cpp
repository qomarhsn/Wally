#include "preferences-window.h"
#include "slideshow-manager.h"
#include "settings-manager.h"
#include "config.h"

#include <glib/gi18n.h>
#include <glib/gstdio.h>

struct _WallyPreferencesWindow
{
    AdwPreferencesWindow parent_instance;
    
    GtkButton *day_folder_button;
    GtkButton *night_folder_button;
    GtkSpinButton *interval_spin;
    GtkButton *apply_button;
    GtkSwitch *same_folder_switch;
    AdwActionRow *night_folder_row;
    AdwSwitchRow *auto_night_mode_switch;
    GtkScale *transition_scale;
    
    WallySettingsManager *settings_manager;
    WallySlideshowManager *slideshow_manager;
    
    char *day_folder_path;
    char *night_folder_path;
};

G_DEFINE_TYPE(WallyPreferencesWindow, wally_preferences_window, ADW_TYPE_PREFERENCES_WINDOW)


static void
on_day_folder_button_clicked(GtkButton *button G_GNUC_UNUSED, WallyPreferencesWindow *self)
{
    GtkFileChooserNative *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    
    chooser = gtk_file_chooser_native_new(_("Select Day Wallpapers Folder"),
                                          GTK_WINDOW(self),
                                          action,
                                          _("Select"),
                                          _("Cancel"));
    
    g_signal_connect(chooser, "response", G_CALLBACK(+[](GtkFileChooserNative *chooser, int response, WallyPreferencesWindow *self) {
        if (response == GTK_RESPONSE_ACCEPT) {
            GtkFileChooser *file_chooser = GTK_FILE_CHOOSER(chooser);
            g_autoptr(GFile) file = gtk_file_chooser_get_file(file_chooser);
            
            g_free(self->day_folder_path);
            self->day_folder_path = g_file_get_path(file);
            
            // Update button label
            g_autofree char *basename = g_file_get_basename(file);
            gtk_button_set_label(self->day_folder_button, basename);
            
            gboolean use_same_folder = gtk_switch_get_active(self->same_folder_switch);
            if (use_same_folder) {
                g_free(self->night_folder_path);
                self->night_folder_path = g_strdup(self->day_folder_path);
                gtk_button_set_label(self->night_folder_button, basename);
            }
            
            GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
            g_settings_set_string(settings, "day-folder-path", self->day_folder_path);
            if (use_same_folder) {
                g_settings_set_string(settings, "night-folder-path", self->night_folder_path);
            }
        }
        g_object_unref(chooser);
    }), self);
    
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(chooser));
}

static void
on_night_folder_button_clicked(GtkButton *button G_GNUC_UNUSED, WallyPreferencesWindow *self)
{
    GtkFileChooserNative *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    
    chooser = gtk_file_chooser_native_new(_("Select Night Wallpapers Folder"),
                                          GTK_WINDOW(self),
                                          action,
                                          _("Select"),
                                          _("Cancel"));
    
    g_signal_connect(chooser, "response", G_CALLBACK(+[](GtkFileChooserNative *chooser, int response, WallyPreferencesWindow *self) {
        if (response == GTK_RESPONSE_ACCEPT) {
            GtkFileChooser *file_chooser = GTK_FILE_CHOOSER(chooser);
            g_autoptr(GFile) file = gtk_file_chooser_get_file(file_chooser);
            
            g_free(self->night_folder_path);
            self->night_folder_path = g_file_get_path(file);
            
            g_autofree char *basename = g_file_get_basename(file);
            gtk_button_set_label(self->night_folder_button, basename);
            
            GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
            g_settings_set_string(settings, "night-folder-path", self->night_folder_path);
        }
        g_object_unref(chooser);
    }), self);
    
    gtk_native_dialog_show(GTK_NATIVE_DIALOG(chooser));
}

static void
on_apply_button_clicked(GtkButton *button G_GNUC_UNUSED, WallyPreferencesWindow *self)
{
    GError *error = NULL;
    
    if (!self->day_folder_path || !self->night_folder_path) {
        g_warning("Please select both day and night folders");
        return;
    }
    
    // Create Wally directories
    g_autofree char *wally_dir = g_build_filename(g_get_home_dir(), "Pictures", "Wally", NULL);
    g_autofree char *day_dest = g_build_filename(wally_dir, "DayWallpapers", NULL);
    g_autofree char *night_dest = g_build_filename(wally_dir, "NightWallpapers", NULL);
    
    // Copy wallpapers
    if (!wally_slideshow_manager_copy_wallpapers(self->slideshow_manager, 
                                                 self->day_folder_path, day_dest, &error)) {
        g_warning("Failed to copy day wallpapers: %s", error->message);
        g_error_free(error);
        return;
    }
    
    if (!wally_slideshow_manager_copy_wallpapers(self->slideshow_manager, 
                                                 self->night_folder_path, night_dest, &error)) {
        g_warning("Failed to copy night wallpapers: %s", error->message);
        g_error_free(error);
        return;
    }
    
    // Get settings
    GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
    int interval_minutes = (int)gtk_spin_button_get_value(self->interval_spin);
    int interval = interval_minutes * 60; // Convert minutes to seconds
    double transition = g_settings_get_double(settings, "transition-duration");
    
    // Create slideshow XML files
    g_autofree char *day_xml = g_build_filename(wally_dir, "day-slideshow.xml", NULL);
    g_autofree char *night_xml = g_build_filename(wally_dir, "night-slideshow.xml", NULL);
    
    if (!wally_slideshow_manager_create_slideshow_xml(self->slideshow_manager,
                                                      day_dest, day_xml, interval, transition, &error)) {
        g_warning("Failed to create day slideshow: %s", error->message);
        g_error_free(error);
        return;
    }
    
    if (!wally_slideshow_manager_create_slideshow_xml(self->slideshow_manager,
                                                      night_dest, night_xml, interval, transition, &error)) {
        g_warning("Failed to create night slideshow: %s", error->message);
        g_error_free(error);
        return;
    }
    
    // Apply wallpapers
    // gboolean is_dark = wally_settings_manager_is_dark_theme(self->settings_manager);
    
    if (!wally_slideshow_manager_apply_wallpaper(self->slideshow_manager, day_xml, FALSE, &error)) {
        g_warning("Failed to apply day wallpaper: %s", error->message);
        g_error_free(error);
        return;
    }
    
    if (!wally_slideshow_manager_apply_wallpaper(self->slideshow_manager, night_xml, TRUE, &error)) {
        g_warning("Failed to apply night wallpaper: %s", error->message);
        g_error_free(error);
        return;
    }
    
    // Mark slideshow as enabled
    g_settings_set_boolean(settings, "slideshow-enabled", TRUE);
    
    g_print("Wallpaper settings applied successfully!\n");
}

static void
on_same_folder_switch_toggled(GtkSwitch *switch_widget G_GNUC_UNUSED, GParamSpec *pspec G_GNUC_UNUSED, WallyPreferencesWindow *self)
{
    gboolean use_same_folder = gtk_switch_get_active(self->same_folder_switch);
    
    // Show/hide night folder row based on switch state
    gtk_widget_set_visible(GTK_WIDGET(self->night_folder_row), !use_same_folder);
    
    // If using same folder, copy day folder path to night folder path
    if (use_same_folder && self->day_folder_path) {
        g_free(self->night_folder_path);
        self->night_folder_path = g_strdup(self->day_folder_path);
        
        // Update night folder button label to match day folder
        const char *day_label = gtk_button_get_label(self->day_folder_button);
        if (day_label && g_strcmp0(day_label, "Choose Folder") != 0) {
            gtk_button_set_label(self->night_folder_button, day_label);
        }
        
        GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
        g_settings_set_string(settings, "night-folder-path", self->night_folder_path);
    }
}


static void
on_theme_changed(GSettings *settings G_GNUC_UNUSED, const char *key, WallyPreferencesWindow *self)
{
    if (g_strcmp0(key, "color-scheme") == 0) {
        GSettings *app_settings = wally_settings_manager_get_settings(self->settings_manager);
        gboolean auto_mode = g_settings_get_boolean(app_settings, "auto-night-mode");
        gboolean slideshow_enabled = g_settings_get_boolean(app_settings, "slideshow-enabled");
        
        if (auto_mode && slideshow_enabled) {
            gboolean is_dark = wally_settings_manager_is_dark_theme(self->settings_manager);
            
            g_autofree char *wally_dir = g_build_filename(g_get_home_dir(), "Pictures", "Wally", NULL);
            g_autofree char *xml_file = g_build_filename(wally_dir, 
                                                         is_dark ? "night-slideshow.xml" : "day-slideshow.xml", NULL);
            
            GError *error = NULL;
            if (g_file_test(xml_file, G_FILE_TEST_EXISTS)) {
                wally_slideshow_manager_apply_wallpaper(self->slideshow_manager, xml_file, is_dark, &error);
                if (error) {
                    g_warning("Failed to switch wallpaper theme: %s", error->message);
                    g_error_free(error);
                }
            }
        }
    }
}

static void
load_settings(WallyPreferencesWindow *self)
{
    GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
    
    // Load folder paths
    g_autofree char *day_path = g_settings_get_string(settings, "day-folder-path");
    g_autofree char *night_path = g_settings_get_string(settings, "night-folder-path");
    
    if (day_path && *day_path) {
        self->day_folder_path = g_strdup(day_path);
        g_autofree char *basename = g_path_get_basename(day_path);
        gtk_button_set_label(self->day_folder_button, basename);
    }
    
    if (night_path && *night_path) {
        self->night_folder_path = g_strdup(night_path);
        g_autofree char *basename = g_path_get_basename(night_path);
        gtk_button_set_label(self->night_folder_button, basename);
    }
    
    // Load other settings
    gboolean auto_night = g_settings_get_boolean(settings, "auto-night-mode");
    adw_switch_row_set_active(self->auto_night_mode_switch, auto_night);
    
    int interval = g_settings_get_int(settings, "slideshow-interval");
    int interval_minutes = interval / 60; // Convert seconds to minutes
    gtk_spin_button_set_value(self->interval_spin, interval_minutes);
    
    double transition = g_settings_get_double(settings, "transition-duration");
    gtk_range_set_value(GTK_RANGE(self->transition_scale), transition);
}

static void
wally_preferences_window_dispose(GObject *object)
{
    WallyPreferencesWindow *self = (WallyPreferencesWindow *)object;
    
    g_clear_object(&self->settings_manager);
    g_clear_object(&self->slideshow_manager);
    g_clear_pointer(&self->day_folder_path, g_free);
    g_clear_pointer(&self->night_folder_path, g_free);
    
    G_OBJECT_CLASS(wally_preferences_window_parent_class)->dispose(object);
}

static void
wally_preferences_window_class_init(WallyPreferencesWindowClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    
    object_class->dispose = wally_preferences_window_dispose;
    
    gtk_widget_class_set_template_from_resource(widget_class, "/com/qomarhsn/wally/ui/preferences.ui");
    
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, day_folder_button);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, night_folder_button);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, apply_button);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, same_folder_switch);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, night_folder_row);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, auto_night_mode_switch);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, interval_spin);
    gtk_widget_class_bind_template_child(widget_class, WallyPreferencesWindow, transition_scale);
}

static void
wally_preferences_window_init(WallyPreferencesWindow *self)
{
    gtk_widget_init_template(GTK_WIDGET(self));
    
    // Initialize managers
    self->settings_manager = wally_settings_manager_new();
    self->slideshow_manager = wally_slideshow_manager_new();
    
    // Connect signals
    g_signal_connect(self->day_folder_button, "clicked", G_CALLBACK(on_day_folder_button_clicked), self);
    g_signal_connect(self->night_folder_button, "clicked", G_CALLBACK(on_night_folder_button_clicked), self);
    g_signal_connect(self->apply_button, "clicked", G_CALLBACK(on_apply_button_clicked), self);
    g_signal_connect(self->same_folder_switch, "notify::active", G_CALLBACK(on_same_folder_switch_toggled), self);
    
    // Bind settings to UI elements
    GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
    
    g_settings_bind(settings, "auto-night-mode",
                    self->auto_night_mode_switch, "active",
                    G_SETTINGS_BIND_DEFAULT);
    
    g_settings_bind(settings, "transition-duration",
                    gtk_range_get_adjustment(GTK_RANGE(self->transition_scale)), "value",
                    G_SETTINGS_BIND_DEFAULT);
    
    g_settings_bind(settings, "use-same-folder",
                    self->same_folder_switch, "active",
                    G_SETTINGS_BIND_DEFAULT);
    
    // Connect interval spin button to save settings when changed
    g_signal_connect(self->interval_spin, "value-changed",
                     G_CALLBACK(+[](GtkSpinButton *spin, gpointer user_data) {
                         WallyPreferencesWindow *self = (WallyPreferencesWindow *)user_data;
                         GSettings *settings = wally_settings_manager_get_settings(self->settings_manager);
                         int minutes = (int)gtk_spin_button_get_value(spin);
                         g_settings_set_int(settings, "slideshow-interval", minutes * 60);
                     }), self);
    
    // Monitor theme changes
    wally_settings_manager_monitor_theme_changes(self->settings_manager,
                                                 G_CALLBACK(on_theme_changed), self);
    
    // Load current settings
    load_settings(self);
}

WallyPreferencesWindow *
wally_preferences_window_new(void)
{
    return static_cast<WallyPreferencesWindow*>(g_object_new(WALLY_TYPE_PREFERENCES_WINDOW, NULL));
}
