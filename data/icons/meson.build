# App icon installation
icon_sizes = ['scalable']

foreach i : icon_sizes
  install_data(
    join_paths(i, 'apps', 'com.qomarhsn.wally.svg'),
    install_dir: join_paths(get_option('datadir'), 'icons', 'hicolor', i, 'apps')
  )
endforeach
