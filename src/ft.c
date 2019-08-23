/**
    @file ft.c

    OSSO Lmarbles FreeType SDL drawing routines.

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

#include <libintl.h>
#include <locale.h>
#include <stdio.h>
#include <unistd.h>
#include <glib.h>
#include "ft.h"

#define BD_X1 0
#define BD_Y1 1
#define BD_X2 2
#define BD_Y2 3

/**
 Calculates the width of the given string in pixels.
 @param face Font face for drawing.
 @param size Font size.
 @param string The string to be drawn.
 @return Width in pixels.
*/
int
FT_TxtWdth(FT_Face face, int size, char *string)
{
    if (!face)
        return -1;
    int len = 0;
    int maxlen = 0;
    FT_ULong c = 0;

    if (FT_Set_Pixel_Sizes(face, 0, size))
    {
        return -1;
    }

    for (; *string; string = g_utf8_next_char(string))
    {
        c = g_utf8_get_char(string);
        if (*string == '\n')
        {
            if (len > maxlen)
                maxlen = len;
            len = 0;
        }
        else if (FT_Load_Char(face, c, 0))
        {
        }
        else
        {
            len += (face->glyph->advance.x >> 6);
        }
    }
    if (len > maxlen)
        return len;
    return maxlen;
}

int
FT_CntLns(FT_Face face, int size, char *string, int width)
{
    if (!face)
        return -1;
    int lines = 1;

    while ((string = FT_FndLnBrk(face, size, string, width)) && *string)
        lines++;

    return lines;
}

/**
 Draws string onto SDL surface with font specified in face.
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
int
FT_DrwTxt(FT_Face face, SDL_Surface * dest, int x, int y,
          int size, char *string, unsigned char r,
          unsigned char g, unsigned char b)
{
    if (!face)
        return -1;
    unsigned char *sptr8 = NULL, *tptr8 = NULL;
    unsigned short *sptr16 = NULL, *tptr16 = NULL;
    int rasterpos = 0, yp, xp;
    int i, j;
    int tr, tg, tb, ta;
    FT_ULong c = 0;


    if (FT_Set_Pixel_Sizes(face, 0, size))
    {
        return -1;
    }


    SDL_LockSurface(dest);
    rasterpos = x;

    if (dest->format->BitsPerPixel == 16)
    {
        sptr16 = dest->pixels;
    }
    else if (dest->format->BitsPerPixel == 32)
    {
        sptr8 = dest->pixels;
    }
    else
    {
        SDL_UnlockSurface(dest);
        return -1;
    }

    for (; *string; string = g_utf8_next_char(string))
    {
        c = g_utf8_get_char(string);
        if (FT_Load_Char(face, c, 0))
        {
            SDL_UnlockSurface(dest);
            return -1;
        }

        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
        {
            SDL_UnlockSurface(dest);
            return -1;
        }

        yp = y - face->glyph->bitmap_top;
        xp = rasterpos + face->glyph->bitmap_left;

        for (i = 0; i < face->glyph->bitmap.rows; i++)
        {
            for (j = 0; j < face->glyph->bitmap.width; j++)
            {
                if (dest->format->BitsPerPixel == 16)
                {
                    if (yp + i < dest->h && xp + j < dest->w
                        && face->glyph->bitmap.buffer[i *
                                                      (face->glyph->bitmap.
                                                       width) + j])
                    {
                        tptr16 =
                            sptr16 + (((yp + i) * dest->pitch) >> 1) + (xp +
                                                                        j);
                        ta = face->glyph->bitmap.buffer[i *
                                                        (face->glyph->bitmap.
                                                         width) + j];
                        tr = (((*tptr16) >> 11) & 31) << 3;
                        tg = (((*tptr16) >> 5) & 63) << 2;
                        tb = ((*tptr16) & 31) << 3;
                        *tptr16 =
                            ((((((ta) * (r - tr)) >> 8) +
                               tr) << 8) & 63488) | ((((((ta) * (g - tg)) >>
                                                        8) +
                                                       tg) << 3) & 2016) |
                            ((((((ta) * (b - tb)) >> 8) + tb) >> 3) & 31);
            /**tptr16 = 31;*/
            /**(sptr16 + ((yp + i)*dest->pitch)) = 63488;*/
                    }
                }
                else if (dest->format->BitsPerPixel == 32)
                {
                    if (yp + i < dest->h && xp + j < dest->w
                        && face->glyph->bitmap.buffer[i *
                                                      (face->glyph->bitmap.
                                                       width) + j])
                    {
                        tptr8 =
                            sptr8 + ((yp + i) * dest->pitch) +
                            ((xp + j) << 2);
                        ta = face->glyph->bitmap.buffer[i *
                                                        (face->glyph->bitmap.
                                                         width) + j];
                        *tptr8 += (((ta) * (b - *tptr8)) >> 8);
                        tptr8++;
                        *tptr8 += (((ta) * (g - *tptr8)) >> 8);
                        tptr8++;
                        *tptr8 += (((ta) * (r - *tptr8)) >> 8);
                    }
                }
            }
        }

        rasterpos += (face->glyph->advance.x >> 6);
        if (rasterpos > dest->w)
        {
            break;
        }
    }

    SDL_UnlockSurface(dest);

    return 0;
}

char *
FT_FndLnBrk(FT_Face face, int size, char *string, int width)
{
    if (!face)
        return NULL;
    char *lastspace = string;
    char *lastlb = string;
    int linelen = 0;
    FT_ULong c = 0;

    if (FT_Set_Pixel_Sizes(face, 0, size))
    {
        return NULL;
    }

    for (; *string; string = g_utf8_next_char(string))
    {
        if (*string == ' ')
            lastspace = string;
        else if (*string == '\n')
        {
            lastspace = string;
            linelen = width + 1;
        }
        c = g_utf8_get_char(string);
        if (FT_Load_Char(face, c, 0))
        {
        }
        else
        {
            linelen += (face->glyph->advance.x >> 6);
            if (linelen > width)
            {
                if (lastspace > lastlb)
                    string = lastspace + 1;
                break;
            }
        }
    }
    return string;
}

int
FT_DrwTxtWrp(FT_Face face, SDL_Surface * dest, int x, int y, int w,
             int size, char *string, unsigned char r,
             unsigned char g, unsigned char b)
{
    if (!face)
        return -1;
    char *lastspace = string;
    char *lastlb = string;
    char *buffer;
    int lines = 1;
    int linelen = 0;
    int buffersize;
    FT_ULong c = 0;

    if (FT_Set_Pixel_Sizes(face, 0, size))
    {
        return -1;
    }

    buffersize = 255;
    buffer = (char *) malloc(buffersize);

    for (; *string; string = g_utf8_next_char(string))
    {
        c = g_utf8_get_char(string);
        if (*string == ' ')
            lastspace = string;
        else if (*string == '\n')
        {
            lastspace = string;
            linelen = w + 1;
        }
        if (FT_Load_Char(face, c, 0))
        {
        }
        else
        {
            linelen += (face->glyph->advance.x >> 6);
            if (linelen > w)
            {
                if (lastspace > lastlb && string != lastspace)
                    string = lastspace + 1;
                if (string - lastlb + 3 > buffersize)
                {
                    free(buffer);
                    buffer = (char *) malloc(string - lastlb + 3);
                    buffersize = string - lastlb + 3;
                    if (!buffer)
                        return -1;
                }
                strncpy(buffer, lastlb, (int) (string - lastlb));
                buffer[(int) (string - lastlb)] = '\0';
                if (string == lastspace)
                    string++;
                FT_DrwTxt(face, dest, x, y, size, buffer, r, g, b);
                y += (size * face->height) / face->units_per_EM;
                lines++;
                linelen = 0;
                lastlb = string;
            }
        }
    }
    if (string > lastlb)
        FT_DrwTxt(face, dest, x, y, size, lastlb, r, g, b);
    free(buffer);
    return lines;
}

/**
 Calculates the height of one row in pixels.
 @param face The font used for drawing.
 @param size The size of the font.
 @return The height of a single row.
*/
int
FT_FnHght(FT_Face face, int size)
{
    if (!face)
        return 0;
    return (size * (face->ascender - face->descender)) / face->units_per_EM;
}

int
FT_LnHght(FT_Face face, int size)
{
    if (!face)
        return 0;
    return (size * face->height) / face->units_per_EM;
}

int
FT_Ascn(FT_Face face, int size)
{
    if (!face)
        return 0;
    return (size * face->ascender) / face->units_per_EM;
}

int
FT_DrwTxtAlgn(FT_Face face, SDL_Surface * dest, int x, int y, int align,
              int size, char *string, unsigned char r,
              unsigned char g, unsigned char b)
{
    if (!face)
        return -1;
    switch (align & FT_ALIGN_X_MASK)
    {
        case FT_ALIGN_X_R:
            x -= FT_TxtWdth(face, size, string);
            break;
        case FT_ALIGN_X_C:
            x -= FT_TxtWdth(face, size, string) / 2;
            break;
        default:
            break;
    }
    switch (align & FT_ALIGN_Y_MASK)
    {
        case FT_ALIGN_Y_T:
            y += FT_Ascn(face, size);
            break;
        case FT_ALIGN_Y_B:
            y -= FT_FnHght(face, size) - FT_Ascn(face, size);
            break;
        case FT_ALIGN_Y_C:
            y -= FT_FnHght(face, size) / 2 - FT_Ascn(face, size);
        default:
            break;
    }
    return FT_DrwTxt(face, dest, x, y, size, string, r, g, b);
}

int
FT_DrwTxtWrpAlgn(FT_Face face, SDL_Surface * dest, int x, int y, int w,
                 int align, int size, char *string, unsigned char r,
                 unsigned char g, unsigned char b)
{
    switch (align & FT_ALIGN_Y_MASK)
    {
        case FT_ALIGN_Y_T:
            y += FT_Ascn(face, size);
            break;
        case FT_ALIGN_Y_B:
            y -= FT_CntLns(face, size, string, w) * FT_FnHght(face, size) -
                FT_Ascn(face, size);
            break;
        case FT_ALIGN_Y_C:
            y -= FT_CntLns(face, size, string, w) * FT_FnHght(face,
                                                              size) / 2 -
                FT_Ascn(face, size);
        default:
            break;
    }
    char *new;
    switch (align & FT_ALIGN_X_MASK)
    {
        case FT_ALIGN_X_R:
            while ((new = FT_FndLnBrk(face, size, string, w)) && *new)
            {
                gchar *temp = g_strndup(string, (int) (new - string));
                FT_DrwTxtAlgn(face, dest, x + w, y, FT_ALIGN_X_R, size,
                              temp, r, g, b);
                g_free(temp);
                y += FT_LnHght(face, size);
                string = new;
            }
            break;
        case FT_ALIGN_X_C:
            while (string && *string
                   && (new = FT_FndLnBrk(face, size, string, w)))
            {
                gchar *temp =
                    g_strndup(string, (int) (new - string - (*new ? 1 : 0)));
                FT_DrwTxtAlgn(face, dest, x + w / 2, y, FT_ALIGN_X_C, size,
                              temp, r, g, b);
                g_free(temp);
                y += FT_LnHght(face, size);
                string = new;
            }
            break;
        case FT_ALIGN_X_L:
        default:
            return FT_DrwTxtWrp(face, dest, x, y, w, size, string, r, g, b);
            break;
    }
    return 1;
}

/**
 Draws string onto SDL surface with font specified in face.
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
int
FT_DrwTxtA(FT_Face face, SDL_Surface * dest, int x, int y,
           int size, char *string, unsigned char r,
           unsigned char g, unsigned char b, unsigned char a)
{
    if (!face)
        return -1;
    unsigned char *sptr8 = NULL, *tptr8 = NULL;
    unsigned short *sptr16 = NULL, *tptr16 = NULL;
    int rasterpos = 0, yp, xp;
    int i, j;
    int tr, tg, tb, ta;
    FT_ULong c = 0;


    if (FT_Set_Pixel_Sizes(face, 0, size))
    {
        return -1;
    }


    SDL_LockSurface(dest);
    rasterpos = x;

    if (dest->format->BitsPerPixel == 16)
    {
        sptr16 = dest->pixels;
    }
    else if (dest->format->BitsPerPixel == 32)
    {
        sptr8 = dest->pixels;
    }
    else
    {
        SDL_UnlockSurface(dest);
        return -1;
    }

    for (; *string; string = g_utf8_next_char(string))
    {
        c = g_utf8_get_char(string);
        if (FT_Load_Char(face, c, 0))
        {
            SDL_UnlockSurface(dest);
            return -1;
        }

        if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL))
        {
            SDL_UnlockSurface(dest);
            return -1;
        }

        yp = y - face->glyph->bitmap_top;
        xp = rasterpos + face->glyph->bitmap_left;

        for (i = 0; i < face->glyph->bitmap.rows; i++)
        {
            for (j = 0; j < face->glyph->bitmap.width; j++)
            {
                if (dest->format->BitsPerPixel == 16)
                {
                    if (yp + i < dest->h && xp + j < dest->w
                        && face->glyph->bitmap.buffer[i *
                                                      (face->glyph->bitmap.
                                                       width) + j])
                    {
                        tptr16 =
                            sptr16 + (((yp + i) * dest->pitch) >> 1) + (xp +
                                                                        j);
                        ta = (int) (a *
                                    face->glyph->bitmap.buffer[i *
                                                               (face->glyph->
                                                                bitmap.
                                                                width) +
                                                               j]) >> 8;
                        tr = (((*tptr16) >> 11) & 31) << 3;
                        tg = (((*tptr16) >> 5) & 63) << 2;
                        tb = ((*tptr16) & 31) << 3;
                        *tptr16 =
                            ((((((ta) * (r - tr)) >> 8) +
                               tr) << 8) & 63488) | ((((((ta) * (g - tg)) >>
                                                        8) +
                                                       tg) << 3) & 2016) |
                            ((((((ta) * (b - tb)) >> 8) + tb) >> 3) & 31);
            /**tptr16 = 31;*/
            /**(sptr16 + ((yp + i)*dest->pitch)) = 63488;*/
                    }
                }
                else if (dest->format->BitsPerPixel == 32)
                {
                    if (yp + i < dest->h && xp + j < dest->w
                        && face->glyph->bitmap.buffer[i *
                                                      (face->glyph->bitmap.
                                                       width) + j])
                    {
                        tptr8 =
                            sptr8 + ((yp + i) * dest->pitch) +
                            ((xp + j) << 2);
                        ta = (int) (a *
                                    face->glyph->bitmap.buffer[i *
                                                               (face->glyph->
                                                                bitmap.
                                                                width) +
                                                               j]) >> 8;
                        *tptr8 += (((ta) * (b - *tptr8)) >> 8);
                        tptr8++;
                        *tptr8 += (((ta) * (g - *tptr8)) >> 8);
                        tptr8++;
                        *tptr8 += (((ta) * (r - *tptr8)) >> 8);
                    }
                }
            }
        }

        rasterpos += (face->glyph->advance.x >> 6);
        if (rasterpos > dest->w)
        {
            break;
        }
    }

    SDL_UnlockSurface(dest);

    return 0;
}
