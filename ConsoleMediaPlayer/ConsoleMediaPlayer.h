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

#define MAX_SONGLENGTH			260
#define MAX_MULTISONGLENGTH		1600
progressbar *m_pProgressBar = NULL;
vector<char*> filenameVector;
int curPlayingIndex;
SDL_TimerID progressBarTimerID = 0;
HANDLE consoleInputHandler;
HANDLE consoleOutputHandler;
HANDLE playProcessHandler;
HANDLE consoleProcessHandler;
int consoleWidth = 0;
BOOL bIsPlaying;

int CMP_Play(void);
int CMP_Stop(void);