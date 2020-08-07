//
// Created by loggi on 07.08.2020.
//
#ifndef MYPROJECT_IMGUIHELPERS_H
#define MYPROJECT_IMGUIHELPERS_H

#include <fmt/format.h>
#include <string_view>
#include <imgui.h>

namespace ImGuiHelper {
template<typename... Param>
static void Text(std::string_view format, Param &&... param) {
	ImGui::TextUnformatted(fmt::format(format, std::forward<Param>(param)...).c_str());
}
}// namespace ImGuiHelper
#endif//MYPROJECT_IMGUIHELPERS_H
