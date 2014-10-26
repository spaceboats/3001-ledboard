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

#include "Emulator.h"

Emulator::Emulator(int width, int height) :
    _width(width), _height(height)
{
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_SetVideoMode(_width * PIXELS_PER_UNIT, _height * PIXELS_PER_UNIT,
            24, SDL_HWSURFACE);
    SDL_WM_SetCaption("board_controller emulator", 0);
}

Emulator::~Emulator()
{
    SDL_Quit();
}

int Emulator::width() const
{
    return _width;
}

int Emulator::height() const
{
    return _height;
}

void Emulator::SetPixel(int x, int y, uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_Rect r;
    r.x = x * PIXELS_PER_UNIT;
    r.y = y * PIXELS_PER_UNIT;
    r.w = PIXELS_PER_UNIT;
    r.h = PIXELS_PER_UNIT;
    SDL_FillRect(window, &r, SDL_MapRGB(window->format, red, green, blue));
    SDL_UpdateRect(window, r.x, r.y, r.w, r.h);
}

void Emulator::Clear()
{
    Fill(0, 0, 0);
}

void Emulator::Fill(uint8_t red, uint8_t green, uint8_t blue)
{
    SDL_FillRect(window, NULL, SDL_MapRGB(window->format, red, green, blue));
    SDL_Flip(window);
}

#endif // EMULATE_LEDBOARD
