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
vector<char*> filenamevector;
int curPlayingIndex;
SDL_TimerID progressBarTimerID = 0;
HANDLE consoleInputHandler;
HANDLE consoleOutputHandler;
HANDLE playProcessHandler;
HANDLE consoleProcessHandler;
int consoleWidth = 0;
BOOL bIsPlaying;
BOOL bExit;

int CMP_OpenFile(void);
int CMP_Play(void);
int CMP_Pause(void);
int CMP_Stop(void);
int CMP_Next(void);
int CMP_Previoius(void);