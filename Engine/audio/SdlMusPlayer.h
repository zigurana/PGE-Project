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

#ifndef SDL_MUS_PLAYER
#define SDL_MUS_PLAYER
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer_ext.h>
#include <SDL2/SDL_stdinc.h>
#include <QHash>
#include <QString>

class PGE_MusPlayer
{
public:
    static int initAudio(int sampleRate=44100, int allocateChannels=32, int bufferSize=4096);

    static QString currentTrack;
    static void MUS_playMusic();
    static QString MUS_Title();
	static void MUS_playMusicFadeIn(int ms);
	static void MUS_pauseMusic();
	static void MUS_stopMusic();
	static void MUS_stopMusicFadeOut(int ms);

    static void MUS_changeVolume(int vlm);
    static void MUS_openFile(QString musFile);

    static int sampleRate();
    static int currentVolume();

	static bool MUS_IsPlaying();
    static bool MUS_IsPaused();
    static bool MUS_IsFading();

    static Uint64 sampleCount();
    static Uint64 MUS_sampleCount();

    static void freeStream();
private:
    static bool isLoaded;
    static Mix_Music *play_mus;
    static int volume;
    static int sRate;
	static bool showMsg;
    static QString showMsg_for;
	
	static SDL_mutex* sampleCountMutex;
    static Uint64 sCount;
    static Uint64 musSCount;
	static void postMixCallback(void *udata, Uint8 *stream, int len);
};


class PGE_Sounds
{
public:
    static void SND_PlaySnd(QString sndFile);
    static void clearSoundBuffer();
    static Mix_Chunk *SND_OpenSnd(QString sndFile);
private:
    static QHash<QString, Mix_Chunk* > chunksBuffer;
    static Mix_Chunk *sound;
    static QString current;
};
#endif
