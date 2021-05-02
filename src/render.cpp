#include "render.h"
#include "ImGuiHelpers.h"
#include "event_sfml.h"
#include "game_state.h"
#include "utility.h"
#include <fmt/format.h>
#include <imgui-SFML.h>
#include <imgui.h>

static constexpr std::array							roadMap = { "Create roadmap",
																				"Create project",
																				"Use C++20 features",
																				"Handling command line arguments",
																				"Reading SFML Joystick States",
																				"Reading SFML Keyboard States",
																				"Reading SFML Mouse States",
																				"Reading SFML Touchscreen States",
																				"Managing Game State",
																				"Making Game Testable",
																				"Making Game State Allocator",
																				"Add Logging to Game Engine",
																				"Draw A Game Map",
																				"Dialog Trees",
																				"Porting From SFML To ..." };
static std::array<bool, roadMap.size()> states	= { false };

namespace game {

	[[nodiscard]] static sf::Time toSFMLTime(const TimeElapsed& time) {
		return sf::microseconds(std::chrono::duration_cast<std::chrono::microseconds>(time.elapsed).count());
	}

	void Render::processEvent(const Event& ev) {

		if (const auto sfmlEvent = game::toSFMLEvent(ev); sfmlEvent) { ImGui::SFML::ProcessEvent(*sfmlEvent); }

		_timeElapsed = false;

		std::visit(game::overloaded{ [&](const game::JoystickEvent auto& jsEvent) { _isJoystickEvent = true; },
																 [&](const game::CloseWindow& /*unused*/) { window.close(); },
																 [&](const game::TimeElapsed& te) {
																	 ImGui::SFML::Update(window, game::toSFMLTime(te));
																	 _timeElapsed = true;
																 },
																 [&](const auto& other) {
																	 // do nothing
																 } },
							 ev);
	}

	Render::Render(int width, int height, float scale)
		: window{ sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "ImGui + SFML = <3" } {
		window.setFramerateLimit(FRAMERATE_LIMIT);
		ImGui::SFML::Init(window);

		const auto scale_factor = scale;
		ImGui::GetStyle().ScaleAllSizes(scale_factor);
		ImGui::GetIO().FontGlobalScale = scale_factor;
	}

	std::optional<Event> Render::getEvent() {
		sf::Event event{};
		if (window.pollEvent(event)) { return toEvent(event); }
		return {};
	}

	void Render::processRender(const GameState& gs) {
		if (!_timeElapsed) {
			// TODO : something more with a linear flow here
			return;
		}

		ImGui::Begin("Road map");
		std::size_t index = 0;
		for (const auto& step : roadMap) {
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
			++index;
		}

		ImGui::End();
		// Joystick display
		ImGui::Begin("Joystick");
		if (!gs._input.joysticks.empty()) {
			ImGuiHelper::Text("Joystick Event: {}", _isJoystickEvent);
			_isJoystickEvent = false;
			for (std::size_t button = 0; button < gs._input.joysticks[0].buttonCount; ++button) {
				ImGuiHelper::Text("{}: {}", button, gs._input.joysticks[0].buttonState[button]);
			}
			for (std::size_t axis = 0; axis < sf::Joystick::AxisCount; ++axis) {

				ImGuiHelper::Text(
					"{}: {}", game::toString(static_cast<sf::Joystick::Axis>(axis)), gs._input.joysticks[0].axisPosition[axis]);
			}
		}
		ImGui::End();
		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}
	void Render::shutdown() {
		ImGui::SFML::Shutdown();
	}


}// namespace game
