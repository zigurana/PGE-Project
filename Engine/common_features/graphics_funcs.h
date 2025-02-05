/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2016 Vitaly Novichkov <admin@wohlnet.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QPixmap>
#include <QPainter>
#include <QImage>

#include "size.h"

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h> // SDL 2 Library
#include <SDL2/SDL_opengl.h>

#ifndef GRAPHICS_FUNCS_H
#define GRAPHICS_FUNCS_H

struct PGE_Pix {
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 a;
};

/*!
 * \brief Helpful graphical functions which are doing various work: I/O, Front+Mask blending, etc.
 */
struct FIBITMAP;
class GraphicsHelps
{
public:
    /*!
     * \brief Initializes FreeImage
     */
    static void  initFreeImage();
    /*!
     * \brief DeInitializes FreeImage
     */
    static void  closeFreeImage();
    /*!
     * \brief Loads image from a disk
     * \param file full or relative path to the file
     * \param convertTo32bit need to convert image into 32bit RGBA
     * \return FreeImage descriptor to loaded image
     */
    static FIBITMAP *loadImage(QString file, bool convertTo32bit=true);
    /*!
     * \brief Loads image from application resources
     * \param file in-resource path to the file
     * \return FreeImage descriptor to loaded image
     */
    static FIBITMAP *loadImageRC(QString file);

    /*!
     * \brief Converts FreeImage into SDL_Surface
     * \param img Source FreeImage descriptor to loaded image
     * \return SDL_Surface pointer
     */
    static SDL_Surface *fi2sdl(FIBITMAP *img);

    /*!
     * \brief Closes image and frees memory
     * \param FreeImage descriptor to loaded image
     */
    static void closeImage(FIBITMAP *img);

    /*!
     * \brief Merges mask and foreground image with bit blitting algorithm
     * 1) draw mask over grey-filled image with bitwise AND per each pixel (white pixels are will not change background)
     * 2) draw foreground over same image with bitwise OR per each pixel (black pixels are will not change background)
     * 3) Calculate alpha-channel level dependent to black-white difference on the mask and on the foreground:
     *    white on the mask is a full transparency, black - is a solid pixels area.
     * \param image
     * \param pathToMask
     */
    static void mergeWithMask(FIBITMAP *image, QString pathToMask);
    /*!
     * \brief Gets metric from image file
     * \param [__in] imageFile Path to image file
     * \param [__out] imgSize Pointer to PGE_Size value
     */
    static bool getImageMetrics(QString imageFile, PGE_Size *imgSize);
    static void getMakedImageInfo(QString rootDir, QString in_imgName, QString &out_maskName, QString &out_errStr, PGE_Size* imgSize=0);
    static QImage convertToGLFormat(const QImage &img);//Taken from QGLWidget
};

#endif // GRAPHICS_FUNCS_H
