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

#ifndef _UTIL_H
#define _UTIL_H

#include <string>
#include <time.h>
#include <vector>
#include "rapidjson/document.h"

typedef uint8_t color_t[3];
typedef uint8_t color_alpha_t[4];

bool print_error(bool assertion, std::string msg);
bool get_color(rapidjson::Value &value, color_t rgb);
unsigned int usec_difference(struct timespec start, struct timespec end);
int b64_octet(const char in[4], unsigned char out[3]);
std::vector<unsigned char> *b64_decode(const std::string b64);
void apply_alpha(const color_alpha_t in, color_t out, const color_t background);

#endif
