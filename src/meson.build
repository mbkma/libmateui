# Library sources
libmateui_sources = [
  'mate-ui-application.c',
  'mate-ui-window.c',
  'mate-ui-menu.c',
  'mate-ui-dialogs.c',
  'mate-ui-settings.c',
  'mate-ui-accel.c',
  'mate-ui-session.c',
  'mate-ui-util.c',
]

# Public headers
libmateui_headers = [
  'mate-ui.h',
  'mate-ui-application.h',
  'mate-ui-window.h',
  'mate-ui-menu.h',
  'mate-ui-dialogs.h',
  'mate-ui-settings.h',
  'mate-ui-accel.h',
  'mate-ui-session.h',
  'mate-ui-util.h',
]

# Dependencies list
libmateui_deps = [
  gtk3_dep,
  gio_dep,
  glib_dep,
]

if x11_dep.found()
  libmateui_deps += x11_dep
endif

if xss_dep.found()
  libmateui_deps += xss_dep
endif

if sm_dep.found() and ice_dep.found()
  libmateui_deps += [sm_dep, ice_dep]
endif

# Include directories
libmateui_inc = include_directories('.')

# Build the shared library
libmateui = shared_library('mateui-' + api_version,
  sources: libmateui_sources,
  dependencies: libmateui_deps,
  include_directories: [config_inc, libmateui_inc],
  version: mateui_version,
  soversion: mateui_major,
  install: true,
)

# Install headers
install_headers(libmateui_headers,
  subdir: 'libmateui-' + api_version + '/libmateui'
)

# Generate pkg-config file
pkg = import('pkgconfig')
pkg.generate(
  libmateui,
  name: 'libmateui',
  description: 'MATE UI library for GTK3 applications',
  version: mateui_version,
  filebase: 'libmateui-' + api_version,
  subdirs: 'libmateui-' + api_version,
  requires: ['gtk+-3.0', 'gio-2.0'],
)

# Declare dependency for examples and other subprojects
libmateui_dep = declare_dependency(
  link_with: libmateui,
  include_directories: [config_inc, libmateui_inc],
  dependencies: libmateui_deps,
)
