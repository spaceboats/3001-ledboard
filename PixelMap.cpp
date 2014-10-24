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

#include "PixelMap.h"
#include <iostream>

PixelMap::PixelMap(rgb_matrix::Canvas &canvas, uint8_t rgb[][3], unsigned int rgb_length)
{
    for (int x = 0; x < canvas.width(); x++)
    {
        for (int y = 0; y < canvas.height(); y++)
        {
            unsigned int offset = y * canvas.width() + x;
            if (offset >= rgb_length)
            {
                canvas.SetPixel(x, y, 0, 0, 0);
            }
            else
            {
                canvas.SetPixel(x, y, rgb[offset][0], rgb[offset][1], rgb[offset][2]);
            }
        }
    }
}

#pragma GCC diagnostic ignored "-Wunused-parameter"
void PixelMap::tick(rgb_matrix::Canvas &canvas)
{
}
#pragma GCC diagnostic pop
