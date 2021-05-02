#pragma once
#include "event.h"
#include <iostream>
namespace game {
	std::istream& operator>>(std::istream& is, EventList& events);
	std::ostream& operator<<(std::ostream& os, const game::EventList& events);
}// namespace game