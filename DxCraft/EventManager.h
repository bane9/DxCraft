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
		friend class EventManager;
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
		friend class EventManager;
		FunctionHolder(const FunctionHolder&) = delete;
		FunctionHolder& operator=(const FunctionHolder&) = delete;
		
		FunctionHolder() = default;

		template<typename ...Args>
		FunctionHolder(void(*fn)(Args...)) 
			: function(fn)
		{
		}

		template<typename ...Args>
		FunctionHolder(Args... args...)
			: function(args...)
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
				std::string error = FunctionHolder::GenerateError(function, args...);
				throw std::exception(error.c_str());
			}
		}

		template<typename ...Args>
		static std::string GenerateError(std::any& fn, Args... args) {
			std::ostringstream oss;
			constexpr std::size_t size = std::tuple_size<std::tuple<Args...>>::value;
			if constexpr (size != 0) {
				const char* targetSignature[] = { typeid(Args).name()... };
				std::string signature = "void (*)(";
				for (int i = 0; i < size; i++) {
					signature += targetSignature[i];
					if (i < size - 1) signature += ", ";
				}
				signature += ")";
				oss << "Function signature mismatch in FunctionHolder\nTarget signature: " << signature
					<< "\nActual signature: " << fn.type().name();
				return oss.str().c_str();
			}
			else {
				return "Function signature mismatch in FunctionHolder, but the target signature was of size 0.";
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
		
		template<typename T, typename ...Args>
		const T* GetFirstPointer(const T& first, const Args&... args) {
			return &first;
		}

		template<typename ...Args>
		void Subscribe(const std::string& key, Args... args) {
			auto& vec = functions[key];
			bool append = true;
			if (vec.size() != 0) {
				for (auto& fn : vec) {
					auto fptr = std::any_cast<Args...>(&fn.function);
					if (fptr == nullptr) {
						std::ostringstream oss;
						const char* targetSignature[] = { typeid(Args).name()... };
						std::size_t size = std::tuple_size<std::tuple<Args...>>::value;
						std::string signature;
						for (int i = 0; i < size; i++) {
							signature += targetSignature[i];
							if (i < size - 1) signature += ", ";
						}
						oss << "The key \"" << key << "\" has already been assigned to a function with a different signature."
							<< "\nTarget signature: " << signature
							<< "\nActual signature: " << fn.function.type().name();
						throw std::exception(oss.str().c_str());
					}
					else if (*fptr == *GetFirstPointer(args...)) {
						append = false;
						break;
					}
				}
			}
			if(append) vec.push_back(FunctionHolder(args...));
		}

		bool HasDataKey(const std::string& key) {
			for (auto& [k, v] : data) {
				if (key == k && v.data.type() != typeid(void)) return true;
			}
			return false;
		}

		bool HasFunctionKey(const std::string& key) {
			for (auto& [k, v] : functions) {
				if (key == k && !v.empty()) return true;
			}
			return false;
		}

		void RemoveDataKey(const std::string& key) {
			data.erase(key);
		}

		void RemoveFunctionKey(const std::string& key) {
			functions.erase(key);
		}

		template<typename ...Args>
		void UnsubscribeGlobal(Args... args) {
			for (auto& [k, v] : functions) {
				Unsubscribe(k, args...);
			}
		}
		
		template<typename ...Args>
		void Unsubscribe(const std::string& key, Args... args) {
			auto& vec = functions[key];
			for (auto it = vec.begin(); it != vec.end(); ++it) {
				auto fptr = std::any_cast<Args...>(&(*it).function);
				if (fptr != nullptr && *fptr == *GetFirstPointer(args...)) {
					vec.erase(it);
					return;
				}
			}
		}

		template<typename ...Args>
		void operator()(std::string key, Args... args) {
			bool found = false;
			for (auto& [k, v] : functions) {
				if (k == key) {
					found = true;
					break;
				}
			}
			if (!found) return;
			auto& vec = functions[key];
			for (auto& fn : vec) {
				fn(args...);
			}
		}

	private:
		std::map<std::string, DataHolder> data;
		std::map<std::string, std::vector<FunctionHolder>> functions;
	};

	inline EventManager GlobalEvt;
}
