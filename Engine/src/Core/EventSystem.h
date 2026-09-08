#pragma once

#include <string>

#include <Core/Common.h>

namespace Refraction::Events {
	class Event {
	public:
		Event() = default;
		virtual ~Event() = default;

		virtual std::string GetName() const { return mName; }
		virtual void Consume() { mConsumed = true; }
		virtual bool Consumed() const { return mConsumed; }
	protected:
		std::string mName = "DEFAULT_EVENT";
	private:
		bool mConsumed = false;
	};

	class AEventDispatcher {
	public:
		virtual ~AEventDispatcher() = default;
		virtual void Dispatch(Common::Shared<Event> event) = 0;
	};
}
