#pragma once
#include <fmt/format.h>
#include <imgui.h>
#include <string_view>

namespace ImGuiHelper {
	template<typename... Param>
	static void Text(std::string_view format, Param&&... param) {
		ImGui::TextUnformatted(fmt::format(format, std::forward<Param>(param)...).c_str());
	}
}// namespace ImGuiHelper
