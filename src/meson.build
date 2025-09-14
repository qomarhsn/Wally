# Source files
wally_sources = [
  'main.cpp',
  'application.cpp',
  'preferences-window.cpp',
  'slideshow-manager.cpp',
  'settings-manager.cpp',
]

# Include generated resources
wally_sources += wally_resources

# Headers
wally_headers = [
  'application.h',
  'preferences-window.h',
  'slideshow-manager.h',
  'settings-manager.h',
]

# Executable
wally_exe = executable('wally',
  wally_sources,
  dependencies: [
    gtk4_dep,
    adwaita_dep,
    gio_dep,
    glib_dep,
  ],
  include_directories: config_h_dir,
  install: true,
  install_dir: get_option('bindir')
)
