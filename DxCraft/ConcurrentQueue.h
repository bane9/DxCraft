#pragma once
#include <queue>
#include <mutex>

template<typename T>
class ConcurentQueue {
	ConcurentQueue(const ConcurentQueue&) = delete;
	ConcurentQueue& operator=(const ConcurentQueue&) = delete;
	ConcurentQueue& operator=(ConcurentQueue&&) = delete;
public:
	ConcurentQueue() = default;
	~ConcurentQueue() = default;

	void push(const T& value) {
		std::unique_lock<std::mutex> lock(mutex);
		queue.push(value);
	}

	T pop() {
		std::unique_lock<std::mutex> lock(mutex);
		T temp = queue.front();
		queue.pop();
		return temp;
	}

	T popOrDefault(){
		std::unique_lock<std::mutex> lock(mutex);
		if (queue.empty()) return T{};
		T temp = queue.front();
		queue.pop();
		return temp;
	}

	T front() {
		std::unique_lock<std::mutex> lock(mutex);
		return queue.front();
	}

	std::size_t size() {
		std::unique_lock<std::mutex> lock(mutex);
		return queue.size();
	}

	bool empty() {
		std::unique_lock<std::mutex> lock(mutex);
		return queue.empty();
	}

private:
	std::queue<T> queue;
	std::mutex mutex;
};
