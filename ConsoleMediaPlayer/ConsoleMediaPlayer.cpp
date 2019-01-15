// ConsoleMediaPlayer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ConsoleMediaPlayer.h"

Uint32 timerfunctiontest(Uint32 interval, void *param)
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
int main(int argc, char **argv)
{
    std::cout << "Hello Console Media Player!\n"; 
	char* filename = (char*)malloc(260);
	for (int i = 1; i < __argc; i++)
	{
		filename =  argv[i];
	}
	ffplay_init(filename, NULL, NULL);
	SDL_TimerID timer = SDL_AddTimer(400, timerfunctiontest, 0);
	ffplay_play(NULL);
	SDL_RemoveTimer(timer);
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
