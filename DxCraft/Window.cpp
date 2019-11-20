#include "Window.h"
#include <exception>
#include "ExceptionMacros.h"
#include "CustomException.h"
#include "ThreadSettings.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui.h"

Window::Window(size_t width, size_t height)
	:hInstance(GetModuleHandle(nullptr)), width(width), height(height)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HandleMessage;
	wc.hInstance = hInstance;
	wc.lpszClassName = wndName;
	RegisterClassEx(&wc);
	
	RECT rt;
	rt.top = 100;
	rt.left = 100;
	rt.bottom = rt.top + height;
	rt.right = rt.left + width;

	FAILED_LAST_EXCEPT("Window setup error", AdjustWindowRect(&rt, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE));

	hWnd = CreateWindowEx(
		0,
		wndName,
		wndName,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME,
		CW_USEDEFAULT, CW_USEDEFAULT, rt.right - rt.left, rt.bottom - rt.top,
		nullptr,
		nullptr,
		hInstance,
		this
	);

	FAILED_LAST_EXCEPT("Window setup error", hWnd);

	gfx = std::make_unique<Graphics>(hWnd, width, height);

	ShowWindow(hWnd, SW_SHOW);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01;
	rid.usUsage = 0x02;
	rid.dwFlags = 0;
	rid.hwndTarget = hWnd;
	FAILED_LAST_EXCEPT("Window setup error", RegisterRawInputDevices(&rid, 1, sizeof(rid)));
	ImGui_ImplWin32_Init(hWnd);
}

Window::~Window() {
	ImGui_ImplWin32_Shutdown();
	UnregisterClass(wndName, hInstance);
	DestroyWindow(hWnd);
}

std::optional<int> Window::processMessages()
{
	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	if (msg.message == WM_QUIT) {
		return static_cast<int>(msg.wParam);
	}
	return {};
}

Graphics& Window::Gfx()
{
	if (!gfx) {
		throw std::exception("Graphics object failed to allocate (Window::Gfx)");
	}
	return *gfx;
}

void Window::enableCursor() noexcept
{
	cursorEnabled = true;
	freeCursor();
	showCursor();
}

void Window::disableCursor() noexcept
{
	hideCursor();
	cursorEnabled = false;
	confineCursor();
}

void Window::confineCursor() noexcept
{
	RECT rect;
	rect.top = height / 2;
	rect.bottom = height / 2;
	rect.left = width / 2;
	rect.right = width / 2;
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}

void Window::freeCursor() noexcept
{
	ClipCursor(nullptr);
}

void Window::hideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);
}

void Window::showCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

bool Window::getCurorEnabled() noexcept
{
	return cursorEnabled;
}

static bool shutdown = false;

LRESULT WINAPI Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (shutdown) return DefWindowProc(hWnd, msg, wParam, lParam);
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	static Window* wnd;
	ImGuiIO imio;

	if(msg != WM_CLOSE)
		imio = ImGui::GetIO();
	else {
		shutdown = true;
		PostQuitMessage(0);
		return 0;
	}
	switch (msg) {
	case WM_CREATE:
		wnd = static_cast<Window*>(reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams);
		break;
	case WM_SIZE:
	case WM_DISPLAYCHANGE:
	{
		
		int width = static_cast<int>(lParam) & 0xffff;
		int height = (static_cast<int>(lParam) & 0xffff0000) >> 16;
		wnd->Gfx().setResoultion(width, height);
		wnd->width = width;
		wnd->height = height;
	}
	break;
	case WM_CHAR:
		wnd->kbd.OnChar(static_cast<char>(wParam));
		break;
	case WM_SYSKEYDOWN:
	case WM_KEYDOWN:
		if (imio.WantCaptureKeyboard)
		{
			break;
		}
		if (!(lParam & 0x40000000) || wnd->kbd.AutorepeatIsEnabled())
			wnd->kbd.OnKeyPressed(static_cast<char>(wParam));
		break;
	case WM_SYSKEYUP:
	case WM_KEYUP:
		if (imio.WantCaptureKeyboard)
		{
			break;
		}
		wnd->kbd.OnKeyReleased(static_cast<char>(wParam));
		break;
	case WM_MOUSEMOVE:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		if (!wnd->cursorEnabled)
		{
			wnd->confineCursor();
			wnd->hideCursor();
			break;
		}
		else {
			ReleaseCapture();
			wnd->showCursor();
		}
		if (pt.x >= 0 && pt.x < wnd->width && pt.y >= 0 && pt.y < wnd->height)
		{
			wnd->mouse.OnMouseMove(pt.x, pt.y);
			if (!wnd->mouse.IsInWindow())
			{
				SetCapture(hWnd);
				wnd->mouse.OnMouseEnter();
			}
		}

		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON)) {
				wnd->mouse.OnMouseMove(pt.x, pt.y);
			}
			else {
				ReleaseCapture();
				wnd->mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnLeftReleased(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= wnd->width || pt.y < 0 || pt.y >= wnd->height) {
			ReleaseCapture();
			wnd->mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnRightReleased(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= wnd->width || pt.y < 0 || pt.y >= wnd->height) {
			ReleaseCapture();
			wnd->mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MBUTTONUP:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnMiddleReleased(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= wnd->width || pt.y < 0 || pt.y >= wnd->height) {
			ReleaseCapture();
			wnd->mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MBUTTONDOWN:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		wnd->mouse.OnMiddlePressed(pt.x, pt.y);
		if (pt.x < 0 || pt.x >= wnd->width || pt.y < 0 || pt.y >= wnd->height) {
			ReleaseCapture();
			wnd->mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		wnd->mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	case WM_KILLFOCUS:
		wnd->kbd.ClearState();
		break;
	case WM_WINDOWPOSCHANGING:
		break;
	case WM_INPUT:
	{
		if (imio.WantCaptureMouse)
		{
			break;
		}
		if (!wnd->mouse.RawEnabled())
		{
			break;
		}
		UINT size{};
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			break;
		}
		wnd->rawBuffer.resize(size);
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			wnd->rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			break;
		}
		auto& ri = reinterpret_cast<const RAWINPUT&>(*wnd->rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			wnd->mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

