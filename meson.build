project('wally', 'cpp',
  version: '1.0.0',
  license: 'MPL-2.0',
  default_options: [
    'cpp_std=c++17',
    'warning_level=2',
    'werror=false',
  ],
)

# Dependencies
gtk4_dep = dependency('gtk4', version: '>= 4.10')
adwaita_dep = dependency('libadwaita-1', version: '>= 1.4')
gio_dep = dependency('gio-2.0', version: '>= 2.74')
glib_dep = dependency('glib-2.0', version: '>= 2.74')

# Application ID and paths
app_id = 'com.qomarhsn.wally'
app_name = 'Wally'

# Configuration
conf = configuration_data()
conf.set_quoted('APP_ID', app_id)
conf.set_quoted('APP_NAME', app_name)
conf.set_quoted('VERSION', meson.project_version())
conf.set_quoted('GETTEXT_PACKAGE', app_id)
conf.set_quoted('LOCALEDIR', get_option('prefix') / get_option('localedir'))

configure_file(
  output: 'config.h',
  configuration: conf,
)

# Include directories
config_h_dir = include_directories('.')

# Subdirectories
subdir('data')
subdir('src')
subdir('po')

# Summary
summary({
  'Application ID': app_id,
  'Version': meson.project_version(),
  'Prefix': get_option('prefix'),
}, section: 'Configuration')
