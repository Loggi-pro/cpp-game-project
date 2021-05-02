#pragma once
#include "event.h"
#include "input.h"

namespace game {
	struct GameState {
		InputHandler _input;

		void processEvent(const Event& ev);
	};

}// namespace game
