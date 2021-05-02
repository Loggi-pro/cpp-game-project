#pragma once

#include "event.h"
#include "input_joystick.h"
#include <array>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

namespace game {

	struct InputHandler {
		bool				 isJoystickEvent = false;
		JoystickList joysticks{};

		void update(const Pressed<JoystickButton>& button) {
			auto& js																= joysticks.getById(button.source.id);
			js.buttonState.at(button.source.button) = true;
		};

		void update(const Released<JoystickButton>& button) {
			auto& js																= joysticks.getById(button.source.id);
			js.buttonState.at(button.source.button) = false;
		};

		void update(const Moved<JoystickAxis>& joy) {
			auto& js														= joysticks.getById(joy.source.id);
			js.axisPosition.at(joy.source.axis) = joy.source.position;
		};
	};
}// namespace game
