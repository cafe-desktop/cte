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

subdir('bte')

src_inc = include_directories('.')

a11y_sources = files(
  'bteaccess.cc',
  'bteaccess.h',
)

debug_sources = files(
  'debug.cc',
  'debug.h',
)

glib_glue_sources = files(
  'glib-glue.hh',
)

icu_sources = files(
  'icu-converter.cc',
  'icu-converter.hh',
  'icu-decoder.cc',
  'icu-decoder.hh',
  'icu-glue.cc',
  'icu-glue.hh',
)

libc_glue_sources = files(
  'libc-glue.hh',
)

modes_sources = files(
  'modes-ecma.hh',
  'modes-private.hh',
  'modes.hh',
)

parser_sources = files(
  'parser-arg.hh',
  'parser-c01.hh',
  'parser-charset-tables.hh',
  'parser-charset.hh',
  'parser-cmd.hh',
  'parser-csi.hh',
  'parser-dcs.hh',
  'parser-decsgr.hh',
  'parser-esc.hh',
  'parser-glue.hh',
  'parser-osc.hh',
  'parser-reply.hh',
  'parser-sci.hh',
  'parser-sgr.hh',
  'parser-string.hh',
  'parser.cc',
  'parser.hh',
)

pty_sources = files(
  'pty.cc',
  'pty.hh',
  'btepty.cc',
  'bteptyinternal.hh',
)

refptr_sources = files(
  'refptr.hh',
)

regex_sources = files(
  'regex.cc',
  'regex.hh'
)

systemd_sources = files(
  'systemd.cc',
  'systemd.hh',
)

utf8_sources = files(
  'utf8.cc',
  'utf8.hh',
)

libbte_common_sources = debug_sources + glib_glue_sources + libc_glue_sources + modes_sources + parser_sources + pty_sources + refptr_sources + regex_sources + utf8_sources + files(
  'attr.hh',
  'bidi.cc',
  'bidi.hh',
  'buffer.h',
  'cairo-glue.hh',
  'caps.hh',
  'cell.hh',
  'chunk.cc',
  'chunk.hh',
  'color-triple.hh',
  'cxx-utils.hh',
  'drawing-cairo.cc',
  'drawing-cairo.hh',
  'fonts-pangocairo.cc',
  'fonts-pangocairo.hh',
  'gobject-glue.hh',
  'keymap.cc',
  'keymap.h',
  'minifont.cc',
  'minifont.hh',
  'missing.cc',
  'missing.hh',
  'reaper.cc',
  'reaper.hh',
  'ring.cc',
  'ring.hh',
  'ringview.cc',
  'ringview.hh',
  'spawn.cc',
  'spawn.hh',
  'utf8.cc',
  'utf8.hh',
  'bte.cc',
  'btedefines.hh',
  'btectk.cc',
  'btectk.hh',
  'bteinternal.hh',
  'btepcre2.h',
  'bteregex.cc',
  'bteregexinternal.hh',
  'bterowdata.cc',
  'bterowdata.hh',
  'bteseq.cc',
  'btespawn.cc',
  'btespawn.hh',
  'btestream-base.h',
  'btestream-file.h',
  'btestream.cc',
  'btestream.h',
  'btetypes.cc',
  'btetypes.hh',
  'bteunistr.cc',
  'bteunistr.h',
  'bteutils.cc',
  'bteutils.h',
  'widget.cc',
  'widget.hh',
)

if get_option('a11y')
  libbte_common_sources += a11y_sources
endif

if get_option('icu')
  libbte_common_sources += icu_sources
endif

if systemd_dep.found()
  libbte_common_sources += systemd_sources
endif

libbte_common_doc_sources = files(
  # These file contain gtk-doc comments to be extracted for docs and gir
  'btectk.cc',
  'btepty.cc',
  'bteregex.cc',
)

# FIXMEchpe this should use files('...') (allowed as per docs, afaict), but that crashes meson
generate_box_drawing = find_program('box_drawing_generate.sh')

libbte_common_sources += custom_target(
  'box-drawing',
  input: 'box_drawing.txt',
  output: 'box_drawing.h',
  capture: true,
  command: [generate_box_drawing, '@INPUT@'],
  install: false,
)

libbte_common_sources += gnome.compile_resources(
  'bteresources',
  'bte.gresource.xml',
  c_name: '_bte',
)

libbte_common_sources += gnome.genmarshal(
  'marshal',
  sources: 'marshal.list',
  prefix: '_bte_marshal',
  internal: true,
  valist_marshallers: true,
)

libbte_common_public_deps = [
  gio_dep,
  glib_dep,
  gobject_dep,
  pango_dep,
]

libbte_common_deps = libbte_common_public_deps + [
  fribidi_dep,
  gnutls_dep,
  icu_dep,
  pcre2_dep,
  libm_dep,
  pthreads_dep,
  systemd_dep,
  zlib_dep,
]

incs = [
  top_inc,
  bte_inc,
]

libbte_common_cppflags = [
  '-DG_LOG_DOMAIN="BTE"',
  '-DLOCALEDIR="@0@"'.format(bte_prefix / bte_localedir),
  '-DBTE_DISABLE_DEPRECATION_WARNINGS',
  '-DBTE_COMPILATION',
  '-UPARSER_INCLUDE_NOP',
]

if get_option('ctk3')
  libbte_ctk3_sources = libbte_common_sources + libbte_ctk3_public_headers + libbte_ctk3_enum_sources
  libbte_ctk3_cppflags = libbte_common_cppflags + ctk3_version_cppflags
  libbte_ctk3_deps = libbte_common_deps + [ctk3_dep]
  libbte_ctk3_public_deps = libbte_common_public_deps + [ctk3_dep]

  libbte_ctk3 = shared_library(
    bte_ctk3_api_name,
    sources: libbte_ctk3_sources,
    version: libbte_ctk3_soversion,
    include_directories: incs,
    dependencies: libbte_ctk3_deps,
    cpp_args: libbte_ctk3_cppflags,
    install: true,
  )

  libbte_ctk3_dep = declare_dependency(
    sources: libbte_ctk3_public_headers,
    include_directories: [src_inc, bte_inc],
    dependencies: libbte_ctk3_deps,
    link_with: libbte_ctk3
  )

  pkg.generate(
    libbte_ctk3,
    version: bte_version,
    name: 'bte',
    description: 'BTE widget for CTK+ 3.0',
    filebase: bte_ctk3_api_name,
    subdirs: bte_ctk3_api_name,
    requires: libbte_ctk3_public_deps,
    variables: 'exec_prefix=${prefix}',
  )
endif

## Tests

# decoder cat

decoder_cat_sources = glib_glue_sources + libc_glue_sources + utf8_sources + debug_sources + files(
  'decoder-cat.cc',
)

if get_option('icu')
  decoder_cat_sources += icu_sources
endif

decoder_cat = executable(
  'decoder-cat',
  decoder_cat_sources,
  dependencies: [glib_dep, icu_dep,],
  cpp_args: [],
  include_directories: top_inc,
  install: false,
)

# parser cat

parser_cat_sources = glib_glue_sources + libc_glue_sources + parser_sources + utf8_sources + debug_sources + files(
  'parser-cat.cc',
)

parser_cat = executable(
  'parser-cat',
  parser_cat_sources,
  dependencies: [glib_dep,],
  cpp_args: ['-DPARSER_INCLUDE_NOP'],
  include_directories: top_inc,
  install: false,
)

# dumpkeys

dumpkeys_sources = files(
  'dumpkeys.c'
)

dumpkeys = executable(
  'dumpkeys',
  sources: dumpkeys_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

# mev

mev_sources = files(
  'mev.c'
)

mev = executable(
  'mev',
  sources: mev_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

# slowcat

slowcat_sources = files(
  'slowcat.c'
)

slowcat = executable(
  'slowcat',
  sources: slowcat_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

# reflect

reflect_sources = files(
  'reflect.c'
)

reflect_textview = executable(
  'reflect-textview',
  sources: reflect_sources,
  dependencies: [ctk3_dep],
  c_args: ['-DUSE_TEXT_VIEW'],
  include_directories: top_inc,
  install: false,
)

reflect_bte = executable(
  'reflect-bte',
  sources: reflect_sources,
  dependencies: [ctk3_dep, libbte_ctk3_dep],
  c_args: [
    '-DUSE_BTE',
    '-DBTE_DISABLE_DEPRECATION_WARNINGS',
  ],
  include_directories: top_inc,
  install: false,
)

# bte-urlencode-cwd

bte_urlencode_cwd_sources = files(
  'urlencode.cc',
)

bte_urlencode_cwd = executable(
  'bte-urlencode-cwd',
  cpp_args: [
    '-nostdlib',
  ],
  include_directories: [top_inc,],
  install: true,
  install_dir: bte_libexecdir,
  sources: bte_urlencode_cwd_sources,
)

# xticker

xticker_sources = files(
  'xticker.c'
)

xticker = executable(
  'xticker',
  sources: xticker_sources,
  dependencies: [glib_dep],
  c_args: ['-DBTE_DISABLE_DEPRECATION_WARNINGS',],
  include_directories: top_inc,
  install: false,
)

# Unit tests

test_modes_sources = modes_sources + files(
  'modes-test.cc',
)

test_modes = executable(
  'test-modes',
  sources: test_modes_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

test_parser_sources = parser_sources + files(
  'parser-test.cc',
)

test_parser = executable(
  'test-parser',
  sources: test_parser_sources,
  dependencies: [glib_dep],
  cpp_args: ['-DPARSER_INCLUDE_NOP'],
  include_directories: top_inc,
  install: false,
)

test_reaper_sources = debug_sources + files(
  'reaper.cc',
  'reaper.hh'
)

test_reaper = executable(
  'test-reaper',
  sources: test_reaper_sources,
  dependencies: [gobject_dep],
  cpp_args: ['-DMAIN'],
  include_directories: top_inc,
  install: false,
)

test_refptr_sources = files(
  'refptr-test.cc',
  'refptr.hh'
)

test_refptr = executable(
  'test-refptr',
  sources: test_refptr_sources,
  dependencies: [glib_dep, gobject_dep],
  include_directories: top_inc,
  install: false,
)

test_tabstops_sources = files(
  'tabstops-test.cc',
  'tabstops.hh'
)

test_stream_sources = files(
  'btestream-base.h',
  'btestream-file.h',
  'btestream.cc',
  'btestream.h',
  'bteutils.cc',
  'bteutils.h',
)

test_stream = executable(
  'test-stream',
  sources: test_stream_sources,
  dependencies: [gio_dep, gnutls_dep, zlib_dep],
  cpp_args: ['-DBTESTREAM_MAIN'],
  include_directories: top_inc,
  install: false,
)

test_tabstops = executable(
  'test-tabstops',
  sources: test_tabstops_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

test_utf8_sources = utf8_sources + files(
  'utf8-test.cc',
)

test_utf8 = executable(
  'test-utf8',
  sources: test_utf8_sources,
  dependencies: [glib_dep],
  include_directories: top_inc,
  install: false,
)

test_btetypes_sources = libc_glue_sources + files(
   'btetypes.cc',
   'btetypes.hh',
)

test_btetypes = executable(
  'test-btetypes',
  sources: test_btetypes_sources,
  dependencies: [glib_dep, pango_dep, ctk3_dep],
  cpp_args: ['-DMAIN'],
  include_directories: top_inc,
  install: false,
)

test_env = [
  'BTE_DEBUG=0'
]

# apparently there is no way to get a name back from an executable(), so it this ugly way
test_units = [
  ['modes', test_modes],
  ['parser', test_parser],
  ['reaper', test_reaper],
  ['refptr', test_refptr],
  ['stream', test_stream],
  ['tabstops', test_tabstops],
  ['utf8', test_utf8],
  ['btetypes', test_btetypes],
]

foreach test: test_units
  test(
    test[0],
    test[1],
    env: test_env,
    timeout: 600, # See issue bte#120
  )
endforeach

test_scripts = [
  ['doc syntax', 'check-doc-syntax.sh'],
]

foreach test: test_scripts
  test(
    test[0],
    files(test[1]),
  )
endforeach

test_scripts_with_arg = [
  ['bte-urlencode-cwd', 'test-bte-urlencode-cwd.sh', bte_urlencode_cwd],
]

foreach test: test_scripts_with_arg
  test(
    test[0],
    files(test[1]),
    args: [test[2]],
  )
endforeach

# Shell integration

bte_csh = configure_file(
  input: 'bte.csh.in',
  output: '@BASENAME@',
  configuration: {
    'libexecdir': bte_prefix / bte_libexecdir,
  },
  install: true,
  install_dir: bte_sysconfdir / 'profile.d',
)

bte_sh = configure_file(
  input: 'bte.sh.in',
  output: '@BASENAME@',
  configuration: {
    'libexecdir': bte_prefix / bte_libexecdir,
  },
  install: true,
  install_dir: bte_sysconfdir / 'profile.d',
)

# Systemd integration

if systemd_dep.found()
  install_data(
    sources: 'bte-spawn-.scope.conf',
    install_dir: bte_systemduserunitdir / 'bte-spawn-.scope.d',
    rename: 'defaults.conf'
  )
endif

# Demo application

subdir('app')
