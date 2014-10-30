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
#include "lodepng/lodepng.h"
#include "util.h"
#include "Emulator.h"
#include "State.h"
#include "Fill.h"
#include "PixelMap.h"
#include "Conway.h"
#include "TextMap.h"

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

    while (std::getline(std::cin, input))
    {
        std::string request_id = input.substr(0, REQUEST_ID_LENGTH);
        std::string command = input.substr(REQUEST_ID_LENGTH, std::string::npos);

        if (command.compare("board") == 0)
        {
            rapidjson::Document response;
            response.SetObject();

            rapidjson::Value height(rapidjson::kNumberType);
            rapidjson::Value width(rapidjson::kNumberType);
            rapidjson::Value refreshrate(rapidjson::kNumberType);

            height = BOARD_ROWS;
            width = BOARD_CHAIN * 32;
            refreshrate = TICK_LENGTH / 1000;

            response.AddMember("height", height, response.GetAllocator());
            response.AddMember("width", width, response.GetAllocator());
            response.AddMember("refreshrate", refreshrate, response.GetAllocator());

            send_ok(request_id, response);
        }
        else
        {
            /* initial sanity check for the input */
            document.Parse(command.c_str());
            if (!check_error(request_id, document.IsObject(), "not JSON object")) continue;
            if (!check_error(request_id, document.HasMember("mode"), "missing \"mode\" key")) continue;
            if (!check_error(request_id, document["mode"].IsString(), "\"mode\" value is not string")) continue;

            /* switch on mode */
            mode = document["mode"].GetString();
            if (mode.compare("fill") == 0)
            {
                if (!check_error(request_id, document.HasMember("color"), "missing \"color\" key")) continue;

                color_t rgb;
                if (!check_error(request_id, get_color(document["color"], rgb), "\"color\" value is invalid")) continue;
                ptr = new Fill(rgb);
            }
            else if (mode.compare("text") == 0)
            {
                scroll_args_t scroll_args;

                if (!check_error(request_id, document.HasMember("font"), "missing \"font\" key")) continue;
                if (!check_error(request_id, document["font"].IsString(), "\"font\" value is not a string")) continue;
                const char* font_name = document["font"].GetString();
                if (!check_error(request_id, document.HasMember("color"), "missing \"color\" key")) continue;
                if (!check_error(request_id, document.HasMember("message"), "missing \"message\" key")) continue;
                if (!check_error(request_id, document["message"].IsString(), "\"message\" value is not a string")) continue;
                const char* message = document["message"].GetString();

                color_t rgb;
                if (!check_error(request_id, get_color(document["color"], rgb), "\"color\" value is invalid")) continue;

                if (!get_scroll_args(request_id, document, scroll_args)) continue;

                ptr = new TextMap(font_name, message, rgb, scroll_args);
            }
            else if (mode.compare("pixelmap") == 0)
            {
                unsigned int width = args->width;
                unsigned int height = args->height;
                scroll_args_t scroll_args;

                if (!check_error(request_id, document.HasMember("data"), "missing \"data\" key")) continue;
                if (!check_error(request_id, document["data"].IsArray(), "\"data\" value is not array")) continue;

                color_t *rgb = new color_t[width * height];
                memset(rgb, 0, sizeof(color_t) * width * height);

                for (unsigned int i = 0; i < std::min(document["data"].Size(), width * height); i++)
                {
                    if (!check_error(request_id, get_color(document["data"][i], rgb[i]), "\"data[" + std::to_string(i) + "\" value is invalid")) continue;
                }

                if (document.HasMember("width"))
                {
                    if (!check_error(request_id, document["width"].IsUint(), "\"width\" value is invalid")) continue;
                    width = document["width"].GetUint();
                }
                if (document.HasMember("height"))
                {
                    if (!check_error(request_id, document["height"].IsUint(), "\"height\" value is invalid")) continue;
                    width = document["height"].GetUint();
                }

                if (!get_scroll_args(request_id, document, scroll_args)) continue;

                ptr = new PixelMap(width, height, rgb, scroll_args);
                delete[] rgb;
            }
            else if (mode.compare("png") == 0)
            {
                scroll_args_t scroll_args;

                if (!check_error(request_id, document.HasMember("data"), "missing \"data\" key")) continue;
                if (!check_error(request_id, document["data"].IsString(), "\"data\" value is not string")) continue;

                std::string data = document["data"].GetString();
                std::vector<unsigned char> *png = b64_decode(data);
                if (!check_error(request_id, png != NULL, "\"data\" value is not valid base64 string")) continue;

                std::vector<unsigned char> rgba_vec;
                unsigned int width, height;
                lodepng::State png_state;
                unsigned int error = lodepng::decode(rgba_vec, width, height, png_state, *png);
                if (!check_error(request_id, !error, std::string("\"data\" decoded is not valid PNG: ") + lodepng_error_text(error))) continue;
                delete png;

                if (!get_scroll_args(request_id, document, scroll_args)) continue;

                color_alpha_t rgba;
                color_t *rgb = new color_t[width * height];
                color_t background;
                if (png_state.info_png.background_defined)
                {
                    background[0] = png_state.info_png.background_r;
                    background[1] = png_state.info_png.background_g;
                    background[2] = png_state.info_png.background_b;
                }
                else
                {
                    memset(background, 0, sizeof(color_t));
                }

                for (unsigned int i = 0; i < width * height; i++)
                {
                    for (unsigned int j = 0; j < 4; j++)
                        rgba[j] = rgba_vec[(i * 4) + j];
                    apply_alpha(rgba, rgb[i], background);
                }

                ptr = new PixelMap(width, height, rgb, scroll_args);
                delete[] rgb;
            }
            else if (mode.compare("conway") == 0)
            {
                color_t rgb = {255, 0, 0};
                unsigned int interval = 200000;

                if (document.HasMember("color"))
                    if (!check_error(request_id, get_color(document["color"], rgb), "\"color\" value is invalid")) continue;
                if (document.HasMember("interval"))
                {
                    if (!check_error(request_id, document["interval"].IsUint(), "\"interval\" value is invalid")) continue;
                    interval = document["interval"].GetUint() * 1000;
                }

                ptr = new Conway(args->width, args->height, rgb, interval / TICK_LENGTH);
            }
            else
            {
                check_error(request_id, false, "invalid mode \"" + mode + "\"");
                continue;
            }

            std::swap(*args->state, ptr);
            args->garbage.push_back(ptr);
            send_ok(request_id);
        }
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
    delete state;

    return 0;
}
