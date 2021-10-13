/*
 * Copyright (C) 2001-2004,2009,2010 Red Hat, Inc.
 * Copyright © 2008, 2009, 2010, 2015 Christian Persch
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
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION: bte-terminal
 * @short_description: A terminal widget implementation
 *
 * A BteTerminal is a terminal emulator implemented as a CTK3 widget.
 *
 * Note that altough #BteTerminal implements the #CtkScrollable interface,
 * you should not place a #BteTerminal inside a #CtkScrolledWindow
 * container, since they are incompatible. Instead, pack the terminal in
 * a horizontal #CtkBox together with a #CtkScrollbar which uses the
 * #CtkAdjustment returned from ctk_scrollable_get_vadjustment().
 */

#include "config.h"

#include <new> /* placement new */
#include <exception>

#include <pwd.h>

#ifdef HAVE_LOCALE_H
#include <locale.h>
#endif

#include <glib.h>
#include <glib/gi18n-lib.h>
#include <ctk/ctk.h>
#include "bte/bteenums.h"
#include "bte/btepty.h"
#include "bte/bteterminal.h"
#include "bte/btetypebuiltins.h"

#include "debug.h"
#include "glib-glue.hh"
#include "gobject-glue.hh"
#include "marshal.h"
#include "reaper.hh"
#include "btedefines.hh"
#include "bteinternal.hh"
#include "widget.hh"

#include "btectk.hh"
#include "bteptyinternal.hh"
#include "bteregexinternal.hh"

#ifdef WITH_A11Y
#include "bteaccess.h"
#endif

#ifdef WITH_ICU
#include "icu-glue.hh"
#endif

#define I_(string) (g_intern_static_string(string))
#define _BTE_PARAM_DEPRECATED (_bte_debug_on(BTE_DEBUG_SIGNALS) ? G_PARAM_DEPRECATED : 0)

#define BTE_TERMINAL_CSS_NAME "bte-terminal"

struct _BteTerminalClassPrivate {
        CtkStyleProvider *fallback_style_provider;
        CtkStyleProvider *style_provider;
};

static void bte_terminal_scrollable_iface_init(CtkScrollableInterface* iface) noexcept;

#ifdef BTE_DEBUG
G_DEFINE_TYPE_WITH_CODE(BteTerminal, bte_terminal, CTK_TYPE_WIDGET,
                        {
                                BteTerminal_private_offset =
                                        g_type_add_instance_private(g_define_type_id, sizeof(bte::platform::Widget));
                        }
                        g_type_add_class_private (g_define_type_id, sizeof (BteTerminalClassPrivate));
                        G_IMPLEMENT_INTERFACE(CTK_TYPE_SCROLLABLE, bte_terminal_scrollable_iface_init)
                        if (_bte_debug_on(BTE_DEBUG_LIFECYCLE)) {
                                g_printerr("bte_terminal_get_type()\n");
                        })
#else
G_DEFINE_TYPE_WITH_CODE(BteTerminal, bte_terminal, CTK_TYPE_WIDGET,
                        {
                                BteTerminal_private_offset =
                                        g_type_add_instance_private(g_define_type_id, sizeof(bte::platform::Widget));
                        }
                        g_type_add_class_private (g_define_type_id, sizeof (BteTerminalClassPrivate));
                        G_IMPLEMENT_INTERFACE(CTK_TYPE_SCROLLABLE, bte_terminal_scrollable_iface_init))
#endif

static inline
bte::platform::Widget* get_widget(BteTerminal* terminal)
{
        return reinterpret_cast<bte::platform::Widget*>(bte_terminal_get_instance_private(terminal));
}

#define WIDGET(t) (get_widget(t))

bte::terminal::Terminal*
_bte_terminal_get_impl(BteTerminal *terminal)
{
        return WIDGET(terminal)->terminal();
}

#define IMPL(t) (_bte_terminal_get_impl(t))

guint signals[LAST_SIGNAL];
GParamSpec *pspecs[LAST_PROP];
GTimer *process_timer;
uint64_t g_test_flags = 0;

static bool
valid_color(CdkRGBA const* color) noexcept
{
        return color->red >= 0. && color->red <= 1. &&
               color->green >= 0. && color->green <= 1. &&
               color->blue >= 0. && color->blue <= 1. &&
               color->alpha >= 0. && color->alpha <= 1.;
}

static void
bte_terminal_set_hadjustment(BteTerminal *terminal,
                             CtkAdjustment *adjustment) noexcept
try
{
        g_return_if_fail(adjustment == nullptr || CTK_IS_ADJUSTMENT(adjustment));
        WIDGET(terminal)->set_hadjustment(bte::glib::make_ref_sink(adjustment));
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_set_vadjustment(BteTerminal *terminal,
                             CtkAdjustment *adjustment) noexcept
try
{
        g_return_if_fail(adjustment == nullptr || CTK_IS_ADJUSTMENT(adjustment));
        WIDGET(terminal)->set_vadjustment(bte::glib::make_ref_sink(adjustment));
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_set_hscroll_policy(BteTerminal *terminal,
                                CtkScrollablePolicy policy) noexcept
try
{
        WIDGET(terminal)->set_hscroll_policy(policy);
        ctk_widget_queue_resize_no_redraw (CTK_WIDGET (terminal));
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_set_vscroll_policy(BteTerminal *terminal,
                                CtkScrollablePolicy policy) noexcept
try
{
        WIDGET(terminal)->set_vscroll_policy(policy);
        ctk_widget_queue_resize_no_redraw (CTK_WIDGET (terminal));
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_real_copy_clipboard(BteTerminal *terminal) noexcept
try
{
	WIDGET(terminal)->copy(BTE_SELECTION_CLIPBOARD, BTE_FORMAT_TEXT);
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_real_paste_clipboard(BteTerminal *terminal) noexcept
try
{
	WIDGET(terminal)->paste(CDK_SELECTION_CLIPBOARD);
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_style_updated (CtkWidget *widget) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);

        CTK_WIDGET_CLASS (bte_terminal_parent_class)->style_updated (widget);

        WIDGET(terminal)->style_updated();
}
catch (...)
{
        bte::log_exception();
}

static gboolean
bte_terminal_key_press(CtkWidget *widget,
                       CdkEventKey *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);

        /* We do NOT want chain up to CtkWidget::key-press-event, since that would
         * cause CtkWidget's keybindings to be handled and consumed. However we'll
         * have to handle the one sane binding (Shift-F10 or MenuKey, to pop up the
         * context menu) ourself, so for now we simply skip the offending keybinding
         * in class_init.
         */

	/* First, check if CtkWidget's behavior already does something with
	 * this key. */
	if (CTK_WIDGET_CLASS(bte_terminal_parent_class)->key_press_event) {
		if ((CTK_WIDGET_CLASS(bte_terminal_parent_class))->key_press_event(widget,
                                                                                   event)) {
			return TRUE;
		}
	}

        return WIDGET(terminal)->key_press(event);
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_key_release(CtkWidget *widget,
                         CdkEventKey *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        return WIDGET(terminal)->key_release(event);
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_motion_notify(CtkWidget *widget,
                           CdkEventMotion *event) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL(widget);
        return WIDGET(terminal)->motion_notify(event);
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_button_press(CtkWidget *widget,
                          CdkEventButton *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        return WIDGET(terminal)->button_press(event);
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_button_release(CtkWidget *widget,
                            CdkEventButton *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        return WIDGET(terminal)->button_release(event);
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_scroll(CtkWidget *widget,
                    CdkEventScroll *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->scroll(event);
        return TRUE;
}
catch (...)
{
        bte::log_exception();
        return true;
}

static gboolean
bte_terminal_focus_in(CtkWidget *widget,
                      CdkEventFocus *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->focus_in(event);
        return FALSE;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static gboolean
bte_terminal_focus_out(CtkWidget *widget,
                       CdkEventFocus *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->focus_out(event);
        return FALSE;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static gboolean
bte_terminal_enter(CtkWidget *widget,
                   CdkEventCrossing *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        gboolean ret = FALSE;

	if (CTK_WIDGET_CLASS (bte_terminal_parent_class)->enter_notify_event) {
		ret = CTK_WIDGET_CLASS (bte_terminal_parent_class)->enter_notify_event (widget, event);
	}

        WIDGET(terminal)->enter(event);

        return ret;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static gboolean
bte_terminal_leave(CtkWidget *widget,
                   CdkEventCrossing *event) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
	gboolean ret = FALSE;

	if (CTK_WIDGET_CLASS (bte_terminal_parent_class)->leave_notify_event) {
		ret = CTK_WIDGET_CLASS (bte_terminal_parent_class)->leave_notify_event (widget, event);
	}

        WIDGET(terminal)->leave(event);

        return ret;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static void
bte_terminal_get_preferred_width(CtkWidget *widget,
				 int       *minimum_width,
				 int       *natural_width) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->get_preferred_width(minimum_width, natural_width);
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_get_preferred_height(CtkWidget *widget,
				  int       *minimum_height,
				  int       *natural_height) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->get_preferred_height(minimum_height, natural_height);
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_size_allocate(CtkWidget *widget,
                           CtkAllocation *allocation) noexcept
try
{
	BteTerminal *terminal = BTE_TERMINAL(widget);
        WIDGET(terminal)->size_allocate(allocation);
}
catch (...)
{
        bte::log_exception();
}

static gboolean
bte_terminal_draw(CtkWidget *widget,
                  cairo_t *cr) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL (widget);
        WIDGET(terminal)->draw(cr);
        return FALSE;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static void
bte_terminal_realize(CtkWidget *widget) noexcept
try
{
	_bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_realize()\n");

        CTK_WIDGET_CLASS(bte_terminal_parent_class)->realize(widget);

        BteTerminal *terminal= BTE_TERMINAL(widget);
        WIDGET(terminal)->realize();
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_unrealize(CtkWidget *widget) noexcept
{
	_bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_unrealize()\n");

        try {
                BteTerminal *terminal = BTE_TERMINAL (widget);
                WIDGET(terminal)->unrealize();
        } catch (...) {
                bte::log_exception();
        }

        CTK_WIDGET_CLASS(bte_terminal_parent_class)->unrealize(widget);
}

static void
bte_terminal_map(CtkWidget *widget) noexcept
try
{
        _bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_map()\n");

        BteTerminal *terminal = BTE_TERMINAL(widget);
        CTK_WIDGET_CLASS(bte_terminal_parent_class)->map(widget);

        WIDGET(terminal)->map();
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_unmap(CtkWidget *widget) noexcept
{
        _bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_unmap()\n");

        try {
                BteTerminal *terminal = BTE_TERMINAL(widget);
                WIDGET(terminal)->unmap();
        } catch (...) {
                bte::log_exception();
        }

        CTK_WIDGET_CLASS(bte_terminal_parent_class)->unmap(widget);
}

static void
bte_terminal_screen_changed (CtkWidget *widget,
                             CdkScreen *previous_screen) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL (widget);

        if (CTK_WIDGET_CLASS (bte_terminal_parent_class)->screen_changed) {
                CTK_WIDGET_CLASS (bte_terminal_parent_class)->screen_changed (widget, previous_screen);
        }

        WIDGET(terminal)->screen_changed(previous_screen);
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_constructed (GObject *object) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL (object);

        G_OBJECT_CLASS (bte_terminal_parent_class)->constructed (object);

        WIDGET(terminal)->constructed();
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_init(BteTerminal *terminal)
try
{
        void *place;
	CtkStyleContext *context;

	_bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_init()\n");

        context = ctk_widget_get_style_context(&terminal->widget);
        ctk_style_context_add_provider (context,
                                        BTE_TERMINAL_GET_CLASS (terminal)->priv->fallback_style_provider,
                                        CTK_STYLE_PROVIDER_PRIORITY_FALLBACK);
        ctk_style_context_add_provider (context,
                                        BTE_TERMINAL_GET_CLASS (terminal)->priv->style_provider,
                                        CTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

        ctk_widget_set_has_window(&terminal->widget, FALSE);

	/* Initialize private data. NOTE: place is zeroed */
	place = bte_terminal_get_instance_private(terminal);
        new (place) bte::platform::Widget(terminal);
}
catch (...)
{
        bte::log_exception();

        // There's not really anything we can do after the
        // construction of Widget failed... we'll crash soon anyway.
        g_assert_not_reached();
}

static void
bte_terminal_dispose(GObject *object) noexcept
{
	_bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_dispose()\n");

        try {
                BteTerminal *terminal = BTE_TERMINAL (object);
                WIDGET(terminal)->dispose();
        } catch (...) {
                bte::log_exception();
        }

	/* Call the inherited dispose() method. */
	G_OBJECT_CLASS(bte_terminal_parent_class)->dispose(object);
}

static void
bte_terminal_finalize(GObject *object) noexcept
{
	_bte_debug_print(BTE_DEBUG_LIFECYCLE, "bte_terminal_finalize()\n");

        try {
                BteTerminal *terminal = BTE_TERMINAL (object);
                WIDGET(terminal)->~Widget();
        } catch (...) {
                bte::log_exception();
        }

	/* Call the inherited finalize() method. */
	G_OBJECT_CLASS(bte_terminal_parent_class)->finalize(object);
}

static void
bte_terminal_get_property (GObject *object,
                           guint prop_id,
                           GValue *value,
                           GParamSpec *pspec) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL (object);
        auto widget = WIDGET(terminal);
        auto impl = IMPL(terminal);

	switch (prop_id)
                {
                case PROP_HADJUSTMENT:
                        g_value_set_object (value, widget->hadjustment());
                        break;
                case PROP_VADJUSTMENT:
                        g_value_set_object (value, widget->vadjustment());
                        break;
                case PROP_HSCROLL_POLICY:
                        g_value_set_enum (value, widget->hscroll_policy());
                        break;
                case PROP_VSCROLL_POLICY:
                        g_value_set_enum (value, widget->vscroll_policy());
                        break;
                case PROP_ALLOW_BOLD:
                        g_value_set_boolean (value, bte_terminal_get_allow_bold (terminal));
                        break;
                case PROP_ALLOW_HYPERLINK:
                        g_value_set_boolean (value, bte_terminal_get_allow_hyperlink (terminal));
                        break;
                case PROP_AUDIBLE_BELL:
                        g_value_set_boolean (value, bte_terminal_get_audible_bell (terminal));
                        break;
                case PROP_BACKSPACE_BINDING:
                        g_value_set_enum (value, widget->backspace_binding());
                        break;
                case PROP_BOLD_IS_BRIGHT:
                        g_value_set_boolean (value, bte_terminal_get_bold_is_bright (terminal));
                        break;
                case PROP_CELL_HEIGHT_SCALE:
                        g_value_set_double (value, bte_terminal_get_cell_height_scale (terminal));
                        break;
                case PROP_CELL_WIDTH_SCALE:
                        g_value_set_double (value, bte_terminal_get_cell_width_scale (terminal));
                        break;
                case PROP_CJK_AMBIGUOUS_WIDTH:
                        g_value_set_int (value, bte_terminal_get_cjk_ambiguous_width (terminal));
                        break;
                case PROP_CURSOR_BLINK_MODE:
                        g_value_set_enum (value, bte_terminal_get_cursor_blink_mode (terminal));
                        break;
                case PROP_CURRENT_DIRECTORY_URI:
                        g_value_set_string (value, bte_terminal_get_current_directory_uri (terminal));
                        break;
                case PROP_CURRENT_FILE_URI:
                        g_value_set_string (value, bte_terminal_get_current_file_uri (terminal));
                        break;
                case PROP_CURSOR_SHAPE:
                        g_value_set_enum (value, bte_terminal_get_cursor_shape (terminal));
                        break;
                case PROP_DELETE_BINDING:
                        g_value_set_enum (value, widget->delete_binding());
                        break;
                case PROP_ENABLE_BIDI:
                        g_value_set_boolean (value, bte_terminal_get_enable_bidi (terminal));
                        break;
                case PROP_ENABLE_SHAPING:
                        g_value_set_boolean (value, bte_terminal_get_enable_shaping (terminal));
                        break;
                case PROP_ENABLE_SIXEL:
                        g_value_set_boolean (value, bte_terminal_get_enable_sixel (terminal));
                        break;
                case PROP_ENCODING:
                        g_value_set_string (value, bte_terminal_get_encoding (terminal));
                        break;
                case PROP_FONT_DESC:
                        g_value_set_boxed (value, bte_terminal_get_font (terminal));
                        break;
                case PROP_FONT_SCALE:
                        g_value_set_double (value, bte_terminal_get_font_scale (terminal));
                        break;
                case PROP_HYPERLINK_HOVER_URI:
                        g_value_set_string (value, impl->m_hyperlink_hover_uri);
                        break;
                case PROP_ICON_TITLE:
                        g_value_set_string (value, bte_terminal_get_icon_title (terminal));
                        break;
                case PROP_INPUT_ENABLED:
                        g_value_set_boolean (value, bte_terminal_get_input_enabled (terminal));
                        break;
                case PROP_MOUSE_POINTER_AUTOHIDE:
                        g_value_set_boolean (value, bte_terminal_get_mouse_autohide (terminal));
                        break;
                case PROP_PTY:
                        g_value_set_object (value, bte_terminal_get_pty(terminal));
                        break;
                case PROP_REWRAP_ON_RESIZE:
                        g_value_set_boolean (value, bte_terminal_get_rewrap_on_resize (terminal));
                        break;
                case PROP_SCROLLBACK_LINES:
                        g_value_set_uint (value, bte_terminal_get_scrollback_lines(terminal));
                        break;
                case PROP_SCROLL_ON_KEYSTROKE:
                        g_value_set_boolean (value, bte_terminal_get_scroll_on_keystroke(terminal));
                        break;
                case PROP_SCROLL_ON_OUTPUT:
                        g_value_set_boolean (value, bte_terminal_get_scroll_on_output(terminal));
                        break;
                case PROP_TEXT_BLINK_MODE:
                        g_value_set_enum (value, bte_terminal_get_text_blink_mode (terminal));
                        break;
                case PROP_WINDOW_TITLE:
                        g_value_set_string (value, bte_terminal_get_window_title (terminal));
                        break;
                case PROP_WORD_CHAR_EXCEPTIONS:
                        g_value_set_string (value, bte_terminal_get_word_char_exceptions (terminal));
                        break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
                }
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_set_property (GObject *object,
                           guint prop_id,
                           const GValue *value,
                           GParamSpec *pspec) noexcept
try
{
        BteTerminal *terminal = BTE_TERMINAL (object);

	switch (prop_id)
                {
                case PROP_HADJUSTMENT:
                        bte_terminal_set_hadjustment (terminal, (CtkAdjustment *)g_value_get_object (value));
                        break;
                case PROP_VADJUSTMENT:
                        bte_terminal_set_vadjustment (terminal, (CtkAdjustment *)g_value_get_object (value));
                        break;
                case PROP_HSCROLL_POLICY:
                        bte_terminal_set_hscroll_policy(terminal, (CtkScrollablePolicy)g_value_get_enum(value));
                        break;
                case PROP_VSCROLL_POLICY:
                        bte_terminal_set_vscroll_policy(terminal, (CtkScrollablePolicy)g_value_get_enum(value));
                        break;
                case PROP_ALLOW_BOLD:
                        bte_terminal_set_allow_bold (terminal, g_value_get_boolean (value));
                        break;
                case PROP_ALLOW_HYPERLINK:
                        bte_terminal_set_allow_hyperlink (terminal, g_value_get_boolean (value));
                        break;
                case PROP_AUDIBLE_BELL:
                        bte_terminal_set_audible_bell (terminal, g_value_get_boolean (value));
                        break;
                case PROP_BACKSPACE_BINDING:
                        bte_terminal_set_backspace_binding (terminal, (BteEraseBinding)g_value_get_enum (value));
                        break;
                case PROP_BOLD_IS_BRIGHT:
                        bte_terminal_set_bold_is_bright (terminal, g_value_get_boolean (value));
                        break;
                case PROP_CELL_HEIGHT_SCALE:
                        bte_terminal_set_cell_height_scale (terminal, g_value_get_double (value));
                        break;
                case PROP_CELL_WIDTH_SCALE:
                        bte_terminal_set_cell_width_scale (terminal, g_value_get_double (value));
                        break;
                case PROP_CJK_AMBIGUOUS_WIDTH:
                        bte_terminal_set_cjk_ambiguous_width (terminal, g_value_get_int (value));
                        break;
                case PROP_CURSOR_BLINK_MODE:
                        bte_terminal_set_cursor_blink_mode (terminal, (BteCursorBlinkMode)g_value_get_enum (value));
                        break;
                case PROP_CURSOR_SHAPE:
                        bte_terminal_set_cursor_shape (terminal, (BteCursorShape)g_value_get_enum (value));
                        break;
                case PROP_DELETE_BINDING:
                        bte_terminal_set_delete_binding (terminal, (BteEraseBinding)g_value_get_enum (value));
                        break;
                case PROP_ENABLE_BIDI:
                        bte_terminal_set_enable_bidi (terminal, g_value_get_boolean (value));
                        break;
                case PROP_ENABLE_SHAPING:
                        bte_terminal_set_enable_shaping (terminal, g_value_get_boolean (value));
                        break;
                case PROP_ENABLE_SIXEL:
                        bte_terminal_set_enable_sixel (terminal, g_value_get_boolean (value));
                        break;
                case PROP_ENCODING:
                        bte_terminal_set_encoding (terminal, g_value_get_string (value), NULL);
                        break;
                case PROP_FONT_DESC:
                        bte_terminal_set_font (terminal, (PangoFontDescription *)g_value_get_boxed (value));
                        break;
                case PROP_FONT_SCALE:
                        bte_terminal_set_font_scale (terminal, g_value_get_double (value));
                        break;
                case PROP_INPUT_ENABLED:
                        bte_terminal_set_input_enabled (terminal, g_value_get_boolean (value));
                        break;
                case PROP_MOUSE_POINTER_AUTOHIDE:
                        bte_terminal_set_mouse_autohide (terminal, g_value_get_boolean (value));
                        break;
                case PROP_PTY:
                        bte_terminal_set_pty (terminal, (BtePty *)g_value_get_object (value));
                        break;
                case PROP_REWRAP_ON_RESIZE:
                        bte_terminal_set_rewrap_on_resize (terminal, g_value_get_boolean (value));
                        break;
                case PROP_SCROLLBACK_LINES:
                        bte_terminal_set_scrollback_lines (terminal, g_value_get_uint (value));
                        break;
                case PROP_SCROLL_ON_KEYSTROKE:
                        bte_terminal_set_scroll_on_keystroke(terminal, g_value_get_boolean (value));
                        break;
                case PROP_SCROLL_ON_OUTPUT:
                        bte_terminal_set_scroll_on_output (terminal, g_value_get_boolean (value));
                        break;
                case PROP_TEXT_BLINK_MODE:
                        bte_terminal_set_text_blink_mode (terminal, (BteTextBlinkMode)g_value_get_enum (value));
                        break;
                case PROP_WORD_CHAR_EXCEPTIONS:
                        bte_terminal_set_word_char_exceptions (terminal, g_value_get_string (value));
                        break;

                        /* Not writable */
                case PROP_CURRENT_DIRECTORY_URI:
                case PROP_CURRENT_FILE_URI:
                case PROP_HYPERLINK_HOVER_URI:
                case PROP_ICON_TITLE:
                case PROP_WINDOW_TITLE:
                        g_assert_not_reached ();
                        break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			return;
                }
}
catch (...)
{
        bte::log_exception();
}

static void
bte_terminal_class_init(BteTerminalClass *klass)
{
	GObjectClass *gobject_class;
	CtkWidgetClass *widget_class;
	CtkBindingSet  *binding_set;

#ifdef BTE_DEBUG
	{
                _bte_debug_init();
		_bte_debug_print(BTE_DEBUG_LIFECYCLE,
                                 "bte_terminal_class_init()\n");
		/* print out the legend */
		_bte_debug_print(BTE_DEBUG_WORK,
                                 "Debugging work flow (top input to bottom output):\n"
                                 "  .  _bte_terminal_process_incoming\n"
                                 "  <  start process_timeout\n"
                                 "  {[ start update_timeout  [ => rate limited\n"
                                 "  T  start of terminal in update_timeout\n"
                                 "  (  start _bte_terminal_process_incoming\n"
                                 "  ?  _bte_invalidate_cells (call)\n"
                                 "  !  _bte_invalidate_cells (dirty)\n"
                                 "  *  _bte_invalidate_all\n"
                                 "  )  end _bte_terminal_process_incoming\n"
                                 "  =  bte_terminal_paint\n"
                                 "  ]} end update_timeout\n"
                                 "  >  end process_timeout\n");
	}
#endif

	_BTE_DEBUG_IF (BTE_DEBUG_UPDATES) cdk_window_set_debug_updates(TRUE);

	bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

	gobject_class = G_OBJECT_CLASS(klass);
	widget_class = CTK_WIDGET_CLASS(klass);

	/* Override some of the default handlers. */
        gobject_class->constructed = bte_terminal_constructed;
        gobject_class->dispose = bte_terminal_dispose;
	gobject_class->finalize = bte_terminal_finalize;
        gobject_class->get_property = bte_terminal_get_property;
        gobject_class->set_property = bte_terminal_set_property;
	widget_class->realize = bte_terminal_realize;
	widget_class->unrealize = bte_terminal_unrealize;
        widget_class->map = bte_terminal_map;
        widget_class->unmap = bte_terminal_unmap;
	widget_class->scroll_event = bte_terminal_scroll;
        widget_class->draw = bte_terminal_draw;
	widget_class->key_press_event = bte_terminal_key_press;
	widget_class->key_release_event = bte_terminal_key_release;
	widget_class->button_press_event = bte_terminal_button_press;
	widget_class->button_release_event = bte_terminal_button_release;
	widget_class->motion_notify_event = bte_terminal_motion_notify;
	widget_class->enter_notify_event = bte_terminal_enter;
	widget_class->leave_notify_event = bte_terminal_leave;
	widget_class->focus_in_event = bte_terminal_focus_in;
	widget_class->focus_out_event = bte_terminal_focus_out;
	widget_class->style_updated = bte_terminal_style_updated;
	widget_class->get_preferred_width = bte_terminal_get_preferred_width;
	widget_class->get_preferred_height = bte_terminal_get_preferred_height;
	widget_class->size_allocate = bte_terminal_size_allocate;
        widget_class->screen_changed = bte_terminal_screen_changed;

        ctk_widget_class_set_css_name(widget_class, BTE_TERMINAL_CSS_NAME);

	/* Initialize default handlers. */
	klass->eof = NULL;
	klass->child_exited = NULL;
	klass->encoding_changed = NULL;
	klass->char_size_changed = NULL;
	klass->window_title_changed = NULL;
	klass->icon_title_changed = NULL;
	klass->selection_changed = NULL;
	klass->contents_changed = NULL;
	klass->cursor_moved = NULL;
	klass->commit = NULL;

	klass->deiconify_window = NULL;
	klass->iconify_window = NULL;
	klass->raise_window = NULL;
	klass->lower_window = NULL;
	klass->refresh_window = NULL;
	klass->restore_window = NULL;
	klass->maximize_window = NULL;
	klass->resize_window = NULL;
	klass->move_window = NULL;

	klass->increase_font_size = NULL;
	klass->decrease_font_size = NULL;

	klass->text_modified = NULL;
	klass->text_inserted = NULL;
	klass->text_deleted = NULL;
	klass->text_scrolled = NULL;

	klass->copy_clipboard = bte_terminal_real_copy_clipboard;
	klass->paste_clipboard = bte_terminal_real_paste_clipboard;

        klass->bell = NULL;

        /* CtkScrollable interface properties */
        g_object_class_override_property (gobject_class, PROP_HADJUSTMENT, "hadjustment");
        g_object_class_override_property (gobject_class, PROP_VADJUSTMENT, "vadjustment");
        g_object_class_override_property (gobject_class, PROP_HSCROLL_POLICY, "hscroll-policy");
        g_object_class_override_property (gobject_class, PROP_VSCROLL_POLICY, "vscroll-policy");

	/* Register some signals of our own. */

        /**
         * BteTerminal::eof:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the terminal receives an end-of-file from a child which
         * is running in the terminal.  This signal is frequently (but not
         * always) emitted with a #BteTerminal::child-exited signal.
         */
        signals[SIGNAL_EOF] =
                g_signal_new(I_("eof"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, eof),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_EOF],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::child-exited:
         * @bteterminal: the object which received the signal
         * @status: the child's exit status
         *
         * This signal is emitted when the terminal detects that a child
         * watched using bte_terminal_watch_child() has exited.
         */
        signals[SIGNAL_CHILD_EXITED] =
                g_signal_new(I_("child-exited"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, child_exited),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__INT,
                             G_TYPE_NONE,
                             1, G_TYPE_INT);
        g_signal_set_va_marshaller(signals[SIGNAL_CHILD_EXITED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__INTv);

        /**
         * BteTerminal::window-title-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the terminal's %window_title field is modified.
         */
        signals[SIGNAL_WINDOW_TITLE_CHANGED] =
                g_signal_new(I_("window-title-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, window_title_changed),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_WINDOW_TITLE_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::icon-title-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the terminal's %icon_title field is modified.
         */
        signals[SIGNAL_ICON_TITLE_CHANGED] =
                g_signal_new(I_("icon-title-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, icon_title_changed),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_ICON_TITLE_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::current-directory-uri-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the current directory URI is modified.
         */
        signals[SIGNAL_CURRENT_DIRECTORY_URI_CHANGED] =
                g_signal_new(I_("current-directory-uri-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             0,
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_CURRENT_DIRECTORY_URI_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::current-file-uri-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the current file URI is modified.
         */
        signals[SIGNAL_CURRENT_FILE_URI_CHANGED] =
                g_signal_new(I_("current-file-uri-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             0,
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_CURRENT_FILE_URI_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::hyperlink-hover-uri-changed:
         * @bteterminal: the object which received the signal
         * @uri: the nonempty target URI under the mouse, or NULL
         * @bbox: the bounding box of the hyperlink anchor text, or NULL
         *
         * Emitted when the hovered hyperlink changes.
         *
         * @uri and @bbox are owned by BTE, must not be modified, and might
         * change after the signal handlers returns.
         *
         * The signal is not re-emitted when the bounding box changes for the
         * same hyperlink. This might change in a future BTE version without notice.
         *
         * Since: 0.50
         */
        signals[SIGNAL_HYPERLINK_HOVER_URI_CHANGED] =
                g_signal_new(I_("hyperlink-hover-uri-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             0,
                             NULL,
                             NULL,
                             _bte_marshal_VOID__STRING_BOXED,
                             G_TYPE_NONE,
                             2, G_TYPE_STRING, CDK_TYPE_RECTANGLE | G_SIGNAL_TYPE_STATIC_SCOPE);
        g_signal_set_va_marshaller(signals[SIGNAL_HYPERLINK_HOVER_URI_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   _bte_marshal_VOID__STRING_BOXEDv);

        /**
         * BteTerminal::encoding-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever the terminal's current encoding has changed.
         *
         * Note: support for non-UTF-8 is deprecated.
         */
        signals[SIGNAL_ENCODING_CHANGED] =
                g_signal_new(I_("encoding-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, encoding_changed),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_ENCODING_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::commit:
         * @bteterminal: the object which received the signal
         * @text: a string of text
         * @size: the length of that string of text
         *
         * Emitted whenever the terminal receives input from the user and
         * prepares to send it to the child process.
         */
        signals[SIGNAL_COMMIT] =
                g_signal_new(I_("commit"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, commit),
                             NULL,
                             NULL,
                             _bte_marshal_VOID__STRING_UINT,
                             G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_UINT);
        g_signal_set_va_marshaller(signals[SIGNAL_COMMIT],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   _bte_marshal_VOID__STRING_UINTv);

        /**
         * BteTerminal::char-size-changed:
         * @bteterminal: the object which received the signal
         * @width: the new character cell width
         * @height: the new character cell height
         *
         * Emitted whenever the cell size changes, e.g. due to a change in
         * font, font-scale or cell-width/height-scale.
         *
         * Note that this signal should rather be called "cell-size-changed".
         */
        signals[SIGNAL_CHAR_SIZE_CHANGED] =
                g_signal_new(I_("char-size-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, char_size_changed),
                             NULL,
                             NULL,
                             _bte_marshal_VOID__UINT_UINT,
                             G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
        g_signal_set_va_marshaller(signals[SIGNAL_CHAR_SIZE_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   _bte_marshal_VOID__UINT_UINTv);

        /**
         * BteTerminal::selection-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever the contents of terminal's selection changes.
         */
        signals[SIGNAL_SELECTION_CHANGED] =
                g_signal_new (I_("selection-changed"),
                              G_OBJECT_CLASS_TYPE(klass),
                              G_SIGNAL_RUN_LAST,
                              G_STRUCT_OFFSET(BteTerminalClass, selection_changed),
                              NULL,
                              NULL,
                              g_cclosure_marshal_VOID__VOID,
                              G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_SELECTION_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::contents-changed:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever the visible appearance of the terminal has changed.
         * Used primarily by #BteTerminalAccessible.
         */
        signals[SIGNAL_CONTENTS_CHANGED] =
                g_signal_new(I_("contents-changed"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, contents_changed),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_CONTENTS_CHANGED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::cursor-moved:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever the cursor moves to a new character cell.  Used
         * primarily by #BteTerminalAccessible.
         */
        signals[SIGNAL_CURSOR_MOVED] =
                g_signal_new(I_("cursor-moved"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, cursor_moved),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_CURSOR_MOVED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::deiconify-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_DEICONIFY_WINDOW] =
                g_signal_new(I_("deiconify-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, deiconify_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_DEICONIFY_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::iconify-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_ICONIFY_WINDOW] =
                g_signal_new(I_("iconify-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, iconify_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_ICONIFY_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::raise-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_RAISE_WINDOW] =
                g_signal_new(I_("raise-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, raise_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_RAISE_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::lower-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_LOWER_WINDOW] =
                g_signal_new(I_("lower-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, lower_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_LOWER_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::refresh-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_REFRESH_WINDOW] =
                g_signal_new(I_("refresh-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, refresh_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_REFRESH_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::restore-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_RESTORE_WINDOW] =
                g_signal_new(I_("restore-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, restore_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_RESTORE_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::maximize-window:
         * @bteterminal: the object which received the signal
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_MAXIMIZE_WINDOW] =
                g_signal_new(I_("maximize-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, maximize_window),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_MAXIMIZE_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::resize-window:
         * @bteterminal: the object which received the signal
         * @width: the desired number of columns
         * @height: the desired number of rows
         *
         * Emitted at the child application's request.
         */
        signals[SIGNAL_RESIZE_WINDOW] =
                g_signal_new(I_("resize-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, resize_window),
                             NULL,
                             NULL,
                             _bte_marshal_VOID__UINT_UINT,
                             G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
        g_signal_set_va_marshaller(signals[SIGNAL_RESIZE_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   _bte_marshal_VOID__UINT_UINTv);

        /**
         * BteTerminal::move-window:
         * @bteterminal: the object which received the signal
         * @x: the terminal's desired location, X coordinate
         * @y: the terminal's desired location, Y coordinate
         *
         * Never emitted.
         *
         * Deprecated: 0.60
         */
        signals[SIGNAL_MOVE_WINDOW] =
                g_signal_new(I_("move-window"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, move_window),
                             NULL,
                             NULL,
                             _bte_marshal_VOID__UINT_UINT,
                             G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
        g_signal_set_va_marshaller(signals[SIGNAL_MOVE_WINDOW],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   _bte_marshal_VOID__UINT_UINTv);

        /**
         * BteTerminal::increase-font-size:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the user hits the '+' key while holding the Control key.
         */
        signals[SIGNAL_INCREASE_FONT_SIZE] =
                g_signal_new(I_("increase-font-size"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, increase_font_size),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_INCREASE_FONT_SIZE],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::decrease-font-size:
         * @bteterminal: the object which received the signal
         *
         * Emitted when the user hits the '-' key while holding the Control key.
         */
        signals[SIGNAL_DECREASE_FONT_SIZE] =
                g_signal_new(I_("decrease-font-size"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, decrease_font_size),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_DECREASE_FONT_SIZE],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::text-modified:
         * @bteterminal: the object which received the signal
         *
         * An internal signal used for communication between the terminal and
         * its accessibility peer. May not be emitted under certain
         * circumstances.
         */
        signals[SIGNAL_TEXT_MODIFIED] =
                g_signal_new(I_("text-modified"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, text_modified),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_TEXT_MODIFIED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::text-inserted:
         * @bteterminal: the object which received the signal
         *
         * An internal signal used for communication between the terminal and
         * its accessibility peer. May not be emitted under certain
         * circumstances.
         */
        signals[SIGNAL_TEXT_INSERTED] =
                g_signal_new(I_("text-inserted"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, text_inserted),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_TEXT_INSERTED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::text-deleted:
         * @bteterminal: the object which received the signal
         *
         * An internal signal used for communication between the terminal and
         * its accessibility peer. May not be emitted under certain
         * circumstances.
         */
        signals[SIGNAL_TEXT_DELETED] =
                g_signal_new(I_("text-deleted"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, text_deleted),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_TEXT_DELETED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::text-scrolled:
         * @bteterminal: the object which received the signal
         * @delta: the number of lines scrolled
         *
         * An internal signal used for communication between the terminal and
         * its accessibility peer. May not be emitted under certain
         * circumstances.
         */
        signals[SIGNAL_TEXT_SCROLLED] =
                g_signal_new(I_("text-scrolled"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, text_scrolled),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__INT,
                             G_TYPE_NONE, 1, G_TYPE_INT);
        g_signal_set_va_marshaller(signals[SIGNAL_TEXT_SCROLLED],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__INTv);

        /**
         * BteTerminal::copy-clipboard:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever bte_terminal_copy_clipboard() is called.
         */
	signals[SIGNAL_COPY_CLIPBOARD] =
                g_signal_new(I_("copy-clipboard"),
			     G_OBJECT_CLASS_TYPE(klass),
			     (GSignalFlags)(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
			     G_STRUCT_OFFSET(BteTerminalClass, copy_clipboard),
			     NULL,
			     NULL,
                             g_cclosure_marshal_VOID__VOID,
			     G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_COPY_CLIPBOARD],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::paste-clipboard:
         * @bteterminal: the object which received the signal
         *
         * Emitted whenever bte_terminal_paste_clipboard() is called.
         */
	signals[SIGNAL_PASTE_CLIPBOARD] =
                g_signal_new(I_("paste-clipboard"),
			     G_OBJECT_CLASS_TYPE(klass),
			     (GSignalFlags)(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
			     G_STRUCT_OFFSET(BteTerminalClass, paste_clipboard),
			     NULL,
			     NULL,
                             g_cclosure_marshal_VOID__VOID,
			     G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_PASTE_CLIPBOARD],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal::bell:
         * @bteterminal: the object which received the signal
         *
         * This signal is emitted when the a child sends a bell request to the
         * terminal.
         */
        signals[SIGNAL_BELL] =
                g_signal_new(I_("bell"),
                             G_OBJECT_CLASS_TYPE(klass),
                             G_SIGNAL_RUN_LAST,
                             G_STRUCT_OFFSET(BteTerminalClass, bell),
                             NULL,
                             NULL,
                             g_cclosure_marshal_VOID__VOID,
                             G_TYPE_NONE, 0);
        g_signal_set_va_marshaller(signals[SIGNAL_BELL],
                                   G_OBJECT_CLASS_TYPE(klass),
                                   g_cclosure_marshal_VOID__VOIDv);

        /**
         * BteTerminal:allow-bold:
         *
         * Controls whether or not the terminal will attempt to draw bold text,
         * by using a bold font variant.
         */
        pspecs[PROP_ALLOW_BOLD] =
                g_param_spec_boolean ("allow-bold", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:allow-hyperlink:
         *
         * Controls whether or not hyperlinks (OSC 8 escape sequence) are recognized and displayed.
         *
         * Since: 0.50
         */
        pspecs[PROP_ALLOW_HYPERLINK] =
                g_param_spec_boolean ("allow-hyperlink", NULL, NULL,
                                      FALSE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:audible-bell:
         *
         * Controls whether or not the terminal will beep when the child outputs the
         * "bl" sequence.
         */
        pspecs[PROP_AUDIBLE_BELL] =
                g_param_spec_boolean ("audible-bell", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:backspace-binding:
         *
         * Controls what string or control sequence the terminal sends to its child
         * when the user presses the backspace key.
         */
        pspecs[PROP_BACKSPACE_BINDING] =
                g_param_spec_enum ("backspace-binding", NULL, NULL,
                                   BTE_TYPE_ERASE_BINDING,
                                   BTE_ERASE_AUTO,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:bold-is-bright:
         *
         * Whether the SGR 1 attribute also switches to the bright counterpart
         * of the first 8 palette colors, in addition to making them bold (legacy behavior)
         * or if SGR 1 only enables bold and leaves the color intact.
         *
         * Since: 0.52
         */
        pspecs[PROP_BOLD_IS_BRIGHT] =
                g_param_spec_boolean ("bold-is-bright", NULL, NULL,
                                      FALSE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:cell-height-scale:
         *
         * Scale factor for the cell height, to increase line spacing. (The font's height is not affected.)
         *
         * Since: 0.52
         */
        pspecs[PROP_CELL_HEIGHT_SCALE] =
                g_param_spec_double ("cell-height-scale", NULL, NULL,
                                     BTE_CELL_SCALE_MIN,
                                     BTE_CELL_SCALE_MAX,
                                     1.,
                                     (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:cell-width-scale:
         *
         * Scale factor for the cell width, to increase letter spacing. (The font's width is not affected.)
         *
         * Since: 0.52
         */
        pspecs[PROP_CELL_WIDTH_SCALE] =
                g_param_spec_double ("cell-width-scale", NULL, NULL,
                                     BTE_CELL_SCALE_MIN,
                                     BTE_CELL_SCALE_MAX,
                                     1.,
                                     (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:cjk-ambiguous-width:
         *
         * This setting controls whether ambiguous-width characters are narrow or wide.
         * (Note that when using a non-UTF-8 encoding set via bte_terminal_set_encoding(),
         * the width of ambiguous-width characters is fixed and determined by the encoding
         * itself.)
         *
         * This setting only takes effect the next time the terminal is reset, either
         * via escape sequence or with bte_terminal_reset().
         */
        pspecs[PROP_CJK_AMBIGUOUS_WIDTH] =
                g_param_spec_int ("cjk-ambiguous-width", NULL, NULL,
                                  1, 2, BTE_DEFAULT_UTF8_AMBIGUOUS_WIDTH,
                                  (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:cursor-blink-mode:
         *
         * Sets whether or not the cursor will blink. Using %BTE_CURSOR_BLINK_SYSTEM
         * will use the #CtkSettings::ctk-cursor-blink setting.
         */
        pspecs[PROP_CURSOR_BLINK_MODE] =
                g_param_spec_enum ("cursor-blink-mode", NULL, NULL,
                                   BTE_TYPE_CURSOR_BLINK_MODE,
                                   BTE_CURSOR_BLINK_SYSTEM,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:cursor-shape:
         *
         * Controls the shape of the cursor.
         */
        pspecs[PROP_CURSOR_SHAPE] =
                g_param_spec_enum ("cursor-shape", NULL, NULL,
                                   BTE_TYPE_CURSOR_SHAPE,
                                   BTE_CURSOR_SHAPE_BLOCK,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:delete-binding:
         *
         * Controls what string or control sequence the terminal sends to its child
         * when the user presses the delete key.
         */
        pspecs[PROP_DELETE_BINDING] =
                g_param_spec_enum ("delete-binding", NULL, NULL,
                                   BTE_TYPE_ERASE_BINDING,
                                   BTE_ERASE_AUTO,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:enable-bidi:
         *
         * Controls whether or not the terminal will perform bidirectional text rendering.
         *
         * Since: 0.58
         */
        pspecs[PROP_ENABLE_BIDI] =
                g_param_spec_boolean ("enable-bidi", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:enable-shaping:
         *
         * Controls whether or not the terminal will shape Arabic text.
         *
         * Since: 0.58
         */
        pspecs[PROP_ENABLE_SHAPING] =
                g_param_spec_boolean ("enable-shaping", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:enable-sixel:
         *
         * Controls whether SIXEL image support is enabled.
         *
         * Since: 0.62
         */
        pspecs[PROP_ENABLE_SIXEL] =
                g_param_spec_boolean ("enable-sixel", nullptr, nullptr,
                                      false,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));


        /**
         * BteTerminal:font-scale:
         *
         * The terminal's font scale.
         */
        pspecs[PROP_FONT_SCALE] =
                g_param_spec_double ("font-scale", NULL, NULL,
                                     BTE_FONT_SCALE_MIN,
                                     BTE_FONT_SCALE_MAX,
                                     1.,
                                     (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:encoding:
         *
         * Controls the encoding the terminal will expect data from the child to
         * be encoded with.  For certain terminal types, applications executing in the
         * terminal can change the encoding.  The default is defined by the
         * application's locale settings.
         *
         * Deprecated: 0.54: Instead of using this, you should use a tool like
         *   luit(1) when support for non-UTF-8 is required
         */
        pspecs[PROP_ENCODING] =
                g_param_spec_string ("encoding", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY | _BTE_PARAM_DEPRECATED));

        /**
         * BteTerminal:font-desc:
         *
         * Specifies the font used for rendering all text displayed by the terminal,
         * overriding any fonts set using ctk_widget_modify_font().  The terminal
         * will immediately attempt to load the desired font, retrieve its
         * metrics, and attempt to resize itself to keep the same number of rows
         * and columns.
         */
        pspecs[PROP_FONT_DESC] =
                g_param_spec_boxed ("font-desc", NULL, NULL,
                                    PANGO_TYPE_FONT_DESCRIPTION,
                                    (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:icon-title:
         *
         * The terminal's so-called icon title, or %NULL if no icon title has been set.
         */
        pspecs[PROP_ICON_TITLE] =
                g_param_spec_string ("icon-title", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:input-enabled:
         *
         * Controls whether the terminal allows user input. When user input is disabled,
         * key press and mouse button press and motion events are not sent to the
         * terminal's child.
         */
        pspecs[PROP_INPUT_ENABLED] =
                g_param_spec_boolean ("input-enabled", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:pointer-autohide:
         *
         * Controls the value of the terminal's mouse autohide setting.  When autohiding
         * is enabled, the mouse cursor will be hidden when the user presses a key and
         * shown when the user moves the mouse.
         */
        pspecs[PROP_MOUSE_POINTER_AUTOHIDE] =
                g_param_spec_boolean ("pointer-autohide", NULL, NULL,
                                      FALSE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:pty:
         *
         * The PTY object for the terminal.
         */
        pspecs[PROP_PTY] =
                g_param_spec_object ("pty", NULL, NULL,
                                     BTE_TYPE_PTY,
                                     (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:rewrap-on-resize:
         *
         * Controls whether or not the terminal will rewrap its contents, including
         * the scrollback buffer, whenever the terminal's width changes.
         *
         * Deprecated: 0.58
         */
        pspecs[PROP_REWRAP_ON_RESIZE] =
                g_param_spec_boolean ("rewrap-on-resize", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:scrollback-lines:
         *
         * The length of the scrollback buffer used by the terminal.  The size of
         * the scrollback buffer will be set to the larger of this value and the number
         * of visible rows the widget can display, so 0 can safely be used to disable
         * scrollback.  Note that this setting only affects the normal screen buffer.
         * For terminal types which have an alternate screen buffer, no scrollback is
         * allowed on the alternate screen buffer.
         */
        pspecs[PROP_SCROLLBACK_LINES] =
                g_param_spec_uint ("scrollback-lines", NULL, NULL,
                                   0, G_MAXUINT,
                                   BTE_SCROLLBACK_INIT,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:scroll-on-keystroke:
         *
         * Controls whether or not the terminal will forcibly scroll to the bottom of
         * the viewable history when the user presses a key.  Modifier keys do not
         * trigger this behavior.
         */
        pspecs[PROP_SCROLL_ON_KEYSTROKE] =
                g_param_spec_boolean ("scroll-on-keystroke", NULL, NULL,
                                      FALSE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:scroll-on-output:
         *
         * Controls whether or not the terminal will forcibly scroll to the bottom of
         * the viewable history when the new data is received from the child.
         */
        pspecs[PROP_SCROLL_ON_OUTPUT] =
                g_param_spec_boolean ("scroll-on-output", NULL, NULL,
                                      TRUE,
                                      (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:text-blink-mode:
         *
         * Controls whether or not the terminal will allow blinking text.
         *
         * Since: 0.52
         */
        pspecs[PROP_TEXT_BLINK_MODE] =
                g_param_spec_enum ("text-blink-mode", NULL, NULL,
                                   BTE_TYPE_TEXT_BLINK_MODE,
                                   BTE_TEXT_BLINK_ALWAYS,
                                   (GParamFlags) (G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:window-title:
         *
         * The terminal's title.
         */
        pspecs[PROP_WINDOW_TITLE] =
                g_param_spec_string ("window-title", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:current-directory-uri:
         *
         * The current directory URI, or %NULL if unset.
         */
        pspecs[PROP_CURRENT_DIRECTORY_URI] =
                g_param_spec_string ("current-directory-uri", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:current-file-uri:
         *
         * The current file URI, or %NULL if unset.
         */
        pspecs[PROP_CURRENT_FILE_URI] =
                g_param_spec_string ("current-file-uri", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:hyperlink-hover-uri:
         *
         * The currently hovered hyperlink URI, or %NULL if unset.
         *
         * Since: 0.50
         */
        pspecs[PROP_HYPERLINK_HOVER_URI] =
                g_param_spec_string ("hyperlink-hover-uri", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        /**
         * BteTerminal:word-char-exceptions:
         *
         * The set of characters which will be considered parts of a word
         * when doing word-wise selection, in addition to the default which only
         * considers alphanumeric characters part of a word.
         *
         * If %NULL, a built-in set is used.
         *
         * Since: 0.40
         */
        pspecs[PROP_WORD_CHAR_EXCEPTIONS] =
                g_param_spec_string ("word-char-exceptions", NULL, NULL,
                                     NULL,
                                     (GParamFlags) (G_PARAM_READABLE | G_PARAM_STATIC_STRINGS | G_PARAM_EXPLICIT_NOTIFY));

        g_object_class_install_properties(gobject_class, LAST_PROP, pspecs);

	/* Disable CtkWidget's keybindings except for Shift-F10 and MenuKey
         * which pop up the context menu.
         */
	binding_set = ctk_binding_set_by_class(bte_terminal_parent_class);
	ctk_binding_entry_skip(binding_set, CDK_KEY_F1, CDK_CONTROL_MASK);
	ctk_binding_entry_skip(binding_set, CDK_KEY_F1, CDK_SHIFT_MASK);
	ctk_binding_entry_skip(binding_set, CDK_KEY_KP_F1, CDK_CONTROL_MASK);
	ctk_binding_entry_skip(binding_set, CDK_KEY_KP_F1, CDK_SHIFT_MASK);

        process_timer = g_timer_new();

        klass->priv = G_TYPE_CLASS_GET_PRIVATE (klass, BTE_TYPE_TERMINAL, BteTerminalClassPrivate);

        klass->priv->fallback_style_provider = CTK_STYLE_PROVIDER (ctk_css_provider_new ());
        /* Some themes don't define text_view_bg */
        ctk_css_provider_load_from_data (CTK_CSS_PROVIDER (klass->priv->fallback_style_provider),
                                         "@define-color text_view_bg @theme_base_color;\n"
                                         "BteTerminal, " BTE_TERMINAL_CSS_NAME " {\n"
                                         "background-color: @text_view_bg;\n"
                                         "color: @theme_text_color;\n"
                                         "}\n",
                                         -1, NULL);
        klass->priv->style_provider = CTK_STYLE_PROVIDER (ctk_css_provider_new ());
        ctk_css_provider_load_from_data (CTK_CSS_PROVIDER (klass->priv->style_provider),
                                         "BteTerminal, " BTE_TERMINAL_CSS_NAME " {\n"
                                         "padding: 1px 1px 1px 1px;\n"
                                         "}\n",
                                         -1, NULL);

#ifdef WITH_A11Y
        /* a11y */
        ctk_widget_class_set_accessible_type(widget_class, BTE_TYPE_TERMINAL_ACCESSIBLE);
#endif
}

static gboolean
bte_terminal_scrollable_get_border(CtkScrollable* scrollable,
                                   CtkBorder* border) noexcept
try
{
        *border = *WIDGET(BTE_TERMINAL(scrollable))->padding();
        return true;
}
catch (...)
{
        bte::log_exception();
        return false;
}

static void
bte_terminal_scrollable_iface_init(CtkScrollableInterface* iface) noexcept
{
        iface->get_border = bte_terminal_scrollable_get_border;
}

/* public API */

/**
 * bte_get_features:
 *
 * Gets a list of features bte was compiled with.
 *
 * Returns: (transfer none): a string with features
 *
 * Since: 0.40
 */
const char *
bte_get_features (void) noexcept
{
        return
#ifdef WITH_FRIBIDI
                "+BIDI"
#else
                "-BIDI"
#endif
                " "
#ifdef WITH_GNUTLS
                "+GNUTLS"
#else
                "-GNUTLS"
#endif
                " "
#ifdef WITH_ICU
                "+ICU"
#else
                "-ICU"
#endif
                " "
#ifdef __linux__
#ifdef WITH_SYSTEMD
                "+SYSTEMD"
#else
                "-SYSTEMD"
#endif
#endif // __linux__
                ;
}

/**
 * bte_get_feature_flags:
 *
 * Gets features BTE was compiled with.
 *
 * Returns: (transfer none): flags from #BteFeatureFlags
 *
 * Since: 0.62
 */
BteFeatureFlags
bte_get_feature_flags(void) noexcept
{
        return BteFeatureFlags(0ULL |
#ifdef WITH_FRIBIDI
                               BTE_FEATURE_FLAG_BIDI |
#endif
#ifdef WITH_ICU
                               BTE_FEATURE_FLAG_ICU |
#endif
#ifdef __linux__
#ifdef WITH_SYSTEMD
                               BTE_FEATURE_FLAG_SYSTEMD |
#endif
#endif // __linux__
                               0ULL);
}

/**
 * bte_get_major_version:
 *
 * Returns the major version of the BTE library at runtime.
 * Contrast this with %BTE_MAJOR_VERSION which represents
 * the version of the BTE library that the code was compiled
 * with.
 *
 * Returns: the major version
 *
 * Since: 0.40
 */
guint
bte_get_major_version (void) noexcept
{
        return BTE_MAJOR_VERSION;
}

/**
 * bte_get_minor_version:
 *
 * Returns the minor version of the BTE library at runtime.
 * Contrast this with %BTE_MINOR_VERSION which represents
 * the version of the BTE library that the code was compiled
 * with.
 *
 * Returns: the minor version
 *
 * Since: 0.40
 */
guint
bte_get_minor_version (void) noexcept
{
        return BTE_MINOR_VERSION;
}

/**
 * bte_get_micro_version:
 *
 * Returns the micro version of the BTE library at runtime.
 * Contrast this with %BTE_MICRO_VERSION which represents
 * the version of the BTE library that the code was compiled
 * with.
 *
 * Returns: the micro version
 *
 * Since: 0.40
 */
guint
bte_get_micro_version (void) noexcept
{
        return BTE_MICRO_VERSION;
}

/**
 * bte_get_user_shell:
 *
 * Gets the user's shell, or %NULL. In the latter case, the
 * system default (usually "/bin/sh") should be used.
 *
 * Returns: (transfer full) (type filename): a newly allocated string with the
 *   user's shell, or %NULL
 */
char *
bte_get_user_shell (void) noexcept
{
	struct passwd *pwd;

	pwd = getpwuid(getuid());
        if (pwd && pwd->pw_shell)
                return g_strdup (pwd->pw_shell);

        return NULL;
}

/**
 * bte_set_test_flags: (skip):
 * @flags: flags
 *
 * Sets test flags. This function is only useful for implementing
 * unit tests for bte itself; it is a no-op in non-debug builds.
 *
 * Since: 0.54
 */
void
bte_set_test_flags(guint64 flags) noexcept
{
#ifdef BTE_DEBUG
        g_test_flags = flags;
#endif
}

/**
 * bte_get_encodings:
 * @include_aliases: whether to include alias names
 *
 * Gets the list of supported legacy encodings.
 *
 * If ICU support is not available, this returns an empty vector.
 * Note that UTF-8 is always supported; you can select it by
 * passing %NULL to bte_terminal_set_encoding().
 *
 * Returns: (transfer full): the list of supported encodings; free with
 *   g_strfreev()
 *
 * Since: 0.60
 * Deprecated: 0.60
 */
char **
bte_get_encodings(gboolean include_aliases) noexcept
try
{
#ifdef WITH_ICU
        return bte::base::get_icu_charsets(include_aliases != FALSE);
#else
        char *empty[] = { nullptr };
        return g_strdupv(empty);
#endif
}
catch (...)
{
        bte::log_exception();

        char *empty[] = { nullptr };
        return g_strdupv(empty);
}

/**
 * bte_get_encoding_supported:
 * @encoding: the name of the legacy encoding
 *
 * Queries whether the legacy encoding @encoding is supported.
 *
 * If ICU support is not available, this function always returns %FALSE.
 *
 * Note that UTF-8 is always supported; you can select it by
 * passing %NULL to bte_terminal_set_encoding().
 *
 * Returns: %TRUE iff the legacy encoding @encoding is supported
 *
 * Since: 0.60
 * Deprecated: 0.60
 */
gboolean
bte_get_encoding_supported(const char *encoding) noexcept
try
{
        g_return_val_if_fail(encoding != nullptr, false);

#ifdef WITH_ICU
        return bte::base::get_icu_charset_supported(encoding);
#else
        return false;
#endif
}
catch (...)
{
        bte::log_exception();
        return false;
}

/* BteTerminal public API */

/**
 * bte_terminal_new:
 *
 * Creates a new terminal widget.
 *
 * Returns: (transfer none) (type Bte.Terminal): a new #BteTerminal object
 */
CtkWidget *
bte_terminal_new(void) noexcept
{
	return (CtkWidget *)g_object_new(BTE_TYPE_TERMINAL, nullptr);
}

/**
 * bte_terminal_copy_clipboard:
 * @terminal: a #BteTerminal
 *
 * Places the selected text in the terminal in the #CDK_SELECTION_CLIPBOARD
 * selection.
 *
 * Deprecated: 0.50: Use bte_terminal_copy_clipboard_format() with %BTE_FORMAT_TEXT
 *   instead.
 */
void
bte_terminal_copy_clipboard(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        IMPL(terminal)->emit_copy_clipboard();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_copy_clipboard_format:
 * @terminal: a #BteTerminal
 * @format: a #BteFormat
 *
 * Places the selected text in the terminal in the #CDK_SELECTION_CLIPBOARD
 * selection in the form specified by @format.
 *
 * For all formats, the selection data (see #CtkSelectionData) will include the
 * text targets (see ctk_target_list_add_text_targets() and
 * ctk_selection_data_targets_includes_text()). For %BTE_FORMAT_HTML,
 * the selection will also include the "text/html" target, which when requested,
 * returns the HTML data in UTF-16 with a U+FEFF BYTE ORDER MARK character at
 * the start.
 *
 * Since: 0.50
 */
void
bte_terminal_copy_clipboard_format(BteTerminal *terminal,
                                   BteFormat format) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(format == BTE_FORMAT_TEXT || format == BTE_FORMAT_HTML);

        WIDGET(terminal)->copy(BTE_SELECTION_CLIPBOARD, format);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_copy_primary:
 * @terminal: a #BteTerminal
 *
 * Places the selected text in the terminal in the #CDK_SELECTION_PRIMARY
 * selection.
 */
void
bte_terminal_copy_primary(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
	_bte_debug_print(BTE_DEBUG_SELECTION, "Copying to PRIMARY.\n");
	WIDGET(terminal)->copy(BTE_SELECTION_PRIMARY, BTE_FORMAT_TEXT);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_paste_clipboard:
 * @terminal: a #BteTerminal
 *
 * Sends the contents of the #CDK_SELECTION_CLIPBOARD selection to the
 * terminal's child. It's called on paste menu item, or when
 * user presses Shift+Insert.
 */
void
bte_terminal_paste_clipboard(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        IMPL(terminal)->emit_paste_clipboard();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_paste_primary:
 * @terminal: a #BteTerminal
 *
 * Sends the contents of the #CDK_SELECTION_PRIMARY selection to the terminal's
 * child. The terminal will call also paste the
 * #CDK_SELECTION_PRIMARY selection when the user clicks with the the second
 * mouse button.
 */
void
bte_terminal_paste_primary(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
	_bte_debug_print(BTE_DEBUG_SELECTION, "Pasting PRIMARY.\n");
	WIDGET(terminal)->paste(CDK_SELECTION_PRIMARY);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_match_add_gregex:
 * @terminal: a #BteTerminal
 * @gregex: a #GRegex
 * @gflags: the #GRegexMatchFlags to use when matching the regex
 *
 * This function does nothing since version 0.60.
 *
 * Returns: -1
 *
 * Deprecated: 0.46: Use bte_terminal_match_add_regex() or bte_terminal_match_add_regex_full() instead.
 */
int
bte_terminal_match_add_gregex(BteTerminal *terminal,
                              GRegex *gregex,
                              GRegexMatchFlags gflags) noexcept
{
        return -1;
}

/**
 * bte_terminal_match_add_regex:
 * @terminal: a #BteTerminal
 * @regex: (transfer none): a #BteRegex
 * @flags: PCRE2 match flags, or 0
 *
 * Adds the regular expression @regex to the list of matching expressions.  When the
 * user moves the mouse cursor over a section of displayed text which matches
 * this expression, the text will be highlighted.
 *
 * Note that @regex should have been created using the %PCRE2_MULTILINE flag.
 *
 * Returns: an integer associated with this expression
 *
 * Since: 0.46
 */
int
bte_terminal_match_add_regex(BteTerminal *terminal,
                             BteRegex    *regex,
                             guint32      flags) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), -1);
	g_return_val_if_fail(regex != NULL, -1);
        g_return_val_if_fail(_bte_regex_has_purpose(regex, bte::base::Regex::Purpose::eMatch), -1);
        g_warn_if_fail(_bte_regex_has_multiline_compile_flag(regex));

        auto impl = IMPL(terminal);
        return impl->regex_match_add(bte::base::make_ref(regex_from_wrapper(regex)),
                                     flags,
                                     BTE_DEFAULT_CURSOR,
                                     impl->regex_match_next_tag()).tag();
}
catch (...)
{
        bte::log_exception();
        return -1;
}

/**
 * bte_terminal_match_check:
 * @terminal: a #BteTerminal
 * @column: the text column
 * @row: the text row
 * @tag: (out) (allow-none): a location to store the tag, or %NULL
 *
 * Checks if the text in and around the specified position matches any of the
 * regular expressions previously set using bte_terminal_match_add().  If a
 * match exists, the text string is returned and if @tag is not %NULL, the number
 * associated with the matched regular expression will be stored in @tag.
 *
 * If more than one regular expression has been set with
 * bte_terminal_match_add(), then expressions are checked in the order in
 * which they were added.
 *
 * Returns: (transfer full) (nullable): a newly allocated string which matches one of the previously
 *   set regular expressions
 *
 * Deprecated: 0.46: Use bte_terminal_match_check_event() instead.
 */
char *
bte_terminal_match_check(BteTerminal *terminal,
                         long column,
                         long row,
			 int *tag) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
        return WIDGET(terminal)->regex_match_check(column, row, tag);
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_match_check_event:
 * @terminal: a #BteTerminal
 * @event: a #CdkEvent
 * @tag: (out) (allow-none): a location to store the tag, or %NULL
 *
 * Checks if the text in and around the position of the event matches any of the
 * regular expressions previously set using bte_terminal_match_add().  If a
 * match exists, the text string is returned and if @tag is not %NULL, the number
 * associated with the matched regular expression will be stored in @tag.
 *
 * If more than one regular expression has been set with
 * bte_terminal_match_add(), then expressions are checked in the order in
 * which they were added.
 *
 * Returns: (transfer full) (nullable): a newly allocated string which matches one of the previously
 *   set regular expressions, or %NULL if there is no match
 */
char *
bte_terminal_match_check_event(BteTerminal *terminal,
                               CdkEvent *event,
                               int *tag) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), FALSE);
        return WIDGET(terminal)->regex_match_check(event, tag);
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_hyperlink_check_event:
 * @terminal: a #BteTerminal
 * @event: a #CdkEvent
 *
 * Returns a nonempty string: the target of the explicit hyperlink (printed using the OSC 8
 * escape sequence) at the position of the event, or %NULL.
 *
 * Proper use of the escape sequence should result in URI-encoded URIs with a proper scheme
 * like "http://", "https://", "file://", "mailto:" etc. This is, however, not enforced by BTE.
 * The caller must tolerate the returned string potentially not being a valid URI.
 *
 * Returns: (transfer full) (nullable): a newly allocated string containing the target of the hyperlink,
 *  or %NULL
 *
 * Since: 0.50
 */
char *
bte_terminal_hyperlink_check_event(BteTerminal *terminal,
                                   CdkEvent *event) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), nullptr);
        return WIDGET(terminal)->hyperlink_check(event);
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_event_check_regex_array: (rename-to bte_terminal_event_check_regex_simple)
 * @terminal: a #BteTerminal
 * @event: a #CdkEvent
 * @regexes: (array length=n_regexes): an array of #BteRegex
 * @n_regexes: number of items in @regexes
 * @match_flags: PCRE2 match flags, or 0
 * @n_matches: (out) (optional): number of items in @matches, which is always equal to @n_regexes
 *
 * Like bte_terminal_event_check_regex_simple(), but returns an array of strings,
 * containing the matching text (or %NULL if no match) corresponding to each of the
 * regexes in @regexes.
 *
 * You must free each string and the array; but note that this is *not* a %NULL-terminated
 * string array, and so you must *not* use g_strfreev() on it.
 *
 * Returns: (nullable) (transfer full) (array length=n_matches): a newly allocated array of strings,
 *   or %NULL if none of the regexes matched
 *
 * Since: 0.62
 */
char**
bte_terminal_event_check_regex_array(BteTerminal *terminal,
                                     CdkEvent *event,
                                     BteRegex **regexes,
                                     gsize n_regexes,
                                     guint32 match_flags,
                                     gsize *n_matches) noexcept
try
{
        if (n_matches)
                *n_matches = n_regexes;

        if (n_regexes == 0)
                return nullptr;

        auto matches = bte::glib::take_free_ptr(g_new0(char*, n_regexes));
        if (!bte_terminal_event_check_regex_simple(terminal,
                                                   event,
                                                   regexes,
                                                   n_regexes,
                                                   match_flags,
                                                   matches.get()))
            return nullptr;

        return matches.release();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_event_check_regex_simple: (skip)
 * @terminal: a #BteTerminal
 * @event: a #CdkEvent
 * @regexes: (array length=n_regexes): an array of #BteRegex
 * @n_regexes: number of items in @regexes
 * @match_flags: PCRE2 match flags, or 0
 * @matches: (out caller-allocates) (array length=n_regexes) (transfer full): a location to store the matches
 *
 * Checks each regex in @regexes if the text in and around the position of
 * the event matches the regular expressions.  If a match exists, the matched
 * text is stored in @matches at the position of the regex in @regexes; otherwise
 * %NULL is stored there.  Each non-%NULL element of @matches should be freed with
 * g_free().
 *
 * Note that the regexes in @regexes should have been created using the %PCRE2_MULTILINE flag.
 *
 * Returns: %TRUE iff any of the regexes produced a match
 *
 * Since: 0.46
 */
gboolean
bte_terminal_event_check_regex_simple(BteTerminal *terminal,
                                      CdkEvent *event,
                                      BteRegex **regexes,
                                      gsize n_regexes,
                                      guint32 match_flags,
                                      char **matches) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), FALSE);
        g_return_val_if_fail(event != NULL, FALSE);
        g_return_val_if_fail(regexes != NULL || n_regexes == 0, FALSE);
        for (gsize i = 0; i < n_regexes; i++) {
                g_return_val_if_fail(_bte_regex_has_purpose(regexes[i], bte::base::Regex::Purpose::eMatch), -1);
                g_warn_if_fail(_bte_regex_has_multiline_compile_flag(regexes[i]));
        }
        g_return_val_if_fail(matches != NULL, FALSE);

        return WIDGET(terminal)->regex_match_check_extra(event,
                                                         regex_array_from_wrappers(regexes),
                                                         n_regexes,
                                                         match_flags,
                                                         matches);
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_event_check_gregex_simple:
 * @terminal: a #BteTerminal
 * @event: a #CdkEvent
 * @regexes: (array length=n_regexes): an array of #GRegex
 * @n_regexes: number of items in @regexes
 * @match_flags: the #GRegexMatchFlags to use when matching the regexes
 * @matches: (out caller-allocates) (array length=n_regexes): a location to store the matches
 *
 * This function does nothing.
 *
 * Returns: %FALSE
 *
 * Since: 0.44
 * Deprecated: 0.46: Use bte_terminal_event_check_regex_simple() instead.
 */
gboolean
bte_terminal_event_check_gregex_simple(BteTerminal *terminal,
                                       CdkEvent *event,
                                       GRegex **regexes,
                                       gsize n_regexes,
                                       GRegexMatchFlags match_flags,
                                       char **matches) noexcept
{
        return FALSE;
}

/**
 * bte_terminal_match_set_cursor:
 * @terminal: a #BteTerminal
 * @tag: the tag of the regex which should use the specified cursor
 * @cursor: (allow-none): the #CdkCursor which the terminal should use when the pattern is
 *   highlighted, or %NULL to use the standard cursor
 *
 * Sets which cursor the terminal will use if the pointer is over the pattern
 * specified by @tag.  The terminal keeps a reference to @cursor.
 *
 * Deprecated: 0.40: Use bte_terminal_match_set_cursor_name() instead.
 */
void
bte_terminal_match_set_cursor(BteTerminal *terminal,
                              int tag,
                              CdkCursor *cursor) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(tag >= 0);
        if (auto rem = IMPL(terminal)->regex_match_get(tag))
                rem->set_cursor(bte::glib::make_ref<CdkCursor>(cursor));
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_match_set_cursor_type:
 * @terminal: a #BteTerminal
 * @tag: the tag of the regex which should use the specified cursor
 * @cursor_type: a #CdkCursorType
 *
 * Sets which cursor the terminal will use if the pointer is over the pattern
 * specified by @tag.
 *
 * Deprecated: 0.54: Use bte_terminal_match_set_cursor_name() instead.
 */
void
bte_terminal_match_set_cursor_type(BteTerminal *terminal,
				   int tag,
                                   CdkCursorType cursor_type) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(tag >= 0);
        if (auto rem = IMPL(terminal)->regex_match_get(tag))
                rem->set_cursor(cursor_type);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_match_set_cursor_name:
 * @terminal: a #BteTerminal
 * @tag: the tag of the regex which should use the specified cursor
 * @cursor_name: the name of the cursor
 *
 * Sets which cursor the terminal will use if the pointer is over the pattern
 * specified by @tag.
 */
void
bte_terminal_match_set_cursor_name(BteTerminal *terminal,
				   int tag,
                                   const char *cursor_name) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(tag >= 0);
        if (auto rem = IMPL(terminal)->regex_match_get(tag))
                rem->set_cursor(cursor_name);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_match_remove:
 * @terminal: a #BteTerminal
 * @tag: the tag of the regex to remove
 *
 * Removes the regular expression which is associated with the given @tag from
 * the list of expressions which the terminal will highlight when the user
 * moves the mouse cursor over matching text.
 */
void
bte_terminal_match_remove(BteTerminal *terminal,
                          int tag) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        IMPL(terminal)->regex_match_remove(tag);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_match_remove_all:
 * @terminal: a #BteTerminal
 *
 * Clears the list of regular expressions the terminal uses to highlight text
 * when the user moves the mouse cursor.
 */
void
bte_terminal_match_remove_all(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        IMPL(terminal)->regex_match_remove_all();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_search_find_previous:
 * @terminal: a #BteTerminal
 *
 * Searches the previous string matching the search regex set with
 * bte_terminal_search_set_regex().
 *
 * Returns: %TRUE if a match was found
 */
gboolean
bte_terminal_search_find_previous (BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->search_find(true);
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_search_find_next:
 * @terminal: a #BteTerminal
 *
 * Searches the next string matching the search regex set with
 * bte_terminal_search_set_regex().
 *
 * Returns: %TRUE if a match was found
 */
gboolean
bte_terminal_search_find_next (BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->search_find(false);
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_search_set_regex:
 * @terminal: a #BteTerminal
 * @regex: (allow-none): a #BteRegex, or %NULL
 * @flags: PCRE2 match flags, or 0
 *
 * Sets the regex to search for. Unsets the search regex when passed %NULL.
 *
 * Note that @regex should have been created using the %PCRE2_MULTILINE flag.
 *
 * Since: 0.46
 */
void
bte_terminal_search_set_regex (BteTerminal *terminal,
                               BteRegex    *regex,
                               guint32      flags) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(regex == nullptr || _bte_regex_has_purpose(regex, bte::base::Regex::Purpose::eSearch));
        g_warn_if_fail(regex == nullptr || _bte_regex_has_multiline_compile_flag(regex));

        IMPL(terminal)->search_set_regex(bte::base::make_ref(regex_from_wrapper(regex)), flags);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_search_get_regex:
 * @terminal: a #BteTerminal
 *
 * Returns: (transfer none): the search #BteRegex regex set in @terminal, or %NULL
 *
 * Since: 0.46
 */
BteRegex *
bte_terminal_search_get_regex(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), nullptr);

        return wrapper_from_regex(IMPL(terminal)->search_regex());
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_search_set_gregex:
 * @terminal: a #BteTerminal
 * @gregex: (allow-none): a #GRegex, or %NULL
 * @gflags: flags from #GRegexMatchFlags
 *
 * This function does nothing since version 0.60.
 *
 * Deprecated: 0.46: use bte_terminal_search_set_regex() instead.
 */
void
bte_terminal_search_set_gregex (BteTerminal *terminal,
				GRegex      *gregex,
                                GRegexMatchFlags gflags) noexcept
{
}

/**
 * bte_terminal_search_get_gregex:
 * @terminal: a #BteTerminal
 *
 * Returns: (transfer none): %NULL
 *
 * Deprecated: 0.46: use bte_terminal_search_get_regex() instead.
 */
GRegex *
bte_terminal_search_get_gregex (BteTerminal *terminal) noexcept
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), nullptr);

        return nullptr;
}

/**
 * bte_terminal_search_set_wrap_around:
 * @terminal: a #BteTerminal
 * @wrap_around: whether search should wrap
 *
 * Sets whether search should wrap around to the beginning of the
 * terminal content when reaching its end.
 */
void
bte_terminal_search_set_wrap_around (BteTerminal *terminal,
				     gboolean     wrap_around) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        IMPL(terminal)->search_set_wrap_around(wrap_around != FALSE);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_search_get_wrap_around:
 * @terminal: a #BteTerminal
 *
 * Returns: whether searching will wrap around
 */
gboolean
bte_terminal_search_get_wrap_around (BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_search_wrap_around;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_select_all:
 * @terminal: a #BteTerminal
 *
 * Selects all text within the terminal (including the scrollback buffer).
 */
void
bte_terminal_select_all (BteTerminal *terminal) noexcept
try
{
	g_return_if_fail (BTE_IS_TERMINAL (terminal));

        IMPL(terminal)->select_all();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_unselect_all:
 * @terminal: a #BteTerminal
 *
 * Clears the current selection.
 */
void
bte_terminal_unselect_all(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail (BTE_IS_TERMINAL (terminal));

        IMPL(terminal)->deselect_all();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_cursor_position:
 * @terminal: a #BteTerminal
 * @column: (out) (allow-none): a location to store the column, or %NULL
 * @row: (out) (allow-none): a location to store the row, or %NULL
 *
 * Reads the location of the insertion cursor and returns it.  The row
 * coordinate is absolute.
 *
 * This method is unaware of BiDi. The returned column is logical column.
 */
void
bte_terminal_get_cursor_position(BteTerminal *terminal,
				 long *column,
                                 long *row) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        auto impl = IMPL(terminal);
	if (column) {
                *column = impl->m_screen->cursor.col;
	}
	if (row) {
                *row = impl->m_screen->cursor.row;
	}
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_pty_new_sync:
 * @terminal: a #BteTerminal
 * @flags: flags from #BtePtyFlags
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Creates a new #BtePty, sets the emulation property
 * from #BteTerminal:emulation, and sets the size using
 * @terminal's size.
 *
 * See bte_pty_new() for more information.
 *
 * Returns: (transfer full): a new #BtePty
 */
BtePty *
bte_terminal_pty_new_sync(BteTerminal *terminal,
                          BtePtyFlags flags,
                          GCancellable *cancellable,
                          GError **error) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);

        auto pty = bte::glib::take_ref(bte_pty_new_sync(flags, cancellable, error));
        if (!pty)
                return nullptr;

        auto impl = IMPL(terminal);
        _bte_pty_set_size(pty.get(),
                          impl->m_row_count,
                          impl->m_column_count,
                          impl->m_cell_height,
                          impl->m_cell_width,
                          nullptr);

        return pty.release();
}
catch (...)
{
        bte::glib::set_error_from_exception(error);
        return nullptr;
}

/**
 * bte_terminal_watch_child:
 * @terminal: a #BteTerminal
 * @child_pid: a #GPid
 *
 * Watches @child_pid. When the process exists, the #BteTerminal::child-exited
 * signal will be called with the child's exit status.
 *
 * Prior to calling this function, a #BtePty must have been set in @terminal
 * using bte_terminal_set_pty().
 * When the child exits, the terminal's #BtePty will be set to %NULL.
 *
 * Note: g_child_watch_add() or g_child_watch_add_full() must not have
 * been called for @child_pid, nor a #GSource for it been created with
 * g_child_watch_source_new().
 *
 * Note: when using the g_spawn_async() family of functions,
 * the %G_SPAWN_DO_NOT_REAP_CHILD flag MUST have been passed.
 */
void
bte_terminal_watch_child (BteTerminal *terminal,
                          GPid child_pid) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(child_pid != -1);

        g_return_if_fail(WIDGET(terminal)->pty() != nullptr);

        IMPL(terminal)->watch_child(child_pid);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_spawn_sync:
 * @terminal: a #BteTerminal
 * @pty_flags: flags from #BtePtyFlags
 * @working_directory: (allow-none): the name of a directory the command should start
 *   in, or %NULL to use the current working directory
 * @argv: (array zero-terminated=1) (element-type filename): child's argument vector
 * @envv: (allow-none) (array zero-terminated=1) (element-type filename): a list of environment
 *   variables to be added to the environment before starting the process, or %NULL
 * @spawn_flags: flags from #GSpawnFlags
 * @child_setup: (allow-none) (scope call): an extra child setup function to run in the child just before exec(), or %NULL
 * @child_setup_data: user data for @child_setup
 * @child_pid: (out) (allow-none) (transfer full): a location to store the child PID, or %NULL
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Starts the specified command under a newly-allocated controlling
 * pseudo-terminal.  The @argv and @envv lists should be %NULL-terminated.
 * The "TERM" environment variable is automatically set to a default value,
 * but can be overridden from @envv.
 * @pty_flags controls logging the session to the specified system log files.
 *
 * Note that %G_SPAWN_DO_NOT_REAP_CHILD will always be added to @spawn_flags.
 *
 * Note also that %G_SPAWN_STDOUT_TO_DEV_NULL, %G_SPAWN_STDERR_TO_DEV_NULL,
 * and %G_SPAWN_CHILD_INHERITS_STDIN are not supported in @spawn_flags, since
 * stdin, stdout and stderr of the child process will always be connected to
 * the PTY.
 *
 * Note that all open file descriptors will be closed in the child. If you want
 * to keep some file descriptor open for use in the child process, you need to
 * use a child setup function that unsets the FD_CLOEXEC flag on that file
 * descriptor.
 *
 * See bte_pty_new(), g_spawn_async() and bte_terminal_watch_child() for more information.
 *
 * Beginning with 0.52, sets PWD to @working_directory in order to preserve symlink components.
 * The caller should also make sure that symlinks were preserved while constructing the value of @working_directory,
 * e.g. by using bte_terminal_get_current_directory_uri(), g_get_current_dir() or get_current_dir_name().
 *
 * Returns: %TRUE on success, or %FALSE on error with @error filled in
 *
 * Deprecated: 0.48: Use bte_terminal_spawn_async() instead.
 */
gboolean
bte_terminal_spawn_sync(BteTerminal *terminal,
                        BtePtyFlags pty_flags,
                        const char *working_directory,
                        char **argv,
                        char **envv,
                        GSpawnFlags spawn_flags,
                        GSpawnChildSetupFunc child_setup,
                        gpointer child_setup_data,
                        GPid *child_pid /* out */,
                        GCancellable *cancellable,
                        GError **error) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), FALSE);
        g_return_val_if_fail(argv != NULL, FALSE);
        g_return_val_if_fail(argv[0] != nullptr, FALSE);
        g_return_val_if_fail(envv == nullptr ||_bte_pty_check_envv(envv), false);
        g_return_val_if_fail((spawn_flags & (BTE_SPAWN_NO_SYSTEMD_SCOPE | BTE_SPAWN_REQUIRE_SYSTEMD_SCOPE)) == 0, FALSE);
        g_return_val_if_fail(child_setup_data == NULL || child_setup, FALSE);
        g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

        auto new_pty = bte::glib::take_ref(bte_terminal_pty_new_sync(terminal, pty_flags, cancellable, error));
        if (!new_pty)
                return false;

        GPid pid;
        if (!_bte_pty_spawn_sync(new_pty.get(),
                                 working_directory,
                                 argv,
                                 envv,
                                 spawn_flags,
                                 child_setup, child_setup_data, nullptr,
                                 &pid,
                                 -1 /* default timeout */,
                                 cancellable,
                                 error))
                return false;

        bte_terminal_set_pty(terminal, new_pty.get());
        bte_terminal_watch_child(terminal, pid);

        if (child_pid)
                *child_pid = pid;

        return true;
}
catch (...)
{
        return bte::glib::set_error_from_exception(error);
}

typedef struct {
        GWeakRef wref;
        BteTerminalSpawnAsyncCallback callback;
        gpointer user_data;
} SpawnAsyncCallbackData;

static gpointer
spawn_async_callback_data_new(BteTerminal *terminal,
                              BteTerminalSpawnAsyncCallback callback,
                              gpointer user_data) noexcept
{
        SpawnAsyncCallbackData *data = g_new0 (SpawnAsyncCallbackData, 1);

        g_weak_ref_init(&data->wref, terminal);
        data->callback = callback;
        data->user_data = user_data;

        return data;
}

static void
spawn_async_callback_data_free(SpawnAsyncCallbackData* data) noexcept
{
        g_weak_ref_clear(&data->wref);
        g_free(data);
}

static void
spawn_async_cb(GObject *source,
               GAsyncResult *result,
               gpointer user_data) noexcept
{
        SpawnAsyncCallbackData *data = reinterpret_cast<SpawnAsyncCallbackData*>(user_data);
        BtePty *pty = BTE_PTY(source);

        auto pid = pid_t{-1};
        auto error = bte::glib::Error{};
        if (source) {
                bte_pty_spawn_finish(pty, result, &pid, error);
        } else {
                (void)g_task_propagate_int(G_TASK(result), error);
                assert(error.error());
        }

        /* Now get a ref to the terminal */
        auto terminal = bte::glib::acquire_ref<BteTerminal>(&data->wref);

        if (terminal) {
                if (pid != -1) {
                        bte_terminal_set_pty(terminal.get(), pty);
                        bte_terminal_watch_child(terminal.get(), pid);
                } else {
                        bte_terminal_set_pty(terminal.get(), nullptr);
                }
        }

        if (data->callback) {
                try {
                        data->callback(terminal.get(), pid, error, data->user_data);
                } catch (...) {
                        bte::log_exception();
                }
        }

        if (!terminal) {
                /* If the terminal was destroyed, we need to abort the child process, if any */
                if (pid != -1) {
                        pid_t pgrp;
                        pgrp = getpgid(pid);
                        if (pgrp != -1 && pgrp != getpgid(getpid())) {
                                kill(-pgrp, SIGHUP);
                        }

                        kill(pid, SIGHUP);
                }
        }

        spawn_async_callback_data_free(data);
}

/**
 * BteTerminalSpawnAsyncCallback:
 * @terminal: the #BteTerminal
 * @pid: a #GPid
 * @error: a #GError, or %NULL
 * @user_data: user data that was passed to bte_terminal_spawn_async
 *
 * Callback for bte_terminal_spawn_async().
 *
 * On success, @pid contains the PID of the spawned process, and @error
 * is %NULL.
 * On failure, @pid is -1 and @error contains the error information.
 *
 * Since: 0.48
 */

/**
 * bte_terminal_spawn_with_fds_async:
 * @terminal: a #BteTerminal
 * @pty_flags: flags from #BtePtyFlags
 * @working_directory: (allow-none): the name of a directory the command should start
 *   in, or %NULL to use the current working directory
 * @argv: (array zero-terminated=1) (element-type filename): child's argument vector
 * @envv: (allow-none) (array zero-terminated=1) (element-type filename): a list of environment
 *   variables to be added to the environment before starting the process, or %NULL
 * @fds: (nullable) (array length=n_fds) (transfer none) (scope call): an array of file descriptors, or %NULL
 * @n_fds: the number of file descriptors in @fds, or 0 if @fds is %NULL
 * @map_fds: (nullable) (array length=n_map_fds) (transfer none) (scope call): an array of integers, or %NULL
 * @n_map_fds: the number of elements in @map_fds, or 0 if @map_fds is %NULL
 * @spawn_flags: flags from #GSpawnFlags
 * @child_setup: (allow-none) (scope async): an extra child setup function to run in the child just before exec(), or %NULL
 * @child_setup_data: (nullable) (closure child_setup): user data for @child_setup, or %NULL
 * @child_setup_data_destroy: (nullable) (destroy child_setup_data): a #GDestroyNotify for @child_setup_data, or %NULL
 * @timeout: a timeout value in ms, -1 for the default timeout, or G_MAXINT to wait indefinitely
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @callback: (nullable) (scope async): a #BteTerminalSpawnAsyncCallback, or %NULL
 * @user_data: (closure callback): user data for @callback, or %NULL
 *
 * A convenience function that wraps creating the #BtePty and spawning
 * the child process on it. See bte_pty_new_sync(), bte_pty_spawn_with_fds_async(),
 * and bte_pty_spawn_finish() for more information.
 *
 * When the operation is finished successfully, @callback will be called
 * with the child #GPid, and a %NULL #GError. The child PID will already be
 * watched via bte_terminal_watch_child().
 *
 * When the operation fails, @callback will be called with a -1 #GPid,
 * and a non-%NULL #GError containing the error information.
 *
 * Note that %G_SPAWN_STDOUT_TO_DEV_NULL, %G_SPAWN_STDERR_TO_DEV_NULL,
 * and %G_SPAWN_CHILD_INHERITS_STDIN are not supported in @spawn_flags, since
 * stdin, stdout and stderr of the child process will always be connected to
 * the PTY.
 *
 * If @fds is not %NULL, the child process will map the file descriptors from
 * @fds according to @map_fds; @n_map_fds must be less or equal to @n_fds.
 * This function will take ownership of the file descriptors in @fds;
 * you must not use or close them after this call.
 *
 * Note that all  open file descriptors apart from those mapped as above
 * will be closed in the child. (If you want to keep some other file descriptor
 * open for use in the child process, you need to use a child setup function
 * that unsets the FD_CLOEXEC flag on that file descriptor manually.)
 *
 * Beginning with 0.60, and on linux only, and unless %BTE_SPAWN_NO_SYSTEMD_SCOPE is
 * passed in @spawn_flags, the newly created child process will be moved to its own
 * systemd user scope; and if %BTE_SPAWN_REQUIRE_SYSTEMD_SCOPE is passed, and creation
 * of the systemd user scope fails, the whole spawn will fail.
 * You can override the options used for the systemd user scope by
 * providing a systemd override file for 'bte-spawn-.scope' unit. See man:systemd.unit(5)
 * for further information.
 *
 * Note that if @terminal has been destroyed before the operation is called,
 * @callback will be called with a %NULL @terminal; you must not do anything
 * in the callback besides freeing any resources associated with @user_data,
 * but taking care not to access the now-destroyed #BteTerminal. Note that
 * in this case, if spawning was successful, the child process will be aborted
 * automatically.
 *
 * Beginning with 0.52, sets PWD to @working_directory in order to preserve symlink components.
 * The caller should also make sure that symlinks were preserved while constructing the value of @working_directory,
 * e.g. by using bte_terminal_get_current_directory_uri(), g_get_current_dir() or get_current_dir_name().
 *
 * Since: 0.62
 */
void
bte_terminal_spawn_with_fds_async(BteTerminal *terminal,
                                  BtePtyFlags pty_flags,
                                  const char *working_directory,
                                  char const* const* argv,
                                  char const* const* envv,
                                  int const* fds,
                                  int n_fds,
                                  int const* fd_map_to,
                                  int n_fd_map_to,
                                  GSpawnFlags spawn_flags,
                                  GSpawnChildSetupFunc child_setup,
                                  gpointer child_setup_data,
                                  GDestroyNotify child_setup_data_destroy,
                                  int timeout,
                                  GCancellable *cancellable,
                                  BteTerminalSpawnAsyncCallback callback,
                                  gpointer user_data) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(cancellable == nullptr || G_IS_CANCELLABLE (cancellable));

        auto error = bte::glib::Error{};
        auto pty = bte::glib::take_ref(bte_terminal_pty_new_sync(terminal, pty_flags, cancellable, error));
        if (!pty) {
                auto task = bte::glib::take_ref(g_task_new(nullptr,
                                                           cancellable,
                                                           spawn_async_cb,
                                                           spawn_async_callback_data_new(terminal, callback, user_data)));
                g_task_return_error(task.get(), error.release());
                return;
        }

        bte_pty_spawn_with_fds_async(pty.get(),
                                     working_directory,
                                     argv,
                                     envv,
                                     fds, n_fds, fd_map_to, n_fd_map_to,
                                     spawn_flags,
                                     child_setup, child_setup_data, child_setup_data_destroy,
                                     timeout, cancellable,
                                     spawn_async_cb,
                                     spawn_async_callback_data_new(terminal, callback, user_data));
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_spawn_async:
 * @terminal: a #BteTerminal
 * @pty_flags: flags from #BtePtyFlags
 * @working_directory: (allow-none): the name of a directory the command should start
 *   in, or %NULL to use the current working directory
 * @argv: (array zero-terminated=1) (element-type filename): child's argument vector
 * @envv: (allow-none) (array zero-terminated=1) (element-type filename): a list of environment
 *   variables to be added to the environment before starting the process, or %NULL
 * @spawn_flags: flags from #GSpawnFlags
 * @child_setup: (allow-none) (scope async): an extra child setup function to run in the child just before exec(), or %NULL
 * @child_setup_data: (nullable) (closure child_setup): user data for @child_setup, or %NULL
 * @child_setup_data_destroy: (nullable) (destroy child_setup_data): a #GDestroyNotify for @child_setup_data, or %NULL
 * @timeout: a timeout value in ms, -1 for the default timeout, or G_MAXINT to wait indefinitely
 * @cancellable: (allow-none): a #GCancellable, or %NULL
 * @callback: (nullable) (scope async): a #BteTerminalSpawnAsyncCallback, or %NULL
 * @user_data: (closure callback): user data for @callback, or %NULL
 *
 * A convenience function that wraps creating the #BtePty and spawning
 * the child process on it. Like bte_terminal_spawn_with_fds_async(),
 * except that this function does not allow passing file descriptors to
 * the child process. See bte_terminal_spawn_with_fds_async() for more
 * information.
 *
 * Since: 0.48
 */
void
bte_terminal_spawn_async(BteTerminal *terminal,
                         BtePtyFlags pty_flags,
                         const char *working_directory,
                         char **argv,
                         char **envv,
                         GSpawnFlags spawn_flags,
                         GSpawnChildSetupFunc child_setup,
                         gpointer child_setup_data,
                         GDestroyNotify child_setup_data_destroy,
                         int timeout,
                         GCancellable *cancellable,
                         BteTerminalSpawnAsyncCallback callback,
                         gpointer user_data) noexcept
{
        bte_terminal_spawn_with_fds_async(terminal, pty_flags, working_directory, argv, envv,
                                          nullptr, 0, nullptr, 0,
                                          spawn_flags,
                                          child_setup, child_setup_data, child_setup_data_destroy,
                                          timeout, cancellable,
                                          callback, user_data);
}

/**
 * bte_terminal_feed:
 * @terminal: a #BteTerminal
 * @data: (array length=length) (element-type guint8) (allow-none): a string in the terminal's current encoding
 * @length: the length of the string, or -1 to use the full length or a nul-terminated string
 *
 * Interprets @data as if it were data received from a child process.
 */
void
bte_terminal_feed(BteTerminal *terminal,
                  const char *data,
                  gssize length) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(length == 0 || data != NULL);

        if (length == 0)
                return;

        auto const len = size_t{length == -1 ? strlen(data) : size_t(length)};
        WIDGET(terminal)->feed({data, len});
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_feed_child:
 * @terminal: a #BteTerminal
 * @text: (array length=length) (element-type guint8) (allow-none): data to send to the child
 * @length: length of @text in bytes, or -1 if @text is NUL-terminated
 *
 * Sends a block of UTF-8 text to the child as if it were entered by the user
 * at the keyboard.
 */
void
bte_terminal_feed_child(BteTerminal *terminal,
                        const char *text,
                        gssize length) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(length == 0 || text != NULL);

        if (length == 0)
                return;

        auto const len = size_t{length == -1 ? strlen(text) : size_t(length)};
        WIDGET(terminal)->feed_child({text, len});
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_feed_child_binary:
 * @terminal: a #BteTerminal
 * @data: (array length=length) (element-type guint8) (allow-none): data to send to the child
 * @length: length of @data
 *
 * Sends a block of binary data to the child.
 *
 * Deprecated: 0.60: Don't send binary data. Use bte_terminal_feed_child() instead to send
 *   UTF-8 text
 */
void
bte_terminal_feed_child_binary(BteTerminal *terminal,
                               const guint8 *data,
                               gsize length) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(length == 0 || data != NULL);

        if (length == 0)
                return;

        WIDGET(terminal)->feed_child_binary({(char*)data, length});
}
catch (...)
{
        bte::log_exception();
}

/**
 * BteSelectionFunc:
 * @terminal: terminal in which the cell is.
 * @column: column in which the cell is.
 * @row: row in which the cell is.
 * @data: (closure): user data.
 *
 * Specifies the type of a selection function used to check whether
 * a cell has to be selected or not.
 *
 * Returns: %TRUE if cell has to be selected; %FALSE if otherwise.
 */

static void
warn_if_callback(BteSelectionFunc func) noexcept
{
        if (!func)
                return;

#ifndef BTE_DEBUG
        static gboolean warned = FALSE;
        if (warned)
                return;
        warned = TRUE;
#endif
        g_warning ("BteSelectionFunc callback ignored.\n");
}

/**
 * bte_terminal_get_text:
 * @terminal: a #BteTerminal
 * @is_selected: (scope call) (allow-none): a #BteSelectionFunc callback
 * @user_data: (closure): user data to be passed to the callback
 * @attributes: (out caller-allocates) (transfer full) (array) (element-type Bte.CharAttributes): location for storing text attributes
 *
 * Extracts a view of the visible part of the terminal.  If @is_selected is not
 * %NULL, characters will only be read if @is_selected returns %TRUE after being
 * passed the column and row, respectively.  A #BteCharAttributes structure
 * is added to @attributes for each byte added to the returned string detailing
 * the character's position, colors, and other characteristics.
 *
 * This method is unaware of BiDi. The columns returned in @attributes are
 * logical columns.
 *
 * Returns: (transfer full): a newly allocated text string, or %NULL.
 */
char *
bte_terminal_get_text(BteTerminal *terminal,
		      BteSelectionFunc is_selected,
		      gpointer user_data,
		      GArray *attributes) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
        warn_if_callback(is_selected);
        auto text = IMPL(terminal)->get_text_displayed(true /* wrap */,
                                                       attributes);
        if (text == nullptr)
                return nullptr;
        return (char*)g_string_free(text, FALSE);
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_get_text_include_trailing_spaces:
 * @terminal: a #BteTerminal
 * @is_selected: (scope call) (allow-none): a #BteSelectionFunc callback
 * @user_data: (closure): user data to be passed to the callback
 * @attributes: (out caller-allocates) (transfer full) (array) (element-type Bte.CharAttributes): location for storing text attributes
 *
 * Extracts a view of the visible part of the terminal.  If @is_selected is not
 * %NULL, characters will only be read if @is_selected returns %TRUE after being
 * passed the column and row, respectively.  A #BteCharAttributes structure
 * is added to @attributes for each byte added to the returned string detailing
 * the character's position, colors, and other characteristics.
 *
 * This method is unaware of BiDi. The columns returned in @attributes are
 * logical columns.
 *
 * Returns: (transfer full): a newly allocated text string, or %NULL.
 *
 * Deprecated: 0.56: Use bte_terminal_get_text() instead.
 */
char *
bte_terminal_get_text_include_trailing_spaces(BteTerminal *terminal,
					      BteSelectionFunc is_selected,
					      gpointer user_data,
					      GArray *attributes) noexcept
{
        return bte_terminal_get_text(terminal, is_selected, user_data, attributes);
}

/**
 * bte_terminal_get_text_range:
 * @terminal: a #BteTerminal
 * @start_row: first row to search for data
 * @start_col: first column to search for data
 * @end_row: last row to search for data
 * @end_col: last column to search for data
 * @is_selected: (scope call) (allow-none): a #BteSelectionFunc callback
 * @user_data: (closure): user data to be passed to the callback
 * @attributes: (out caller-allocates) (transfer full) (array) (element-type Bte.CharAttributes): location for storing text attributes
 *
 * Extracts a view of the visible part of the terminal.  If @is_selected is not
 * %NULL, characters will only be read if @is_selected returns %TRUE after being
 * passed the column and row, respectively.  A #BteCharAttributes structure
 * is added to @attributes for each byte added to the returned string detailing
 * the character's position, colors, and other characteristics.  The
 * entire scrollback buffer is scanned, so it is possible to read the entire
 * contents of the buffer using this function.
 *
 * This method is unaware of BiDi. The columns passed in @start_col and @end_row,
 * and returned in @attributes are logical columns.
 *
 * Returns: (transfer full): a newly allocated text string, or %NULL.
 */
char *
bte_terminal_get_text_range(BteTerminal *terminal,
			    long start_row,
                            long start_col,
			    long end_row,
                            long end_col,
			    BteSelectionFunc is_selected,
			    gpointer user_data,
			    GArray *attributes) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
        warn_if_callback(is_selected);
        auto text = IMPL(terminal)->get_text(start_row, start_col,
                                             end_row, end_col,
                                             false /* block */,
                                             true /* wrap */,
                                             attributes);
        if (text == nullptr)
                return nullptr;
        return (char*)g_string_free(text, FALSE);
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_reset:
 * @terminal: a #BteTerminal
 * @clear_tabstops: whether to reset tabstops
 * @clear_history: whether to empty the terminal's scrollback buffer
 *
 * Resets as much of the terminal's internal state as possible, discarding any
 * unprocessed input data, resetting character attributes, cursor state,
 * national character set state, status line, terminal modes (insert/delete),
 * selection state, and encoding.
 *
 */
void
bte_terminal_reset(BteTerminal *terminal,
                   gboolean clear_tabstops,
                   gboolean clear_history) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        IMPL(terminal)->reset(clear_tabstops, clear_history, true);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_size:
 * @terminal: a #BteTerminal
 * @columns: the desired number of columns
 * @rows: the desired number of rows
 *
 * Attempts to change the terminal's size in terms of rows and columns.  If
 * the attempt succeeds, the widget will resize itself to the proper size.
 */
void
bte_terminal_set_size(BteTerminal *terminal,
                      long columns,
                      long rows) noexcept
try
{
        g_return_if_fail(columns >= 1);
        g_return_if_fail(rows >= 1);

        IMPL(terminal)->set_size(columns, rows);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_text_blink_mode:
 * @terminal: a #BteTerminal
 *
 * Checks whether or not the terminal will allow blinking text.
 *
 * Returns: the blinking setting
 *
 * Since: 0.52
 */
BteTextBlinkMode
bte_terminal_get_text_blink_mode(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), BTE_TEXT_BLINK_ALWAYS);
        return WIDGET(terminal)->text_blink_mode();
}
catch (...)
{
        bte::log_exception();
        return BTE_TEXT_BLINK_ALWAYS;
}

/**
 * bte_terminal_set_text_blink_mode:
 * @terminal: a #BteTerminal
 * @text_blink_mode: the #BteTextBlinkMode to use
 *
 * Controls whether or not the terminal will allow blinking text.
 *
 * Since: 0.52
 */
void
bte_terminal_set_text_blink_mode(BteTerminal *terminal,
                                 BteTextBlinkMode text_blink_mode) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (WIDGET(terminal)->set_text_blink_mode(text_blink_mode))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_TEXT_BLINK_MODE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_allow_bold:
 * @terminal: a #BteTerminal
 *
 * Checks whether or not the terminal will attempt to draw bold text,
 * by using a bold font variant.
 *
 * Returns: %TRUE if bolding is enabled, %FALSE if not
 */
gboolean
bte_terminal_get_allow_bold(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_allow_bold;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_allow_bold:
 * @terminal: a #BteTerminal
 * @allow_bold: %TRUE if the terminal should attempt to draw bold text
 *
 * Controls whether or not the terminal will attempt to draw bold text,
 * by using a bold font variant.
 */
void
bte_terminal_set_allow_bold(BteTerminal *terminal,
                            gboolean allow_bold) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_allow_bold(allow_bold != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_ALLOW_BOLD]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_allow_hyperlink:
 * @terminal: a #BteTerminal
 *
 * Checks whether or not hyperlinks (OSC 8 escape sequence) are allowed.
 *
 * Returns: %TRUE if hyperlinks are enabled, %FALSE if not
 *
 * Since: 0.50
 */
gboolean
bte_terminal_get_allow_hyperlink(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), FALSE);
        return IMPL(terminal)->m_allow_hyperlink;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_allow_hyperlink:
 * @terminal: a #BteTerminal
 * @allow_hyperlink: %TRUE if the terminal should allow hyperlinks
 *
 * Controls whether or not hyperlinks (OSC 8 escape sequence) are allowed.
 *
 * Since: 0.50
 */
void
bte_terminal_set_allow_hyperlink(BteTerminal *terminal,
                                 gboolean allow_hyperlink) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_allow_hyperlink(allow_hyperlink != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_ALLOW_HYPERLINK]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_audible_bell:
 * @terminal: a #BteTerminal
 *
 * Checks whether or not the terminal will beep when the child outputs the
 * "bl" sequence.
 *
 * Returns: %TRUE if audible bell is enabled, %FALSE if not
 */
gboolean
bte_terminal_get_audible_bell(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_audible_bell;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_audible_bell:
 * @terminal: a #BteTerminal
 * @is_audible: %TRUE if the terminal should beep
 *
 * Controls whether or not the terminal will beep when the child outputs the
 * "bl" sequence.
 */
void
bte_terminal_set_audible_bell(BteTerminal *terminal,
                              gboolean is_audible) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_audible_bell(is_audible != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_AUDIBLE_BELL]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_backspace_binding:
 * @terminal: a #BteTerminal
 * @binding: a #BteEraseBinding for the backspace key
 *
 * Modifies the terminal's backspace key binding, which controls what
 * string or control sequence the terminal sends to its child when the user
 * presses the backspace key.
 */
void
bte_terminal_set_backspace_binding(BteTerminal *terminal,
                                   BteEraseBinding binding) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(binding >= BTE_ERASE_AUTO && binding <= BTE_ERASE_TTY);

        if (WIDGET(terminal)->set_backspace_binding(binding))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_BACKSPACE_BINDING]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_bold_is_bright:
 * @terminal: a #BteTerminal
 *
 * Checks whether the SGR 1 attribute also switches to the bright counterpart
 * of the first 8 palette colors, in addition to making them bold (legacy behavior)
 * or if SGR 1 only enables bold and leaves the color intact.
 *
 * Returns: %TRUE if bold also enables bright, %FALSE if not
 *
 * Since: 0.52
 */
gboolean
bte_terminal_get_bold_is_bright(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_bold_is_bright;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_bold_is_bright:
 * @terminal: a #BteTerminal
 * @bold_is_bright: %TRUE if bold should also enable bright
 *
 * Sets whether the SGR 1 attribute also switches to the bright counterpart
 * of the first 8 palette colors, in addition to making them bold (legacy behavior)
 * or if SGR 1 only enables bold and leaves the color intact.
 *
 * Since: 0.52
 */
void
bte_terminal_set_bold_is_bright(BteTerminal *terminal,
                                gboolean bold_is_bright) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_bold_is_bright(bold_is_bright != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_BOLD_IS_BRIGHT]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_char_height:
 * @terminal: a #BteTerminal
 *
 * Returns: the height of a character cell
 *
 * Note that this method should rather be called bte_terminal_get_cell_height,
 * because the return value takes cell-height-scale into account.
 */
glong
bte_terminal_get_char_height(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), -1);
	return IMPL(terminal)->get_cell_height();
}
catch (...)
{
        bte::log_exception();
        return -1;
}

/**
 * bte_terminal_get_char_width:
 * @terminal: a #BteTerminal
 *
 * Returns: the width of a character cell
 *
 * Note that this method should rather be called bte_terminal_get_cell_width,
 * because the return value takes cell-width-scale into account.
 */
glong
bte_terminal_get_char_width(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), -1);
	return IMPL(terminal)->get_cell_width();
}
catch (...)
{
        bte::log_exception();
        return -1;
}

/**
 * bte_terminal_get_cjk_ambiguous_width:
 * @terminal: a #BteTerminal
 *
 *  Returns whether ambiguous-width characters are narrow or wide.
 * (Note that when using a non-UTF-8 encoding set via bte_terminal_set_encoding(),
 * the width of ambiguous-width characters is fixed and determined by the encoding
 * itself.)
 *
 * Returns: 1 if ambiguous-width characters are narrow, or 2 if they are wide
 */
int
bte_terminal_get_cjk_ambiguous_width(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), 1);
        return IMPL(terminal)->m_utf8_ambiguous_width;
}
catch (...)
{
        bte::log_exception();
        return 1;
}

/**
 * bte_terminal_set_cjk_ambiguous_width:
 * @terminal: a #BteTerminal
 * @width: either 1 (narrow) or 2 (wide)
 *
 * This setting controls whether ambiguous-width characters are narrow or wide.
 * (Note that when using a non-UTF-8 encoding set via bte_terminal_set_encoding(),
 * the width of ambiguous-width characters is fixed and determined by the encoding
 * itself.)
 */
void
bte_terminal_set_cjk_ambiguous_width(BteTerminal *terminal, int width) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(width == 1 || width == 2);

        if (IMPL(terminal)->set_cjk_ambiguous_width(width))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_CJK_AMBIGUOUS_WIDTH]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_background:
 * @terminal: a #BteTerminal
 * @background: the new background color
 *
 * Sets the background color for text which does not have a specific background
 * color assigned.  Only has effect when no background image is set and when
 * the terminal is not transparent.
 */
void
bte_terminal_set_color_background(BteTerminal *terminal,
                                  const CdkRGBA *background) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(background != NULL);
        g_return_if_fail(valid_color(background));

        auto impl = IMPL(terminal);
        impl->set_color_background(bte::color::rgb(background));
        impl->set_background_alpha(background->alpha);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_bold:
 * @terminal: a #BteTerminal
 * @bold: (allow-none): the new bold color or %NULL
 *
 * Sets the color used to draw bold text in the default foreground color.
 * If @bold is %NULL then the default color is used.
 */
void
bte_terminal_set_color_bold(BteTerminal *terminal,
                            const CdkRGBA *bold) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(bold == nullptr || valid_color(bold));

        auto impl = IMPL(terminal);
        if (bold)
                impl->set_color_bold(bte::color::rgb(bold));
        else
                impl->reset_color_bold();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_cursor:
 * @terminal: a #BteTerminal
 * @cursor_background: (allow-none): the new color to use for the text cursor, or %NULL
 *
 * Sets the background color for text which is under the cursor.  If %NULL, text
 * under the cursor will be drawn with foreground and background colors
 * reversed.
 */
void
bte_terminal_set_color_cursor(BteTerminal *terminal,
                              const CdkRGBA *cursor_background) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(cursor_background == nullptr || valid_color(cursor_background));

        auto impl = IMPL(terminal);
        if (cursor_background)
                impl->set_color_cursor_background(bte::color::rgb(cursor_background));
        else
                impl->reset_color_cursor_background();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_cursor_foreground:
 * @terminal: a #BteTerminal
 * @cursor_foreground: (allow-none): the new color to use for the text cursor, or %NULL
 *
 * Sets the foreground color for text which is under the cursor.  If %NULL, text
 * under the cursor will be drawn with foreground and background colors
 * reversed.
 *
 * Since: 0.44
 */
void
bte_terminal_set_color_cursor_foreground(BteTerminal *terminal,
                                         const CdkRGBA *cursor_foreground) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(cursor_foreground == nullptr || valid_color(cursor_foreground));

        auto impl = IMPL(terminal);
        if (cursor_foreground)
                impl->set_color_cursor_foreground(bte::color::rgb(cursor_foreground));
        else
                impl->reset_color_cursor_foreground();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_foreground:
 * @terminal: a #BteTerminal
 * @foreground: the new foreground color
 *
 * Sets the foreground color used to draw normal text.
 */
void
bte_terminal_set_color_foreground(BteTerminal *terminal,
                                  const CdkRGBA *foreground) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(foreground != nullptr);
        g_return_if_fail(valid_color(foreground));

        IMPL(terminal)->set_color_foreground(bte::color::rgb(foreground));
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_highlight:
 * @terminal: a #BteTerminal
 * @highlight_background: (allow-none): the new color to use for highlighted text, or %NULL
 *
 * Sets the background color for text which is highlighted.  If %NULL,
 * it is unset.  If neither highlight background nor highlight foreground are set,
 * highlighted text (which is usually highlighted because it is selected) will
 * be drawn with foreground and background colors reversed.
 */
void
bte_terminal_set_color_highlight(BteTerminal *terminal,
                                 const CdkRGBA *highlight_background) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(highlight_background == nullptr || valid_color(highlight_background));

        auto impl = IMPL(terminal);
        if (highlight_background)
                impl->set_color_highlight_background(bte::color::rgb(highlight_background));
        else
                impl->reset_color_highlight_background();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_color_highlight_foreground:
 * @terminal: a #BteTerminal
 * @highlight_foreground: (allow-none): the new color to use for highlighted text, or %NULL
 *
 * Sets the foreground color for text which is highlighted.  If %NULL,
 * it is unset.  If neither highlight background nor highlight foreground are set,
 * highlighted text (which is usually highlighted because it is selected) will
 * be drawn with foreground and background colors reversed.
 */
void
bte_terminal_set_color_highlight_foreground(BteTerminal *terminal,
                                            const CdkRGBA *highlight_foreground) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(highlight_foreground == nullptr || valid_color(highlight_foreground));

        auto impl = IMPL(terminal);
        if (highlight_foreground)
                impl->set_color_highlight_foreground(bte::color::rgb(highlight_foreground));
        else
                impl->reset_color_highlight_foreground();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_colors:
 * @terminal: a #BteTerminal
 * @foreground: (allow-none): the new foreground color, or %NULL
 * @background: (allow-none): the new background color, or %NULL
 * @palette: (array length=palette_size zero-terminated=0) (element-type Cdk.RGBA) (allow-none): the color palette
 * @palette_size: the number of entries in @palette
 *
 * @palette specifies the new values for the 256 palette colors: 8 standard colors,
 * their 8 bright counterparts, 6x6x6 color cube, and 24 grayscale colors.
 * Omitted entries will default to a hardcoded value.
 *
 * @palette_size must be 0, 8, 16, 232 or 256.
 *
 * If @foreground is %NULL and @palette_size is greater than 0, the new foreground
 * color is taken from @palette[7].  If @background is %NULL and @palette_size is
 * greater than 0, the new background color is taken from @palette[0].
 */
void
bte_terminal_set_colors(BteTerminal *terminal,
                        const CdkRGBA *foreground,
                        const CdkRGBA *background,
                        const CdkRGBA *palette,
                        gsize palette_size) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
	g_return_if_fail((palette_size == 0) ||
			 (palette_size == 8) ||
			 (palette_size == 16) ||
			 (palette_size == 232) ||
			 (palette_size == 256));
        g_return_if_fail(foreground == nullptr || valid_color(foreground));
        g_return_if_fail(background == nullptr || valid_color(background));
        for (gsize i = 0; i < palette_size; ++i)
                g_return_if_fail(valid_color(&palette[i]));

        bte::color::rgb fg;
        if (foreground)
                fg = bte::color::rgb(foreground);
        bte::color::rgb bg;
        if (background)
                bg = bte::color::rgb(background);

        bte::color::rgb* pal = nullptr;
        if (palette_size) {
                pal = g_new0(bte::color::rgb, palette_size);
                for (gsize i = 0; i < palette_size; ++i)
                        pal[i] = bte::color::rgb(palette[i]);
        }

        auto impl = IMPL(terminal);
        impl->set_colors(foreground ? &fg : nullptr,
                         background ? &bg : nullptr,
                         pal, palette_size);
        impl->set_background_alpha(background ? background->alpha : 1.0);
        g_free(pal);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_default_colors:
 * @terminal: a #BteTerminal
 *
 * Reset the terminal palette to reasonable compiled-in default color.
 */
void
bte_terminal_set_default_colors(BteTerminal *terminal) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        IMPL(terminal)->set_colors_default();
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_column_count:
 * @terminal: a #BteTerminal
 *
 * Returns: the number of columns
 */
glong
bte_terminal_get_column_count(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), -1);
	return IMPL(terminal)->m_column_count;
}
catch (...)
{
        bte::log_exception();
        return -1;
}

/**
 * bte_terminal_get_current_directory_uri:
 * @terminal: a #BteTerminal
 *
 * Returns: (nullable) (transfer none): the URI of the current directory of the
 *   process running in the terminal, or %NULL
 */
const char *
bte_terminal_get_current_directory_uri(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
        auto impl = IMPL(terminal);
        return impl->m_current_directory_uri.size() ? impl->m_current_directory_uri.data() : nullptr;
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_get_current_file_uri:
 * @terminal: a #BteTerminal
 *
 * Returns: (nullable) (transfer none): the URI of the current file the
 *   process running in the terminal is operating on, or %NULL if
 *   not set
 */
const char *
bte_terminal_get_current_file_uri(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
        auto impl = IMPL(terminal);
        return impl->m_current_file_uri.size() ? impl->m_current_file_uri.data() : nullptr;
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_get_cursor_blink_mode:
 * @terminal: a #BteTerminal
 *
 * Returns the currently set cursor blink mode.
 *
 * Return value: cursor blink mode.
 */
BteCursorBlinkMode
bte_terminal_get_cursor_blink_mode(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), BTE_CURSOR_BLINK_SYSTEM);

        return WIDGET(terminal)->cursor_blink_mode();
}
catch (...)
{
        bte::log_exception();
        return BTE_CURSOR_BLINK_SYSTEM;
}

/**
 * bte_terminal_set_cursor_blink_mode:
 * @terminal: a #BteTerminal
 * @mode: the #BteCursorBlinkMode to use
 *
 * Sets whether or not the cursor will blink. Using %BTE_CURSOR_BLINK_SYSTEM
 * will use the #CtkSettings::ctk-cursor-blink setting.
 */
void
bte_terminal_set_cursor_blink_mode(BteTerminal *terminal,
                                   BteCursorBlinkMode mode) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(mode >= BTE_CURSOR_BLINK_SYSTEM && mode <= BTE_CURSOR_BLINK_OFF);

        if (WIDGET(terminal)->set_cursor_blink_mode(mode))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_CURSOR_BLINK_MODE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_cursor_shape:
 * @terminal: a #BteTerminal
 *
 * Returns the currently set cursor shape.
 *
 * Return value: cursor shape.
 */
BteCursorShape
bte_terminal_get_cursor_shape(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), BTE_CURSOR_SHAPE_BLOCK);

        return WIDGET(terminal)->cursor_shape();
}
catch (...)
{
        bte::log_exception();
        return BTE_CURSOR_SHAPE_BLOCK;
}

/**
 * bte_terminal_set_cursor_shape:
 * @terminal: a #BteTerminal
 * @shape: the #BteCursorShape to use
 *
 * Sets the shape of the cursor drawn.
 */
void
bte_terminal_set_cursor_shape(BteTerminal *terminal,
                              BteCursorShape shape) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(shape >= BTE_CURSOR_SHAPE_BLOCK && shape <= BTE_CURSOR_SHAPE_UNDERLINE);

        if (WIDGET(terminal)->set_cursor_shape(shape))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_CURSOR_SHAPE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_delete_binding:
 * @terminal: a #BteTerminal
 * @binding: a #BteEraseBinding for the delete key
 *
 * Modifies the terminal's delete key binding, which controls what
 * string or control sequence the terminal sends to its child when the user
 * presses the delete key.
 */
void
bte_terminal_set_delete_binding(BteTerminal *terminal,
                                BteEraseBinding binding) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(binding >= BTE_ERASE_AUTO && binding <= BTE_ERASE_TTY);

        if (WIDGET(terminal)->set_delete_binding(binding))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_DELETE_BINDING]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_enable_bidi:
 * @terminal: a #BteTerminal
 *
 * Checks whether the terminal performs bidirectional text rendering.
 *
 * Returns: %TRUE if BiDi is enabled, %FALSE if not
 *
 * Since: 0.58
 */
gboolean
bte_terminal_get_enable_bidi(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
        return IMPL(terminal)->m_enable_bidi;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_enable_bidi:
 * @terminal: a #BteTerminal
 * @enable_bidi: %TRUE to enable BiDi support
 *
 * Controls whether or not the terminal will perform bidirectional text rendering.
 *
 * Since: 0.58
 */
void
bte_terminal_set_enable_bidi(BteTerminal *terminal,
                             gboolean enable_bidi) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_enable_bidi(enable_bidi != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_ENABLE_BIDI]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_enable_shaping:
 * @terminal: a #BteTerminal
 *
 * Checks whether the terminal shapes Arabic text.
 *
 * Returns: %TRUE if Arabic shaping is enabled, %FALSE if not
 *
 * Since: 0.58
 */
gboolean
bte_terminal_get_enable_shaping(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
        return IMPL(terminal)->m_enable_shaping;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_enable_shaping:
 * @terminal: a #BteTerminal
 * @enable_shaping: %TRUE to enable Arabic shaping
 *
 * Controls whether or not the terminal will shape Arabic text.
 *
 * Since: 0.58
 */
void
bte_terminal_set_enable_shaping(BteTerminal *terminal,
                                gboolean enable_shaping) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_enable_shaping(enable_shaping != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_ENABLE_SHAPING]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_encoding:
 * @terminal: a #BteTerminal
 *
 * Determines the name of the encoding in which the terminal expects data to be
 * encoded, or %NULL if UTF-8 is in use.
 *
 * Returns: (nullable) (transfer none): the current encoding for the terminal
 *
 * Deprecated: 0.54: Support for non-UTF-8 is deprecated.
 */
const char *
bte_terminal_get_encoding(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);
	return WIDGET(terminal)->encoding();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_set_encoding:
 * @terminal: a #BteTerminal
 * @codeset: (allow-none): target charset, or %NULL to use UTF-8
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Changes the encoding the terminal will expect data from the child to
 * be encoded with.  For certain terminal types, applications executing in the
 * terminal can change the encoding. If @codeset is %NULL, it uses "UTF-8".
 *
 * Note: Support for non-UTF-8 is deprecated and may get removed altogether.
 * Instead of this function, you should use a wrapper like luit(1) when
 * spawning the child process.
 *
 * Returns: %TRUE if the encoding could be changed to the specified one,
 *  or %FALSE with @error set to %G_CONVERT_ERROR_NO_CONVERSION.
 *
 * Deprecated: 0.54: Support for non-UTF-8 is deprecated.
 */
gboolean
bte_terminal_set_encoding(BteTerminal *terminal,
                          const char *codeset,
                          GError **error) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), FALSE);
        g_return_val_if_fail(error == NULL || *error == NULL, FALSE);

        auto const freezer = bte::glib::FreezeObjectNotify{terminal};

        auto const rv = IMPL(terminal)->set_encoding(codeset, error);
        if (rv) {
                g_signal_emit(freezer.get(), signals[SIGNAL_ENCODING_CHANGED], 0);
                g_object_notify_by_pspec(freezer.get(), pspecs[PROP_ENCODING]);
        }

        return rv;
}
catch (...)
{
        return bte::glib::set_error_from_exception(error);
}

/**
 * bte_terminal_get_font:
 * @terminal: a #BteTerminal
 *
 * Queries the terminal for information about the fonts which will be
 * used to draw text in the terminal.  The actual font takes the font scale
 * into account, this is not reflected in the return value, the unscaled
 * font is returned.
 *
 * Returns: (transfer none): a #PangoFontDescription describing the font the
 * terminal uses to render text at the default font scale of 1.0.
 */
const PangoFontDescription *
bte_terminal_get_font(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), nullptr);

        return IMPL(terminal)->unscaled_font_description();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_set_font:
 * @terminal: a #BteTerminal
 * @font_desc: (allow-none): a #PangoFontDescription for the desired font, or %NULL
 *
 * Sets the font used for rendering all text displayed by the terminal,
 * overriding any fonts set using ctk_widget_modify_font().  The terminal
 * will immediately attempt to load the desired font, retrieve its
 * metrics, and attempt to resize itself to keep the same number of rows
 * and columns.  The font scale is applied to the specified font.
 */
void
bte_terminal_set_font(BteTerminal *terminal,
                      const PangoFontDescription* font_desc) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_font_desc(font_desc))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_FONT_DESC]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_font_scale:
 * @terminal: a #BteTerminal
 *
 * Returns: the terminal's font scale
 */
gdouble
bte_terminal_get_font_scale(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), 1.);

        return IMPL(terminal)->m_font_scale;
}
catch (...)
{
        bte::log_exception();
        return 1.;
}

/**
 * bte_terminal_set_font_scale:
 * @terminal: a #BteTerminal
 * @scale: the font scale
 *
 * Sets the terminal's font scale to @scale.
 */
void
bte_terminal_set_font_scale(BteTerminal *terminal,
                            double scale) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        scale = CLAMP(scale, BTE_FONT_SCALE_MIN, BTE_FONT_SCALE_MAX);
        if (IMPL(terminal)->set_font_scale(scale))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_FONT_SCALE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_cell_height_scale:
 * @terminal: a #BteTerminal
 *
 * Returns: the terminal's cell height scale
 *
 * Since: 0.52
 */
double
bte_terminal_get_cell_height_scale(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), 1.);

        return IMPL(terminal)->m_cell_height_scale;
}
catch (...)
{
        bte::log_exception();
        return 1.;
}

/**
 * bte_terminal_set_cell_height_scale:
 * @terminal: a #BteTerminal
 * @scale: the cell height scale
 *
 * Sets the terminal's cell height scale to @scale.
 *
 * This can be used to increase the line spacing. (The font's height is not affected.)
 * Valid values go from 1.0 (default) to 2.0 ("double spacing").
 *
 * Since: 0.52
 */
void
bte_terminal_set_cell_height_scale(BteTerminal *terminal,
                                   double scale) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        scale = CLAMP(scale, BTE_CELL_SCALE_MIN, BTE_CELL_SCALE_MAX);
        if (IMPL(terminal)->set_cell_height_scale(scale))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_CELL_HEIGHT_SCALE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_cell_width_scale:
 * @terminal: a #BteTerminal
 *
 * Returns: the terminal's cell width scale
 *
 * Since: 0.52
 */
double
bte_terminal_get_cell_width_scale(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), 1.);

        return IMPL(terminal)->m_cell_width_scale;
}
catch (...)
{
        bte::log_exception();
        return 1.;
}

/**
 * bte_terminal_set_cell_width_scale:
 * @terminal: a #BteTerminal
 * @scale: the cell width scale
 *
 * Sets the terminal's cell width scale to @scale.
 *
 * This can be used to increase the letter spacing. (The font's width is not affected.)
 * Valid values go from 1.0 (default) to 2.0.
 *
 * Since: 0.52
 */
void
bte_terminal_set_cell_width_scale(BteTerminal *terminal,
                                  double scale) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        scale = CLAMP(scale, BTE_CELL_SCALE_MIN, BTE_CELL_SCALE_MAX);
        if (IMPL(terminal)->set_cell_width_scale(scale))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_CELL_WIDTH_SCALE]);
}
catch (...)
{
        bte::log_exception();
}

/* Just some arbitrary minimum values */
#define MIN_COLUMNS (16)
#define MIN_ROWS    (2)

/**
 * bte_terminal_get_geometry_hints:
 * @terminal: a #BteTerminal
 * @hints: (out caller-allocates): a #CdkGeometry to fill in
 * @min_rows: the minimum number of rows to request
 * @min_columns: the minimum number of columns to request
 *
 * Fills in some @hints from @terminal's geometry. The hints
 * filled are those covered by the %CDK_HINT_RESIZE_INC,
 * %CDK_HINT_MIN_SIZE and %CDK_HINT_BASE_SIZE flags.
 *
 * See ctk_window_set_geometry_hints() for more information.
 *
 * @terminal must be realized (see ctk_widget_get_realized()).
 *
 * Deprecated: 0.52
 */
void
bte_terminal_get_geometry_hints(BteTerminal *terminal,
                                CdkGeometry *hints,
                                int min_rows,
                                int min_columns) noexcept
{
        CtkWidget *widget;
        CtkBorder padding;

        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(hints != NULL);
        widget = &terminal->widget;
        g_return_if_fail(ctk_widget_get_realized(widget));

        auto impl = IMPL(terminal);

        auto context = ctk_widget_get_style_context(widget);
        ctk_style_context_get_padding(context, ctk_style_context_get_state(context),
                                      &padding);

        hints->base_width  = padding.left + padding.right;
        hints->base_height = padding.top  + padding.bottom;
        hints->width_inc   = impl->m_cell_width;
        hints->height_inc  = impl->m_cell_height;
        hints->min_width   = hints->base_width  + hints->width_inc  * min_columns;
        hints->min_height  = hints->base_height + hints->height_inc * min_rows;

	_bte_debug_print(BTE_DEBUG_WIDGET_SIZE,
                         "[Terminal %p] Geometry cell       width %ld height %ld\n"
                         "                       base       width %d height %d\n"
                         "                       increments width %d height %d\n"
                         "                       minimum    width %d height %d\n",
                         terminal,
                         impl->m_cell_width, impl->m_cell_height,
                         hints->base_width, hints->base_height,
                         hints->width_inc, hints->height_inc,
                         hints->min_width, hints->min_height);
}

/**
 * bte_terminal_set_geometry_hints_for_window:
 * @terminal: a #BteTerminal
 * @window: a #CtkWindow
 *
 * Sets @terminal as @window's geometry widget. See
 * ctk_window_set_geometry_hints() for more information.
 *
 * @terminal must be realized (see ctk_widget_get_realized()).
 *
 * Deprecated: 0.52
 */
void
bte_terminal_set_geometry_hints_for_window(BteTerminal *terminal,
                                           CtkWindow *window) noexcept
{
        CdkGeometry hints;

        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(ctk_widget_get_realized(&terminal->widget));

        bte_terminal_get_geometry_hints(terminal, &hints, MIN_ROWS, MIN_COLUMNS);
        ctk_window_set_geometry_hints(window,
                                      NULL,
                                      &hints,
                                      (CdkWindowHints)(CDK_HINT_RESIZE_INC |
                                                       CDK_HINT_MIN_SIZE |
                                                       CDK_HINT_BASE_SIZE));
}

/**
 * bte_terminal_get_has_selection:
 * @terminal: a #BteTerminal
 *
 * Checks if the terminal currently contains selected text.  Note that this
 * is different from determining if the terminal is the owner of any
 * #CtkClipboard items.
 *
 * Returns: %TRUE if part of the text in the terminal is selected.
 */
gboolean
bte_terminal_get_has_selection(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
        return !IMPL(terminal)->m_selection_resolved.empty();
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_get_icon_title:
 * @terminal: a #BteTerminal
 *
 * Returns: (transfer none): the icon title
 */
const char *
bte_terminal_get_icon_title(BteTerminal *terminal) noexcept
{
	return IMPL(terminal)->m_icon_title.data();
}

/**
 * bte_terminal_get_input_enabled:
 * @terminal: a #BteTerminal
 *
 * Returns whether the terminal allow user input.
 */
gboolean
bte_terminal_get_input_enabled (BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);

        return IMPL(terminal)->m_input_enabled;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_input_enabled:
 * @terminal: a #BteTerminal
 * @enabled: whether to enable user input
 *
 * Enables or disables user input. When user input is disabled,
 * the terminal's child will not receive any key press, or mouse button
 * press or motion events sent to it.
 */
void
bte_terminal_set_input_enabled (BteTerminal *terminal,
                                gboolean enabled) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_input_enabled(enabled != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_INPUT_ENABLED]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_mouse_autohide:
 * @terminal: a #BteTerminal
 *
 * Determines the value of the terminal's mouse autohide setting.  When
 * autohiding is enabled, the mouse cursor will be hidden when the user presses
 * a key and shown when the user moves the mouse.  This setting can be changed
 * using bte_terminal_set_mouse_autohide().
 *
 * Returns: %TRUE if autohiding is enabled, %FALSE if not
 */
gboolean
bte_terminal_get_mouse_autohide(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_mouse_autohide;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_mouse_autohide:
 * @terminal: a #BteTerminal
 * @setting: whether the mouse pointer should autohide
 *
 * Changes the value of the terminal's mouse autohide setting.  When autohiding
 * is enabled, the mouse cursor will be hidden when the user presses a key and
 * shown when the user moves the mouse.  This setting can be read using
 * bte_terminal_get_mouse_autohide().
 */
void
bte_terminal_set_mouse_autohide(BteTerminal *terminal,
                                gboolean setting) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_mouse_autohide(setting != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_MOUSE_POINTER_AUTOHIDE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_set_pty:
 * @terminal: a #BteTerminal
 * @pty: (allow-none): a #BtePty, or %NULL
 *
 * Sets @pty as the PTY to use in @terminal.
 * Use %NULL to unset the PTY.
 */
void
bte_terminal_set_pty(BteTerminal *terminal,
                     BtePty *pty) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(pty == NULL || BTE_IS_PTY(pty));

        auto const freezer = bte::glib::FreezeObjectNotify{terminal};

        if (WIDGET(terminal)->set_pty(pty))
                g_object_notify_by_pspec(freezer.get(), pspecs[PROP_PTY]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_pty:
 * @terminal: a #BteTerminal
 *
 * Returns the #BtePty of @terminal.
 *
 * Returns: (transfer none): a #BtePty, or %NULL
 */
BtePty *
bte_terminal_get_pty(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail (BTE_IS_TERMINAL (terminal), nullptr);
        return WIDGET(terminal)->pty();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_get_rewrap_on_resize:
 * @terminal: a #BteTerminal
 *
 * Checks whether or not the terminal will rewrap its contents upon resize.
 *
 * Returns: %TRUE if rewrapping is enabled, %FALSE if not
 *
 * Deprecated: 0.58
 */
gboolean
bte_terminal_get_rewrap_on_resize(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
	return IMPL(terminal)->m_rewrap_on_resize;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_rewrap_on_resize:
 * @terminal: a #BteTerminal
 * @rewrap: %TRUE if the terminal should rewrap on resize
 *
 * Controls whether or not the terminal will rewrap its contents, including
 * the scrollback history, whenever the terminal's width changes.
 *
 * Deprecated: 0.58
 */
void
bte_terminal_set_rewrap_on_resize(BteTerminal *terminal,
                                  gboolean rewrap) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_rewrap_on_resize(rewrap != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_REWRAP_ON_RESIZE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_row_count:
 * @terminal: a #BteTerminal
 *
 *
 * Returns: the number of rows
 */
glong
bte_terminal_get_row_count(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), -1);
	return IMPL(terminal)->m_row_count;
}
catch (...)
{
        bte::log_exception();
        return -1;
}

/**
 * bte_terminal_set_scrollback_lines:
 * @terminal: a #BteTerminal
 * @lines: the length of the history buffer
 *
 * Sets the length of the scrollback buffer used by the terminal.  The size of
 * the scrollback buffer will be set to the larger of this value and the number
 * of visible rows the widget can display, so 0 can safely be used to disable
 * scrollback.
 *
 * A negative value means "infinite scrollback".
 *
 * Note that this setting only affects the normal screen buffer.
 * No scrollback is allowed on the alternate screen buffer.
 */
void
bte_terminal_set_scrollback_lines(BteTerminal *terminal,
                                  glong lines) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(lines >= -1);

        auto const freezer = bte::glib::FreezeObjectNotify{terminal};

        if (IMPL(terminal)->set_scrollback_lines(lines))
                g_object_notify_by_pspec(freezer.get(), pspecs[PROP_SCROLLBACK_LINES]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_scrollback_lines:
 * @terminal: a #BteTerminal
 *
 * Returns: length of the scrollback buffer used by the terminal.
 * A negative value means "infinite scrollback".
 *
 * Since: 0.52
 */
glong
bte_terminal_get_scrollback_lines(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), 0);
        return IMPL(terminal)->m_scrollback_lines;
}
catch (...)
{
        bte::log_exception();
        return 0;
}

/**
 * bte_terminal_set_scroll_on_keystroke:
 * @terminal: a #BteTerminal
 * @scroll: whether the terminal should scroll on keystrokes
 *
 * Controls whether or not the terminal will forcibly scroll to the bottom of
 * the viewable history when the user presses a key.  Modifier keys do not
 * trigger this behavior.
 */
void
bte_terminal_set_scroll_on_keystroke(BteTerminal *terminal,
                                     gboolean scroll) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_scroll_on_keystroke(scroll != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_SCROLL_ON_KEYSTROKE]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_scroll_on_keystroke:
 * @terminal: a #BteTerminal
 *
 * Returns: whether or not the terminal will forcibly scroll to the bottom of
 * the viewable history when the user presses a key.  Modifier keys do not
 * trigger this behavior.
 *
 * Since: 0.52
 */
gboolean
bte_terminal_get_scroll_on_keystroke(BteTerminal *terminal) noexcept
try
{
    g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
    return IMPL(terminal)->m_scroll_on_keystroke;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_set_scroll_on_output:
 * @terminal: a #BteTerminal
 * @scroll: whether the terminal should scroll on output
 *
 * Controls whether or not the terminal will forcibly scroll to the bottom of
 * the viewable history when the new data is received from the child.
 */
void
bte_terminal_set_scroll_on_output(BteTerminal *terminal,
                                  gboolean scroll) noexcept
try
{
	g_return_if_fail(BTE_IS_TERMINAL(terminal));

        if (IMPL(terminal)->set_scroll_on_output(scroll != FALSE))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_SCROLL_ON_OUTPUT]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_scroll_on_output:
 * @terminal: a #BteTerminal
 *
 * Returns: whether or not the terminal will forcibly scroll to the bottom of
 * the viewable history when the new data is received from the child.
 *
 * Since: 0.52
 */
gboolean
bte_terminal_get_scroll_on_output(BteTerminal *terminal) noexcept
try
{
    g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
    return IMPL(terminal)->m_scroll_on_output;
}
catch (...)
{
        bte::log_exception();
        return false;
}

/**
 * bte_terminal_get_window_title:
 * @terminal: a #BteTerminal
 *
 * Returns: (nullable) (transfer none): the window title, or %NULL
 */
const char *
bte_terminal_get_window_title(BteTerminal *terminal) noexcept
try
{
	g_return_val_if_fail(BTE_IS_TERMINAL(terminal), nullptr);
	return IMPL(terminal)->m_window_title.data();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_get_word_char_exceptions:
 * @terminal: a #BteTerminal
 *
 * Returns the set of characters which will be considered parts of a word
 * when doing word-wise selection, in addition to the default which only
 * considers alphanumeric characters part of a word.
 *
 * If %NULL, a built-in set is used.
 *
 * Returns: (nullable) (transfer none): a string, or %NULL
 *
 * Since: 0.40
 */
const char *
bte_terminal_get_word_char_exceptions(BteTerminal *terminal) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), NULL);

        return WIDGET(terminal)->word_char_exceptions();
}
catch (...)
{
        bte::log_exception();
        return nullptr;
}

/**
 * bte_terminal_set_word_char_exceptions:
 * @terminal: a #BteTerminal
 * @exceptions: a string of ASCII punctuation characters, or %NULL
 *
 * With this function you can provide a set of characters which will
 * be considered parts of a word when doing word-wise selection, in
 * addition to the default which only considers alphanumeric characters
 * part of a word.
 *
 * The characters in @exceptions must be non-alphanumeric, each character
 * must occur only once, and if @exceptions contains the character
 * U+002D HYPHEN-MINUS, it must be at the start of the string.
 *
 * Use %NULL to reset the set of exception characters to the default.
 *
 * Since: 0.40
 */
void
bte_terminal_set_word_char_exceptions(BteTerminal *terminal,
                                      const char *exceptions) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        auto stropt = exceptions ? std::make_optional<std::string_view>(exceptions) : std::nullopt;
        if (WIDGET(terminal)->set_word_char_exceptions(stropt))
                g_object_notify_by_pspec(G_OBJECT(terminal), pspecs[PROP_WORD_CHAR_EXCEPTIONS]);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_write_contents_sync:
 * @terminal: a #BteTerminal
 * @stream: a #GOutputStream to write to
 * @flags: a set of #BteWriteFlags
 * @cancellable: (allow-none): a #GCancellable object, or %NULL
 * @error: (allow-none): a #GError location to store the error occuring, or %NULL
 *
 * Write contents of the current contents of @terminal (including any
 * scrollback history) to @stream according to @flags.
 *
 * If @cancellable is not %NULL, then the operation can be cancelled by triggering
 * the cancellable object from another thread. If the operation was cancelled,
 * the error %G_IO_ERROR_CANCELLED will be returned in @error.
 *
 * This is a synchronous operation and will make the widget (and input
 * processing) during the write operation, which may take a long time
 * depending on scrollback history and @stream availability for writing.
 *
 * Returns: %TRUE on success, %FALSE if there was an error
 */
gboolean
bte_terminal_write_contents_sync (BteTerminal *terminal,
                                  GOutputStream *stream,
                                  BteWriteFlags flags,
                                  GCancellable *cancellable,
                                  GError **error) noexcept
try
{
        g_return_val_if_fail(BTE_IS_TERMINAL(terminal), false);
        g_return_val_if_fail(G_IS_OUTPUT_STREAM(stream), false);

        return IMPL(terminal)->write_contents_sync(stream, flags, cancellable, error);
}
catch (...)
{
        return bte::glib::set_error_from_exception(error);
}

/**
 * bte_terminal_set_clear_background:
 * @terminal: a #BteTerminal
 * @setting:
 *
 * Sets whether to paint the background with the background colour.
 * The default is %TRUE.
 *
 * This function is rarely useful. One use for it is to add a background
 * image to the terminal.
 *
 * Since: 0.52
 */
void
bte_terminal_set_clear_background(BteTerminal* terminal,
                                  gboolean setting) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));

        IMPL(terminal)->set_clear_background(setting != FALSE);
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_color_background_for_draw:
 * @terminal: a #BteTerminal
 * @color: (out): a location to store a #GdbRGBA color
 *
 * Returns the background colour, as used by @terminal when
 * drawing the background, which may be different from
 * the color set by bte_terminal_set_color_background().
 *
 * Note: you must only call this function while handling the
 * CtkWidget::draw signal.
 *
 * This function is rarely useful. One use for it is if you disable
 * drawing the background (see bte_terminal_set_clear_background())
 * and then need to draw the background yourself.
 *
 * Since: 0.54
 */
void
bte_terminal_get_color_background_for_draw(BteTerminal* terminal,
                                           CdkRGBA* color) noexcept
try
{
        g_return_if_fail(BTE_IS_TERMINAL(terminal));
        g_return_if_fail(color != nullptr);

        auto impl = IMPL(terminal);
        auto const c = impl->get_color(BTE_DEFAULT_BG);
        color->red = c->red / 65535.;
        color->green = c->green / 65535.;
        color->blue = c->blue / 65535.;
        color->alpha = impl->m_background_alpha;
}
catch (...)
{
        bte::log_exception();
        *color = {0., 0., 0., 1.};
}

/**
 * bte_terminal_set_enable_sixel:
 * @terminal: a #BteTerminal
 * @enabled: whether to enable SIXEL images
 *
 * This function does nothing.
 *
 * Since: 0.62
 */
void
bte_terminal_set_enable_sixel(BteTerminal *terminal,
                              gboolean enabled) noexcept
try
{
}
catch (...)
{
        bte::log_exception();
}

/**
 * bte_terminal_get_enable_sixel:
 * @terminal: a #BteTerminal
 *
 * Returns: %FALSE
 *
 * Since: 0.62
 */
gboolean
bte_terminal_get_enable_sixel(BteTerminal *terminal) noexcept
try
{
        return false;
}
catch (...)
{
        bte::log_exception();
        return false;
}

namespace bte {

using namespace std::literals;

static void
exception_append_to_string(std::exception const& e,
                           std::string& what,
                           int level = 0)
{
        if (level > 0)
                what += ": "sv;
        what += e.what();

        try {
                std::rethrow_if_nested(e);
        } catch (std::exception const& en) {
                exception_append_to_string(en, what, level + 1);
        } catch (...) {
                what += ": Unknown nested exception"sv;
        }
}

#ifdef BTE_DEBUG
void log_exception(char const* func,
                   char const* filename,
                   int const line) noexcept
try
{
        auto what = std::string{};

        try {
                throw; // rethrow current exception
        } catch (std::bad_alloc const& e) {
                g_error("Allocation failure: %s\n", e.what());
        } catch (std::exception const& e) {
                exception_append_to_string(e, what);
        } catch (...) {
                what = "Unknown exception"sv;
        }

        _bte_debug_print(BTE_DEBUG_EXCEPTIONS,
                         "Caught exception in %s [%s:%d]: %s\n",
                         func, filename, line, what.c_str());
}
catch (...)
{
        _bte_debug_print(BTE_DEBUG_EXCEPTIONS,
                         "Caught exception while logging an exception in %s [%s:%d]\n",
                         func, filename, line);
}
#endif /* BTE_DEBUG */

namespace glib {

bool set_error_from_exception(GError** error
#ifdef BTE_DEBUG
                              , char const* func
                              , char const* filename
                              , int const line
#endif
                              ) noexcept
try
{
        auto what = std::string{};

        try {
                throw; // rethrow current exception
        } catch (std::bad_alloc const& e) {
                g_error("Allocation failure: %s\n", e.what());
        } catch (std::exception const& e) {
                exception_append_to_string(e, what);
        } catch (...) {
                what = "Unknown exception"sv;
        }

#ifdef BTE_DEBUG
        auto msg = bte::glib::take_string(g_strdup_printf("Caught exception in %s [%s:%d]: %s",
                                                          func, filename, line,
                                                          what.c_str()));
#else
        auto msg = bte::glib::take_string(g_strdup_printf("Caught exception: %s",
                                                          what.c_str()));
#endif
        auto msg_str = bte::glib::take_string(g_utf8_make_valid(msg.get(), -1));
        g_set_error_literal(error,
                            G_IO_ERROR,
                            G_IO_ERROR_FAILED,
                            msg_str.get());
        _bte_debug_print(BTE_DEBUG_EXCEPTIONS, "%s", msg_str.get());

        return false;
}
catch (...)
{
        bte::log_exception();
#ifdef BTE_DEBUG
        g_set_error(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                    "Caught exception while logging an exception in %s [%s:%d]\n",
                    func, filename, line);
#else
        g_set_error_literal(error, G_IO_ERROR, G_IO_ERROR_FAILED,
                            "Caught exception while logging an exception");
#endif
        return false;
}

} // namespace glib
} // namespace bte
