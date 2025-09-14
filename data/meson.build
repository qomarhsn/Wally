# Import i18n module
i18n = import('i18n')

# Desktop file
desktop_file = i18n.merge_file(
  input: 'com.qomarhsn.wally.desktop.in',
  output: 'com.qomarhsn.wally.desktop',
  type: 'desktop',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'applications')
)

desktop_utils = find_program('desktop-file-validate', required: false)
if desktop_utils.found()
  test('Validate desktop file', desktop_utils,
    args: [desktop_file]
  )
endif

# AppStream file
appstream_file = i18n.merge_file(
  input: 'com.qomarhsn.wally.metainfo.xml.in',
  output: 'com.qomarhsn.wally.metainfo.xml',
  po_dir: '../po',
  install: true,
  install_dir: join_paths(get_option('datadir'), 'metainfo')
)

appstream_util = find_program('appstream-util', required: false)
if appstream_util.found()
  test('Validate appstream file', appstream_util,
    args: ['validate', appstream_file]
  )
endif

# GSettings schema
install_data('schemas/com.qomarhsn.wally.gschema.xml',
  install_dir: join_paths(get_option('datadir'), 'glib-2.0/schemas')
)

compile_schemas = find_program('glib-compile-schemas', required: false)
if compile_schemas.found()
  test('Validate schema file', compile_schemas,
    args: ['--strict', '--dry-run', meson.current_source_dir() / 'schemas']
  )
endif

# GResource for UI files
gnome = import('gnome')
wally_resources = gnome.compile_resources('wally-resources',
  'wally.gresource.xml',
  source_dir: '.',
  c_name: 'wally'
)

# Icons
subdir('icons')
