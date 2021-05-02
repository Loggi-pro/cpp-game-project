#pragma once
#include "event.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <optional>
namespace game {
	struct GameState;
	class Render {
		const unsigned int FRAMERATE_LIMIT = 60;
		sf::RenderWindow	 window;
		bool							 _timeElapsed			= false;
		bool							 _isJoystickEvent = false;

	public:
		Render(int width, int height, float scale);
		void processEvent(const Event& ev);

		bool isOpen() const {
			return window.isOpen();
		}

		[[maybe_unused]] std::optional<Event> getEvent();

		void processRender(const GameState& gs);

		void shutdown();
	};

}// namespace game
