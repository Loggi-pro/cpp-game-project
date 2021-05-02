#include "event_handler.h"
#include "render.h"
#include "utility.h"
#include <thread>
namespace game {
	void EventHandler::loadEvents(EventList&& ev) {
		_events = ev;
	}
	Event EventHandler::getNextEvent(Render& render) {
		if (!_events.empty()) {
			auto event = _events.front();
			_events.erase(_events.begin());
			// TODO process replay event
			return event;
		}
		if (auto mbEvent = render.getEvent(); mbEvent) { return mbEvent.value(); }
		const auto nextTick		 = Clock::now();
		const auto timeElapsed = nextTick - _lastTick;
		_lastTick							 = nextTick;
		return TimeElapsed{ timeElapsed };
	}

	void EventHandler::replayProcessEvent(const Event& event) {
		// TODO move this to render, and process on replay
		/* std::visit(overloaded{ [](const TimeElapsed& te) { std::this_thread::sleep_for(te.elapsed); },
													 [&](const Moved<Mouse>& me) {
															sf::Mouse::setPosition({ me.source.x, me.source.y }, window);
													 },
													 [](const auto& unused) {} },
							 event);*/
	}


}// namespace game