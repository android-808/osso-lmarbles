/**
    @file ft.c

    OSSO Lmarbles FreeType SDL drawing header.

    Copyright (c) 2004, 2005 Nokia Corporation.
	
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
    02111-1307 USA
*/

#ifndef FT_H
#define FT_H

#include <SDL/SDL.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#define FT_ALIGN_X_L 0x0
#define FT_ALIGN_X_R 0x10
#define FT_ALIGN_X_C 0x20
#define FT_ALIGN_X_MASK 0xf0
#define FT_ALIGN_Y_T 0x1
#define FT_ALIGN_Y_B 0x2
#define FT_ALIGN_Y_C 0x3
#define FT_ALIGN_Y_BL 0x0
#define FT_ALIGN_Y_MASK 0xf

/**
 Calculates the width of the longest line of given UTF-8 string in pixels.
 @param face Font face for drawing.
 @param size Font size.
 @param string The string to be drawn.
 @return Width in pixels.
*/
int FT_TxtWdth(FT_Face face, int size, char *string);

/**
 Calculates how many rows the UTF-8 string takes.
 @param face Font face for drawing.
 @param size Font size.
 @param string The string to be drawn.
 @param width Width of output in pixels.
 @return Lines the string takes.
*/
int FT_CntLns(FT_Face face, int size, char *string, int width);

/**
 Calculates the next place for a line break.
 @param face Font face for drawing.
 @param size Font size.
 @param string The string to be drawn.
 @param width Width of output in pixels.
 @return Position where the next line starts.
*/
char *FT_FndLnBrk(FT_Face face, int size, char *string, int width);

/**
 Draws UTF-8 string onto SDL surface with font specified in face.
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxt(FT_Face face, SDL_Surface * dest, int x, int y,
              int size, char *string, unsigned char r,
              unsigned char g, unsigned char b);

/**
 Draws UTF-8 string onto SDL surface with font specified in face.
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxt(FT_Face face, SDL_Surface * dest, int x, int y,
              int size, char *string, unsigned char r,
              unsigned char g, unsigned char b);
/**
 Draws UTF-8 string onto SDL surface with font specified in face wrapping lines.
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param w Width of text.
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxtWrp(FT_Face face, SDL_Surface * dest, int x, int y, int w,
                 int size, char *string, unsigned char r,
                 unsigned char g, unsigned char b);


/**
 Calculates the (maximum) height of one character in pixels.
 @param face The font used for drawing.
 @param size The size of the font.
 @return The height of a single row.
*/
int FT_FnHght(FT_Face face, int size);

/**
 Calculates the height of one line in pixels.
 @param face The font used for drawing.
 @param size The size of the font.
 @return The height of a single row.
*/
int FT_LnHght(FT_Face face, int size);

/**
 Calculates the highest point of font at given size.
 @param face The font used for drawing.
 @param size The size of the font.
 @return The height of the highest point.
*/
int FT_Ascn(FT_Face face, int size);

/**
 Draws UTF-8 string onto SDL surface with font specified in face.
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param align X/Y alignment
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxtAlgn(FT_Face face, SDL_Surface * dest, int x, int y, int align,
                  int size, char *string, unsigned char r,
                  unsigned char g, unsigned char b);

/**
 Draws UTF-8 string onto SDL surface with font specified in face with wrapping.
 TODO: X alignment
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param w Width of text in pixels.
 @param align X/Y alignment
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxtWrpAlgn(FT_Face face, SDL_Surface * dest, int x, int y, int w,
                     int align, int size, char *string, unsigned char r,
                     unsigned char g, unsigned char b);

/**
 Draws UTF-8 string onto SDL surface with font specified in face with alpha.
 @param face Font to use for string drawing.
 @param dest SDL Surface to draw on.
 @param x X coordinate where to start drawing.
 @param y Y coordinate where to start drawing.
 @param size Fontsize to be used.
 @param r RGB Red value.
 @param g RGB Green value.
 @param b RGB Blue value.
 @param a Alhpa value.
 @return -1 on error, 0 on success.
*/
int FT_DrwTxtA(FT_Face face, SDL_Surface * dest, int x, int y,
               int size, char *string, unsigned char r,
               unsigned char g, unsigned char b, unsigned char a);
#endif /* FT_H */
