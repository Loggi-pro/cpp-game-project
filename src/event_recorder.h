#pragma once
#include "event.h"

namespace game {
	class EventRecorder {
	private:
		EventList _events{ game::TimeElapsed{} };
		uint32_t	_eventsProcessed{ 0 };

	public:
		void processEvent(const Event& ev);

		void printInfo() const;

		void serialize(std::string_view fileName) const;
	};

}// namespace game
