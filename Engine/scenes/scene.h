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

#ifndef SCENE_H
#define SCENE_H

#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_events.h>
#include <common_features/fader.h>
#include <common_features/rectf.h>
#include <script/lua_engine.h>
#include <scenes/_base/gfx_effect.h>
#include <data_configs/spawn_effect_def.h>

#include <functional>
#include <QList>

#include <chrono>

struct LoopTiming
{
    inline LoopTiming()
    {
        start_render    = 0;
        stop_render     = 0;
        doUpdate_render = 0.0f;
        start_physics   = 0;
        stop_physics    = 0;
        start_events    = 0;
        stop_events     = 0;
        start_common    = 0;
    }

    inline Uint32 passedCommonTime()
    {
        return SDL_GetTicks() - start_common;
    }

    Uint32 start_render;
    Uint32 stop_render;
    float doUpdate_render;

    Uint32 start_physics;
    Uint32 stop_physics;

    Uint32 start_events;
    Uint32 stop_events;

    Uint32 start_common;
};

class Scene
{
    void construct();
public:
    void updateTickValue();

    enum TypeOfScene
    {
        _Unknown=0,
        Loading,
        Title,
        Level,
        World,
        Credits,
        GameOver
    };

    Scene();
    Scene(TypeOfScene _type);
    virtual ~Scene();
    virtual void onKeyInput(int key);             //!< Triggering when pressed game specific key
    virtual void onKeyboardPressed(SDL_Scancode scancode); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardPressedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardReleased(SDL_Scancode scancode); //!< Triggering when pressed any key on keyboard
    virtual void onKeyboardReleasedSDL(SDL_Keycode sdl_key, Uint16 modifier); //!< Triggering when pressed any key on keyboard
    virtual void onMouseMoved(SDL_MouseMotionEvent &mmevent);
    virtual void onMousePressed(SDL_MouseButtonEvent &mbevent);
    virtual void onMouseReleased(SDL_MouseButtonEvent &mbevent);
    virtual void onMouseWheel(SDL_MouseWheelEvent &wheelevent);
    virtual void processEvents();
    virtual LuaEngine* getLuaEngine();

    virtual void update();
    virtual void updateLua();
    virtual void render();
    virtual void renderMouse();
    virtual int exec(); //scene's loop
    TypeOfScene type();

    void addRenderFunction(const std::function<void()>& renderFunc);
    void clearRenderFunctions();

    virtual bool isVizibleOnScreen(PGE_RectF &rect);
    virtual bool isVizibleOnScreen(double x, double y, double w, double h);

    bool isExiting();
    bool doShutDown();
    /**************Fader**************/
    bool isOpacityFadding();
    void setFade(int speed, float target, float step);
    PGE_Fader fader;
    /**************Fader**************/

    /*  Effects engine   */
    typedef QList<Scene_Effect>    SceneEffectsArray;
    SceneEffectsArray  WorkingEffects;
    ///
    /// \brief launchStaticEffect
    /// Starts static effect by ID at position X,Y relative to left-top corner of effect picture
    /// \param effectID ID of effect from lvl_effects.ini
    /// \param startX X position relative to left side of effect picture
    /// \param startY Y position relative to top side of effect picture
    /// \param animationLoops Number of loops before effect will be finished. 0 - unlimited while time is not exited
    /// \param delay max time of effect working. 0 - unlimited while loops are not exited or while effect still vizible of screen.
    /// \param velocityX Horizontal motion speed (pixels per 1/65 second [independent to framerate])
    /// \param velocityY Vertical motion speed (pixels per 1/65 second [independent to framerate])
    /// \param gravity Y-gravitation will cause falling of effect picture
    /// \param phys Additional physical settings
    ///
    void  launchEffect(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity, int direction=0, Scene_Effect_Phys phys=Scene_Effect_Phys());

    ///
    /// \brief launchStaticEffectC
    /// Starts static effect by ID at position X,Y relative to center of effect picture
    /// \param effectID ID of effect from lvl_effects.ini
    /// \param startX X position relative to center of effect picture
    /// \param startY Y position relative to center of effect picture
    /// \param animationLoops Number of loops before effect will be finished. 0 - unlimited while time is not exited
    /// \param delay max time of effect working. 0 - unlimited while loops are not exited or while effect still vizible of screen.
    /// \param velocityX Horizontal motion speed (pixels per 1/65 second [independent to framerate])
    /// \param velocityY Vertical motion speed (pixels per 1/65 second [independent to framerate])
    /// \param gravity Y-gravitation will cause falling of effect picture
    /// \param phys Additional physical settings
    ///
    void launchStaticEffectC(long effectID, float startX, float startY, int animationLoops, int delay, float velocityX, float velocityY, float gravity, int direction=0, Scene_Effect_Phys phys=Scene_Effect_Phys());

    void launchEffect(SpawnEffectDef effect_def, bool centered=false);

    void processEffects(float ticks);
    /*  Effects engine   */

    QString errorString();

protected:
    bool        running;
    bool        _doShutDown;
    bool        doExit;
    int         uTick;
    float       uTickf;

    /************waiting timer************/
    typedef std::chrono::high_resolution_clock StClock;
    typedef std::chrono::high_resolution_clock::time_point StTimePt;
    void wait(float ms);
    /************waiting timer************/
    QString _errorString;
private:
    TypeOfScene sceneType;
    float dif;

    QVector<std::function<void()> > renderFunctions;
};

#endif // SCENE_H
