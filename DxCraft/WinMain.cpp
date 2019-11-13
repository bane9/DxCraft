#include "WinConfig.h"
#include "Game.h"
#include "CustomException.h"

int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nShowCmd)
{
	try {
		return Game{1280, 720}.start();
	}
	catch (const CustomException & e) {
		MessageBoxA(nullptr, e.what(), "App Exception", MB_OK | MB_ICONERROR);
	}
	catch (const std::exception & e) {
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
	}
	catch (...) {
		MessageBoxA(nullptr, "No details availavle", "Unknown Exception", MB_OK | MB_ICONERROR);
	}
	return 0;
}