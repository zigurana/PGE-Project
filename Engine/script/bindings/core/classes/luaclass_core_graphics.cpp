#include <QFileInfo>
#include "luaclass_core_graphics.h"
#include <common_features/graphics_funcs.h>
#include <data_configs/config_manager.h>
#include <scenes/scene.h>

//#include <luabind/adopt_policy.hpp>
//#include <luabind/out_value_policy.hpp>

//! Cached textures
QHash<QString, PGE_Texture> Binding_Core_Graphics::textureCache;

//! Current path to level/world custom directory (where look for image files by default)
QString Binding_Core_Graphics::rootPath;

void Binding_Core_Graphics::setRootPath(QString path)
{
    rootPath = path;
    if(!rootPath.endsWith('/'))
        rootPath.append('/');
}

PGE_Texture *Binding_Core_Graphics::loadImage(std::string filePath)
{
    PGE_Texture texture;
    QString fPath = QString::fromStdString(filePath);

    if(!QFileInfo(fPath).isAbsolute())
    {
        fPath = rootPath + fPath;
    }

    if(textureCache.contains(fPath))
    {
        //Don't load same texture!
        return &textureCache[fPath];
    }
    GlRenderer::loadTextureP(texture, fPath);
    textureCache[fPath] = texture;
    return &textureCache[fPath];
}

luabind::adl::object Binding_Core_Graphics::getPixelData(const PGE_Texture *img, int &width, int &height, lua_State *L)
{
    if ( !img || !img->inited || (img->texture==0) ) {
            luaL_error(L, "Internal error: Failed to find image resource!");
            return luabind::object();
    }
    luabind::object returnTable = luabind::newtable(L);
    int bufferSize = GlRenderer::getPixelDataSize(img);
    if(bufferSize==0)
    {
        luaL_error(L, "Internal error: Invalid image resource!");
        return luabind::object();
    }
    unsigned char* buffer = new unsigned char[bufferSize];
    GlRenderer::getPixelData(img, buffer);
    int i = 1;
    for(i=1; i <= bufferSize; ++i)
    {
        returnTable[i] = buffer[i];
    }
    width  = img->w;
    height = img->h;
    return returnTable;
}

void Binding_Core_Graphics::clearCache()
{
    for(QHash<QString, PGE_Texture>::iterator it = textureCache.begin(); it != textureCache.end(); it++)
    {
        PGE_Texture &texture = *it;
        GlRenderer::deleteTexture(texture);
    }
    textureCache.clear();
}

void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, lua_State *L)
{
    drawImage(texture, xPos, yPos, 1.0f, L);
}


void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float opacity, lua_State *L)
{
    if(!texture) return;
    LuaGlobal::getEngine(L)->getBaseScene()->addRenderFunction([=]()
    {
        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture((PGE_Texture*)texture, xPos, yPos);
    });
}


void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, lua_State *L)
{
    drawImage(texture, xPos, yPos, sourceX, sourceY, width, height, 1.0f, L);
}


void Binding_Core_Graphics::drawImage(const PGE_Texture *texture, float xPos, float yPos, float sourceX, float sourceY, float width, float height, float opacity, lua_State *L)
{
    if(!texture) return;
    LuaGlobal::getEngine(L)->getBaseScene()->addRenderFunction([=]()
    {
        float x = xPos, y = yPos,
             sX = sourceX, sY = sourceY,
              w = width, h = height,
            txW = (float)texture->w, txH = (float)texture->h;

        if(w <= 0.0)
            return;

        if(h <= 0.0)
            return;

        if(sX < 0.0f)
        {
            if(sX < -txW)
                return;//Don't draw out of view!
            x = x-sX;
            sX = 0.0f;
        }
        else if(sX > txW)
            return; //Don't draw if out of view!
        if(sY < 0.0f)
        {
            if(sY < -txH)
                return;//Don't draw out of view!
            y = y-sY;
            sY = 0.0f;
        }
        else if(sourceY > txH)
            return;//Don't draw if out of view!

        if(w > (txW - sX))
        {
            w = (txW - sX);
        }

        if(h > (txH - sY))
        {
            h = (txH - sY);
        }

        float left   = (sX/txW);
        float top    = (sY/txH);
        float right  = ((sX+w)/txW);
        float bottom = ((sY+h)/txH);

        GlRenderer::setTextureColor(1.0f, 1.0f, 1.0f, opacity);
        GlRenderer::renderTexture((PGE_Texture*)texture, x, y, w, h, top, bottom, left, right);
    });
}

float Binding_Core_Graphics::alignToCenter(float x, float width)
{
    return (float)GlRenderer::alignToCenter((int)x, (int)width);
}

luabind::scope Binding_Core_Graphics::PGETexture_bindToLua()
{
    using namespace luabind;
    return
        class_<PGE_Texture>("LuaImageResource")
            //Properties
            .def_readonly("w", &PGE_Texture::w)
            .def_readonly("h", &PGE_Texture::h)
            .def_readonly("texture", &PGE_Texture::texture);
}

luabind::scope Binding_Core_Graphics::bindToLua()
{
    using namespace luabind;
    return
        namespace_("Graphics")[
            def("loadImage", &loadImage),
            //def("getPixelData", &getPixelData, pure_out_value(_2) + pure_out_value(_3)),
            def("drawImage", (void(*)(const PGE_Texture*, float, float, lua_State*))&drawImage),
            def("drawImage", (void(*)(const PGE_Texture*, float, float, float, lua_State*))&drawImage),
            def("drawImage", (void(*)(const PGE_Texture*, float, float, float, float, float, float, lua_State*))&drawImage),
            def("drawImage", (void(*)(const PGE_Texture*, float, float, float, float, float, float, float, lua_State*))&drawImage),
            def("alignToHCenter", &alignToCenter)
        ];
}
