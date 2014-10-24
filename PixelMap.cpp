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
#include "Board.h"

PixelMap::PixelMap(unsigned int width, unsigned int height, color_t rgb_in[], unsigned int rgb_length) :
    filled(false)
{
    unsigned int length = width * height;
    rgb = new color_t[length];

    for (unsigned int i = 0; i < std::min(length, rgb_length); i++)
    {
        memcpy(rgb[i], rgb_in[i], sizeof(color_t));
    }
    for (unsigned int i = std::min(length, rgb_length); i < length; i++)
    {
        memset(rgb[i], 0, sizeof(color_t));
    }
}

PixelMap::~PixelMap() {
    delete[] rgb;
}

void PixelMap::tick(rgb_matrix::Canvas &canvas)
{
    if (!filled)
    {
        for (int x = 0; x < canvas.width(); x++)
        {
            for (int y = 0; y < canvas.height(); y++)
            {
                unsigned int offset = y * canvas.width() + x;
                canvas.SetPixel(x, y, rgb[offset][0], rgb[offset][1], rgb[offset][2]);
            }
        }
        filled = true;
    }
}
