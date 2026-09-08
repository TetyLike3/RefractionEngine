#pragma once

#include <functional>

#include <Core/Common.h>

namespace Refraction::Engine {
	template <typename Derived>
	class Singleton {
	public:
		// Creates a new instance of the singleton
		// CAUTION: Will replace the previous instance
		template <typename... Args>
		static inline Common::Ref<Derived> MakeInstance(Args&&... args) {
			SingletonInstance = Common::NewShared<Derived>(std::forward<Args>(args)...);
			return GetInstance();
		}

		// Returns a reference to the singleton
		static inline Common::Ref<Derived> GetInstance() { return SingletonInstance; }

		// Access wrapper with some error handling
		static inline void Try(std::function<void(Common::Shared<Derived> singleton)> f) {
			if (SingletonInstance) {
				f(SingletonInstance);
			} else {
				throw Common::RuntimeError("No instance exists");
			}
		}

	private:
		inline static Common::Shared<Derived> SingletonInstance = nullptr;
	};
}