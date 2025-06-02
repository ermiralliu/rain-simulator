#ifndef WINDOW_HELPERS
#define WINDOW_HELPERS

#include "window_helpers.h"

// here I used Win32 Api on windows but now I'm gonna get creative, and maybe use forks and whatnot?
// make an application for this and switch the stdout to SDL. Then close it.
// will be a bit less efficient cause it will create a new process each time but who cares

void showConsole(bool show){ //shows console
	// HWND consoleWindow = FindWindowA("ConsoleWindowClass", NULL);
	// ShowWindow(consoleWindow, show);
} 

void exeToForeground(){ //hides the console after using it and brings application to foreground
	// HWND gameWindow = FindWindowA(NULL, "Rain Simulator" );
	// showConsole(false);
	// SetActiveWindow(gameWindow);
}

#endif
