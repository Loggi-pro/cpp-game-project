#pragma once
#include "event.h"
#include <SFML/Window/Event.hpp>
#include <optional>

namespace game {
	std::optional<sf::Event> toSFMLEvent(const Event& event);
	Event										 toEvent(const sf::Event& event);
}// namespace game