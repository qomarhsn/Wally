#pragma once

#include <glib-object.h>
#include <gio/gio.h>
#include <string>
#include <vector>

G_BEGIN_DECLS

#define WALLY_TYPE_SLIDESHOW_MANAGER (wally_slideshow_manager_get_type())

G_DECLARE_FINAL_TYPE(WallySlideshowManager, wally_slideshow_manager, WALLY, SLIDESHOW_MANAGER, GObject)

WallySlideshowManager *wally_slideshow_manager_new(void);

gboolean wally_slideshow_manager_create_slideshow_xml(WallySlideshowManager *self,
                                                      const char *folder_path,
                                                      const char *output_path,
                                                      int interval_seconds,
                                                      double transition_duration,
                                                      GError **error);

gboolean wally_slideshow_manager_apply_wallpaper(WallySlideshowManager *self,
                                                  const char *xml_path,
                                                  gboolean is_dark_theme,
                                                  GError **error);

gboolean wally_slideshow_manager_copy_wallpapers(WallySlideshowManager *self,
                                                  const char *source_folder,
                                                  const char *dest_folder,
                                                  GError **error);

void wally_slideshow_manager_next_wallpaper(WallySlideshowManager *self);

G_END_DECLS
