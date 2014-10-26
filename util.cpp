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

int b64_octet(const char in[4], unsigned char out[3])
{
    unsigned long octet = 0;
    int bytes = 3;

    for (unsigned int i = 0; i < 4; i++)
    {
        unsigned char index;

        if (in[i] == '=')
        {
            if (i == 0 || i == 1)
                return -1;
            else if (i == 2 && in[i] != '=')
                return -1;
            else if (i == 2)
                bytes = 1;
            else
                bytes = 2;
            break;
        }
        else if ('A' <= in[i] && in[i] <= 'Z')
            index = (in[i] - 'A');
        else if ('a' <= in[i] && in[i] <= 'z')
            index = (in[i] - 'a' + 26);
        else if ('0' <= in[i] && in[i] <= '9')
            index = (in[i] - '0' + 52);
        else if (in[i] == '+')
            index = 62;
        else if (in[i] == '/')
            index = 63;
        else
            return -1;

        octet += index << (6 * (3 - i));
    }

    out[0] = octet >> 16;
    out[1] = octet >> 8;
    out[2] = octet;

    return bytes;
}

std::vector<unsigned char> *b64_decode(const std::string b64)
{
    if (b64.length() % 4 != 0)
    {
        return NULL;
    }

    std::vector<unsigned char> *vec = new std::vector<unsigned char>(b64.length() / 4 * 3, 0);
    int bytes;
    char octet_in[4];
    unsigned char octet_out[3];

    for (unsigned int i = 0; i < b64.length() / 4; i++)
    {
        for (int j = 0; j < 4; j++)
            octet_in[j] = b64[(i * 4) + j];
        bytes = b64_octet(octet_in, octet_out);
        if (bytes == -1)
        {
            delete vec;
            return NULL;
        }
        for (int j = 0; j < bytes; j++)
        {
            (*vec)[(i * 3) + j] = octet_out[j];
        }
        if (bytes != 3)
            for (int j = bytes; j < 3; j++)
                vec->pop_back();
    }

    return vec;
}

void apply_alpha(const color_alpha_t in, color_t out, const color_t background)
{
    double alpha = ((double) in[3]) / 255.0;

    for (unsigned int i = 0; i < 3; i++)
    {
        out[i] = ((double) in[i]) * alpha + ((double) background[i]) * (1.0 - alpha);
    }
}
