#include "event_sfml.h"
#include "utility.h"

namespace game {
	static sf::Event::KeyEvent toSFMLEventInternal(const Key& key) {
		return { .code = key.key, .alt = key.alt, .control = key.control, .shift = key.shift, .system = key.system };
	}

	static sf::Event::JoystickButtonEvent toSFMLEventInternal(const JoystickButton& joy) {
		return { .joystickId = joy.id, .button = joy.button };
	}

	static sf::Event::JoystickMoveEvent toSFMLEventInternal(const JoystickAxis& joy) {
		return { .joystickId = joy.id, .axis = static_cast<sf::Joystick::Axis>(joy.axis), .position = joy.position };
	}

	static sf::Event::MouseMoveEvent toSFMLEventInternal(const Mouse& mouse) {
		return { .x = mouse.x, .y = mouse.y };
	}

	static sf::Event::MouseButtonEvent toSFMLEventInternal(const MouseButton& mouseButton) {
		return { .button = static_cast<sf::Mouse::Button>(mouseButton.button),
						 .x			 = mouseButton.mouse.x,
						 .y			 = mouseButton.mouse.y };
	}


	static sf::Event toSFMLEventInternal(const Pressed<JoystickButton>& value) {
		return sf::Event{ .type = sf::Event::JoystickButtonPressed, .joystickButton = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Released<JoystickButton>& value) {
		return sf::Event{ .type = sf::Event::JoystickButtonReleased, .joystickButton = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Pressed<Key>& value) {
		return sf::Event{ .type = sf::Event::KeyPressed, .key = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Released<Key>& value) {
		return sf::Event{ .type = sf::Event::KeyReleased, .key = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Moved<JoystickAxis>& value) {
		return sf::Event{ .type = sf::Event::JoystickMoved, .joystickMove = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Moved<Mouse>& value) {
		return sf::Event{ .type = sf::Event::MouseMoved, .mouseMove = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Pressed<MouseButton>& value) {
		return sf::Event{ .type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const Released<MouseButton>& value) {
		return sf::Event{ .type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source) };
	}

	static sf::Event toSFMLEventInternal(const CloseWindow& /*unused*/) {
		return sf::Event{ .type = sf::Event::Closed, .size = {} };
	}

	std::optional<sf::Event> toSFMLEvent(const game::Event& event) {
		return std::visit(
			overloaded{ [&](const auto& value) -> std::optional<sf::Event> { return toSFMLEventInternal(value); },
									[&](const TimeElapsed & /*unused*/) -> std::optional<sf::Event> {// dummy event, for NOP
										return {};
									},
									[&](const std::monostate & /*unused*/) -> std::optional<sf::Event> {// dummy event for NOP
										return {};
									}

			},
			event);
	}

	Event toEvent(const sf::Event& event) {
		switch (event.type) {
		case sf::Event::Closed:
			return CloseWindow{};
		case sf::Event::JoystickButtonPressed:
			return Pressed<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
		case sf::Event::JoystickButtonReleased:
			return Released<JoystickButton>{ event.joystickButton.joystickId, event.joystickButton.button };
		case sf::Event::JoystickMoved:
			return Moved<JoystickAxis>{ event.joystickMove.joystickId,
																	static_cast<unsigned int>(event.joystickMove.axis),
																	event.joystickMove.position };
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
}// namespace game