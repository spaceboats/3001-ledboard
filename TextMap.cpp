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

TextMap::TextMap(const char* ttf_font_file, const char* message, const color_t color, const scroll_args_t scroll_args_in)
{
    initLibrary();
    initFace(ttf_font_file);

    // doesn't seem to like anything below 8 pixels.
    FT_Set_Pixel_Sizes(face, 0, 16);
    unsigned int width;
    unsigned int height;
    getSize(width, height, message);
    color_t *rgb = new color_t[width*height];

    // set everything to black
    memset(rgb, 0, sizeof(color_t) * width * height);
    getMsgRgbMat(rgb, color, width, height, message);
    pixel_map = new PixelMap(width, height, rgb, scroll_args_in);
    delete[] rgb;

    // TODO: either delete the face and library here, or do it in the destructor.
    /*
    FT_Load_Char(face, message[0], FT_LOAD_RENDER);
    unsigned int width = face->glyph->bitmap.width;
    unsigned int height = face->glyph->bitmap.rows;
    color_t *rgb = new color_t[width*height];

    // set everything to black
    memset(rgb, 0, sizeof(color_t) * width * height); 

    for(unsigned int i = 0; i < height; i++)
    {
        for(unsigned int j = 0; j < width; j++)
        {
            if(face->glyph->bitmap.buffer[i*width + j] == 255)
            {
                rgb[i*width + j][0] = color[0];
                rgb[i*width + j][1] = color[1];
                rgb[i*width + j][2] = color[2];
            }
            else
            {
                //std::cout << " ";
            }
        }
        //std::cout << "\n";
    }

    pixel_map = new PixelMap(width, height, rgb, scroll_args_in);
    delete[] rgb;
    */

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

bool TextMap::getMsgRgbMat(color_t* rgb, const color_t color, unsigned int width, unsigned int height, const char* message)
{
    if(library == 0)
    {
        std::cout << "Cannot fill the rgb matrix because the library was null.\n";
        return false;
    }
    else if(face == 0)
    {
        std::cout << "Cannot fill the rgb matrix because the face was null.\n";
        return false;
    }

    unsigned int penX = 0;
    // unsigned int penY = 0;
    for(unsigned int i = 0; i < strlen(message); i++)
    {
        // TODO: Find out what FT_LOAD_RENDER does.
        FT_Load_Char(face, message[i], FT_LOAD_RENDER);
        unsigned int bitmapWidth = face->glyph->bitmap.width;
        unsigned int bitmapHeight = face->glyph->bitmap.rows;
        // if(bitmapHeight > height || penX+bitmapWidth > width)
        if((bitmapHeight-1) * bitmapWidth + (bitmapWidth-1) + penX >= height*width)
        {
            std::cout << "Either the height or width went oustide the bounds of the array.\n";
            return false;
            // TODO: Maybe set the rgb back to black.
        }

        for(unsigned int h = 0; h < bitmapHeight; h++)
        {
            for(unsigned int w = 0; w < bitmapWidth; w++)
            {
                if(face->glyph->bitmap.buffer[h*bitmapWidth + w] == 255)
                {
                    rgb[(height - bitmapHeight + h)*width + w + penX][0] = color[0];
                    rgb[(height - bitmapHeight + h)*width + w + penX][1] = color[0];
                    rgb[(height - bitmapHeight + h)*width + w + penX][1] = color[0];
                }
            }
        }

        penX += bitmapWidth + 1;
    }

    return true;
}

// SUMMARY: Gets the overall width and height needed to render this message to the board.
// NOTE: Sets height and width to 0 and returns when library or face are null.
void TextMap::getSize(unsigned int &width, unsigned int &height, const char* message)
{
    width = 0;
    height = 0;
    if(library == 0)
    {
        std::cout << "Cannot get the width and height because the library was null.\n";
        return;
    }
    else if(face == 0)
    {
        std::cout << "Cannot get the width and height because the face was null.\n";
        return;
    }

    for(unsigned int i = 0; i < strlen(message); i++)
    {
        FT_Load_Char(face, message[i], FT_LOAD_RENDER);

        // plus one for the space between chars
        width += face->glyph->bitmap.width + 1;
        if(face->glyph->bitmap.rows > height)
            height = face->glyph->bitmap.rows;
    }
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
    if(pixel_map != 0)
    {
        pixel_map->tick(canvas);
    }
    /*
    std::vector<PixelMap>::iterator it;

    for( it = pixel_maps.begin(); it != pixel_maps.end(); it++ )
    {
        it->tick(canvas);
    }
    */   
}
