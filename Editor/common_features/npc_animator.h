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

#ifndef NPC_ANIMATOR_H
#define NPC_ANIMATOR_H

#include <QObject>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QList>
#include <QString>

#include <data_configs/obj_npc.h>

class AdvNpcAnimator : public QObject
{
    Q_OBJECT
public:
    AdvNpcAnimator();
    AdvNpcAnimator(QPixmap &sprite, obj_npc &config);
    ~AdvNpcAnimator();

    void buildAnimator(QPixmap &sprite, obj_npc &config);

    QPixmap image(int dir, int frame=-1);
    QPixmap &wholeImage();

    QRect  &frameRect(int dir);

    void setFrameL(int y);
    void setFrameR(int y);

    void start();
    void stop();

    int speed;
signals:
    void onFrame();

private slots:
    void nextFrame();

private:
    obj_npc setup;

    QPixmap *mainImage;

    //QVector<QPixmap> frames; //Whole image
    //void createAnimationFrames();

    bool animated;

    int frameSpeed;
    int frameStyle;

    bool aniBiDirect;

    int curDirect;
    int frameStep;

    bool customAnimate;
    int customAniAlg; //custom animation algorythm 0 - forward, 1 - frameJump
    int custom_frameFL;//first left
    int custom_frameEL;//end left / jump step
    int custom_frameFR;//first right
    int custom_frameER;//enf right / jump step

    bool frameSequance;

    QList<int> frames_listL;     //Current frame srquence
    QList<int> frames_listR;     //Current frame srquence

    int frameCurrent;
    QTimer * timer;

    int framesCountOneSide;
    int framesCountTotal;
    int frameHeight; // size of one frame
    int frameWidth; // sprite width
    int spriteHeight; //sprite height

    //Animation alhorithm
    bool aniDirectL;
    bool aniDirectR;

    int CurrentFrameL;
    int CurrentFrameR;

    int frameCurrentL;
    int frameCurrentR;

    QRect   frame_rect_L;
    QRect   frame_rect_R;

    int frameFirstL;
    int frameLastL;

    int frameFirstR;
    int frameLastR;

};

#endif // NPC_ANIMATOR_H
