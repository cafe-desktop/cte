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

gir_dep = dependency('gobject-introspection-1.0', version: '>= 0.9.0')

if get_option('ctk3')
  libvte_ctk3_gir_includes = [
    'Gdk-3.0',
    'Gtk-3.0',
    'Pango-1.0',
  ]

  libvte_ctk3_gir = gnome.generate_gir(
    libvte_ctk3,
    sources: libvte_ctk3_public_headers + libvte_common_doc_sources,
    includes: libvte_ctk3_gir_includes,
    dependencies: libvte_ctk3_dep,
    extra_args: '-DVTE_COMPILATION',
    nsversion: vte_ctk3_api_version,
    namespace: 'Vte',
    export_packages: vte_ctk3_api_name,
    header: 'vte' / 'vte.h',
    install: true,
  )
endif
