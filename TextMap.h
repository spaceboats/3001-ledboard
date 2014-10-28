/* Copyright (c) 2014 Zane Ralston
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
 * State subclass that displays a text map
 */

#ifndef _TEXT_MAP_H
#define _TEXT_MAP_H

#include "led-matrix.h"
#include "util.h"
#include "PixelMap.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

class TextMap : public State
{
    public:
        TextMap(const char* ttf_font_file, const char* message, const color_t color, const scroll_args_t scroll_args);
        ~TextMap();

        void tick(rgb_matrix::Canvas &canvas);

    private:
        PixelMap* pixel_map;
        FT_Library library;
        FT_Face face;
        bool initLibrary();
        bool initFace(const char* ttf_font_file, unsigned int face_index = 0);
};

#endif
