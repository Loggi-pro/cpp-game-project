//
// Created by Loggi on 07.08.2020.
//
#ifndef MYPROJECT_INPUT_H
#define MYPROJECT_INPUT_H

#include "utility.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <array>
#include <chrono>
#include <string>
#include <string_view>
#include <variant>
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

struct GameState {
	using Clock													= std::chrono::steady_clock;
	Clock::time_point					_lastTick = game::GameState::Clock::now();
	std::chrono::milliseconds _msElapsed;


	struct CloseWindow {};

	struct TimeElapsed {
		Clock::duration elapsed;

		[[nodiscard]] sf::Time toSFMLTime() const {
			return sf::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
		}
	};

	template<typename Source>
	struct Pressed {
		Source source;
	};
	template<typename Source>
	struct Released {
		Source source;
	};

	template<typename Source>
	struct Moved {
		Source source;
	};

	struct JoystickButton {
		unsigned int id;
		unsigned int button;
	};

	struct JoystickAxis {
		unsigned int id;
		unsigned int axis;
		float				 position;
	};

	struct Mouse {
		int x;
		int y;
	};
	struct MouseButton {
		int		button;
		Mouse mouse;
	};

	struct Key {
		bool							alt, control, system, shift;
		sf::Keyboard::Key key;
	};

	struct Joystick {
		unsigned int																id;
		std::string																	name;
		unsigned int																buttonCount;
		std::array<bool, sf::Joystick::ButtonCount> buttonState;
		std::array<float, sf::Joystick::AxisCount>	axisPosition;
	};

	std::vector<Joystick> joysticks{};


	static void refreshJoystick(Joystick &js) {

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
		refreshJoystick(js);

		return js;
	}


	static Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id) {
		auto joystick = std::find_if(begin(joysticks), end(joysticks), [id](const auto &j) { return j.id == id; });

		if (joystick == joysticks.end()) {
			joysticks.push_back(loadJoystick(id));
			return joysticks.back();
		} else {
			return *joystick;
		}
	}

	void update(const Pressed<JoystickButton> &button) {
		auto &js																= joystickById(joysticks, button.source.id);
		js.buttonState.at(button.source.button) = true;
	};
	void update(const Released<JoystickButton> &button) {
		auto &js																= joystickById(joysticks, button.source.id);
		js.buttonState.at(button.source.button) = false;
	};
	void update(const Moved<JoystickAxis> &joy) {
		auto &js														= joystickById(joysticks, joy.source.id);
		js.axisPosition.at(joy.source.axis) = joy.source.position;
	};
	using Event = std::variant<std::monostate,
														 Pressed<JoystickButton>,
														 Released<JoystickButton>,
														 Pressed<Key>,
														 Released<Key>,
														 Moved<JoystickAxis>,
														 Moved<Mouse>,
														 Pressed<MouseButton>,
														 Released<MouseButton>,
														 CloseWindow,
														 TimeElapsed>;

	static sf::Event::KeyEvent toSFMLEventInternal(const Key &key) {
		return { .code = key.key, .alt = key.alt, .control = key.control, .shift = key.shift, .system = key.system };
	}
	static sf::Event::JoystickButtonEvent toSFMLEventInternal(const JoystickButton &joy) {
		return { .joystickId = joy.id, .button = joy.button };
	}
	static sf::Event::JoystickMoveEvent toSFMLEventInternal(const JoystickAxis &joy) {
		return { .joystickId = joy.id, .axis = static_cast<sf::Joystick::Axis>(joy.axis), .position = joy.position };
	}
	static sf::Event::MouseMoveEvent	 toSFMLEventInternal(const Mouse &mouse) { return { .x = mouse.x, .y = mouse.y }; }
	static sf::Event::MouseButtonEvent toSFMLEventInternal(const MouseButton &mouseButton) {
		return { .button = static_cast<sf::Mouse::Button>(mouseButton.button),
						 .x			 = mouseButton.mouse.x,
						 .y			 = mouseButton.mouse.y };
	}


	static sf::Event toSFMLEventInternal(const Pressed<JoystickButton> &value) {
		return sf::Event{ .type = sf::Event::JoystickButtonPressed, .joystickButton = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const Released<JoystickButton> &value) {
		return sf::Event{ .type = sf::Event::JoystickButtonReleased, .joystickButton = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Pressed<Key> &value) {
		return sf::Event{ .type = sf::Event::KeyPressed, .key = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const Released<Key> &value) {
		return sf::Event{ .type = sf::Event::KeyReleased, .key = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const Moved<JoystickAxis> &value) {
		return sf::Event{ .type = sf::Event::JoystickMoved, .joystickMove = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const Moved<Mouse> &value) {
		return sf::Event{ .type = sf::Event::MouseMoved, .mouseMove = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Pressed<MouseButton> &value) {
		return sf::Event{ .type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const Released<MouseButton> &value) {
		return sf::Event{ .type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source) };
	}
	static sf::Event toSFMLEventInternal(const CloseWindow & /*unused*/) {
		return sf::Event{ .type = sf::Event::Closed, .size = {} };
	}


	static std::optional<sf::Event> toSFMLEvent(const Event &event) {
		return std::visit(
			overloaded{ [&](const auto &value) -> std::optional<sf::Event> { return toSFMLEventInternal(value); },
									[&](const TimeElapsed & /*unused*/) -> std::optional<sf::Event> {// dummy event, for NOP
										return {};
									},
									[&](const std::monostate & /*unused*/) -> std::optional<sf::Event> {// dummy event for NOP
										return {};
									}

			},
			event);
	}

	Event getNextEvent(sf::RenderWindow &window) {
		sf::Event event{};
		if (window.pollEvent(event)) { return toEvent(event); }

		const auto nextTick		 = Clock::now();
		const auto timeElapsed = nextTick - _lastTick;
		_lastTick							 = nextTick;

		return TimeElapsed{ timeElapsed };
		// gs.timeElapsed = timeElapsed;
	}


	Event toEvent(const sf::Event &event) {
		switch (event.type) {
		case sf::Event::Closed:
			return CloseWindow{};
		case sf::Event::JoystickButtonPressed:
			return Pressed<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
		case sf::Event::JoystickButtonReleased:
			return Released<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
		case sf::Event::JoystickMoved:
			return Moved<JoystickAxis>{ event.joystickMove.joystickId, event.joystickMove.axis, event.joystickMove.position };
			// Mouse
		case sf::Event::MouseButtonPressed:
			return Pressed<MouseButton>{ event.mouseButton.button, { event.mouseButton.x, event.mouseButton.y } };
		case sf::Event::MouseButtonReleased:
			return Released<MouseButton>{ event.mouseButton.button, { event.mouseButton.x, event.mouseButton.y } };
			//		case sf::Event::MouseEntered:
			//		case sf::Event::MouseLeft:
		case sf::Event::MouseMoved:
			return Moved<Mouse>{ event.mouseMove.x, event.mouseMove.y };
			//		case sf::Event::MouseWheelScrolled:
			// Keyboard
		case sf::Event::KeyPressed:
			return Pressed<Key>{ event.key.alt, event.key.control, event.key.system, event.key.shift, event.key.code };
		case sf::Event::KeyReleased:
			return Released<Key>{ event.key.alt, event.key.control, event.key.system, event.key.shift, event.key.code };
		default:
			return std::monostate{};
		}
	}
};// namespace game
template<typename T>
concept JoystickEvent =
	std::is_same_v<
		T,
		GameState::Pressed<
			GameState::JoystickButton>> || std::is_same_v<T, GameState::Released<GameState::JoystickButton>> || std::is_same_v<T, GameState::Moved<GameState::JoystickAxis>>;

}// namespace game

#endif// MYPROJECT_INPUT_H
