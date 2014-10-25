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

#ifdef EMULATE_LEDBOARD

#ifndef _EMULATOR_H
#define _EMULATOR_H

#include "SDL/SDL.h"
#include "led-matrix.h"

#define PIXELS_PER_UNIT 8

class Emulator : public rgb_matrix::Canvas
{
    public:
        Emulator(int width, int height);
        ~Emulator();

        int width() const;
        int height() const;
        void SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue);
        void Clear();
        void Fill(uint8_t red, uint8_t green, uint8_t blue);

    private:
        int _width;
        int _height;

        SDL_Surface *window;
};

#endif

#endif // EMULATE_LEDBOARD
