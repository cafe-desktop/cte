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

bte_inc = include_directories('.')

libbte_common_enum_headers = files(
  # These files contain enums to be extracted by glib-mkenums
  'btedeprecated.h',
  'bteenums.h',
)

libbte_ctk3_enum_sources = gnome.mkenums(
  'btetypebuiltins.h',
  sources: libbte_common_enum_headers,
  c_template: '../btetypebuiltins.cc.template',
  h_template: '../btetypebuiltins.h.template',
  install_header: true,
  install_dir: bte_includedir / bte_ctk3_api_path
)

libbte_common_public_headers = files(
  'bte.h',
  'btedeprecated.h',
  'bteenums.h',
  'bteglobals.h',
  'btemacros.h',
  'btepty.h',
  'bteregex.h',
  'bteterminal.h',
)

bteversion_conf = configuration_data()
bteversion_conf.set('BTE_MAJOR_VERSION', bte_major_version)
bteversion_conf.set('BTE_MINOR_VERSION', bte_minor_version)
bteversion_conf.set('BTE_MICRO_VERSION', bte_micro_version)

libbte_version_headers = configure_file(
  input: 'bteversion.h.in',
  output: '@BASENAME@',
  configuration: bteversion_conf,
  install: false,
)

if get_option('ctk3')
  libbte_ctk3_public_headers = libbte_common_public_headers + [libbte_version_headers]

  install_headers(
    libbte_ctk3_public_headers,
    subdir: bte_ctk3_api_path
  )

  # BUG! Due to meson bug, this header cannot be installed with the rule above. Instead,
  # use the install_header attribute in the mkenums call, and add the header afterwards
  # to the list.
  libbte_ctk3_public_headers += libbte_ctk3_enum_sources[1]

endif
