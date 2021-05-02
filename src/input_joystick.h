#pragma once
#include <SFML/Window/Joystick.hpp>
#include <array>
#include <iterator>
#include <vector>

namespace game {

	constexpr std::string_view toString(const sf::Joystick::Axis axis) {
		switch (axis) {
		case sf::Joystick::Axis::PovX:
			return "PovX";
		case sf::Joystick::Axis::PovY:
			return "PovY";
		case sf::Joystick::Axis::R:
			return "R";
		case sf::Joystick::Axis::U:
			return "U";
		case sf::Joystick::Axis::V:
			return "V";
		case sf::Joystick::Axis::X:
			return "X";
		case sf::Joystick::Axis::Y:
			return "Y";
		case sf::Joystick::Axis::Z:
			return "Z";
		}
		abort();
	}

	struct Joystick {
		unsigned int																id;
		std::string																	name;
		unsigned int																buttonCount;
		std::array<bool, sf::Joystick::ButtonCount> buttonState;
		std::array<float, sf::Joystick::AxisCount>	axisPosition;
	};

	struct JoystickList : public std::vector<Joystick> {
	private:
		static void updateJoystick(Joystick& js) {
			sf::Joystick::update();

			for (unsigned int button = 0; button < js.buttonCount; ++button) {
				js.buttonState.at(button) = sf::Joystick::isButtonPressed(js.id, button);
			}

			for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
				js.axisPosition.at(axis) = sf::Joystick::getAxisPosition(js.id, static_cast<sf::Joystick::Axis>(axis));
			}
		}

		static Joystick loadJoystick(unsigned int id) {
			const auto identification = sf::Joystick::getIdentification(id);
			Joystick	 js{ id, static_cast<std::string>(identification.name), sf::Joystick::getButtonCount(id), {}, {} };
			updateJoystick(js);

			return js;
		}

	public:
		Joystick& getById(unsigned int id) {
			auto joystick = std::find_if(std::begin(*this), std::end(*this), [id](const auto& j) { return j.id == id; });

			if (joystick == this->end()) {
				this->push_back(loadJoystick(id));
				return this->back();
			} else {
				return *joystick;
			}
		}
	};

}// namespace game