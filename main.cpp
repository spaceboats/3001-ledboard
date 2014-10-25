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
 *
 ******************************************************************************
 *
 * Listen for new states (JSON) on a Unix socket and update an LED board.
 *
 * Note that the led-matrix library this depends on is GPLv2
 */

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <list>
#include <thread>
#include <unistd.h>
#include "led-matrix.h"
#include "rapidjson/document.h"
#include "util.h"
#include "Emulator.h"
#include "State.h"
#include "Fill.h"
#include "PixelMap.h"
#include "Conway.h"

// number of LED rows on board
#define BOARD_ROWS 16
// how many boards are chained together
#define BOARD_CHAIN 3
// length of a tick, in microseconds
#define TICK_LENGTH 50000

struct read_state_args_t
{
    unsigned int width;
    unsigned int height;
    State **state;
    std::list<State *> garbage;
};

void read_state(read_state_args_t *args)
{
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

            color_t rgb;
            if (!print_error(get_color(document["color"], rgb), "\"color\" value is invalid")) continue;
            ptr = new Fill(rgb);
        }
        else if (mode.compare("pixelmap") == 0)
        {
            if (!print_error(document.HasMember("data"), "missing \"data\" key")) continue;
            if (!print_error(document["data"].IsArray(), "\"data\" value is not array")) continue;

            color_t *rgb = new color_t[document["data"].Size()];
            for (unsigned int i = 0; i < document["data"].Size(); i++)
            {
                if (!print_error(get_color(document["data"][i], rgb[i]), "\"data[" + std::to_string(i) + "\" value is invalid")) continue;
            }
            ptr = new PixelMap(args->width, args->height, rgb, (unsigned int) document["data"].Size());
            delete[] rgb;
        }
        else if (mode.compare("conway") == 0)
        {
            color_t rgb = {255, 0, 0};
            unsigned int interval = 200000;

            if (document.HasMember("color"))
                if (!print_error(get_color(document["color"], rgb), "\"color\" value is invalid")) continue;
            if (document.HasMember("interval"))
            {
                if (!print_error(document["interval"].IsUint(), "\"interval\" value is invalid")) continue;
                interval = document["interval"].GetUint() * 1000;
            }

            ptr = new Conway(args->width, args->height, rgb, interval / TICK_LENGTH);
        }
        else
        {
            print_error(false, "invalid mode \"" + mode + "\"");
            continue;
        }

        std::swap(*args->state, ptr);
        args->garbage.push_back(ptr);
    }
}

unsigned int tick(State *state, rgb_matrix::Canvas &canvas, std::list<State *> &garbage)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    state->tick(canvas);
    while (!garbage.empty())
    {
        delete garbage.front();
        garbage.pop_front();
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);
    return usec_difference(start, end);
}

int main()
{
    unsigned int tick_time;
    State *state;
    read_state_args_t read_state_args;
    read_state_args.state = &state;

    // seed RNG
    srand(time(NULL));

#ifndef EMULATE_LEDBOARD
    // set up GPIO pins
    rgb_matrix::GPIO io;
    if (!io.Init())
    {
        return 1;
    }
#endif // !EMULATE_LEDBORAD

    // set up LED matrix
#ifdef EMULATE_LEDBOARD
    Emulator matrix(BOARD_CHAIN * 32, BOARD_ROWS);
#else
    rgb_matrix::RGBMatrix matrix(&io, BOARD_ROWS, BOARD_CHAIN);
#endif // EMULATE_LEDBOARD
    read_state_args.width = matrix.width();
    read_state_args.height = matrix.height();

    color_t rgb = {0, 0, 0};
    state = new Fill(rgb);

    std::thread(read_state, &read_state_args).detach();

    // main loop here: read until stdin is EOF
    while (!std::cin.eof())
    {
        tick_time = tick(state, matrix, read_state_args.garbage);
        if (TICK_LENGTH > tick_time)
            usleep(std::max((unsigned int) 0, TICK_LENGTH - tick_time));
    }

    // clean up
    matrix.Clear();

    return 0;
}
