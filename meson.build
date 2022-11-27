project('org.nickvision.tubeconverter', ['cpp', 'c'], version: '2022.11.2', meson_version: '>= 0.63.0', default_options: ['warning_level=3', 'werror=false', 'cpp_std=c++20'])

gnome = import('gnome')

threads = dependency('Threads')
gtk = dependency('gtk4', version: '>= 4.8.0')
adwaita = dependency('libadwaita-1', version: '>= 1.2.0')
jsoncpp = dependency('jsoncpp')

subdir('src')
subdir('po')

executable('org.nickvision.tubeconverter', sources, dependencies: [threads, adwaita, jsoncpp], install: true)
install_data(resources, install_dir: 'share/icons/hicolor/scalable/apps')
install_data(resources_symbolic, install_dir: 'share/icons/hicolor/symbolic/apps')
install_data('org.nickvision.tubeconverter.desktop', install_dir: 'share/applications')
install_data('org.nickvision.tubeconverter.metainfo.xml', install_dir: 'share/metainfo')
install_data('po/CREDITS.json', install_dir: 'share/org.nickvision.tubeconverter')
gnome.post_install(gtk_update_icon_cache: true, update_desktop_database: true)