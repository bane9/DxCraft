#pragma once
#include <queue>
#include <bitset>
#include <optional>

class Keyboard
{
	friend class Window;
public:
	class Event {
	public:
		enum class Type {
			Press,
			Release,
			Invalid
		};
		Event() : type(Type::Invalid) {}
		Event(Type type, unsigned char code) : type(type), code(code) {}
		bool isPress() {
			return type == Type::Press;
		}
		bool isRelease() {
			return type == Type::Release;
		}
		bool isValid() {
			return type != Type::Invalid;
		}
		unsigned char GetCode() {
			return code;
		}
	private:
		Type type;
		unsigned char code{};
	};
public:
	Keyboard();
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	bool KeyIsPressed(unsigned char keycode) const noexcept;
	std::optional<Event> ReadKey() noexcept;
	bool KeyIsEmpty() const noexcept;
	void FlushKey() noexcept;
	std::optional<char> ReadChar() noexcept;
	bool CharIsEmpty() const noexcept;
	void FlushChar() noexcept;
	void Flush() noexcept;
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutorepeatIsEnabled() const noexcept;
private:
	void OnKeyPressed(unsigned char keycode) noexcept;
	void OnKeyReleased(unsigned char keycode) noexcept;
	void OnChar(char character) noexcept;
	void ClearState() noexcept;
	template<typename T>
	static void TrimBuffer(std::queue<T>& buffer) noexcept;
	static constexpr unsigned int nKeys = 256;
	static constexpr unsigned int bufferSize = 16;
	bool autorepeatEnabled = false;
	std::bitset<nKeys> keystates;
	std::queue<Event> keybuffer;
	std::queue<char> charbuffer;

};

template<typename T>
inline void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}