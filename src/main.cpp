#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <imgui.h>
#include <imgui-SFML.h>
#include <array>
#include "ImGuiHelpers.h"
#include "input.h"
//1. must be white line before Options
//2. must be two spaces between description and default
static constexpr auto USAGE =
	R"(The Game.
	Usage:
		game [options]

	Options:
		-h --help			Show this screen.
		--width=<WIDTH>			Screen width in pixels  [default: 1024].
		--height=<HEIGHT>		Screen height on pixels  [default: 768].
		--scale=<SCALE>			Scaling factor  [default: 2].
		--version			Show version.
)";


/*

 */
int main(int argc, const char **argv) {
	std::map<std::string, docopt::value> args = docopt::docopt(USAGE,
		{ std::next(argv), std::next(argv, argc) },
		true,//show help if requested
		"The Game 0.0");//version string
	const auto width = args["--width"].asLong();
	const auto height = args["--height"].asLong();
	const auto scale = args["--scale"].asLong();

	if (width < 0 || height < 0 || scale < 1 || scale > 5) {
		spdlog::error("Command line options are out of reasonable range.");
		for (auto const &arg : args) {
			if (arg.second.isString()) {
				spdlog::info("Parameter set: {}='{}'", arg.first, arg.second.asString());
			}
		}
		abort();
	}
	spdlog::set_level(spdlog::level::debug);
	//Use the default logger (stdout, multi-threaded, colored)
	spdlog::info("Starting ImGui + SFML");
	sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(width), static_cast<unsigned int>(height)), "ImGui + SFML = <3");
	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	const auto scale_factor = static_cast<float>(scale);
	ImGui::GetStyle().ScaleAllSizes(scale_factor);
	ImGui::GetIO().FontGlobalScale = scale_factor;
	constexpr std::array roadMap = {
		"Create roadmap",
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
		"Porting From SFML To ..."
	};
	std::array<bool, roadMap.size()> states = { false };


	std::vector<game::Joystick> joysticks;
	bool joystickEvent{ false };
	sf::Clock deltaClock;

	while (window.isOpen()) {
		sf::Event event{};

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			switch (event.type) {
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::JoystickConnected: {
				//const auto &js = joystickById(joysticks, event.joystickConnect.joystickId);
				joystickEvent = true;
				break;
			}
			case sf::Event::JoystickDisconnected: {
				//const auto &js = joystickById(joysticks, event.joystickConnect.joystickId);
				joystickEvent = true;
				break;
			}
			case sf::Event::JoystickButtonPressed: {
				auto &js = game::joystickById(joysticks, event.joystickButton.joystickId);
				js.buttonState.at(event.joystickButton.button) = true;
				joystickEvent = true;
				break;
			}
			case sf::Event::JoystickButtonReleased: {
				auto &js = game::joystickById(joysticks, event.joystickButton.joystickId);
				js.buttonState.at(event.joystickButton.button) = false;
				joystickEvent = true;
				break;
			}
			case sf::Event::JoystickMoved: {
				auto &js = game::joystickById(joysticks, event.joystickMove.joystickId);
				js.axisPosition.at(event.joystickMove.axis) = event.joystickMove.position;
				joystickEvent = true;
				break;
			}
			default:
				//spdlog::trace("Unhandled Event Type");
				break;
			}
		}

		ImGui::SFML::Update(window, deltaClock.restart());
		ImGui::Begin("Road map");
		std::size_t index = 0;
		for (const auto &step : roadMap) {
			ImGui::Checkbox(fmt::format("{} : {}", index, step).c_str(), &states.at(index));
			++index;
		}

		ImGui::End();
		//Joystick display
		ImGui::Begin("Joystick");
		ImGui::TextUnformatted(fmt::format("JS Event: {}", joystickEvent).c_str());
		if (!joysticks.empty()) {
			for (std::size_t button = 0; button < joysticks[0].buttonCount; ++button) {
				ImGuiHelper::Text("{}: {}", button, joysticks[0].buttonState[button]);
			}
			for (std::size_t axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
				ImGuiHelper::Text("{}: {}", game::toString(static_cast<sf::Joystick::Axis>(axis)), joysticks[0].axisPosition[axis]);
			}
		}
		ImGui::End();
		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
