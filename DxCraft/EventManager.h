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
			: data(std::move(other.data))
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

	struct FunctionHolder {
		FunctionHolder(const FunctionHolder&) = delete;
		FunctionHolder& operator=(const FunctionHolder&) = delete;
		
		FunctionHolder() = default;

		template<typename ...Args>
		FunctionHolder(void(*fn)(Args...)) 
			: function(fn)
		{
		}

		FunctionHolder(FunctionHolder&& other) noexcept
			: function(std::move(other.function))
		{
		}

		FunctionHolder& operator=(FunctionHolder&& other) noexcept
		{
			function = std::move(other.function);
			return *this;
		}

		template<typename ...Args>
		FunctionHolder& operator=(void(*fn)(Args...))
		{
			function = fn;
		}

		template<typename ...Args>
		void operator()(Args... args) {
			try {
				std::any_cast<void(*)(Args...)>(function)(args...);
			}
			catch (const std::bad_any_cast&) {
				std::ostringstream oss;
				const char* targetSignature[] = { typeid(Args).name()... };
				std::string signature = "void (__cdecl*)(";
				std::size_t size = std::tuple_size<std::tuple<Args...>>::value;
				for (int i = 0; i < size; i++) {
					signature += targetSignature[i];
					if(i < size - 1)signature += ", ";
				}
				signature += ")";
				oss << "Function signature mismatch in FunctionHolder\nTarget signature: " << signature
					<< "\nActual signature: " << function.type().name();
				throw std::exception(oss.str().c_str());
			}
		}


	private:
		std::any function;
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
		
		template<typename ...Args>
		void Subscribe(std::string key, Args... args) {
			functions[key] = FunctionHolder(args...);
		}

		template<typename ...Args>
		void operator()(std::string key, Args... args) {
			functions[key](args...);
		}

	private:
		std::map<std::string, DataHolder> data;
		std::map<std::string, FunctionHolder> functions;
	};

	inline EventManager GlobalEvt;
}
