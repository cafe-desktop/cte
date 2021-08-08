/*
 * Copyright © 2015 Christian Persch
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
 * SECTION: bte-regex
 * @short_description: Regex for matching and searching. Uses PCRE2 internally.
 *
 * Since: 0.46
 */

#include "config.h"

#include <exception>

#include "btemacros.h"
#include "bteenums.h"
#include "bteregex.h"
#include "btepcre2.h"

#include "regex.hh"
#include "bteregexinternal.hh"
#include "glib-glue.hh"
#include "cxx-utils.hh"

#define IMPL(wrapper) (regex_from_wrapper(wrapper))

/* Type registration */

#pragma GCC diagnostic push
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wcast-function-type"
#endif
G_DEFINE_BOXED_TYPE(BteRegex, bte_regex,
                    bte_regex_ref, (GBoxedFreeFunc)bte_regex_unref)
#pragma GCC diagnostic pop

G_DEFINE_QUARK(bte-regex-error, bte_regex_error)

/**
 * bte_regex_ref:
 * @regex: (transfer none): a #BteRegex
 *
 * Increases the reference count of @regex by one.
 *
 * Returns: @regex
 */
BteRegex *
bte_regex_ref(BteRegex *regex) noexcept
{
        g_return_val_if_fail(regex != nullptr, nullptr);

        return wrapper_from_regex(IMPL(regex)->ref());
}

/**
 * bte_regex_unref:
 * @regex: (transfer full): a #BteRegex
 *
 * Decreases the reference count of @regex by one, and frees @regex
 * if the refcount reaches zero.
 *
 * Returns: %NULL
 */
BteRegex *
bte_regex_unref(BteRegex* regex) noexcept
{
        g_return_val_if_fail(regex != nullptr, nullptr);

        IMPL(regex)->unref();
        return nullptr;
}

static BteRegex*
bte_regex_new(bte::base::Regex::Purpose purpose,
              std::string_view const& pattern,
              uint32_t flags,
              GError** error) noexcept
try
{
        return wrapper_from_regex(bte::base::Regex::compile(purpose, pattern, flags, error));
}
catch (...)
{
        bte::glib::set_error_from_exception(error);
        return nullptr;
}

/**
 * bte_regex_new_for_match:
 * @pattern: a regex pattern string
 * @pattern_length: the length of @pattern in bytes, or -1 if the
 *  string is NUL-terminated and the length is unknown
 * @flags: PCRE2 compile flags
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Compiles @pattern into a regex for use as a match regex
 * with bte_terminal_match_add_regex() or
 * bte_terminal_event_check_regex_simple().
 *
 * See man:pcre2pattern(3) for information
 * about the supported regex language.
 *
 * The regex will be compiled using %PCRE2_UTF and possibly other flags, in
 * addition to the flags supplied in @flags.
 *
 * Returns: (transfer full): a newly created #BteRegex, or %NULL with @error filled in
 */
BteRegex *
bte_regex_new_for_match(const char *pattern,
                        gssize      pattern_length,
                        guint32     flags,
                        GError    **error) noexcept
try
{
        auto const len = size_t{pattern_length == -1 ? strlen(pattern) : size_t(pattern_length)};
        return bte_regex_new(bte::base::Regex::Purpose::eMatch,
                             {pattern, len},
                             flags,
                             error);
}
catch (...)
{
        bte::glib::set_error_from_exception(error);
        return nullptr;
}

/**
 * bte_regex_new_for_search:
 * @pattern: a regex pattern string
 * @pattern_length: the length of @pattern in bytes, or -1 if the
 *  string is NUL-terminated and the length is unknown
 * @flags: PCRE2 compile flags
 * @error: (allow-none): return location for a #GError, or %NULL
 *
 * Compiles @pattern into a regex for use as a search regex
 * with bte_terminal_search_set_regex().
 *
 * See man:pcre2pattern(3) for information
 * about the supported regex language.
 *
 * The regex will be compiled using %PCRE2_UTF and possibly other flags, in
 * addition to the flags supplied in @flags.
 *
 * Returns: (transfer full): a newly created #BteRegex, or %NULL with @error filled in
 */
BteRegex *
bte_regex_new_for_search(const char *pattern,
                         gssize      pattern_length,
                         guint32     flags,
                         GError    **error) noexcept
try
{
        auto const len = size_t{pattern_length == -1 ? strlen(pattern) : size_t(pattern_length)};
        return bte_regex_new(bte::base::Regex::Purpose::eSearch,
                             {pattern, len},
                             flags,
                             error);
}
catch (...)
{
        bte::glib::set_error_from_exception(error);
        return nullptr;
}

/**
 * bte_regex_jit:
 * @regex: a #BteRegex
 *
 * If the platform supports JITing, JIT compiles @regex.
 *
 * Returns: %TRUE if JITing succeeded (or PCRE2 was built without
 *   JIT support), or %FALSE with @error filled in
 */
gboolean
bte_regex_jit(BteRegex *regex,
              guint     flags,
              GError  **error) noexcept
try
{
        g_return_val_if_fail(regex != nullptr, false);

        return IMPL(regex)->jit(flags, error);
}
catch (...)
{
        return bte::glib::set_error_from_exception(error);
}

bool
_bte_regex_has_purpose(BteRegex *regex,
                       bte::base::Regex::Purpose purpose)
{
        g_return_val_if_fail(regex != nullptr, false);

        return IMPL(regex)->has_purpose(purpose);
}

bool
_bte_regex_has_multiline_compile_flag(BteRegex *regex)
{
        g_return_val_if_fail(regex != nullptr, 0);

        return IMPL(regex)->has_compile_flags(PCRE2_MULTILINE);
}

/**
 * bte_regex_substitute:
 * @regex: a #BteRegex
 * @subject: the subject string
 * @replacement: the replacement string
 * @flags: PCRE2 match flags
 * @error: (nullable): return location for a #GError, or %NULL
 *
 * See man:pcre2api(3) on pcre2_substitute() for more information.
 *
 * Returns: (transfer full): the substituted string, or %NULL
 *   if an error occurred
 *
 * Since: 0.56
 */
char *
bte_regex_substitute(BteRegex *regex,
                     const char *subject,
                     const char *replacement,
                     guint32 flags,
                     GError **error) noexcept
try
{
        g_return_val_if_fail(regex != nullptr, nullptr);
        g_return_val_if_fail(subject != nullptr, nullptr);
        g_return_val_if_fail(replacement != nullptr, nullptr);
        g_return_val_if_fail (!(flags & PCRE2_SUBSTITUTE_OVERFLOW_LENGTH), nullptr);

        auto const r = IMPL(regex)->substitute(subject, replacement, flags, error);
        return r ? g_strndup(r->c_str(), r->size()) : nullptr;
}
catch (...)
{
        bte::glib::set_error_from_exception(error);
        return nullptr;
}
