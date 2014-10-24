/* Copyright (c) 2014 Ian Weller
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
 */

#include <cstdlib>
#include <cstring>
#include <utility>
#include "Conway.h"

Conway::Conway(unsigned int width, unsigned int height, const color_t rgb_in, int tick_interval_in)
{
    this->width = width;
    this->height = height;
    for (int i = 0; i < 3; i++)
    {
        rgb[i] = rgb_in[i];
    }

    ticks = 0;
    if (tick_interval_in < 1)
        tick_interval_in = 1;
    tick_interval = tick_interval_in;

    cur = new bool[width*height];
    next = new bool[width*height];
    for (unsigned int i = 0; i < width*height; i++)
    {
        cur[i] = rand() % 2;
    }
}

Conway::~Conway()
{
    delete[] cur;
    delete[] next;
}

void Conway::tick(rgb_matrix::Canvas &canvas)
{
    ticks = (ticks + 1) % tick_interval;
    if (ticks == 0)
    {
        unsigned int neighbors;

        for (int x = 0; x < canvas.width(); x++)
        {
            for (int y = 0; y < canvas.height(); y++)
            {
                int i = pos(x, y);

                /* render cur to the board */
                cur[i] ? canvas.SetPixel(x, y, rgb[0], rgb[1], rgb[2]) : canvas.SetPixel(x, y, 0, 0, 0);

                /* generate next */
                neighbors = 0;
                for (int xmod = -1; xmod <= 1; xmod++)
                {
                    for (int ymod = -1; ymod <= 1; ymod++)
                    {
                        neighbors += cur[pos(x + xmod, y + ymod)];
                    }
                }
                if (neighbors == 3)
                    next[i] = true;
                else if (neighbors == 4)
                    next[i] = cur[i];
                else
                    next[i] = false;
            }
        }

        /* swap next into cur */
        //memcpy(cur, next, sizeof(bool) * width * height);
        std::swap(cur, next);
    }
}

unsigned int Conway::pos(int x, int y)
{
    while (x < 0)
        x += width;
    while (x >= width)
        x -= width;
    while (y < 0)
        y += height;
    while (y >= height)
        y -= height;

    return y * width + x;
}
