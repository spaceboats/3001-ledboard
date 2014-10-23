/* Copyright (c) 2014 Ian Weller
 * Copyright (c) 2014 Alex Gustafson
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

#include <iostream>
#include <string>
#include <sys/time.h>
#include "rapidjson/document.h"
#include "util.h"
#include "Board.h"
#include "Fill.h"

// FIXME Fill is overridden by the Canvas::Fill method within Board
typedef Fill FillMode;

void Board::read_state(State **state) {
    State *ptr;
    std::string input, mode;
    rapidjson::Document document;

    while (!std::getline(std::cin, input).eof())
    {
        /* initial sanity check for the input */
        document.Parse(input.c_str());
        if (!print_error(document.IsObject(), "not JSON object")) continue;
        if (!print_error(document.HasMember("mode"), "missing \"mode\" key")) continue;
        if (!print_error(document["mode"].IsString(), "\"mode\" value is not string")) continue;

        /* switch on mode */
        mode = document["mode"].GetString();
        if (mode.compare("fill") == 0)
        {
            if (!print_error(document.HasMember("color"), "missing \"color\" key")) continue;

            uint8_t rgb[3];
            if (!print_error(get_color(document["color"], rgb), "\"color\" value is invalid")) continue;
            ptr = new FillMode(rgb); // FIXME see typedef at top of Board.cpp
        }
        else
        {
            print_error(false, "invalid mode \"" + mode + "\"");
            continue;
        }

        std::swap(*state, ptr);
        delete ptr;
    }
}

Board::Board(rgb_matrix::GPIO *io, int rows = 32, int chained_displays = 1) :
    rgb_matrix::RGBMatrix(io, rows, chained_displays)
{
    uint8_t rgb[3] = {0, 0, 0};
    state = new FillMode(rgb);

    read_state_thread = new std::thread(read_state, &state);
}

bool Board::tick(unsigned int &tick_time)
{
    struct timeval start, end;
    bool eof;

    gettimeofday(&start, NULL);

    state->tick(*this);

    eof = std::cin.eof();
    if (eof)
    {
        read_state_thread->join();
    }

    gettimeofday(&end, NULL);

    tick_time = microsecond_difference(start, end);

    return !eof;
}
