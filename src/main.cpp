#include "ImGuiHelpers.h"
#include "input.h"
#include "utility.h"
#include <SFML/Graphics/CircleShape.hpp>
#include <array>
#include <docopt/docopt.h>
#include <imgui-SFML.h>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include <string>

// 1. must be white line before Options
// 2. must be two spaces between description and default
static constexpr auto USAGE =
	R"(The Game.
	Usage:
		game [options]

	Options:
		-h --help			Show this screen.
		--width=<WIDTH>			Screen width in pixels  [default: 1024].
		--height=<HEIGHT>		Screen height on pixels  [default: 768].
		--scale=<SCALE>			Scaling factor  [default: 1].
		--version			Show version.
)";

/*

 */
int main(int argc, const char **argv) {
	std::map<std::string, docopt::value> args		= docopt::docopt(USAGE,
																														 { std::next(argv), std::next(argv, argc) },
																														 true,// show help if requested
																														 "The Game 0.0");// version string
	const auto													 width	= args["--width"].asLong();
	const auto													 height = args["--height"].asLong();
	const auto													 scale	= args["--scale"].asLong();

	if (width < 0 || height < 0 || scale < 1 || scale > 5) {
		spdlog::error("Command line options are out of reasonable range.");
		for (auto const &arg : args) {
			if (arg.second.isString()) { spdlog::info("Parameter set: {}='{}'", arg.first, arg.second.asString()); }
		}
		abort();
	}
	spdlog::set_level(spdlog::level::debug);
	// Use the default logger (stdout, multi-threaded, colored)
	spdlog::info("Starting ImGui + SFML");
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)),
													"ImGui + SFML = <3");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	const auto scale_factor = static_cast<float>(scale);
	ImGui::GetStyle().ScaleAllSizes(scale_factor);
	ImGui::GetIO().FontGlobalScale					 = scale_factor;
	constexpr std::array						 roadMap = { "Create roadmap",
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
	std::array<bool, roadMap.size()> states	 = { false };

	game::GameState gs;

	while (window.isOpen()) {

		const auto event = gs.getNextEvent(window);
		if (const auto sfmlEvent = game::GameState::toSFMLEvent(event); sfmlEvent) {
			ImGui::SFML::ProcessEvent(*sfmlEvent);
		}

		bool joystickEvent = false;
		bool timeElapsed	 = false;
		std::visit(game::overloaded{ [&](const game::JoystickEvent auto &jsEvent) {
																	joystickEvent = true;
																	gs.update(jsEvent);
																},
																 [&](const game::GameState::CloseWindow & /*unused*/) { window.close(); },
																 [&](const game::GameState::TimeElapsed &te) {
																	 ImGui::SFML::Update(window, te.toSFMLTime());
																	 timeElapsed = true;
																 },
																 [&](const std::monostate & /*unused*/) {

																 },
																 [&](const auto & /*unused*/) {
																	 // do nothing
																 } },
							 event);
		if (!timeElapsed) {
			// todo : something more with a linear flow here
			continue;
		}
		ImGui::Begin("Road map");
		std::size_t index = 0;
		for (const auto &step : roadMap) {
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
			++index;
		}

		ImGui::End();
		// Joystick display
		ImGui::Begin("Joystick");
		if (!gs.joysticks.empty()) {
			ImGuiHelper::Text("Joystick Event: {}", joystickEvent);
			for (std::size_t button = 0; button < gs.joysticks[0].buttonCount; ++button) {
				ImGuiHelper::Text("{}: {}", button, gs.joysticks[0].buttonState[button]);
			}
			for (std::size_t axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
				ImGuiHelper::Text(
					"{}: {}", game::toString(static_cast<sf::Joystick::Axis>(axis)), gs.joysticks[0].axisPosition[axis]);
			}
		}
		ImGui::End();
		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
