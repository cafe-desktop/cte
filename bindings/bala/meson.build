# Copyright © 2018, 2019 Iñigo Martínez
# Copyright © 2019 Christian Persch
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 3 of the License, or (at your
# option) any later version.
#
# This library is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this library.  If not, see <https://www.gnu.org/licenses/>.

assert(get_option('gir'), 'gir is required for bala support')
assert(get_option('ctk3'), 'bala support only available for ctk3')

add_languages('vala', required: false)

#balac = meson.get_compiler('bala')
#assert(balac.version().version_compare('>= 0.24.0'), 'bala >= 0.24 required')

#posix_dep = balac.find_library('posix')

#libvte_ctk3_bapi_deps = [
#  'gio-2.0',
#  'glib-2.0',
#  'pango',
#  'ctk+-3.0',
#]

#libvte_ctk3_bapi_dep = gnome.generate_bapi(
#  vte_ctk3_api_name,
#  sources: libvte_ctk3_gir[0],
#  packages: libvte_ctk3_bapi_deps,
#  install: true,
#)

# Vala test application

vapp_resource_data = files(
  'app.ui',
  'search-popover.ui',
)

vapp_resource_sources = gnome.compile_resources(
  'appresources',
  'app.gresource.xml',
  c_name: 'app',
  dependencies: vapp_resource_data,
  export: true,
)

vapp_sources = vapp_resource_sources + files(
  'config.bapi',
  'app.bala'
)

vapp_cflags = [
  '-Wno-unused-but-set-variable',
  '-Wno-unused-variable',
]

vapp_balaflags = [
  '--enable-deprecated'
]

#if balac.version().version_compare('>= 0.31.1')
  vapp_balaflags += '--disable-since-check'
#endif

#if ctk3_dep.version().version_compare('>= 3.16')
  vapp_balaflags += '--define=CTK_3_16'
#endif

vapp_incs = [
  top_inc,
  src_inc,
  vte_inc,
]

vapp_deps = [
  gio_dep,
  glib_dep,
  ctk3_dep,
#  libvte_ctk3_bapi_dep,
#  posix_dep,
]

vapp = executable(
  'bala-test',
  sources: vapp_sources,
  include_directories: vapp_incs,
  dependencies: vapp_deps,
  c_args: vapp_cflags,
  bala_args: vapp_balaflags,
  install: false,
)
