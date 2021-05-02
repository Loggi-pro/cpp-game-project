#include "event_serialize.h"
#include "utility.h"
#include <nlohmann/json.hpp>
#include <string>

namespace nlohmann {
	template<>
	struct adl_serializer<game::Clock::duration> {
		static void to_json(nlohmann::json& j, const game::Clock::duration& duration) {
			j = nlohmann::json{ { "nanoseconds", std::chrono::nanoseconds{ duration }.count() } };
		}

		static void from_json(const nlohmann::json& j, game::Clock::duration& duration) {
			std::uint64_t value = j.at("nanoseconds");
			duration						= std::chrono::nanoseconds(value);
		}
	};

	template<>
	struct adl_serializer<sf::Keyboard::Key> {
		static void to_json(nlohmann::json& j, const sf::Keyboard::Key k) {
			j = nlohmann::json{ { "key", static_cast<int>(k) } };
		}

		static void from_json(const nlohmann::json& j, sf::Keyboard::Key& k) {
			k = static_cast<sf::Keyboard::Key>(j.at("key").get<int>());
		}
	};

}// namespace nlohmann

namespace game {
	template<typename EventType, typename... Param>
	void serialize(nlohmann::json& j, const Param&... param) {
		nlohmann::json innerObj;
		std::size_t		 index = 0;
		(innerObj.emplace(EventType::elements.at(index++), param), ...);
		nlohmann::json outerObj;
		outerObj.emplace(EventType::name, innerObj);
		j = outerObj;
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.empty()) {
		serialize<EventType>(j);
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.size() == 1) {
		const auto& [elem0] = event;
		serialize<EventType>(j, elem0);
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.size() == 2) {
		const auto& [elem0, elem1] = event;
		serialize<EventType>(j, elem0, elem1);
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.size() == 3) {
		const auto& [elem0, elem1, elem2] = event;
		serialize<EventType>(j, elem0, elem1, elem2);
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.size() == 4) {
		const auto& [elem0, elem1, elem2, elem3] = event;
		serialize<EventType>(j, elem0, elem1, elem2, elem3);
	}

	template<typename EventType>
	void to_json(nlohmann::json& j, const EventType& event) requires(EventType::elements.size() == 5) {
		const auto& [elem0, elem1, elem2, elem3, elem4] = event;
		serialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
	}

	void to_json(nlohmann::json& j, const game::Event& event) {
		std::visit(game::overloaded{ [](const std::monostate& /*unused*/) {}, [&j](const auto& e) { to_json(j, e); } },
							 event);
	}

	template<typename EventType, typename... Param>
	void deserialize(const nlohmann::json& j, Param&... param) {
		const auto& top = j.at(std::string{ EventType::name });
		if (top.size() != sizeof...(Param)) { throw std::logic_error("Deserialization size mismatch"); }
		std::size_t cur_elem = 0;
		(top.at(std::string{ EventType::elements[cur_elem++] }).get_to(param), ...);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 0) {
		deserialize<EventType>(j);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 1) {
		auto& [elem0] = event;
		deserialize<EventType>(j, elem0);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 2) {
		auto& [elem0, elem1] = event;
		deserialize<EventType>(j, elem0, elem1);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 3) {
		auto& [elem0, elem1, elem2] = event;
		deserialize<EventType>(j, elem0, elem1, elem2);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 4) {
		auto& [elem0, elem1, elem2, elem3] = event;
		deserialize<EventType>(j, elem0, elem1, elem2, elem3);
	}

	template<typename EventType>
	void from_json(const nlohmann::json& j, EventType& event) requires(EventType::elements.size() == 5) {
		auto& [elem0, elem1, elem2, elem3, elem4] = event;
		deserialize<EventType>(j, elem0, elem1, elem2, elem3, elem4);
	}

	template<typename... T>
	void choose_variant(const nlohmann::json& j, std::variant<std::monostate, T...>& variant) {
		bool matched		 = false;
		auto try_variant = [&]<typename Variant>() {
			if (!matched) {
				try {
					Variant obj;
					from_json(j, obj);
					variant = obj;
					matched = true;
				} catch (const std::exception&) {
					// parse error, continue
				}
			}
		};
		(try_variant.template operator()<T>(), ...);
	}

	void from_json(const nlohmann::json& j, game::Event& event) {
		choose_variant(j, event);
	}

	std::istream& operator>>(std::istream& is, EventList& events) {
		const auto j = nlohmann::json::parse(is);
		events			 = j.get<EventList>();
		return is;
	}

	std::ostream& operator<<(std::ostream& os, const game::EventList& events) {
		nlohmann::json serialized(events);
		os << serialized;
		return os;
	}
}// namespace game
