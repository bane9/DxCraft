#pragma once
#include <string>
#include <tuple>
#include <functional>
#include <any>
#include <vector>
#include <map>
#include <mutex>
#include <sstream>

namespace Evt {
	
	struct DataHolder {
		DataHolder(const DataHolder&) = delete;
		DataHolder& operator=(const DataHolder&) = delete;

		DataHolder() = default;

		template<typename T>
		DataHolder(const T& rhs)
		{
			operator=(rhs);
		}

		DataHolder(DataHolder&& other) noexcept
			: data(std::move(data))
		{
		}

		DataHolder& operator=(DataHolder&& other) noexcept
		{
			data = std::move(other.data);
			return *this;
		}
		
		template<typename T>
		DataHolder& operator=(const T& data) {
			this->data = data;
			return *this;
		}

		template<typename T>
		operator T&() {
			T* out = nullptr;
			if ((out = std::any_cast<T>(&data)) == nullptr) {
				std::ostringstream oss;
				oss << "Type cast mismatch in DataHolder\nTarget type: " << typeid(T).name()
					<< "\nActual type: " << data.type().name();
				throw std::exception(oss.str().c_str());
			}
			return *out;
		}

	private:
		std::any data;
	};

	class EventManager
	{
		EventManager(const EventManager&) = delete;
		EventManager& operator=(const EventManager&) = delete;
	public:
		EventManager() = default;
		DataHolder& operator[](const char* key) {
			return data[std::string(key)];
		}
		DataHolder& operator[](const std::string& key) {
			return data[key];
		}
		
	private:
		std::map<std::string, DataHolder> data;
	};

	inline EventManager GlobalEvt;
}
