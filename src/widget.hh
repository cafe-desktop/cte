/*
 * Copyright © 2018 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "bteterminal.h"
#include "btepty.h"

#include "bteinternal.hh"

#include "fwd.hh"
#include "refptr.hh"

namespace bte {

namespace terminal {

class Terminal;

} // namespace terminal

namespace platform {

class Widget {
public:
        friend class bte::terminal::Terminal;

        Widget(BteTerminal* t);
        ~Widget() noexcept;

        Widget(Widget const&) = delete;
        Widget(Widget&&) = delete;
        Widget& operator= (Widget const&) = delete;
        Widget& operator= (Widget&&) = delete;

        GObject* object() const noexcept { return reinterpret_cast<GObject*>(m_widget); }
        CtkWidget* ctk() const noexcept { return m_widget; }
        BteTerminal* bte() const noexcept { return reinterpret_cast<BteTerminal*>(m_widget); }

        inline constexpr bte::terminal::Terminal* terminal() const noexcept { return m_terminal; }

        void constructed() noexcept;
        void dispose() noexcept;
        void realize() noexcept;
        void unrealize() noexcept;
        void map() noexcept;
        void unmap() noexcept;
        void style_updated() noexcept;
        void draw(cairo_t *cr) noexcept { m_terminal->widget_draw(cr); }
        void get_preferred_width(int *minimum_width,
                                 int *natural_width) const noexcept { m_terminal->widget_get_preferred_width(minimum_width, natural_width); }
        void get_preferred_height(int *minimum_height,
                                  int *natural_height) const noexcept { m_terminal->widget_get_preferred_height(minimum_height, natural_height); }
        void size_allocate(CtkAllocation *allocation) noexcept;

        void focus_in(CdkEventFocus *event) noexcept { m_terminal->widget_focus_in(); }
        void focus_out(CdkEventFocus *event) noexcept { m_terminal->widget_focus_out(); }
        bool key_press(CdkEventKey *event) noexcept { return m_terminal->widget_key_press(key_event_from_cdk(event)); }
        bool key_release(CdkEventKey *event) noexcept { return m_terminal->widget_key_release(key_event_from_cdk(event)); }
        bool button_press(CdkEventButton *event) noexcept { return m_terminal->widget_mouse_press(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }
        bool button_release(CdkEventButton *event) noexcept { return m_terminal->widget_mouse_release(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }
        void enter(CdkEventCrossing *event) noexcept { m_terminal->widget_mouse_enter(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }
        void leave(CdkEventCrossing *event) noexcept { m_terminal->widget_mouse_leave(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }
        bool scroll(CdkEventScroll *event) noexcept { return m_terminal->widget_mouse_scroll(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }
        bool motion_notify(CdkEventMotion *event) noexcept { return m_terminal->widget_mouse_motion(*mouse_event_from_cdk(reinterpret_cast<CdkEvent*>(event))); }

        void grab_focus() noexcept { ctk_widget_grab_focus(ctk()); }

        bool primary_paste_enabled() const noexcept;
        void paste(CdkAtom board) noexcept { m_terminal->widget_paste(board); }
        void copy(BteSelection sel,
                  BteFormat format) noexcept { m_terminal->widget_copy(sel, format); }
        void paste_received(char const* text) noexcept { m_terminal->widget_paste_received(text); }
        void clipboard_cleared(CtkClipboard *clipboard) noexcept { m_terminal->widget_clipboard_cleared(clipboard); }
        void clipboard_requested(CtkClipboard *target_clipboard,
                                 CtkSelectionData *data,
                                 guint info) noexcept { m_terminal->widget_clipboard_requested(target_clipboard, data, info); }

        void screen_changed (CdkScreen *previous_screen) noexcept;
        void settings_changed() noexcept;

        void beep() noexcept;

        void set_hadjustment(bte::glib::RefPtr<CtkAdjustment>&& adjustment) noexcept { m_hadjustment = std::move(adjustment); }
        void set_vadjustment(bte::glib::RefPtr<CtkAdjustment>&& adjustment) { terminal()->widget_set_vadjustment(std::move(adjustment)); }
        auto hadjustment() noexcept { return m_hadjustment.get(); }
        auto vadjustment() noexcept { return terminal()->vadjustment(); }
        void set_hscroll_policy(CtkScrollablePolicy policy) noexcept { m_hscroll_policy = policy; }
        void set_vscroll_policy(CtkScrollablePolicy policy) noexcept { m_vscroll_policy = policy; }
        auto hscroll_policy() const noexcept { return m_hscroll_policy; }
        auto vscroll_policy() const noexcept { return m_vscroll_policy; }
        auto padding() const noexcept { return terminal()->padding(); }

        bool set_cursor_blink_mode(BteCursorBlinkMode mode) { return terminal()->set_cursor_blink_mode(bte::terminal::Terminal::CursorBlinkMode(mode)); }
        auto cursor_blink_mode() const noexcept { return BteCursorBlinkMode(terminal()->cursor_blink_mode()); }

        bool set_cursor_shape(BteCursorShape shape) { return terminal()->set_cursor_shape(bte::terminal::Terminal::CursorShape(shape)); }
        auto cursor_shape() const noexcept { return BteCursorShape(terminal()->cursor_shape()); }

        bool set_backspace_binding(BteEraseBinding mode) { return terminal()->set_backspace_binding(bte::terminal::Terminal::EraseMode(mode)); }
        auto backspace_binding() const noexcept { return BteEraseBinding(terminal()->backspace_binding()); }

        bool set_delete_binding(BteEraseBinding mode) { return terminal()->set_delete_binding(bte::terminal::Terminal::EraseMode(mode)); }
        auto delete_binding() const noexcept { return BteEraseBinding(terminal()->delete_binding()); }

        bool set_text_blink_mode(BteTextBlinkMode mode) { return terminal()->set_text_blink_mode(bte::terminal::Terminal::TextBlinkMode(mode)); }
        auto text_blink_mode() const noexcept { return BteTextBlinkMode(terminal()->text_blink_mode()); }

        bool set_word_char_exceptions(std::optional<std::string_view> stropt);
        auto word_char_exceptions() const noexcept { return m_word_char_exceptions ? m_word_char_exceptions.value().c_str() : nullptr; }

        char const* encoding() const noexcept { return m_terminal->encoding(); }

        void emit_child_exited(int status) noexcept;
        void emit_eof() noexcept;

        bool set_pty(BtePty* pty) noexcept;
        inline auto pty() const noexcept { return m_pty.get(); }

        void feed(std::string_view const& str) { terminal()->feed(str); }
        void feed_child(std::string_view const& str) { terminal()->feed_child(str); }
        void feed_child_binary(std::string_view const& str) { terminal()->feed_child_binary(str); }

        char *regex_match_check(bte::grid::column_t column,
                                bte::grid::row_t row,
                                int* tag)
        {
                return terminal()->regex_match_check(column, row, tag);
        }

        char* regex_match_check(CdkEvent* event,
                                int* tag)
        {
                if (auto mouse_event = mouse_event_from_cdk(event))
                        return terminal()->regex_match_check(*mouse_event, tag);
                else
                        return nullptr;
        }

        bool regex_match_check_extra(CdkEvent* event,
                                     bte::base::Regex const** regexes,
                                     size_t n_regexes,
                                     uint32_t match_flags,
                                     char** matches)
        {
                if (auto mouse_event = mouse_event_from_cdk(event))
                        return terminal()->regex_match_check_extra(*mouse_event, regexes, n_regexes, match_flags, matches);
                else
                        return false;
        }

        char* hyperlink_check(CdkEvent* event)
        {
                if (auto mouse_event = mouse_event_from_cdk(event))
                        return terminal()->hyperlink_check(*mouse_event);
                else
                        return nullptr;
        }

        bool should_emit_signal(int id) noexcept;

protected:

        enum class CursorType {
                eDefault,
                eInvisible,
                eMousing,
                eHyperlink
        };

        CdkWindow* event_window() const noexcept { return m_event_window; }

        bool realized() const noexcept
        {
                return ctk_widget_get_realized(m_widget);
        }

        bte::glib::RefPtr<CdkCursor> create_cursor(CdkCursorType cursor_type) const noexcept;

        void set_cursor(CursorType type) noexcept;
        void set_cursor(CdkCursor* cursor) noexcept;
        void set_cursor(Cursor const& cursor) noexcept;

        bool im_filter_keypress(bte::terminal::KeyEvent const& event) noexcept;

        void im_focus_in() noexcept;
        void im_focus_out() noexcept;

        void im_reset() noexcept
        {
                if (m_im_context)
                        ctk_im_context_reset(m_im_context.get());
        }

        void im_set_cursor_location(cairo_rectangle_int_t const* rect) noexcept;

        void unset_pty() noexcept;

        unsigned key_event_translate_ctrlkey(bte::terminal::KeyEvent const& event) const noexcept;

public: // FIXMEchpe
        void im_preedit_changed() noexcept;

private:
        unsigned read_modifiers_from_cdk(CdkEvent* event) const noexcept;
        bte::terminal::KeyEvent key_event_from_cdk(CdkEventKey* event) const;
        std::optional<bte::terminal::MouseEvent> mouse_event_from_cdk(CdkEvent* event) const;

        CtkWidget* m_widget;

        bte::terminal::Terminal* m_terminal;

        /* Event window */
        CdkWindow *m_event_window;

        /* Cursors */
        bte::glib::RefPtr<CdkCursor> m_default_cursor;
        bte::glib::RefPtr<CdkCursor> m_invisible_cursor;
        bte::glib::RefPtr<CdkCursor> m_mousing_cursor;
        bte::glib::RefPtr<CdkCursor> m_hyperlink_cursor;

        /* Input method */
        bte::glib::RefPtr<CtkIMContext> m_im_context;

        /* PTY */
        bte::glib::RefPtr<BtePty> m_pty;

        /* Misc */
        std::optional<std::string> m_word_char_exceptions{};

        bte::glib::RefPtr<CtkAdjustment> m_hadjustment{};
        uint32_t m_hscroll_policy : 1;
        uint32_t m_vscroll_policy : 1;
};

} // namespace platform

} // namespace bte
