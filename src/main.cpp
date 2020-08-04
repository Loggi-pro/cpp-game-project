#include <imgui.h>
#include <imgui-SFML.h>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <spdlog/spdlog.h>
#include <docopt/docopt.h>
#include <functional>
#include <iostream>
#include <array>
#include <string>

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
int main(int argc, const char **argv)
{
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
		"Reading SFML Input States",
		"Managing Game State",
		"Making Game Testable",
		"Making Game State Allocator",
		"Add Logging to Game Engine",
		"Draw A Game Map",
		"Dialog Trees",
		"Porting From SFML To ..."
	};
	std::array<bool, roadMap.size()> states = { false };
	sf::Clock deltaClock;

	while (window.isOpen()) {
		sf::Event event{};

		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);

			if (event.type == sf::Event::Closed) {
				window.close();
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
		window.clear();
		ImGui::SFML::Render(window);
		window.display();
	}

	ImGui::SFML::Shutdown();
}
