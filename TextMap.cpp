/* Copyright (c) 2014 Zane Ralston
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
 * State subclass that displays a text map
 *
 */

#include "TextMap.h"
#include <iostream>

TextMap::TextMap(const char* ttf_font_file, const scroll_args_t scroll_args_in)
{
    initLibrary();
    initFace(ttf_font_file);

    /*
    if(FT_Set_Pixel_Sizes(face, 0, 16))
    {
        std::cout << "error with pixel set\n";
    }

    FT_Load_Char(face, 'a', FT_LOAD_RENDER);
    FT_BBox acbox;
    FT_Glyph glyph;
    FT_Get_Glyph( face->glyph, &glyph);
    FT_Glyph_Get_CBox( glyph, FT_GLYPH_BBOX_PIXELS, &acbox ); 
    int x = 0;
    */
}

TextMap::~TextMap()
{

}

bool TextMap::initLibrary()
{
    if(FT_Init_FreeType(&library))
    {
        std::cout << "Library failed to initialize\n";
        return false;
    }
    
    return true;
}

bool TextMap::initFace(const char* ttf_font_file, unsigned int face_index)
{
    if(library == 0)
    {
        std::cout << "The library being used was null, resulting in being unable to initialize the face object.\n";
        return false;
    }

    if(FT_New_Face(library, ttf_font_file, face_index, &face))
    {
        std::cout << "Unable to open/read font file (" << ttf_font_file << ")\n";
        //std::cout << "Unable to open/read font file\n";
        return false;
    } 

    return true;
}

void TextMap::tick(rgb_matrix::Canvas &canvas)
{
    //pixel_map->tick(canvas);
    /*
    std::vector<PixelMap>::iterator it;

    for( it = pixel_maps.begin(); it != pixel_maps.end(); it++ )
    {
        it->tick(canvas);
    }
    */   
}
