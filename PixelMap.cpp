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

PixelMap::PixelMap(rgb_matrix::Canvas &canvas_in, uint8_t rgb_in[][3], unsigned int rgb_length)
{
    width = canvas_in.width();
    height = canvas_in.height();
    length = width * height;
    rgb = new uint8_t*[length];
    for (int i = 0; i < std::min(length, rgb_length); i++)
    {
        rgb[i] = new uint8_t[3];
        rgb[i][0] = rgb_in[i][0];
        rgb[i][1] = rgb_in[i][1];
        rgb[i][2] = rgb_in[i][2];
    }
    for (int i = std::min(length, rgb_length); i < length; i++)
    {
        rgb[i] = new uint8_t[3];
        rgb[i][0] = rgb[i][1] = rgb[i][2] = 0;
    }
}

PixelMap::~PixelMap(){
    for (int i = 0; i < length; i++){
        delete rgb[i];
    }
    delete rgb;
}

void PixelMap::tick(rgb_matrix::Canvas &canvas)
{
    for (int x = 0; x < width; x++)
    {
        for(int y = 0; y < height; y++)
        {
            int offset = y * width + x;
            std::cout << "r: " << rgb[offset][0] << "g: " << rgb[offset][1] << "b: " << rgb[offset][2] << "\n";
            canvas.SetPixel(x, y, rgb[offset][0], rgb[offset][1], rgb[offset][2]);
        }
    }
}
