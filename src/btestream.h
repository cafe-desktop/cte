/*
 * Copyright (C) 2009,2010 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Red Hat Author(s): Behdad Esfahbod
 */

#ifndef btestream_h_included
#define btestream_h_included

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

typedef struct _BteStream BteStream;

void _bte_stream_reset (BteStream *stream, gsize offset);
gboolean _bte_stream_read (BteStream *stream, gsize offset, char *data, gsize len);
void _bte_stream_append (BteStream *stream, const char *data, gsize len);
void _bte_stream_truncate (BteStream *stream, gsize offset);
void _bte_stream_advance_tail (BteStream *stream, gsize offset);
gsize _bte_stream_tail (BteStream *stream);
gsize _bte_stream_head (BteStream *stream);

/* Various streams */

BteStream *
_bte_file_stream_new (void);

G_END_DECLS

#endif
