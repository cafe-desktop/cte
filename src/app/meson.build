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

app_sources = glib_glue_sources + libc_glue_sources + refptr_sources + files(
  'app.cc',
)

app_resource_data = files(
  'appmenu.ui',
  'search-popover.ui',
  'window.ui',
)

app_common_cppflags = [
  '-DG_LOG_DOMAIN="VTE"',
  '-DLOCALEDIR="@0@"'.format(vte_prefix / vte_localedir),
  '-DVTE_DISABLE_DEPRECATION_WARNINGS',
]

app_common_deps = [
  pcre2_dep,
]

if get_option('ctk3')

  app_ctk3_resource_sources = gnome.compile_resources(
    'resources.cc',
    'app.gresource.xml',
    c_name: 'app',
    dependencies: app_resource_data,
  )

  app_ctk3_sources = app_sources + app_ctk3_resource_sources
  app_ctk3_cppflags = app_common_cppflags + ctk3_version_cppflags
  app_ctk3_deps = app_common_deps + [libvte_ctk3_dep]

  app_ctk3 = executable(
    'vte-' + vte_ctk3_api_version,
    app_ctk3_sources,
    dependencies: app_ctk3_deps,
    cpp_args: app_ctk3_cppflags,
    include_directories: top_inc,
    install: true,
  )
endif
