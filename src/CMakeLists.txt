# Generic test that uses conan libs
add_executable(game main.cpp input.h ImGuiHelpers.h utility.h)

if (ENABLE_PCH)
    # This sets a global PCH parameter, each project will build its own PCH, which is a good idea if any #define's change
    #
    # consider breaking this out per project as necessary
    target_precompile_headers(
            game
            INTERFACE
            <vector>
            <string>
            <map>
            <utility>
            <string_view>
            <array>
            <docopt/docopt.h>
            <imgui-SFML.h>
            <imgui.h>
            <spdlog/spdlog.h>
            <fmt/format.h>
            <SFML/Graphics/RenderWindow.hpp>
            <SFML/Window/Event.hpp>
    )

endif ()

target_link_libraries(
        game
        PRIVATE project_options
        project_warnings
        CONAN_PKG::docopt.cpp
        CONAN_PKG::fmt
        CONAN_PKG::spdlog
        CONAN_PKG::imgui-sfml
        CONAN_PKG::nlohmann_json
)

