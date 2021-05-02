#include "event_recorder.h"
#include "event_serialize.h"
#include "utility.h"
#include <fstream>
#include <spdlog/spdlog.h>

namespace game {
	void EventRecorder::processEvent(const Event& ev) {
		std::visit(
			game::overloaded{ [](game::TimeElapsed& prev, const game::TimeElapsed& next) { prev.elapsed += next.elapsed; },
												[&](const auto& /*prev*/, const std::monostate& /*unused*/) {},
												[&](const auto& /*prev*/, const auto& next) { _events.push_back(next); } },
			_events.back(),
			ev);

		++_eventsProcessed;
	}
	void EventRecorder::printInfo() const {

		spdlog::info("Total events processed: {}, total recorded {}", _eventsProcessed, _events.size());

		for (const auto& event : _events) {
			std::visit(game::overloaded{ [](const auto& event_obj) { spdlog::info("Event: {}", event_obj.name); },
																	 [](const std::monostate& /*unused*/) { spdlog::info("monostate"); } },
								 event);
		}
	}

	void EventRecorder::serialize(std::string_view fileName) const {
		std::ofstream ofs{ fileName.data() };
		ofs << _events;
	}

}// namespace game
