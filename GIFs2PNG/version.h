/*
 * GIFs2PNG, a free tool for merge GIF images with his masks and save into PNG
 * This is a part of the Platformer Game Engine by Wohlstand, a free platform for game making
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

#include "../pge_version.h" //Global Project version file

#ifndef EDITOR_VERSION_H
#define EDITOR_VERSION_H

//Version of this program
#define _FILE_VERSION "2.0.0"
#define _FILE_RELEASE ""

#define _VF1 2
#define _VF2 0
#define _VF3 0
#define _VF4 0


#define _FILE_DESC "Masked GIFs to PNG converter"

#define _INTERNAL_NAME "GIFs2PNG"

#ifdef _WIN32
	#define _ORIGINAL_NAME "GIFs2PNG.exe" // for Windows platforms
#else
	#define _ORIGINAL_NAME "GIFs2PNG" // for any other platforms
#endif

//Uncomment this for enable detal logging
//#define DEBUG

#endif
