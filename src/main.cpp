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

int main([[maybe_unused]] int argc, [[maybe_unused]] const char **argv)
{
  //Use the default logger (stdout, multi-threaded, colored)
  spdlog::info("Starting ImGui + SFML");
  sf::RenderWindow window(sf::VideoMode(1024, 760), "ImGui + SFML = <3");
  window.setFramerateLimit(60);
  ImGui::SFML::Init(window);
  constexpr auto scale_factor = 2.0;
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
