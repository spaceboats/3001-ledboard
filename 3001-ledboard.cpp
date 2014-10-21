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
 *
 ******************************************************************************
 *
 * Listen for new states (JSON) on a Unix socket and update an LED board.
 *
 * Note that the led-matrix library this depends on is GPLv2
 */

#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>
#include "led-matrix.h"

// number of LED rows on board
#define BOARD_ROWS 16
// how many boards are chained together
#define BOARD_CHAIN 3
// length of a tick, in milliseconds
#define TICK_LENGTH 50

typedef unsigned int board_state_t;

void read_state(board_state_t **state)
{
    unsigned int x, *ptr;

    while (std::cin >> x)
    {
        if (std::cin.eof())
        {
            break;
        }

        ptr = new unsigned int;
        *ptr = x;
        std::swap(*state, ptr);
        delete ptr;
    }
}

class Board : public rgb_matrix::RGBMatrix
{
    private:
        board_state_t *state;
        std::thread *read_state_thread;

    public:
        Board(rgb_matrix::GPIO *io, int rows = 32, int chained_displays = 1)
            : rgb_matrix::RGBMatrix(io, rows, chained_displays)
        {
            state = NULL;

            read_state_thread = new std::thread(read_state, &state);
        }

        bool tick()
        {
            if (state == NULL)
            {
                Fill(0, 0, 0);
            }
            else
            {
                Fill(0, 0, *state);
            }

            if (std::cin.eof())
            {
                read_state_thread->join();
            }

            return !std::cin.eof();
        }
};

int main()
{
    // set up GPIO pins
    rgb_matrix::GPIO io;
    if (!io.Init())
    {
        return 1;
    }

    // set up LED matrix
    Board *board = new Board(&io, BOARD_ROWS, BOARD_CHAIN);

    // main loop here: read until stdin is EOF
    while (board->tick())
    {
        usleep(TICK_LENGTH);
    }

    // clean up
    board->Clear();
    delete board;

    return 0;
}
