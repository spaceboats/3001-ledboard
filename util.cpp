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

#include <cstdlib>
#include <iostream>
#include "util.h"

bool print_error(bool assertion, std::string msg)
{
    if (!assertion)
    {
        std::cout << msg << std::endl;
    }

    return assertion;
}

bool get_color(rapidjson::Value &value, color_t rgb)
{
    /* [255, 255, 255] */
    if (value.IsArray())
    {
        if (value.Size() != 3) return false;
        for (int i = 0; i < 3; i++)
        {
            if (!value[i].IsInt()) return false;
            rgb[i] = value[i].GetInt();
        }
    }
    /* "a1b2c3" */
    else if (value.IsString())
    {
        if (value.GetStringLength() == 6)
        {
            std::string hex = "0x" + std::string(value.GetString(), value.GetStringLength());
            char *p;
            unsigned long int i = strtoul(hex.c_str(), &p, 16);
            if (*p != '\0') return false;

            rgb[0] = i >> 16;
            rgb[1] = i >> 8;
            rgb[2] = i;
        }
        else return false;
    }
    /* 8884 => 0x0022b4 */
    else if (value.IsUint())
    {
        unsigned int i = value.GetUint();
        rgb[0] = i >> 16;
        rgb[1] = i >> 8;
        rgb[2] = i;
    }
    else
    {
        return false;
    }

    return true;
}

unsigned int usec_difference(struct timespec start, struct timespec end)
{
    int diff = (end.tv_nsec - start.tv_nsec) / 1000;
    return diff + ((end.tv_sec - start.tv_sec) * 1e6);
}
