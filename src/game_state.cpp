#include "game_state.h"
#include "utility.h"
namespace game {

	void GameState::processEvent(const Event& ev) {
		std::visit(game::overloaded{ [&](const game::JoystickEvent auto& jsEvent) {
																	_input.isJoystickEvent = true;
																	_input.update(jsEvent);
																},
																 [&](const auto& event) {} },
							 ev);
	}
}// namespace game