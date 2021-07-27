/*
 * Copyright (C) 2003 Red Hat, Inc.
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
 */

#include <config.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctk/ctk.h>
#include <atk/atk.h>
#ifdef USE_VTE
#include <vte/vte.h>
#endif

static GArray *contents = NULL;

#ifdef USE_TEXT_VIEW
/*
 * Implementation for a TextView widget.
 */
static void
terminal_init_text_view(GtkWidget **widget)
{
	*widget = ctk_text_view_new();
	ctk_text_view_set_editable(CTK_TEXT_VIEW(*widget), TRUE);
}
static void
terminal_shell_text_view(GtkWidget *widget)
{
	/* no-op */
}
static GtkAdjustment *
terminal_adjustment_text_view(GtkWidget *terminal)
{
        return ctk_scrollable_get_vadjustment(CTK_SCROLLABLE(terminal));
}
#endif
#ifdef USE_VTE
/*
 * Implementation for a VteTerminal widget.
 */
static void
terminal_init_vte(GtkWidget **terminal)
{
	*terminal = vte_terminal_new();
	g_signal_connect(G_OBJECT(*terminal), "eof",
			 G_CALLBACK(ctk_main_quit), NULL);
	g_signal_connect(G_OBJECT(*terminal), "child-exited",
			 G_CALLBACK(ctk_main_quit), NULL);
}
static void
terminal_shell_vte(GtkWidget *terminal)
{
        char *argv[2];

        argv[0] = vte_get_user_shell ();
        argv[1] = NULL;
	vte_terminal_spawn_sync(VTE_TERMINAL(terminal),
                                       VTE_PTY_DEFAULT,
                                       g_get_home_dir() ? g_get_home_dir() : NULL,
                                       argv,
                                       NULL,
                                       0, NULL, NULL,
                                       NULL,
                                       NULL,
                                       NULL);
}
#endif

/*
 * Update the contents of the widget with the data from our contents array.
 */
static void
update_contents(AtkObject *obj, GtkWidget *widget)
{
	guint caret, i;
	GString *s;

	caret = (guint)atk_text_get_caret_offset(ATK_TEXT(obj));
	s = g_string_new(NULL);
	for (i = 0; i < contents->len; i++) {
		if (i == caret) {
			s = g_string_append(s, "[CARET]");
		}
		s = g_string_append_unichar(s,
					    g_array_index(contents,
							  gunichar,
							  i));
	}
	if (i == caret) {
		s = g_string_append(s, "[CARET]");
	}
	if (CTK_IS_LABEL(widget)) {
		ctk_label_set_text(CTK_LABEL(widget), s->str);
		ctk_label_set_selectable(CTK_LABEL(widget),
					 atk_text_get_n_selections(ATK_TEXT(obj)) > 0);
		if (ctk_label_get_selectable(CTK_LABEL(widget))) {
			int selection_start, selection_end;
			atk_text_get_selection(ATK_TEXT(obj), 0,
					       &selection_start,
					       &selection_end);
			ctk_label_select_region(CTK_LABEL(widget),
						selection_start, selection_end);
		}
	}
	g_string_free(s, TRUE);
}

/* Handle inserted text by inserting the text into our gunichar array. */
static void
text_changed_insert(AtkObject *obj, gint offset, gint length, gpointer data)
{
	char *inserted, *p;
	gunichar c;
	int i;

	inserted = atk_text_get_text(ATK_TEXT(obj), offset, offset + length);

	if (!g_utf8_validate(inserted, -1, NULL)) {
		g_free(inserted);
		g_error("UTF-8 validation error");
		return;
	}

	p = inserted;
	i = 0;
	while (i < length) {
		c = g_utf8_get_char(p);
		if ((guint)(offset + i) >= contents->len) {
			g_array_append_val(contents, c);
		} else {
			g_array_insert_val(contents, offset + i, c);
		}
		i++;
		p = g_utf8_next_char(p);
	}

#ifdef VTE_DEBUG
	if ((getenv("REFLECT_VERBOSE") != NULL) &&
	    (atol(getenv("REFLECT_VERBOSE")) != 0)) {
		g_printerr("Inserted %d chars ('%.*s') at %d,",
			length, (int)(p - inserted), inserted, offset);
		g_printerr(" buffer contains %d characters.\n",
			contents->len);
	}
#endif

	g_free(inserted);

	update_contents(obj, CTK_WIDGET(data));
}

/* Handle deleted text by removing the text from our gunichar array. */
static void
text_changed_delete(AtkObject *obj, gint offset, gint length, gpointer data)
{
	int i;
	for (i = offset + length - 1; i >= offset; i--) {
		if ((guint)i > contents->len - 1) {
			g_warning("Invalid character %d was deleted.\n", i);
		}
		g_array_remove_index(contents, i);
	}
#ifdef VTE_DEBUG
	if ((getenv("REFLECT_VERBOSE") != NULL) &&
	    (atol(getenv("REFLECT_VERBOSE")) != 0)) {
		g_printerr("Deleted %d chars at %d.\n", length, offset);
	}
#endif
	update_contents(obj, CTK_WIDGET(data));
}

static void
text_caret_moved(AtkObject *obj, gint offset, gpointer data)
{
	update_contents(obj, CTK_WIDGET(data));
}

static void
text_selection_changed(AtkObject *obj, gpointer data)
{
	update_contents(obj, CTK_WIDGET(data));
}

/* Wrapper versions. */
static void
terminal_init(GtkWidget **terminal)
{
	*terminal = NULL;
#ifdef USE_TEXT_VIEW
	terminal_init_text_view(terminal);
	return;
#endif
#ifdef USE_VTE
	terminal_init_vte(terminal);
	return;
#endif
	g_assert_not_reached();
}
static void
terminal_shell(GtkWidget *terminal)
{
#ifdef USE_TEXT_VIEW
	terminal_shell_text_view(terminal);
	return;
#endif
#ifdef USE_VTE
	terminal_shell_vte(terminal);
	return;
#endif
	g_assert_not_reached();
}
static GtkAdjustment *
terminal_adjustment(GtkWidget *terminal)
{
#ifdef USE_TEXT_VIEW
	return terminal_adjustment_text_view(terminal);
#endif
#ifdef USE_VTE
	return ctk_scrollable_get_vadjustment(CTK_SCROLLABLE(terminal));
#endif
	g_assert_not_reached();
}

int
main(int argc, char **argv)
{
	GtkWidget *label, *terminal, *tophalf, *pane, *window, *sw;
	AtkObject *obj;
	char *text, *p;
	gunichar c;
	guint count;

	ctk_init(&argc, &argv);

	contents = g_array_new(TRUE, FALSE, sizeof(gunichar));

	terminal_init(&terminal);

#ifdef USE_TEXT_VIEW
	tophalf = ctk_scrolled_window_new(NULL, terminal_adjustment(terminal));
	ctk_scrolled_window_set_policy(CTK_SCROLLED_WINDOW(tophalf),
				       CTK_POLICY_AUTOMATIC,
				       CTK_POLICY_AUTOMATIC);
	ctk_container_add(CTK_CONTAINER(tophalf), terminal);
#else
        tophalf = ctk_grid_new();

        ctk_grid_attach(CTK_GRID(tophalf), terminal, 0, 0, 1, 1);
	ctk_widget_show(terminal);

        GtkWidget* scrollbar = ctk_scrollbar_new(CTK_ORIENTATION_VERTICAL,
                                                 terminal_adjustment(terminal));
        ctk_grid_attach(CTK_GRID(tophalf), scrollbar, 1, 0, 1, 1);
	ctk_widget_show(scrollbar);
#endif
	ctk_widget_show(terminal);

	label = ctk_label_new("");
	ctk_label_set_justify(CTK_LABEL(label), CTK_JUSTIFY_LEFT);
        ctk_label_set_xalign(CTK_LABEL(label), 0.0);
        ctk_label_set_yalign(CTK_LABEL(label), 0.0);

	sw = ctk_scrolled_window_new(NULL, NULL);
        ctk_container_add(CTK_CONTAINER(sw), label);
	ctk_widget_show(label);

	pane = ctk_paned_new (CTK_ORIENTATION_VERTICAL);
	ctk_paned_pack1(CTK_PANED(pane), tophalf, TRUE, FALSE);
	ctk_paned_pack2(CTK_PANED(pane), sw, TRUE, FALSE);
	ctk_widget_show(tophalf);
	ctk_widget_show(sw);

	window = ctk_window_new(CTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "delete_event",
			 G_CALLBACK(ctk_main_quit), NULL);
	ctk_container_add(CTK_CONTAINER(window), pane);
	ctk_widget_show(pane);

	obj = ctk_widget_get_accessible(terminal);
	g_assert(obj != NULL);
	g_signal_connect(G_OBJECT(obj), "text-changed::insert",
			 G_CALLBACK(text_changed_insert), label);
	g_signal_connect(G_OBJECT(obj), "text-changed::delete",
			 G_CALLBACK(text_changed_delete), label);
	g_signal_connect(G_OBJECT(obj), "text-caret-moved",
			 G_CALLBACK(text_caret_moved), label);
	g_signal_connect(G_OBJECT(obj), "text-selection-changed",
			 G_CALLBACK(text_selection_changed), label);

	count = (guint)atk_text_get_character_count(ATK_TEXT(obj));
	if (count > 0) {
		text = atk_text_get_text(ATK_TEXT(obj), 0, count);
		if (text != NULL) {
			for (p = text;
			     contents->len < count;
			     p = g_utf8_next_char(p)) {
				c = g_utf8_get_char(p);
				g_array_append_val(contents, c);
			}
			g_free(text);
		}
	}
	terminal_shell(terminal);

	ctk_window_set_default_size(CTK_WINDOW(window), 600, 450);
	ctk_widget_show(window);

	update_contents(obj, terminal);

	ctk_main();

	g_array_free(contents, TRUE);
	contents = NULL;

	return 0;
}
