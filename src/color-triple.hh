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
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <cstdint>

typedef uint64_t bte_color_triple_t;

#define BTE_COLOR_TRIPLE_FORE_SHIFT     (0)
#define BTE_COLOR_TRIPLE_BACK_SHIFT     (25)
#define BTE_COLOR_TRIPLE_DECO_SHIFT     (50)

#define BTE_COLOR_TRIPLE_RGB_MASK(rb,gb,bb) ((1ULL << ((rb) + (gb) + (bb) + 1)) - 1)
#define BTE_COLOR_TRIPLE_FORE_MASK      (BTE_COLOR_TRIPLE_RGB_MASK(8, 8, 8) << BTE_COLOR_TRIPLE_FORE_SHIFT)
#define BTE_COLOR_TRIPLE_BACK_MASK      (BTE_COLOR_TRIPLE_RGB_MASK(8, 8, 8) << BTE_COLOR_TRIPLE_BACK_SHIFT)
#define BTE_COLOR_TRIPLE_DECO_MASK      (BTE_COLOR_TRIPLE_RGB_MASK(4, 5, 4) << BTE_COLOR_TRIPLE_DECO_SHIFT)

#define BTE_COLOR_TRIPLE_INIT(fg,bg,dc) (uint64_t(fg) << BTE_COLOR_TRIPLE_FORE_SHIFT | \
                                         uint64_t(bg) << BTE_COLOR_TRIPLE_BACK_SHIFT | \
                                         uint64_t(dc) << BTE_COLOR_TRIPLE_DECO_SHIFT)

#define BTE_COLOR_TRIPLE_INIT_DEFAULT   (BTE_COLOR_TRIPLE_INIT(BTE_DEFAULT_FG, \
                                                               BTE_DEFAULT_BG, \
                                                               BTE_DEFAULT_FG))

static constexpr inline bte_color_triple_t bte_color_triple_init(void)
{
        return BTE_COLOR_TRIPLE_INIT_DEFAULT;
}

static constexpr inline bte_color_triple_t bte_color_triple_copy(bte_color_triple_t ct)
{
        return ct;
}

static inline void bte_color_triple_set_fore(bte_color_triple_t* ct,
                                             uint32_t fore)
{
        *ct = (*ct & ~BTE_COLOR_TRIPLE_FORE_MASK) | (uint64_t(fore)) << BTE_COLOR_TRIPLE_FORE_SHIFT;
}

static inline void bte_color_triple_set_back(bte_color_triple_t* ct,
                                             uint32_t back)
{
        *ct = (*ct & ~BTE_COLOR_TRIPLE_BACK_MASK) | (uint64_t(back)) << BTE_COLOR_TRIPLE_BACK_SHIFT;
}

static inline void bte_color_triple_set_deco(bte_color_triple_t* ct,
                                             uint32_t deco)
{
        *ct = (*ct & ~BTE_COLOR_TRIPLE_DECO_MASK) | (uint64_t(deco)) << BTE_COLOR_TRIPLE_DECO_SHIFT;
}

static inline constexpr uint32_t bte_color_triple_get_fore(bte_color_triple_t ct)
{
        return uint32_t((ct >> BTE_COLOR_TRIPLE_FORE_SHIFT) & BTE_COLOR_TRIPLE_RGB_MASK(8, 8, 8));
}

static inline constexpr uint32_t bte_color_triple_get_back(bte_color_triple_t ct)
{
        return uint32_t((ct >> BTE_COLOR_TRIPLE_BACK_SHIFT) & BTE_COLOR_TRIPLE_RGB_MASK(8, 8, 8));
}

static inline constexpr uint32_t bte_color_triple_get_deco(bte_color_triple_t ct)
{
        return uint32_t((ct >> BTE_COLOR_TRIPLE_DECO_SHIFT) & BTE_COLOR_TRIPLE_RGB_MASK(4, 5, 4));
}

static inline void bte_color_triple_get(bte_color_triple_t ct,
                                        uint32_t* fore,
                                        uint32_t* back,
                                        uint32_t* deco)
{
        *fore = bte_color_triple_get_fore(ct);
        *back = bte_color_triple_get_back(ct);
        *deco = bte_color_triple_get_deco(ct);
}
