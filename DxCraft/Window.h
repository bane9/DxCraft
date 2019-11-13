#pragma once
#include "WinConfig.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>
#include <vector>
#include <optional>

class Window
{
public:
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	Window(size_t width, size_t height);
	~Window();
	static LRESULT WINAPI HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static std::optional<int> processMessages();
	Graphics& Gfx();
	void enableCursor() noexcept;
	void disableCursor() noexcept;
	void freeCursor() noexcept;
	void hideCursor() noexcept;
	void showCursor() noexcept;
	bool getCurorEnabled() noexcept;
	void confineCursor() noexcept;
	Keyboard kbd;
	Mouse mouse;

private:
	HWND hWnd = {};
	HINSTANCE hInstance;
	const wchar_t* wndName = L"DxCraft";
	size_t width, height;
	std::unique_ptr<Graphics> gfx;
	bool cursorEnabled = true;
	std::vector<BYTE> rawBuffer;
};

