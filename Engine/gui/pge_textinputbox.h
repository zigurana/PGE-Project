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

#ifndef PGE_TextInputBox_H
#define PGE_TextInputBox_H

#include "pge_boxbase.h"
#include "../scenes/scene.h"

#include <graphics/gl_renderer.h>
#include <graphics/gl_color.h>
#include <common_features/rect.h>
#include <common_features/point.h>
#include <common_features/pointf.h>
#include <common_features/size.h>
#include <common_features/sizef.h>

#include <controls/control_keys.h>

#include <QColor>
#include <QString>

class PGE_TextInputBox : public PGE_BoxBase
{
public:
    PGE_TextInputBox();
    PGE_TextInputBox(Scene * _parentScene=NULL, QString msg="Message box is works!",
               msgType _type=msg_info, PGE_Point boxCenterPos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    PGE_TextInputBox(const PGE_TextInputBox &mb);
    ~PGE_TextInputBox();

    void setBoxSize(float _Width, float _Height, float _padding);
    void update(float ticks);
    void render();
    void restart();
    bool isRunning();
    void exec();

    void processLoader(float ticks);
    void processBox(float tickTime);
    void processUnLoader(float ticks);

    void setInputText(QString text);
    QString inputText();

    static void info(QString msg);
    //static void info(std::string msg);
    static void warn(QString msg);
    //static void warn(std::string msg);
    static void error(QString msg);
    //static void error(std::string msg);
    static void fatal(QString msg);
    //static void fatal(std::string msg);

private:
    void construct(QString msg="Message box is works!",
                    msgType _type=msg_info, PGE_Point pos=PGE_Point(-1,-1), float _padding=-1, QString texture="");
    int     _page;
    bool    running;
    int     fontID;
    GlColor fontRgba;

    QString _inputText_src;
    QString _inputText;
    QString _inputText_printable;
    Sint32 cursor;
    Sint32 selection_len;
    int _text_input_h_offset;

    bool  blink_shown;
    float blink_timeout;

    controller_keys keys;

    msgType type;
    PGE_Rect _sizeRect;
    QString message;
    float width;
    float height;
    float padding;
    QColor bg_color;
    void updateControllers();
};

#endif // PGE_TextInputBox_H
