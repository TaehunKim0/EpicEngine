#include "stdafx.h"
#include "System.h"

INT APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, INT)
{
	System* system = new System;
	if (!system)
		return -1;

	if (system->Initialize())
		system->Run();

	system->Shutdown();
	delete system;
	system = nullptr;

	return 0;
}