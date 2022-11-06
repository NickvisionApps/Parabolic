i18n = import('i18n')
# define GETTEXT_PACKAGE and LOCALE_DIR
add_project_arguments('-DGETTEXT_PACKAGE="' + meson.project_name() + '"', language:'cpp')
add_project_arguments('-DLOCALE_DIR="' + join_paths(get_option('prefix'), get_option('localedir')) + '"', language:'cpp')
i18n.gettext(meson.project_name())