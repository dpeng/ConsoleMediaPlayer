// ConsoleMediaPlayer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include "ConsoleMediaPlayer.h"

static void pushLogsToConsole(const char *fmt, va_list vargs)
{
	char logbuf[1024 * 8];
	DWORD len;
	memset(logbuf, 0, sizeof(logbuf));

	vsprintf_s(logbuf, fmt, vargs);
	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), logbuf, (DWORD)strlen(logbuf), &len, NULL);
}

static void consolePrint(const char *fmt, ...)
{
	va_list vargs;
	va_start(vargs, fmt);
	pushLogsToConsole(fmt, vargs);
	va_end(vargs);
}

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

int CMP_Next(void)
{
	if ((curPlayingIndex + 1) < filenamevector.size()) curPlayingIndex++;
	else curPlayingIndex = 0;
	int ret = CMP_Play();
	return ret;
}


int CMP_Previoius(void)
{
	if (curPlayingIndex > 0) curPlayingIndex--;
	else curPlayingIndex = filenamevector.size() - 1;
	int ret = CMP_Play();
	return ret;
}

Uint32 endOfFileTimer(Uint32 interval, void *param)
{
	CMP_Stop();
	int ret = CMP_Next();
	return ret;
}

DWORD WINAPI playProcess(LPVOID pParam)
{
	bIsPlaying = TRUE;
	int ret = ffplay_play(NULL);
	//0 means return successful
	if (ret == 0) {
		SDL_AddTimer(40, endOfFileTimer, 0); //inform the timer that this file play comes to an end
	}
	else std::cout << "playing error, will pass play this file.!\n";
	return ret;
}

DWORD ProcessConsoleInput(INPUT_RECORD* pInputRec, DWORD dwInputs)
{
	if (pInputRec == NULL)	return 1;
	switch (pInputRec->EventType)
	{
	case KEY_EVENT:
		if (pInputRec->Event.KeyEvent.bKeyDown)
		{
			switch (pInputRec->Event.KeyEvent.wVirtualKeyCode)
			{
			case 0x50: /*VK_P*/
				CMP_Stop();
				CMP_Previoius();
				break;
			case 0x4e: /*VK_N*/
				CMP_Stop();
				CMP_Next();
				break;
			case 0x46: /*VK_F*/
				//OnWndFullScreen();
				break;
			case 0x48: /*VK_H*/
				//printHelpInfomation();
				break;
			case 0x49:/*VK_I*/
				double curTime;
				int	totalTime;
				curTime = ffplay_get_stream_curtime();
				totalTime = ffplay_get_stream_totaltime();
				if ((totalTime >= 1) && !isnan(curTime))
					consolePrint("current Time: %d, Total Time: %d, Playing percentage:%d                    \n",
					(int)curTime,
						totalTime,
						int(100 * curTime / totalTime));
				break;
			case 0x4c:/*VK_L*/
				for (int i = 0; i < filenamevector.size(); i++)
					if (i == curPlayingIndex)
						consolePrint("%03d: %s---current-playing---\n", i, filenamevector[i]);
					else
						consolePrint("%03d: %s\n", i, filenamevector[i]);
				break;
			case 0x4f:/*VK_O*/
				CMP_OpenFile();
				CMP_Play();
				break;
			case 0x51:/*VK_Q*/
				bExit = TRUE;
				break;
			case 0x53:/*VK_S*/
				//OnBnClickedButtonPlaytonextframe();
				break;
			case 0x57:/*VK_W*/
				//ffplay_toggle_display();
				break;
			case VK_SPACE:
				if (bIsPlaying)
					CMP_Pause();
				else
					CMP_Play();
				break;
			case VK_MEDIA_NEXT_TRACK:
				CMP_Stop();
				CMP_Next();
				break;
			case VK_MEDIA_PREV_TRACK:
				CMP_Stop();
				CMP_Previoius();
				break;
			case VK_MEDIA_PLAY_PAUSE:
				CMP_Pause();
				break;
			case VK_VOLUME_UP:
			case 0x30:/*  0  */
				ffplay_toggle_update_volume(1, 0.75);
				break;
			case VK_VOLUME_DOWN:
			case 0x39:/*  9  */
				ffplay_toggle_update_volume(-1, 0.75);
				break;
			case VK_VOLUME_MUTE:
			case 0x4d:/*  M  */
				ffplay_toggle_mute();
				break;
			case VK_RETURN:
				break;
			case VK_ESCAPE:
				bExit = TRUE;
				break;
			default:
				break;
			}
		}
		break;
	case MOUSE_EVENT:
		if (pInputRec->Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
		{
			if ((consoleWidth > 0) && (pInputRec->Event.MouseEvent.dwMousePosition.X >= 0) &&
				(consoleWidth >= pInputRec->Event.MouseEvent.dwMousePosition.X))
			{
				double pos = (double)pInputRec->Event.MouseEvent.dwMousePosition.X / (double)consoleWidth;
				if (bIsPlaying)
				{
					int ns, hh, mm, ss;
					int tns, thh, tmm, tss;
					tns = (int)(ffplay_get_meida_duration() / 1000000LL);
					thh = tns / 3600;
					tmm = (tns % 3600) / 60;
					tss = (tns % 60);
					ns = (int)(pos * tns);
					hh = ns / 3600;
					mm = (ns % 3600) / 60;
					ss = (ns % 60);
					consolePrint("Seek to %2.0f%% (%02d:%02d:%02d) of total duration (%02d:%02d:%02d)                        \n",
						pos * 100, hh, mm, ss, thh, tmm, tss);
					ffplay_seek(pos);
				}
			}
		}

		break;
	case WINDOW_BUFFER_SIZE_EVENT:
		break;
	case MENU_EVENT:
		break;
	case FOCUS_EVENT:
		break;
	default:
		break;
	}
	return 0;
}

DWORD WINAPI consoleInputMonitor(LPVOID pParam)
{
	DWORD   dwInputs = 1;
	INPUT_RECORD lpBuffer[MAX_PATH];
	FlushConsoleInputBuffer(consoleInputHandler);
	while (consoleProcessHandler)
	{
		// If there are input events in buffer, this object is signaled
		WaitForSingleObject(consoleInputHandler, INFINITE);
		GetNumberOfConsoleInputEvents(consoleInputHandler, &dwInputs);
		ReadConsoleInput(consoleInputHandler, lpBuffer, min(dwInputs, MAX_PATH), &dwInputs);

		ProcessConsoleInput(lpBuffer, 0);
	}
	return 0;
}

inline char* wcharToUtf8(wchar_t* w)
{
	int len = WideCharToMultiByte(CP_UTF8, 0, w, -1, 0, 0, 0, 0);
	char* utfstr = (char*)malloc(MAX_PATH);
	if (utfstr) WideCharToMultiByte(CP_UTF8, 0, w, -1, utfstr, len + 1, 0, 0);
	return utfstr;
}

int CMP_OpenFile(void)
{
	OPENFILENAME ofn;
	TCHAR szFile[4 * 1024];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = szFile;
	ofn.lpstrFile[0] = 0;
	ofn.nMaxFile = 4 * 1024;
	ofn.lpstrFilter = L"MP3 Files (*.mp3)\0*.mp3\0All Files  (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_ALLOWMULTISELECT;

	if (GetOpenFileName(&ofn) == TRUE)
	{
		char *fullfilename;
		if (GetFileAttributes(szFile) != FILE_ATTRIBUTE_DIRECTORY)
		{
			fullfilename = wcharToUtf8(szFile); // mesh code
			filenamevector.push_back(fullfilename);
		}
		else
		{
			int index = 0;
			TCHAR filename[MAX_PATH];
			TCHAR path[MAX_PATH];
			memset(filename, 0, sizeof(filename));
			memset(path, 0, sizeof(path));
			int i = 0;
			bool alreadygotpath = false;
			for (index = 0; index < 4 * 1024 - 1; index++)
			{
				if (szFile[index] == NULL)
				{
					if (alreadygotpath)
					{
						fullfilename = wcharToUtf8(path);
						strcat(fullfilename, wcharToUtf8(filename));
						filenamevector.push_back(fullfilename);
					}
					else
					{
						path[i] = '\\';
					    alreadygotpath = true;
					}
					memset(filename, 0, sizeof(filename));
					i = 0;
					//look for double null end
					if (szFile[index + 1] == NULL) { break; }
				}
				else
				{
					if (alreadygotpath)
					{
						filename[i++] = szFile[index];
					}
					else
					{
						path[i++] = szFile[index];
					}
				}
			}
		}
	}
	return 0;
}
int CMP_Play(void)
{
	int ret;
	if (filenamevector.size() == 0 || bIsPlaying)
	{
		ret = -1;
	}
	else
	{
		ret = ffplay_init(filenamevector[curPlayingIndex], NULL, NULL);
		DWORD threadID;
		if (ret == 0)
		{
			//need show the play area again because when we close the SDL, it will hide the play window
			playProcessHandler = CreateThread(NULL, 0, playProcess, 0, 0, &threadID);
			progressBarTimerID = SDL_AddTimer(400, progressBarTimer, 0); // for console progress bar useage
		}
		else std::cout << "playing error, will pass play this file.!\n";
	}

	return ret;
}

int CMP_Pause(void)
{
	ffplay_pause();
	return 0;
}

int CMP_Stop(void)
{
	TerminateThread(playProcessHandler, 0);
	CloseHandle(playProcessHandler);
	playProcessHandler = NULL;
	//TerminateThread(consoleProcessHandler, 0);
	//CloseHandle(consoleProcessHandler);
	//consoleProcessHandler = NULL;
	SDL_RemoveTimer(progressBarTimerID);
	bIsPlaying = FALSE;
	ffplay_stop();
	return 0;
}

int main(int argc, char **argv)
{
	bExit = FALSE;
	std::cout << "Hello Console Media Player!\n";
	for (int i = 1; i < __argc; i++)
	{
		filenamevector.push_back(argv[i]);
	}
	//TCHAR tfn[MAX_PATH];
	//wcscpy(tfn, L"C:\\N-5CG8031PV3-Data\\dpeng\\Music\\你开车我放歌(中文)\\前方-马雨阳.mp3");
	//char *fn = (char*)malloc(MAX_PATH);
	//size_t tfnsize = sizeof(tfn);
	//size_t fnsize = sizeof(fn);
	//wcscpy((wchar_t *)fn, tfn);//doesnt work
	//lstrcpy((LPWSTR)fn, tfn);//doesnt work
	//wcstombs_s(&converted, fn, len, tfn, _TRUNCATE);//doesnt work
	//fn = wcharToUtf8(tfn); // mesh code
	//AtlW2AHelper(fn, tfn, tfnsize); //mesh code

	curPlayingIndex = 0;
	playProcessHandler = NULL;
	consoleProcessHandler = NULL;
	consoleInputHandler = GetStdHandle(STD_INPUT_HANDLE);
	consoleOutputHandler = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwPreviousMode = 0;
	GetConsoleMode(consoleInputHandler, &dwPreviousMode);
	DWORD dwNewMode = dwPreviousMode | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT;
	dwNewMode &= ~ENABLE_QUICK_EDIT_MODE;
	SetConsoleMode(consoleInputHandler, dwNewMode);

	//get the width of console window
	CONSOLE_SCREEN_BUFFER_INFO lpConsoleScreenBufferInfo;
	GetConsoleScreenBufferInfo(consoleOutputHandler, &lpConsoleScreenBufferInfo);
	consoleWidth = lpConsoleScreenBufferInfo.dwSize.X;

	DWORD threadID;
	consoleProcessHandler = CreateThread(NULL, 0, consoleInputMonitor, 0, 0, &threadID);
	if(filenamevector.size() > 0)
		CMP_Play();
	while (!bExit)
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
