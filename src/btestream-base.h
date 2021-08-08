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

#include <glib-object.h>
#include <gio/gio.h>

G_BEGIN_DECLS

/*
 * BteStream: Abstract base stream class
 */

struct _BteStream {
	GObject parent;
};

typedef struct _BteStreamClass {
	GObjectClass parent_class;

	void (*reset) (BteStream *stream, gsize offset);
	gboolean (*read) (BteStream *stream, gsize offset, char *data, gsize len);
	void (*append) (BteStream *stream, const char *data, gsize len);
	void (*truncate) (BteStream *stream, gsize offset);
	void (*advance_tail) (BteStream *stream, gsize offset);
	gsize (*tail) (BteStream *stream);
	gsize (*head) (BteStream *stream);
} BteStreamClass;

static GType _bte_stream_get_type (void);
#define VTE_TYPE_STREAM _bte_stream_get_type ()
#define VTE_STREAM_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VTE_TYPE_STREAM, BteStreamClass))

G_DEFINE_ABSTRACT_TYPE (BteStream, _bte_stream, G_TYPE_OBJECT)

static void
_bte_stream_class_init (BteStreamClass *klass G_GNUC_UNUSED)
{
}

static void
_bte_stream_init (BteStream *stream G_GNUC_UNUSED)
{
}

void
_bte_stream_reset (BteStream *stream, gsize offset)
{
	VTE_STREAM_GET_CLASS (stream)->reset (stream, offset);
}

gboolean
_bte_stream_read (BteStream *stream, gsize offset, char *data, gsize len)
{
	return VTE_STREAM_GET_CLASS (stream)->read (stream, offset, data, len);
}

void
_bte_stream_append (BteStream *stream, const char *data, gsize len)
{
	VTE_STREAM_GET_CLASS (stream)->append (stream, data, len);
}

void
_bte_stream_truncate (BteStream *stream, gsize offset)
{
	VTE_STREAM_GET_CLASS (stream)->truncate (stream, offset);
}

void
_bte_stream_advance_tail (BteStream *stream, gsize offset)
{
	VTE_STREAM_GET_CLASS (stream)->advance_tail (stream, offset);
}

gsize
_bte_stream_tail (BteStream *stream)
{
	return VTE_STREAM_GET_CLASS (stream)->tail (stream);
}

gsize
_bte_stream_head (BteStream *stream)
{
	return VTE_STREAM_GET_CLASS (stream)->head (stream);
}

G_END_DECLS

