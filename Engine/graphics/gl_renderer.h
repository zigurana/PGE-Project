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

#ifndef GL_RENDERER_H
#define GL_RENDERER_H

#include <QString>
#include "render/render_base.h"
#include <common_features/rectf.h>

extern Render_Base* g_renderer;

struct SDL_Thread;
class  QImage;

class GlRenderer
{
public:
    enum RenderEngineType {
        RENDER_INVALID=-1,
        RENDER_AUTO=0,
        RENDER_OPENGL_2_1,
        RENDER_OPENGL_3_1,
        RENDER_SW_SDL
    };
    static RenderEngineType setRenderer(RenderEngineType rtype=RENDER_AUTO);

    static void setup_OpenGL21();
    static void setup_OpenGL31();
    static void setup_SW_SDL();

    static unsigned int SDL_InitFlags();

    static bool init();
    static bool uninit();

    static QString ScreenshotPath;
    static void makeShot();
    static int  makeShot_action(void *_pixels);
    static bool ready();
    static void flush();
    static void repaint();
    /*!
     * \brief Changed color to clear screen
     * \param r level of red (from 0.0 to 1.0)
     * \param g level of green (from 0.0 to 1.0)
     * \param b level of blue (from 0.0 to 1.0)
     * \param a level of alpha (from 0.0 to 1.0)
     */
    static void setClearColor(float r, float g, float b, float a);
    /*!
     * \brief Clear screen with pre-defined clear color
     */
    static void clearScreen();

    static void renderTexture(PGE_Texture *texture, float x, float y); //!<Render texture as-is
    static void renderTexture(PGE_Texture *texture, float x, float y, float w, float h, float ani_top=0.0f, float ani_bottom=1.0f, float ani_left=0.0f, float ani_right=1.0f);//!<Render matrix animation fragment
    static void renderRect(float x, float y, float w, float h, GLfloat red=1.f, GLfloat green=1.f, GLfloat blue=1.f, GLfloat alpha=1.f, bool filled=true);
    static void renderRectBR(float _left, float _top, float _right, float _bottom, GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
    static void renderTextureCur(float x, float y, float w, float h, float ani_top=0.0f, float ani_bottom=1.0f, float ani_left=0.0f, float ani_right=1.0f);//!< Draw currently binded texture

    static void BindTexture(PGE_Texture *texture);
    static void setTextureColor(float Red, float Green, float Blue, float Alpha=1.0f);
    static void UnBindTexture();

    static PGE_Point  MapToScr(PGE_Point point);
    static PGE_Point  MapToScr(int x, int y);
    static int  alignToCenter(int x, int w);
    static void setViewport(int x, int y, int w, int h);
    static void resetViewport();
    static void setViewportSize(int w, int h);
    static void setWindowSize(int w, int h);

    static PGE_Texture loadTexture(QString path, QString maskPath="");
    static void loadTextureP(PGE_Texture &target, QString path, QString maskPath="");
    static GLuint QImage2Texture(QImage *img, PGE_Texture &tex);
    static void deleteTexture(PGE_Texture &tx);

    static int  getPixelDataSize(const PGE_Texture *tx);
    static void getPixelData(const PGE_Texture *tx, unsigned char* pixelData);

private:

    //Virtual resolution of renderable zone
    static int window_w;
    static int window_h;

    //Scale of virtual and window resolutuins
    static float scale_x;
    static float scale_y;
    //Side offsets to keep ratio
    static float offset_x;
    static float offset_y;

    static SDL_Thread *thread;
    static bool _isReady;
};

#endif // GL_RENDERER_H

