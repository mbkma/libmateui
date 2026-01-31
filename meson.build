project('libmateui', 'c',
  version: '1.0.0',
  license: 'GPL-2.0-or-later',
  meson_version: '>= 0.59.0',
  default_options: [
    'c_std=gnu11',
    'warning_level=2',
    'buildtype=debugoptimized',
  ]
)

# Version info for shared library
mateui_version = meson.project_version()
version_array = mateui_version.split('.')
mateui_major = version_array[0].to_int()
mateui_minor = version_array[1].to_int()
mateui_micro = version_array[2].to_int()

# API version for parallel installability
api_version = '1.0'

# Directories
prefix = get_option('prefix')
libdir = join_paths(prefix, get_option('libdir'))
includedir = join_paths(prefix, get_option('includedir'))
datadir = join_paths(prefix, get_option('datadir'))
pkgconfigdir = join_paths(libdir, 'pkgconfig')

# Dependencies
gtk3_dep = dependency('gtk+-3.0', version: '>= 3.22.0')
gio_dep = dependency('gio-2.0', version: '>= 2.50.0')
glib_dep = dependency('glib-2.0', version: '>= 2.50.0')

# Optional X11 support for session inhibit
x11_dep = dependency('x11', required: false)
xss_dep = dependency('xscrnsaver', required: false)
sm_dep = dependency('sm', required: false)
ice_dep = dependency('ice', required: false)

# Configuration data
conf_data = configuration_data()
conf_data.set_quoted('PACKAGE_NAME', meson.project_name())
conf_data.set_quoted('PACKAGE_VERSION', mateui_version)
conf_data.set('MATEUI_MAJOR_VERSION', mateui_major)
conf_data.set('MATEUI_MINOR_VERSION', mateui_minor)
conf_data.set('MATEUI_MICRO_VERSION', mateui_micro)
conf_data.set10('HAVE_X11', x11_dep.found())
conf_data.set10('HAVE_XSS', xss_dep.found())
conf_data.set10('HAVE_SM', sm_dep.found() and ice_dep.found())

configure_file(
  output: 'config.h',
  configuration: conf_data
)

config_inc = include_directories('.')

# Subdirectories
subdir('src')

if get_option('examples')
  subdir('examples')
endif

# Summary
summary({
  'prefix': prefix,
  'libdir': libdir,
  'includedir': includedir,
}, section: 'Directories')

summary({
  'GTK+ version': gtk3_dep.version(),
  'X11 support': x11_dep.found(),
  'XScreenSaver support': xss_dep.found(),
  'Session management': sm_dep.found() and ice_dep.found(),
}, section: 'Configuration')
