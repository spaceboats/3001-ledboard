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
#include <unistd.h>
#include "led-matrix.h"
#include "util.h"
#include "Board.h"

// number of LED rows on board
#define BOARD_ROWS 16
// how many boards are chained together
#define BOARD_CHAIN 3
// length of a tick, in microseconds
#define TICK_LENGTH 50000

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
        if (TICK_LENGTH > tick_time)
            usleep(std::max((unsigned int) 0, TICK_LENGTH - tick_time));
    }

    // clean up
    board->Clear();
    delete board;

    return 0;
}
