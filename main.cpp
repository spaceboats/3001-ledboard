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
#include <string>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include "led-matrix.h"
#include "rapidjson/document.h"
#include "util.h"
#include "State.h"
#include "Fill.h"

// number of LED rows on board
#define BOARD_ROWS 16
// how many boards are chained together
#define BOARD_CHAIN 3
// length of a tick, in milliseconds
#define TICK_LENGTH 50

void read_state(State **state)
{
    State *ptr;
    std::string input, mode;
    rapidjson::Document document;

    while (!std::cin.eof())
    {
        std::getline(std::cin, input);

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
            ptr = new Fill(rgb);
        }

        std::swap(*state, ptr);
        delete ptr;
    }
}

unsigned int microsecond_difference(struct timeval start, struct timeval end)
{
    int diff = (end.tv_usec - start.tv_usec) / 1000;
    return diff + (end.tv_sec - start.tv_sec) * 1000;
}

class Board : public rgb_matrix::RGBMatrix
{
    private:
        State *state;
        std::thread *read_state_thread;

    public:
        Board(rgb_matrix::GPIO *io, int rows = 32, int chained_displays = 1)
            : rgb_matrix::RGBMatrix(io, rows, chained_displays)
        {
            state = NULL;

            read_state_thread = new std::thread(read_state, &state);
        }

        bool tick(unsigned int &tick_time)
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
};

int main()
{
    unsigned int tick_time;
    Board *board;

    // set up GPIO pins
    rgb_matrix::GPIO io;
    if (!io.Init())
    {
        return 1;
    }

    // set up LED matrix
    board = new Board(&io, BOARD_ROWS, BOARD_CHAIN);

    // main loop here: read until stdin is EOF
    while (board->tick(tick_time))
    {
        usleep(TICK_LENGTH - tick_time);
    }

    // clean up
    board->Clear();
    delete board;

    return 0;
}
