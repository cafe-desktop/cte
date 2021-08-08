/*
 * Copyright © 2018, 2019 Christian Persch
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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

#define BTE_ATTR_VALUE_MASK(bits)      ((1U << (bits)) - 1U)
#define BTE_ATTR_MASK(shift,bits)      (BTE_ATTR_VALUE_MASK(bits) << (shift))

/* Number of visible columns (as determined by g_unichar_iswide(c)).
 * Also (ab)used for tabs; bug 353610.
 */
#define BTE_ATTR_COLUMNS_SHIFT         (0)
#define BTE_ATTR_COLUMNS_BITS          (4) /* Has to be able to store the value of 8. */
#define BTE_ATTR_COLUMNS_MASK          (BTE_ATTR_MASK(BTE_ATTR_COLUMNS_SHIFT, BTE_ATTR_COLUMNS_BITS))
#define BTE_ATTR_COLUMNS_VALUE_MASK    (BTE_ATTR_VALUE_MASK(BTE_ATTR_COLUMNS_BITS))
#define BTE_ATTR_COLUMNS(v)            ((v) << BTE_ATTR_COLUMNS_SHIFT)

/* A continuation cell */
#define BTE_ATTR_FRAGMENT_SHIFT        (BTE_ATTR_COLUMNS_SHIFT + BTE_ATTR_COLUMNS_BITS)
#define BTE_ATTR_FRAGMENT_BITS         (1)
#define BTE_ATTR_FRAGMENT_MASK         (BTE_ATTR_MASK(BTE_ATTR_FRAGMENT_SHIFT, BTE_ATTR_FRAGMENT_BITS))
#define BTE_ATTR_FRAGMENT              (1U << BTE_ATTR_FRAGMENT_SHIFT)

#define BTE_ATTR_BOLD_SHIFT            (BTE_ATTR_FRAGMENT_SHIFT + BTE_ATTR_FRAGMENT_BITS)
#define BTE_ATTR_BOLD_BITS             (1)
#define BTE_ATTR_BOLD_MASK             (BTE_ATTR_MASK(BTE_ATTR_BOLD_SHIFT, BTE_ATTR_BOLD_BITS))
#define BTE_ATTR_BOLD                  (1U << BTE_ATTR_BOLD_SHIFT)

#define BTE_ATTR_ITALIC_SHIFT          (BTE_ATTR_BOLD_SHIFT + BTE_ATTR_BOLD_BITS)
#define BTE_ATTR_ITALIC_BITS           (1)
#define BTE_ATTR_ITALIC_MASK           (BTE_ATTR_MASK(BTE_ATTR_ITALIC_SHIFT, BTE_ATTR_ITALIC_BITS))
#define BTE_ATTR_ITALIC                (1U << BTE_ATTR_ITALIC_SHIFT)

/* 0: none, 1: single, 2: double, 3: curly */
#define BTE_ATTR_UNDERLINE_SHIFT       (BTE_ATTR_ITALIC_SHIFT + BTE_ATTR_ITALIC_BITS)
#define BTE_ATTR_UNDERLINE_BITS        (2)
#define BTE_ATTR_UNDERLINE_MASK        (BTE_ATTR_MASK(BTE_ATTR_UNDERLINE_SHIFT, BTE_ATTR_UNDERLINE_BITS))
#define BTE_ATTR_UNDERLINE_VALUE_MASK  (BTE_ATTR_VALUE_MASK(BTE_ATTR_UNDERLINE_BITS))
#define BTE_ATTR_UNDERLINE(v)          ((v) << BTE_ATTR_UNDERLINE_SHIFT)

#define BTE_ATTR_STRIKETHROUGH_SHIFT   (BTE_ATTR_UNDERLINE_SHIFT + BTE_ATTR_UNDERLINE_BITS)
#define BTE_ATTR_STRIKETHROUGH_BITS    (1)
#define BTE_ATTR_STRIKETHROUGH_MASK    (BTE_ATTR_MASK(BTE_ATTR_STRIKETHROUGH_SHIFT, BTE_ATTR_STRIKETHROUGH_BITS))
#define BTE_ATTR_STRIKETHROUGH         (1U << BTE_ATTR_STRIKETHROUGH_SHIFT)

#define BTE_ATTR_OVERLINE_SHIFT        (BTE_ATTR_STRIKETHROUGH_SHIFT + BTE_ATTR_STRIKETHROUGH_BITS)
#define BTE_ATTR_OVERLINE_BITS         (1)
#define BTE_ATTR_OVERLINE_MASK         (BTE_ATTR_MASK(BTE_ATTR_OVERLINE_SHIFT, BTE_ATTR_OVERLINE_BITS))
#define BTE_ATTR_OVERLINE              (1U << BTE_ATTR_OVERLINE_SHIFT)

#define BTE_ATTR_REVERSE_SHIFT         (BTE_ATTR_OVERLINE_SHIFT + BTE_ATTR_OVERLINE_BITS)
#define BTE_ATTR_REVERSE_BITS          (1)
#define BTE_ATTR_REVERSE_MASK          (BTE_ATTR_MASK(BTE_ATTR_REVERSE_SHIFT, BTE_ATTR_REVERSE_BITS))
#define BTE_ATTR_REVERSE               (1U << BTE_ATTR_REVERSE_SHIFT)

#define BTE_ATTR_BLINK_SHIFT           (BTE_ATTR_REVERSE_SHIFT + BTE_ATTR_REVERSE_BITS)
#define BTE_ATTR_BLINK_BITS            (1)
#define BTE_ATTR_BLINK_MASK            (BTE_ATTR_MASK(BTE_ATTR_BLINK_SHIFT, BTE_ATTR_BLINK_BITS))
#define BTE_ATTR_BLINK                 (1U << BTE_ATTR_BLINK_SHIFT)

/* also known as faint, half intensity etc. */
#define BTE_ATTR_DIM_SHIFT             (BTE_ATTR_BLINK_SHIFT + BTE_ATTR_BLINK_BITS)
#define BTE_ATTR_DIM_BITS              (1)
#define BTE_ATTR_DIM_MASK              (BTE_ATTR_MASK(BTE_ATTR_DIM_SHIFT, BTE_ATTR_DIM_BITS))
#define BTE_ATTR_DIM                   (1U << BTE_ATTR_DIM_SHIFT)

#define BTE_ATTR_INVISIBLE_SHIFT       (BTE_ATTR_DIM_SHIFT + BTE_ATTR_DIM_BITS)
#define BTE_ATTR_INVISIBLE_BITS        (1)
#define BTE_ATTR_INVISIBLE_MASK        (BTE_ATTR_MASK(BTE_ATTR_INVISIBLE_SHIFT, BTE_ATTR_INVISIBLE_BITS))
#define BTE_ATTR_INVISIBLE             (1U << BTE_ATTR_INVISIBLE_SHIFT)

/* Used internally only */
#define BTE_ATTR_BOXED_SHIFT           (31)
#define BTE_ATTR_BOXED_BITS            (1)
#define BTE_ATTR_BOXED_MASK            (BTE_ATTR_MASK(BTE_ATTR_BOXED_SHIFT, BTE_ATTR_BOXED_BITS))
#define BTE_ATTR_BOXED                 (1U << BTE_ATTR_BOXED_SHIFT)

/* All attributes except DIM and BOXED */
#define BTE_ATTR_ALL_MASK              (BTE_ATTR_BOLD_MASK | \
                                        BTE_ATTR_ITALIC_MASK | \
                                        BTE_ATTR_UNDERLINE_MASK | \
                                        BTE_ATTR_STRIKETHROUGH_MASK | \
                                        BTE_ATTR_OVERLINE_MASK | \
                                        BTE_ATTR_REVERSE_MASK | \
                                        BTE_ATTR_BLINK_MASK | \
                                        BTE_ATTR_INVISIBLE_MASK)

#define BTE_ATTR_NONE                  (0U)
#define BTE_ATTR_DEFAULT               (BTE_ATTR_COLUMNS(1))

static inline void bte_attr_set_bool(uint32_t* attr,
                                     uint32_t mask,
                                     bool value)
{
        if (value)
                *attr |= mask;
        else
                *attr &= ~mask;
}

static inline void bte_attr_set_value(uint32_t* attr,
                                      uint32_t mask,
                                      unsigned int shift,
                                      uint32_t value)
{
        g_assert_cmpuint(value << shift, <=, mask); /* assurance */
        *attr = (*attr & ~mask) | ((value << shift) & mask /* assurance */);
}

static constexpr inline bool bte_attr_get_bool(uint32_t attr,
                                               unsigned int shift)
{
        return (attr >> shift) & 1U;
}

static constexpr inline unsigned int bte_attr_get_value(uint32_t attr,
                                                        uint32_t value_mask,
                                                        unsigned int shift)
{
        return (attr >> shift) & value_mask;
}
