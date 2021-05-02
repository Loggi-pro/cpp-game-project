#pragma once
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <array>
#include <chrono>
#include <string_view>
#include <variant>
#include <vector>

namespace game {
	using Clock = std::chrono::steady_clock;

	struct CloseWindow {
		constexpr static std::string_view								 name{ "CloseWindow" };
		constexpr static std::array<std::string_view, 0> elements{};
	};

	struct TimeElapsed {
		constexpr static std::string_view name{ "TimeElapsed" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "elapsed" });
		Clock::duration										elapsed;
	};

	template<typename Source>
	struct Pressed {
		constexpr static std::string_view name{ "Pressed" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "source" });
		Source														source;
	};
	template<typename Source>
	struct Released {
		constexpr static std::string_view name{ "Released" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "source" });
		Source														source;
	};

	template<typename Source>
	struct Moved {
		constexpr static std::string_view name{ "Moved" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "source" });
		Source														source;
	};

	struct JoystickButton {
		constexpr static std::string_view name{ "JoystickButton" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "id", "button" });
		unsigned int											id;
		unsigned int											button;
	};

	struct JoystickAxis {
		constexpr static std::string_view name{ "JoystickAxis" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "id", "axis", "position" });
		unsigned int											id;
		unsigned int											axis;
		float															position;
	};

	struct Mouse {
		constexpr static std::string_view name{ "Mouse" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "x", "y" });
		int																x;
		int																y;
	};
	struct MouseButton {
		constexpr static std::string_view name{ "MouseButton" };
		constexpr static auto							elements = std::to_array<std::string_view>({ "button", "mouse" });
		int																button;
		Mouse															mouse;
	};

	struct Key {
		constexpr static std::string_view name{ "Key" };
		constexpr static auto elements = std::to_array<std::string_view>({ "alt", "control", "system", "shift", "key" });
		bool									alt, control, system, shift;
		sf::Keyboard::Key			key;
	};


	template<typename T>
	concept JoystickEvent =
		std::is_same_v<T, Pressed<JoystickButton>> || std::is_same_v<T, Released<JoystickButton>> || std::is_same_v<T, Moved<JoystickAxis>>;

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

	struct EventList : public std::vector<Event> {
		EventList(std::initializer_list<Event> init)
			: std::vector<Event>{ init } {}
		EventList() = default;
	};
}// namespace game