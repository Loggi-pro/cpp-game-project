#pragma once
#include "event.h"
#include <SFML/Graphics/RenderWindow.hpp>

namespace game {
	class Render;

	struct EventHandler {
	private:
		EventList					_events;
		Clock::time_point _lastTick = game::Clock::now();
		void							replayProcessEvent(const Event& ev);

	public:
		void	loadEvents(EventList&& ev);
		Event getNextEvent(Render& render);
	};

}// namespace game