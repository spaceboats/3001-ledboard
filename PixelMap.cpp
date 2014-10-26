/* Copyright (c) 2014 Alex Gustafson
 * Copyright (c) 2014 Ian Weller
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 ******************************************************************************
 *
 * State subclass that displays a pixel map
 *
 */

#include <algorithm>
#include <cstring>
#include "PixelMap.h"

PixelMap::PixelMap(unsigned int rgb_width_in, unsigned int rgb_height_in,
        const color_t rgb_in[], const scroll_args_t scroll_args_in) :
    filled(false), scrolling(true), offset(0), ticks(0),
    rgb_width(rgb_width_in), rgb_height(rgb_height_in)
{
    unsigned int length = rgb_width * rgb_height;
    rgb = new color_t[length];
    memcpy(rgb, rgb_in, sizeof(color_t) * rgb_width * rgb_height);
    copy_scroll_args(scroll_args, scroll_args_in);
    tick_interval = ms_to_ticks(scroll_args.interval);
    tick_wait = ms_to_ticks(scroll_args.wait);
}

PixelMap::~PixelMap() {
    delete[] rgb;
}

void PixelMap::tick(rgb_matrix::Canvas &canvas)
{
    if ((scroll_args.dir == SCROLL_NONE && !filled) || (scroll_args.dir != SCROLL_NONE && scrolling))
    {
        if (scroll_args.dir == SCROLL_NONE || ++ticks >= tick_interval || !filled)
        {
            ticks = 0;

            unsigned int xpos, ypos;

            for (int x = 0; x < canvas.width(); x++)
            {
                xpos = (scroll_args.dir == SCROLL_HORIZONTAL) ? (x + offset) % (rgb_width + scroll_args.padding) : x;

                for (int y = 0; y < canvas.height(); y++)
                {
                    ypos = (scroll_args.dir == SCROLL_VERTICAL) ? (y + offset) % (rgb_height + scroll_args.padding) : y;

                    if ((scroll_args.dir == SCROLL_HORIZONTAL && xpos >= (rgb_width)) ||
                            (scroll_args.dir == SCROLL_VERTICAL && ypos >= (rgb_height)))
                        canvas.SetPixel(x, y,
                                scroll_args.padding_color[0],
                                scroll_args.padding_color[1],
                                scroll_args.padding_color[2]);
                    else if (xpos >= rgb_width || ypos >= rgb_height)
                        canvas.SetPixel(x, y, 0, 0, 0);
                    else
                    {
                        unsigned int pos = ypos * rgb_width + xpos;
                        canvas.SetPixel(x, y, rgb[pos][0], rgb[pos][1], rgb[pos][2]);
                    }
                }
            }

            filled = true;
            if (scroll_args.dir != SCROLL_NONE)
                if (++offset >= ((scroll_args.dir == SCROLL_HORIZONTAL ? rgb_width : rgb_height) + scroll_args.padding))
                {
                    offset = 0;
                    if (tick_wait > 0)
                        scrolling = false;
                }
        }
    }
    else if (scroll_args.dir != SCROLL_NONE && !scrolling)
    {
        if (++ticks >= tick_wait)
        {
            ticks = 0;
            scrolling = true;
        }
    }
}
