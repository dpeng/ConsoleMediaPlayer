// ConsoleMediaPlayer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include "ConsoleMediaPlayer.h"

Uint32 progressBarTimer(Uint32 interval, void *param)
{
	double curTime;
	int	totalTime;
	curTime = ffplay_get_stream_curtime();
	totalTime = ffplay_get_stream_totaltime();

	if ((totalTime >= 1) && !isnan(curTime))
	{
		if (NULL == m_pProgressBar)
		{
			static char buf[20];
			int seconds = totalTime;
			int hours = seconds / 3600;
			seconds -= hours * 3600;
			int minutes = seconds / 60;
			seconds -= minutes * 60;
			sprintf_s(buf, "%02d:%02d", minutes, seconds);
			m_pProgressBar = progressbar_new(buf, 100);
		}
		else
		{
			m_pProgressBar->currentTime = (unsigned long)curTime;
			m_pProgressBar->leftTime = (unsigned long)(totalTime - curTime);
			progressbar_update(m_pProgressBar, (unsigned long)(curTime * 100 / totalTime));
		}
	}
	return interval;
}

Uint32 endOfFileTimer(Uint32 interval, void *param)
{
	if ((curPlayingIndex + 1) < filenameVector.size()) curPlayingIndex++;
	else curPlayingIndex = 0;
	CMP_Play();
	return 0;
}

DWORD WINAPI playProcess(LPVOID pParam)
{
	int ret = ffplay_play(NULL);
	//0 means return successful
	if (ret == 0) {
		//pThis->cleanupResource(FALSE);
		SDL_AddTimer(40, endOfFileTimer, 0); // for console progress bar useage //inform the timer that this file play comes to an end		
	}
	else std::cout << "playing error, will pass play this file.!\n";
	return ret;
}

int CMP_Play(void)
{
	int ret = ffplay_init(filenameVector[curPlayingIndex], NULL, NULL);
	DWORD threadID;
	if (ret == 0)
	{
		//need show the play area again because when we close the SDL, it will hide the play window
		CreateThread(NULL, 0, playProcess, 0, 0, &threadID);
		progressBarTimerID = SDL_AddTimer(400, progressBarTimer, 0); // for console progress bar useage
	}
	else std::cout << "playing error, will pass play this file.!\n";

	return ret;
}

int CMP_Stop(void)
{
	SDL_RemoveTimer(progressBarTimerID);
	ffplay_stop();
	return 0;
}

int main(int argc, char **argv)
{
	std::cout << "Hello Console Media Player!\n";
	for (int i = 1; i < __argc; i++)
	{
		filenameVector.push_back(argv[i]);
	}
	curPlayingIndex = 0;
	CMP_Play();
	while (1)
	{
		Sleep(100);
	}
}
// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
