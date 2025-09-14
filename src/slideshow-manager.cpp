#include "slideshow-manager.h"
#include "config.h"

#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

struct _WallySlideshowManager
{
    GObject parent_instance;
};

G_DEFINE_FINAL_TYPE(WallySlideshowManager, wally_slideshow_manager, G_TYPE_OBJECT)

static void
wally_slideshow_manager_finalize(GObject *object G_GNUC_UNUSED)
{
    G_OBJECT_CLASS(wally_slideshow_manager_parent_class)->finalize(object);
}

static void
wally_slideshow_manager_class_init(WallySlideshowManagerClass *klass G_GNUC_UNUSED)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->finalize = wally_slideshow_manager_finalize;
}

static void
wally_slideshow_manager_init(WallySlideshowManager *self G_GNUC_UNUSED)
{
}

WallySlideshowManager *
wally_slideshow_manager_new(void)
{
    return static_cast<WallySlideshowManager*>(g_object_new(WALLY_TYPE_SLIDESHOW_MANAGER, NULL));
}

static std::vector<std::string>
get_image_files(const std::string& folder_path)
{
    std::vector<std::string> image_files;
    const std::vector<std::string> extensions = {".jpg", ".jpeg", ".png", ".bmp", ".webp", ".tiff", ".svg"};
    
    try {
        for (const auto& entry : std::filesystem::directory_iterator(folder_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string extension = entry.path().extension().string();
                
                // Convert extension to lowercase
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
                
                // Check if it's an image file
                if (std::find(extensions.begin(), extensions.end(), extension) != extensions.end()) {
                    image_files.push_back(entry.path().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        g_warning("Error reading directory %s: %s", folder_path.c_str(), e.what());
    }
    
    // Sort files for consistent ordering
    std::sort(image_files.begin(), image_files.end());
    return image_files;
}

gboolean
wally_slideshow_manager_create_slideshow_xml(WallySlideshowManager *self,
                                              const char *folder_path,
                                              const char *output_path,
                                              int interval_seconds,
                                              double transition_duration,
                                              GError **error)
{
    g_return_val_if_fail(WALLY_IS_SLIDESHOW_MANAGER(self), FALSE);
    g_return_val_if_fail(folder_path != NULL, FALSE);
    g_return_val_if_fail(output_path != NULL, FALSE);
    
    // Get image files from the folder
    std::vector<std::string> image_files = get_image_files(folder_path);
    
    if (image_files.empty()) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                    "No image files found in folder: %s", folder_path);
        return FALSE;
    }
    
    GString *xml_content = g_string_new("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
    g_string_append(xml_content, "<!DOCTYPE background SYSTEM \"gnome-wp-list.dtd\">\n");
    g_string_append(xml_content, "<background>\n");
    g_string_append(xml_content, "  <starttime>\n");
    g_string_append(xml_content, "    <year>2024</year>\n");
    g_string_append(xml_content, "    <month>01</month>\n");
    g_string_append(xml_content, "    <day>01</day>\n");
    g_string_append(xml_content, "    <hour>00</hour>\n");
    g_string_append(xml_content, "    <minute>00</minute>\n");
    g_string_append(xml_content, "    <second>00</second>\n");
    g_string_append(xml_content, "  </starttime>\n");
    for (size_t i = 0; i < image_files.size(); i++) {
        const std::string& current_file = image_files[i];
        const std::string& next_file = image_files[(i + 1) % image_files.size()];
        
        g_string_append_printf(xml_content,
            "  <static>\n"
            "    <duration>%d</duration>\n"
            "    <file>%s</file>\n"
            "  </static>\n",
            interval_seconds, current_file.c_str());
        
        g_string_append_printf(xml_content,
            "  <transition>\n"
            "    <duration>%.1f</duration>\n"
            "    <from>%s</from>\n"
            "    <to>%s</to>\n"
            "  </transition>\n",
            transition_duration, current_file.c_str(), next_file.c_str());
    }
    
    g_string_append(xml_content, "</background>\n");
    
    GError *write_error = NULL;
    gboolean success = g_file_set_contents(output_path, xml_content->str, xml_content->len, &write_error);
    
    if (!success) {
        g_propagate_error(error, write_error);
    }
    
    g_string_free(xml_content, TRUE);
    return success;
}

gboolean
wally_slideshow_manager_apply_wallpaper(WallySlideshowManager *self,
                                        const char *xml_path,
                                        gboolean is_dark_theme,
                                        GError **error)
{
    g_return_val_if_fail(WALLY_IS_SLIDESHOW_MANAGER(self), FALSE);
    g_return_val_if_fail(xml_path != NULL, FALSE);
    
    // Construct file URI
    g_autofree char *file_uri = g_filename_to_uri(xml_path, NULL, error);
    if (!file_uri) {
        return FALSE;
    }
    
    // Apply wallpaper using gsettings
    const char *schema = "org.gnome.desktop.background";
    const char *key = is_dark_theme ? "picture-uri-dark" : "picture-uri";
    
    g_autoptr(GSettings) bg_settings = g_settings_new(schema);
    gboolean success = g_settings_set_string(bg_settings, key, file_uri);
    
    if (success) {
        g_settings_sync();
    } else {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                    "Failed to apply wallpaper via gsettings");
    }
    
    return success;
}

gboolean
wally_slideshow_manager_copy_wallpapers(WallySlideshowManager *self,
                                        const char *source_folder,
                                        const char *dest_folder,
                                        GError **error)
{
    g_return_val_if_fail(WALLY_IS_SLIDESHOW_MANAGER(self), FALSE);
    g_return_val_if_fail(source_folder != NULL, FALSE);
    g_return_val_if_fail(dest_folder != NULL, FALSE);
    
    // Create destination directory if it doesn't exist
    if (g_mkdir_with_parents(dest_folder, 0755) != 0) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                    "Failed to create destination directory: %s", dest_folder);
        return FALSE;
    }
    
    // Get image files from source folder
    std::vector<std::string> image_files = get_image_files(source_folder);
    
    if (image_files.empty()) {
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_NOT_FOUND,
                    "No image files found in source folder: %s", source_folder);
        return FALSE;
    }
    
    // Copy each image file
    for (const std::string& source_file : image_files) {
        std::filesystem::path source_path(source_file);
        std::filesystem::path dest_path = std::filesystem::path(dest_folder) / source_path.filename();
        
        try {
            std::filesystem::copy_file(source_path, dest_path, 
                                       std::filesystem::copy_options::overwrite_existing);
        } catch (const std::filesystem::filesystem_error& e) {
            g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                        "Failed to copy file %s: %s", source_file.c_str(), e.what());
            return FALSE;
        }
    }
    
    return TRUE;
}

void
wally_slideshow_manager_next_wallpaper(WallySlideshowManager *self)
{
    g_return_if_fail(WALLY_IS_SLIDESHOW_MANAGER(self));
    
    // Force GNOME to refresh the wallpaper by temporarily setting a different value
    // and then setting it back. This triggers the slideshow to advance.
    g_autoptr(GSettings) bg_settings = g_settings_new("org.gnome.desktop.background");
    
    g_autofree char *current_uri = g_settings_get_string(bg_settings, "picture-uri");
    
    // Temporarily set to a different value to force refresh
    g_settings_set_string(bg_settings, "picture-uri", "");
    g_settings_sync();
    
    // Set back to original value to continue slideshow
    g_settings_set_string(bg_settings, "picture-uri", current_uri);
    g_settings_sync();
}
