#include "event_handler.h"
#include "event_recorder.h"
#include "event_serialize.h"
#include "game_state.h"
#include "render.h"
#include "utility.h"
#include <array>
#include <docopt/docopt.h>
#include <fstream>
#include <spdlog/spdlog.h>
#include <string>

// 1. must be white line before Options
// 2. must be two spaces between description and default
static constexpr auto USAGE =
	R"(The Game.
	Usage:
		game [options]

	Options:
		-h --help				Show this screen.
		--width=<WIDTH>			Screen width in pixels  [default: 1024].
		--height=<HEIGHT>		Screen height on pixels  [default: 768].
		--scale=<SCALE>			Scaling factor  [default: 1].
		--version				Show version.
		--replay=<EVENTFILE>	JSON file of events to play.
)";

/*

 */
int main(int argc, const char** argv) {
	std::map<std::string, docopt::value> args		= docopt::docopt(USAGE,
																														 { std::next(argv), std::next(argv, argc) },
																														 true,// show help if requested
																														 "The Game 0.0");// version string
	const auto													 width	= args["--width"].asLong();
	const auto													 height = args["--height"].asLong();
	const auto													 scale	= args["--scale"].asLong();

	if (width < 0 || height < 0 || scale < 1 || scale > 5) {
		spdlog::error("Command line options are out of reasonable range.");
		for (auto const& arg : args) {
			if (arg.second.isString()) { spdlog::info("Parameter set: {}='{}'", arg.first, arg.second.asString()); }
		}
		abort();
	}
	spdlog::set_level(spdlog::level::debug);
	// Use the default logger (stdout, multi-threaded, colored)
	spdlog::info("Starting ImGui + SFML");
	game::Render render{ width, height, static_cast<float>(scale) };

	game::GameState			gs;
	game::EventHandler	eventHandler;
	game::EventRecorder recorder;
	if (args["--replay"]) {
		const auto			eventFile = args["--replay"].asString();
		std::ifstream		ifs{ eventFile };
		game::EventList initialEvents;
		ifs >> initialEvents;
		eventHandler.loadEvents(std::move(initialEvents));
	}

	while (render.isOpen()) {

		const auto event = eventHandler.getNextEvent(render);
		recorder.processEvent(event);
		render.processEvent(event);
		gs.processEvent(event);

		std::visit(game::overloaded{ [&](const game::TimeElapsed& te) {},
																 [&](const std::monostate& /*unused*/) {},
																 [&](const auto& event) {
																	 // do nothing
																	 spdlog::info("Process event: {}", event.name);
																 } },
							 event);
		render.processRender(gs);
	}
	render.shutdown();

	recorder.printInfo();
	recorder.serialize("events.json");


	return EXIT_SUCCESS;
}
