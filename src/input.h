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
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include <thread>

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
		using Clock = std::chrono::steady_clock;
		Clock::time_point _lastTick = game::GameState::Clock::now();
		std::chrono::milliseconds _msElapsed;


		struct CloseWindow {
			constexpr static std::string_view name{"CloseWindow"};
			constexpr static std::array<std::string_view, 0> elements{};
		};

		struct TimeElapsed {
			constexpr static std::string_view name{"TimeElapsed"};
			constexpr static auto elements = std::to_array<std::string_view>({"elapsed"});
			Clock::duration elapsed;

			[[nodiscard]] sf::Time toSFMLTime() const {
				return sf::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count());
			}
		};

		template<typename Source>
		struct Pressed {
			constexpr static std::string_view name{"Pressed"};
			constexpr static auto elements = std::to_array<std::string_view>({"source"});
			Source source;
		};
		template<typename Source>
		struct Released {
			constexpr static std::string_view name{"Released"};
			constexpr static auto elements = std::to_array<std::string_view>({"source"});
			Source source;
		};

		template<typename Source>
		struct Moved {
			constexpr static std::string_view name{"Moved"};
			constexpr static auto elements = std::to_array<std::string_view>({"source"});
			Source source;
		};

		struct JoystickButton {
			constexpr static std::string_view name{"JoystickButton"};
			constexpr static auto elements = std::to_array<std::string_view>({"id", "button"});
			unsigned int id;
			unsigned int button;
		};

		struct JoystickAxis {
			constexpr static std::string_view name{"JoystickAxis"};
			constexpr static auto elements = std::to_array<std::string_view>({"id", "axis", "position"});
			unsigned int id;
			unsigned int axis;
			float position;
		};

		struct Mouse {
			constexpr static std::string_view name{"Mouse"};
			constexpr static auto elements = std::to_array<std::string_view>({"x", "y"});
			int x;
			int y;
		};
		struct MouseButton {
			constexpr static std::string_view name{"MouseButton"};
			constexpr static auto elements = std::to_array<std::string_view>({"button", "mouse"});
			int button;
			Mouse mouse;
		};

		struct Key {
			constexpr static std::string_view name{"Key"};
			constexpr static auto elements = std::to_array<std::string_view>(
					{"alt", "control", "system", "shift", "key"});
			bool alt, control, system, shift;
			sf::Keyboard::Key key;
		};

		struct Joystick {
			unsigned int id;
			std::string name;
			unsigned int buttonCount;
			std::array<bool, sf::Joystick::ButtonCount> buttonState;
			std::array<float, sf::Joystick::AxisCount> axisPosition;
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
			Joystick js{id, static_cast<std::string>(identification.name), sf::Joystick::getButtonCount(id), {}, {}};
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
			auto &js = joystickById(joysticks, button.source.id);
			js.buttonState.at(button.source.button) = true;
		};

		void update(const Released<JoystickButton> &button) {
			auto &js = joystickById(joysticks, button.source.id);
			js.buttonState.at(button.source.button) = false;
		};

		void update(const Moved<JoystickAxis> &joy) {
			auto &js = joystickById(joysticks, joy.source.id);
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
			return {.code = key.key, .alt = key.alt, .control = key.control, .shift = key.shift, .system = key.system};
		}

		static sf::Event::JoystickButtonEvent toSFMLEventInternal(const JoystickButton &joy) {
			return {.joystickId = joy.id, .button = joy.button};
		}

		static sf::Event::JoystickMoveEvent toSFMLEventInternal(const JoystickAxis &joy) {
			return {.joystickId = joy.id, .axis = static_cast<sf::Joystick::Axis>(joy.axis), .position = joy.position};
		}

		static sf::Event::MouseMoveEvent toSFMLEventInternal(const Mouse &mouse) {
			return {.x = mouse.x, .y = mouse.y};
		}

		static sf::Event::MouseButtonEvent toSFMLEventInternal(const MouseButton &mouseButton) {
			return {.button = static_cast<sf::Mouse::Button>(mouseButton.button),
					.x             = mouseButton.mouse.x,
					.y             = mouseButton.mouse.y};
		}


		static sf::Event toSFMLEventInternal(const Pressed<JoystickButton> &value) {
			return sf::Event{.type = sf::Event::JoystickButtonPressed, .joystickButton = toSFMLEventInternal(
					value.source)};
		}

		static sf::Event toSFMLEventInternal(const Released<JoystickButton> &value) {
			return sf::Event{.type = sf::Event::JoystickButtonReleased, .joystickButton = toSFMLEventInternal(
					value.source)};
		}

		static sf::Event toSFMLEventInternal(const Pressed<Key> &value) {
			return sf::Event{.type = sf::Event::KeyPressed, .key = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const Released<Key> &value) {
			return sf::Event{.type = sf::Event::KeyReleased, .key = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const Moved<JoystickAxis> &value) {
			return sf::Event{.type = sf::Event::JoystickMoved, .joystickMove = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const Moved<Mouse> &value) {
			return sf::Event{.type = sf::Event::MouseMoved, .mouseMove = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const Pressed<MouseButton> &value) {
			return sf::Event{.type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const Released<MouseButton> &value) {
			return sf::Event{.type = sf::Event::MouseButtonPressed, .mouseButton = toSFMLEventInternal(value.source)};
		}

		static sf::Event toSFMLEventInternal(const CloseWindow & /*unused*/) {
			return sf::Event{.type = sf::Event::Closed, .size = {}};
		}


		static std::optional<sf::Event> toSFMLEvent(const Event &event) {
			return std::visit(
					overloaded{
							[&](const auto &value) -> std::optional<sf::Event> { return toSFMLEventInternal(value); },
							[&](const TimeElapsed & /*unused*/) -> std::optional<sf::Event> {// dummy event, for NOP
								return {};
							},
							[&](const std::monostate & /*unused*/) -> std::optional<sf::Event> {// dummy event for NOP
								return {};
							}

					},
					event);
		}

		std::vector<Event> _pendingEvents;

		void setEvents(const std::vector<Event> &events) {
			_pendingEvents = std::move(events);
		}


		Event getNextEvent(sf::RenderWindow &window) {
			if (!_pendingEvents.empty()) {
				auto event = _pendingEvents.front();
				_pendingEvents.erase(_pendingEvents.begin());
				std::visit(overloaded{
						[](const TimeElapsed &te) {
							std::this_thread::sleep_for(te.elapsed);
						},
						[&](const Moved<Mouse> &me) {
							sf::Mouse::setPosition({me.source.x, me.source.y}, window);
						},
						[](const auto &) {

						}
				}, event);
				return event;
			}
			sf::Event event{};
			if (window.pollEvent(event)) { return toEvent(event); }

			const auto nextTick = Clock::now();
			const auto timeElapsed = nextTick - _lastTick;
			_lastTick = nextTick;

			return TimeElapsed{timeElapsed};
			// gs.timeElapsed = timeElapsed;
		}


		Event toEvent(const sf::Event &event) {
			switch (event.type) {
				case sf::Event::Closed:
					return CloseWindow{};
				case sf::Event::JoystickButtonPressed:
					return Pressed<JoystickButton>{event.joystickButton.joystickId, event.joystickButton.button};
				case sf::Event::JoystickButtonReleased:
					return Released<JoystickButton>{event.joystickButton.joystickId, event.joystickButton.button};
				case sf::Event::JoystickMoved:
					return Moved<JoystickAxis>{event.joystickMove.joystickId,
											   static_cast<unsigned int>(event.joystickMove.axis),
											   event.joystickMove.position};
					// Mouse
				case sf::Event::MouseButtonPressed:
					return Pressed<MouseButton>{event.mouseButton.button, {event.mouseButton.x, event.mouseButton.y}};
				case sf::Event::MouseButtonReleased:
					return Released<MouseButton>{event.mouseButton.button, {event.mouseButton.x, event.mouseButton.y}};
					//		case sf::Event::MouseEntered:
					//		case sf::Event::MouseLeft:
				case sf::Event::MouseMoved:
					return Moved<Mouse>{event.mouseMove.x, event.mouseMove.y};
					//		case sf::Event::MouseWheelScrolled:
					// Keyboard
				case sf::Event::KeyPressed:
					return Pressed<Key>{event.key.alt, event.key.control, event.key.system, event.key.shift,
										event.key.code};
				case sf::Event::KeyReleased:
					return Released<Key>{event.key.alt, event.key.control, event.key.system, event.key.shift,
										 event.key.code};
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
					GameState::JoystickButton>> || std::is_same_v<T, GameState::Released<GameState::JoystickButton>> ||
	std::is_same_v<T, GameState::Moved<GameState::JoystickAxis>>;


}// namespace game


namespace nlohmann {
	template<>
	struct adl_serializer<game::GameState::Clock::duration> {
		static void to_json(nlohmann::json &j, const game::GameState::Clock::duration &duration) {
			j = nlohmann::json{{"nanoseconds", std::chrono::nanoseconds{duration}.count()}};
		}

		static void from_json(const nlohmann::json &j, game::GameState::Clock::duration &duration) {
			std::uint64_t value = j.at("nanoseconds");
			duration = std::chrono::nanoseconds(value);
		}
	};

	template<>
	struct adl_serializer<sf::Keyboard::Key> {
		static void to_json(nlohmann::json &j, const sf::Keyboard::Key k) {
			j = nlohmann::json{{"key", static_cast<int>(k)}};
		}

		static void from_json(const nlohmann::json &j, sf::Keyboard::Key &k) {
			k = static_cast<sf::Keyboard::Key>(j.at("key").get<int>());
		}
	};
}// namespace nlohmann


namespace game {


	template<typename EventType, typename ...Param>
	void serialize(nlohmann::json &j, const Param &... param) {
		nlohmann::json innerObj;
		std::size_t index = 0;
		(innerObj.emplace(EventType::elements.at(index++), param), ...);
		nlohmann::json outerObj;
		outerObj.emplace(EventType::name, innerObj);
		j = outerObj;
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.empty()) {
		serialize<EventType>(j);
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.size() == 1) {
		const auto &[elem0] =event;
		serialize<EventType>(j, elem0);
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.size() == 2) {
		const auto &[elem0, elem1] =event;
		serialize<EventType>(j, elem0, elem1);
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.size() == 3) {
		const auto &[elem0, elem1, elem2] =event;
		serialize<EventType>(j, elem0, elem1, elem2);
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.size() == 4) {
		const auto &[elem0, elem1, elem2, elem3] =event;
		serialize<EventType>(j, elem0, elem1, elem2, elem3);
	}

	template<typename EventType>
	void to_json(nlohmann::json &j, const EventType &event) requires (EventType::elements.size() == 5) {
		const auto &[elem0, elem1, elem2, elem3, elem4] =event;
		serialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
	}

	void to_json(nlohmann::json &j, const game::GameState::Event &event) {
		std::visit(game::overloaded{[](const std::monostate & /*unused*/) {},
									[&j](const auto &e) { to_json(j, e); }
		}, event);
	}

	template<typename EventType, typename ...Param>
	void deserialize(const nlohmann::json &j, Param &... param) {
		const auto &top = j.at(std::string{EventType::name});
		if (top.size() != sizeof...(Param)) {
			throw std::logic_error("Deserialization size mismatch");
		}
		std::size_t cur_elem = 0;
		(top.at(std::string{EventType::elements[cur_elem++]}).get_to(param), ...);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 0) {
		deserialize<EventType>(j);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 1) {
		auto &[elem0] = event;
		deserialize<EventType>(j, elem0);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 2) {
		auto &[elem0, elem1] = event;
		deserialize<EventType>(j, elem0, elem1);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 3) {
		auto &[elem0, elem1, elem2] = event;
		deserialize<EventType>(j, elem0, elem1, elem2);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 4) {
		auto &[elem0, elem1, elem2, elem3] = event;
		deserialize<EventType>(j, elem0, elem1, elem2, elem3);
	}

	template<typename EventType>
	void from_json(const nlohmann::json &j, EventType &event) requires (EventType::elements.size() == 5) {
		auto &[elem0, elem1, elem2, elem3, elem4] = event;
		deserialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
	}

	template<typename ...T>
	void choose_variant(const nlohmann::json &j, std::variant<std::monostate, T...> &variant) {
		bool matched = false;
		auto try_variant = [&] < typename Variant > () {
			if (!matched) {
				try {
					Variant obj;
					from_json(j, obj);
					variant = obj;
					matched = true;
				} catch (const std::exception &) {
					//parse error, continue
				}
			}
		};
		(try_variant.template operator()<T>(), ...);
	}

	void from_json(const nlohmann::json &j, game::GameState::Event &event) {
		choose_variant(j, event);
	}
}// namespace game

#endif// MYPROJECT_INPUT_H
