#include "WinConfig.h"
#include "Mouse.h"


std::pair<int, int> Mouse::GetPos() const noexcept
{
	return { x,y };
}

std::optional<Mouse::RawDelta> Mouse::ReadRawDelta() noexcept
{
	mouseMutex.lock();
	if (rawDeltaBuffer.empty()) {
		mouseMutex.unlock();
		return {};
	}
	const RawDelta d = rawDeltaBuffer.front();
	rawDeltaBuffer.pop();
	mouseMutex.unlock();
	return d;
}

int Mouse::GetPosX() const noexcept
{
	return x;
}

int Mouse::GetPosY() const noexcept
{
	return y;
}

bool Mouse::IsInWindow() const noexcept
{
	return isInWindow;
}

bool Mouse::LeftIsPressed() const noexcept
{
	return leftIsPressed;
}

bool Mouse::MiddleIsPressed() const noexcept
{
	return middleIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
	return rightIsPressed;
}

void Mouse::clearStates() noexcept
{
	leftIsPressed = false;
	middleIsPressed = false;
	rightIsPressed = false;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
	mouseMutex.lock();
	if (buffer.size() > 0) {
		Mouse::Event e = buffer.front();
		buffer.pop();
		mouseMutex.unlock();
		return e;
	}
	mouseMutex.unlock();
	return {};
}

void Mouse::Flush() noexcept
{
	buffer = std::queue<Event>();
}

void Mouse::EnableRaw() noexcept
{
	rawEnabled = true;
}

void Mouse::DisableRaw() noexcept
{
	rawEnabled = false;
}

bool Mouse::RawEnabled() const noexcept
{
	return rawEnabled;
}

void Mouse::OnMouseMove(int newx, int newy) noexcept
{
	x = newx;
	y = newy;

	buffer.push(Mouse::Event(Mouse::Event::Type::Move, *this));
	TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
	isInWindow = false;
	buffer.push(Mouse::Event(Mouse::Event::Type::Leave, *this));
	TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
	isInWindow = true;
	buffer.push(Mouse::Event(Mouse::Event::Type::Enter, *this));
	TrimBuffer();
}

void Mouse::OnRawDelta(int dx, int dy) noexcept
{
	rawDeltaBuffer.push({ dx,dy });
	TrimBuffer();
}

void Mouse::OnLeftPressed(int x, int y) noexcept
{
	leftIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::LPress, *this));
	TrimBuffer();
}

void Mouse::OnLeftReleased(int x, int y) noexcept
{
	leftIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::LRelease, *this));
	TrimBuffer();
}

void Mouse::OnMiddlePressed(int x, int y) noexcept
{
	middleIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::MPress, *this));
	TrimBuffer();
}

void Mouse::OnMiddleReleased(int x, int y) noexcept
{
	middleIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::MRelease, *this));
	TrimBuffer();
}

void Mouse::OnRightPressed(int x, int y) noexcept
{
	rightIsPressed = true;

	buffer.push(Mouse::Event(Mouse::Event::Type::RPress, *this));
	TrimBuffer();
}

void Mouse::OnRightReleased(int x, int y) noexcept
{
	rightIsPressed = false;

	buffer.push(Mouse::Event(Mouse::Event::Type::RRelease, *this));
	TrimBuffer();
}

void Mouse::OnWheelUp(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelUp, *this));
	TrimBuffer();
}

void Mouse::OnWheelDown(int x, int y) noexcept
{
	buffer.push(Mouse::Event(Mouse::Event::Type::WheelDown, *this));
	TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
	mouseMutex.lock();
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
	mouseMutex.unlock();
}

void Mouse::TrimRawInputBuffer() noexcept
{
	mouseMutex.lock();
	while (rawDeltaBuffer.size() > bufferSize) {
		rawDeltaBuffer.pop();
	}
	mouseMutex.unlock();
}

void Mouse::OnWheelDelta(int x, int y, int delta) noexcept
{
	wheelDeltaCarry += delta;
	while (wheelDeltaCarry >= WHEEL_DELTA) {
		wheelDeltaCarry -= WHEEL_DELTA;
		OnWheelUp(x, y);
	}

	while (wheelDeltaCarry <= -WHEEL_DELTA) {
		wheelDeltaCarry += WHEEL_DELTA;
		OnWheelDown(x, y);
	}
}
