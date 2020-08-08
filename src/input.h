//
// Created by Loggi on 07.08.2020.
//
#ifndef MYPROJECT_INPUT_H
#define MYPROJECT_INPUT_H

#include <vector>
#include <string>
#include <string_view>
#include <array>
#include <SFML/Window/Joystick.hpp>
#include <variant>
#include <chrono>

namespace game {
    constexpr std::string_view toString(const sf::Joystick::Axis axis) {
        switch (axis) {
            case sf::Joystick::Axis::PovX:
                return "PovX";
            case sf::Joystick::Axis::PovY:
                return "PovY";
            case sf::Joystick::Axis::R:
                return "R";
            case sf::Joystick::Axis::U:
                return "U";
            case sf::Joystick::Axis::V:
                return "V";
            case sf::Joystick::Axis::X:
                return "X";
            case sf::Joystick::Axis::Y:
                return "Y";
            case sf::Joystick::Axis::Z:
                return "Z";
        }
        abort();
    }

    struct GameState {
        std::chrono::milliseconds msElapsed;

        struct Joystick {
            unsigned int id;
            std::string name;
            unsigned int buttonCount;
            std::array<bool, sf::Joystick::ButtonCount> buttonState;
            std::array<float, sf::Joystick::AxisCount> axisPosition;
        };

        std::vector<Joystick> joysticks{};

        static void refreshJoystick(Joystick &js) {

            sf::Joystick::update();

            for (unsigned int button = 0; button < js.buttonCount; ++button) {
                js.buttonState.at(button) = sf::Joystick::isButtonPressed(js.id, button);
            }

            for (unsigned int axis = 0; axis < sf::Joystick::AxisCount; ++axis) {
                js.axisPosition.at(axis) = sf::Joystick::getAxisPosition(js.id, static_cast<sf::Joystick::Axis>(axis));
            }
        }

        static Joystick loadJoystick(unsigned int id) {
            const auto identification = sf::Joystick::getIdentification(id);
            Joystick js{id, static_cast<std::string>(identification.name), sf::Joystick::getButtonCount(id), {}, {}};
            refreshJoystick(js);

            return js;
        }


        static Joystick &joystickById(std::vector<Joystick> &joysticks, unsigned int id) {
            auto joystick = std::find_if(begin(joysticks), end(joysticks), [id](const auto &j) { return j.id == id; });

            if (joystick == joysticks.end()) {
                joysticks.push_back(loadJoystick(id));
                return joysticks.back();
            } else {
                return *joystick;
            }
        }


        struct CloseWindow {
        };
        using Event = std::variant<Joystick, CloseWindow, std::nullopt_t>;

        Event processEvent(const sf::Event &event) {

            switch (event.type) {
                case sf::Event::Closed:
                    return CloseWindow{};
                case sf::Event::JoystickButtonPressed: {
                    auto &js = joystickById(joysticks, event.joystickButton.joystickId);
                    js.buttonState.at(event.joystickButton.button) = true;
                    return js;
                }
                case sf::Event::JoystickButtonReleased: {
                    auto &js = joystickById(joysticks, event.joystickButton.joystickId);
                    js.buttonState.at(event.joystickButton.button) = false;
                    return js;
                }
                case sf::Event::JoystickMoved: {
                    auto &js = joystickById(joysticks, event.joystickMove.joystickId);
                    js.axisPosition.at(event.joystickMove.axis) = event.joystickMove.position;
                    return js;
                }
                case sf::Event::KeyPressed: {

                    return {};
                }
                default:
                    //spdlog::trace("Unhandled Event Type");
                    return {};
            }
        }
    };
}// namespace game
#endif//MYPROJECT_INPUT_H
