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

#ifndef SIMPLE_ANIMATOR_H
#define SIMPLE_ANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QMutex>
#include <QVector>
#include <QMutexLocker>

#include "animation_timer.h"

class SimpleAnimator : public QObject, public TimedAnimation
{
    Q_OBJECT
public:
    SimpleAnimator(QObject *parent=0);
    SimpleAnimator(const SimpleAnimator &a, QObject *parent=0);
    SimpleAnimator &operator=(const SimpleAnimator& a);
    explicit SimpleAnimator(QPixmap &sprite, bool enables=false, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                    bool rev=false, bool bid=false, QObject *parent=0);
    ~SimpleAnimator();
    QPixmap image(int frame=-1);
    QRect &frameRect();
    QRectF frameRectF();
    QPixmap &wholeImage();
    void setSettings(QPixmap &sprite, bool enables=false, int framesq=1, int fspeed=64, int First=0, int Last=-1,
                   bool rev=false, bool bid=false);

    void setFrameSequance(QList<int> sequance);

    void setFrame(int y);

    void start();
    void stop();

    void resetFrame();

    int frameDelay;

    void nextFrame();

private:
    QMutex	mutex;

    QPixmap *mainImage; //Whole image
    //QVector<QPixmap> frames; //Whole image

    bool       frame_sequance_enabled;
    QList<int> frame_sequance;
    int        frame_sequance_cur;

    QRect      frame_rect;

    //void createAnimationFrames();
    int CurrentFrame;

    bool animated;

    bool bidirectional;
    bool reverce;

    QPoint framePos;
    //! Number of pre-defined frames
    int framesCount;
    //! Height of one frame
    int frameHeight;
    //! Width of one frame
    int frameWidth; // sprite width
    //! Height of entire sprite
    int spriteHeight; //sprite height

    //Animation alhorithm
    int frameFirst;
    int frameLast;

};

#endif // SIMPLE_ANIMATOR_H
