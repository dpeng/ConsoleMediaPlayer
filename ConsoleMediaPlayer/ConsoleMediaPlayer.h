#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;
extern "C"
{
#include <ffplay/ffplay.h>
#include <SDL/include/SDL_timer.h>
#include <progressbar/progressbar.h>
}


progressbar *m_pProgressBar = NULL;
vector<char*> filenameVector;
int curPlayingIndex;
SDL_TimerID progressBarTimerID = 0;


int CMP_Play(void);
int CMP_Stop(void);