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

#include "wld_tilebox.h"

#include <QStack>

WorldNode::WorldNode()
{
    x=0;
    y=0;
    w=ConfigManager::default_grid;
    h=ConfigManager::default_grid;
    r=0.f;
    g=0.f;
    b=1.f;
    Z=0.0;
    type=unknown;
    texture.w=0;
    texture.h=0;
    texture.frame_w=0;
    texture.frame_h=0;
    texture.texture=0;
    animatorID=0;
    animated=false;
    vizible=true;
}

WorldNode::~WorldNode() {}

WorldNode::WorldNode(const WorldNode &xx)
{
    x=xx.x;
    y=xx.y;
    w=xx.w;
    h=xx.h;
    r=xx.r;
    g=xx.g;
    b=xx.b;
    Z=xx.Z;
    type=xx.type;
    texture=xx.texture;
    animatorID=xx.animatorID;
    animated=xx.animated;
    vizible=xx.vizible;
}

bool WorldNode::collidePoint(long rx, long ry)
{
    if(rx<x) return false;
    if(ry<y) return false;
    if(rx>x+w) return false;
    if(ry>y+h) return false;
    return true;
}

bool WorldNode::collideWith(WorldNode *it)
{
    if((it->x+it->w) <= x) return false;
    if((it->y+it->h) <= y) return false;
    if( (x+w) <= it->x) return false;
    if( (y+h) <= it->y) return false;
    return true;
}




WldTileItem::WldTileItem(WorldTiles _data): WorldNode()
{
    data = _data;
    x=data.x;
    y=data.y;
    Z=0.0+(double(data.array_id)*0.0000001);
    type=tile;
}

WldTileItem::WldTileItem(const WldTileItem &x): WorldNode(x)
{
    data = x.data;
    type=tile;
    setup=x.setup;
}

WldTileItem::~WldTileItem()
{}

bool WldTileItem::init()
{
    r=1.f;
    g=1.f;
    b=0.f;
    if(!ConfigManager::wld_tiles.contains(data.id))
        return false;

    long tID=ConfigManager::getTileTexture(data.id);
    if(tID<0) return false;
    texture = ConfigManager::world_textures[tID];

    setup = ConfigManager::wld_tiles[data.id];
    animated   = setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldTileItem::render(float rx, float ry)
{
    AniPos a(0,1);
    if(animated) //Get current animated frame
        a = ConfigManager::Animator_Tiles[animatorID].image();
    GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
}








WldSceneryItem::WldSceneryItem(WorldScenery _data): WorldNode()
{
    data = _data;
    x=data.x;
    y=data.y;
    w = 16;
    h = 16;
    Z=10.0+(double(data.array_id)*0.0000001);
    vizible=true;
    type=scenery;
}

WldSceneryItem::WldSceneryItem(const WldSceneryItem &x): WorldNode(x)
{
    data = x.data;
    vizible=x.vizible;
    type=scenery;
    setup=x.setup;
}

WldSceneryItem::~WldSceneryItem()
{}

bool WldSceneryItem::init()
{
    r=1.f;
    g=1.f;
    b=0.f;
    if(!ConfigManager::wld_scenery.contains(data.id))
        return false;
    long tID= ConfigManager::getSceneryTexture(data.id);
    if(tID<0) return false;
    texture = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_scenery[data.id];

    animated   = setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldSceneryItem::render(float rx, float ry)
{
    AniPos a(0,1);
    if(animated) //Get current animated frame
        a = ConfigManager::Animator_Scenery[animatorID].image();
    GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
}







WldPathItem::WldPathItem(WorldPaths _data): WorldNode()
{
    data = _data;
    x=data.x;
    y=data.y;
    Z=20.0+(double(data.array_id)*0.0000001);
    vizible=true;
    type=path;
}

WldPathItem::WldPathItem(const WldPathItem &x): WorldNode(x)
{
    data = x.data;
    vizible=x.vizible;
    type=path;
    setup=x.setup;
}

WldPathItem::~WldPathItem()
{}

bool WldPathItem::init()
{
    r=1.f;
    g=1.f;
    b=0.f;
    if(!ConfigManager::wld_paths.contains(data.id))
        return false;
    long tID = ConfigManager::getWldPathTexture(data.id);
    if(tID<0) return false;
    texture = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_paths[data.id];

    animated   = setup.animated;
    animatorID = setup.animator_ID;
    w = texture.frame_w;
    h = texture.frame_h;
    return true;
}

void WldPathItem::render(float rx, float ry)
{
    AniPos a(0,1);
    if(animated) //Get current animated frame
        a = ConfigManager::Animator_WldPaths[animatorID].image();
    GlRenderer::renderTexture(&texture, rx, ry, w, h, a.first, a.second);
}










WldLevelItem::WldLevelItem(WorldLevels _data): WorldNode()
{
    data = _data;
    x=data.x;
    y=data.y;
    Z=30.0+(double(data.array_id)*0.0000001);
    vizible=true;
    type=level;
    offset_x=0;
    offset_y=0;
    _path_offset_x=0;
    _path_offset_y=0;
    _path_big_offset_x=0;
    _path_big_offset_y=0;
}

WldLevelItem::WldLevelItem(const WldLevelItem &x): WorldNode(x)
{
    data = x.data;
    vizible=x.vizible;
    type=level;
    setup=x.setup;
    offset_x=x.offset_x;
    offset_y=x.offset_y;
    _path_offset_x=x._path_offset_x;
    _path_offset_y=x._path_offset_y;
    _path_big_offset_x=x._path_big_offset_x;
    _path_big_offset_y=x._path_big_offset_y;
    _path_tex=x._path_tex;
    _path_big_tex=x._path_big_tex;
}

WldLevelItem::~WldLevelItem()
{}

bool WldLevelItem::init()
{
    r=1.f;
    g=1.f;
    b=0.f;
    if(!ConfigManager::wld_levels.contains(data.id))
        return false;
    long tID = ConfigManager::getWldLevelTexture(data.id);
    if(tID<0) return false; texture = ConfigManager::world_textures[tID];
    tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.path);
    if(tID<0) return false; _path_tex = ConfigManager::world_textures[tID];
    tID = ConfigManager::getWldLevelTexture(ConfigManager::marker_wlvl.bigpath);
    if(tID<0) return false; _path_big_tex = ConfigManager::world_textures[tID];
    setup = ConfigManager::wld_levels[data.id];

    animated   = setup.animated;
    animatorID = setup.animator_ID;
    w = ConfigManager::default_grid;
    h = ConfigManager::default_grid;
    offset_x = (ConfigManager::default_grid/2)-(texture.frame_w/2);
    offset_y = ConfigManager::default_grid-texture.frame_h;
    _path_offset_x = (ConfigManager::default_grid/2)-(_path_tex.frame_w/2);
    _path_offset_y = ConfigManager::default_grid-_path_tex.frame_h;
    _path_big_offset_x = (ConfigManager::default_grid/2)-(_path_big_tex.frame_w/2);
    _path_big_offset_y = ConfigManager::default_grid-_path_big_tex.frame_h+(ConfigManager::default_grid/4);
    return true;
}

void WldLevelItem::render(float rx, float ry)
{
    AniPos a(0,1);
    if(animated) //Get current animated frame
        a = ConfigManager::Animator_WldLevel[animatorID].image();
    if(data.pathbg)
        GlRenderer::renderTexture(&_path_tex, rx+_path_offset_x, ry+_path_offset_y);
    if(data.bigpathbg)
        GlRenderer::renderTexture(&_path_big_tex, rx+_path_big_offset_x, ry+_path_big_offset_y);

    GlRenderer::renderTexture(&texture, rx+offset_x, ry+offset_y, texture.frame_w, texture.frame_h, a.first, a.second);
}








WldMusicBoxItem::WldMusicBoxItem(WorldMusic _data): WorldNode()
{
    data = _data;
    x=data.x;
    y=data.y;
    Z=-10.0;
    type=musicbox;
}

WldMusicBoxItem::WldMusicBoxItem(const WldMusicBoxItem &x): WorldNode(x)
{
    data = x.data;
    type=musicbox;
}

WldMusicBoxItem::~WldMusicBoxItem()
{}










TileBox::TileBox()
{
    gridSize=ConfigManager::default_grid;
    gridSize_h=ConfigManager::default_grid/2;
}

TileBox::TileBox(unsigned long size)
{
    gridSize=size;
    gridSize_h=size/2;
}

TileBox::~TileBox()
{
    clean();
}

void TileBox::addNode(long X, long Y, long W, long H, WorldNode *item)
{
    registerElement(item, X, Y, W, H);
}

static bool _TreeSearchCallback(WorldNode* item, void* arg)
{
    QVector<WorldNode*>*list = static_cast<QVector<WorldNode*>* >(arg);
    if(list)
    {
        if(item) (*list).push_back(item);
    }
    return true;
}


static bool _TreeSearchCallback_with_vizibility(WorldNode* item, void* arg)
{
    QVector<WorldNode*>*list = static_cast<QVector<WorldNode*>* >(arg);
    if(list)
    {
        if(item && item->vizible)
        {
            (*list).push_back(item);
        }
    }
    return true;
}

void TileBox::query(long X, long Y, QVector<WorldNode *> &list)
{
    //PGE_Point t = applyGrid(X,Y);
    RPoint lt={ X-gridSize_h+1l, Y-gridSize_h+1l };
    RPoint rb={ X+gridSize_h-1l, Y+gridSize_h-1l };
    tree.Search(lt, rb, _TreeSearchCallback, (void*)&list);
}

void TileBox::query(long Left, long Top, long Right, long Bottom, QVector<WorldNode *> &list, bool z_sort)
{
    RPoint lt={Left-gridSize_h, Top-gridSize_h};
    RPoint rb={Right+gridSize_h, Bottom+gridSize_h};
    tree.Search(lt, rb, z_sort?_TreeSearchCallback_with_vizibility:_TreeSearchCallback, (void*)&list);
    if(z_sort)
    {
        sortElements(list);
    }
}

void TileBox::sortElements(QVector<WorldNode *> &list)
{
    if(list.size()<=1) return; //Nothing to sort!
    QStack<int> beg;
    QStack<int> end;
    WorldNode* piv;
    int i=0, L, R, swapv;
    beg.push_back(0);
    end.push_back(list.size());
    while (i>=0)
    {
        L=beg[i]; R=end[i]-1;
        if (L<R)
        {
            piv=list[L];
            while (L<R)
            {
                while ((list[R]->Z>=piv->Z) && (L<R)) R--;
                if (L<R) list[L++]=list[R];

                while ((list[L]->Z<=piv->Z) && (L<R)) L++;
                if (L<R) list[R--]=list[L];
            }
            list[L]=piv; beg.push_back(L+1); end.push_back(end[i]); end[i++]=(L);
            if((end[i]-beg[i]) > (end[i-1]-beg[i-1]))
            {
                swapv=beg[i]; beg[i]=beg[i-1]; beg[i-1]=swapv;
                swapv=end[i]; end[i]=end[i-1]; end[i-1]=swapv;
            }
        }
        else
        {
            i--;
            beg.pop_back();
            end.pop_back();
        }
    }
}

void TileBox::clean()
{
    //map.clear();
    tree.RemoveAll();
}

const long &TileBox::grid()
{
    return gridSize;
}

const long &TileBox::grid_half()
{
    return gridSize_h;
}

void TileBox::registerElement(WorldNode *item)
{
    RPoint lt={item->x, item->y};
    RPoint rb={item->x+item->w, item->y+item->h};
    if(item->w<=0) { rb[0]=item->x+1;}
    if(item->h<=0) { rb[1]=item->y+1;}
    tree.Insert(lt, rb, item);
}

void TileBox::registerElement(WorldNode *item, long X, long Y, long W, long H)
{
    RPoint lt={X, Y};
    RPoint rb={X+W, Y+H};
    if(W<=0) { rb[0]=X+1;}
    if(H<=0) { rb[1]=Y+1;}
    tree.Insert(lt, rb, item);
}

void TileBox::unregisterElement(WorldNode *item)
{
    RPoint lt={item->x, item->y};
    RPoint rb={item->x+item->w, item->y+item->h};
    if(item->w<=0) { rb[0]=item->x+1;}
    if(item->h<=0) { rb[1]=item->y+1;}
    tree.Remove(lt, rb, item);
}

PGE_Point TileBox::applyGrid(long x, long y)
{
    PGE_Point source;
    source.setX(x);
    source.setY(y);
    int gridX, gridY;
    if(gridSize>0)
    { //ATTACH TO GRID

        if((int)source.x()<0)
        {
            gridX=(int)source.x()+1+(abs((int)source.x()+1)%gridSize)-gridSize;
        }
        else
        {
            gridX=((int)source.x()-(int)source.x() % gridSize);
        }

        if((int)source.y()<0)
        {
            gridY = (int)source.y()+1+(abs((int)source.y()+1) % gridSize)-gridSize;
        }
        else
        {
            gridY = ((int)source.y() - (int)source.y() % gridSize);
        }

        return PGE_Point(gridX, gridY);
    }
    else
        return source;
}
