/*
 * Platformer Game Engine by Wohlstand, a free platform for game making
 * Copyright (c) 2014-2016 Vitaly Novichkov <admin@wohlnet.ru>
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

#ifndef GRAPHICS_FUNCS_H
#define GRAPHICS_FUNCS_H

struct FIBITMAP;
class GraphicsHelps
{
public:
    static FIBITMAP *loadImage(QString file, bool convertTo32bit);
    static void mergeWithMask(FIBITMAP *image, QString pathToMask);

    static QPixmap mergeToRGBA(QPixmap image, QImage mask);
    static void mergeToRGBA_BitWise(QImage &image, QImage mask);
    static void mergeToRGBA(QPixmap &img, QImage &mask, QString path, QString maskpath);
    static void loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QImage &out_Mask, QString &out_errStr);
    static void loadMaskedImage(QString rootDir, QString in_imgName, QString &out_maskName, QPixmap &out_Img, QString &out_errStr);

    static QImage  fromBMP(QString& file);
    static QImage  fromGIF(QString& file);
    static QPixmap loadPixmap(QString file);
    static QImage  loadQImage(QString file);

    static void loadQImage(QImage &target, QString file, QString maskPath = "");

    /*!
     * \brief Resizes image to requested size with keeping aspect ration, but with making convas of requested size as-is (empty space will be filled with transparency)
     * \param imageInOut Image to scale
     * \param targetSize Image size to process
     */
    static void squareImageR(QPixmap &imageInOut, QSize targetSize);

    /*!
     * \brief Converts number into image where that number has been drawn
     * \param number Target number to draw
     * \return Image with drawn number value
     */
    static QPixmap drawDegitFont(int number);

    static bool toGif(QImage& img, QString& path);
};

#endif // GRAPHICS_FUNCS_H
