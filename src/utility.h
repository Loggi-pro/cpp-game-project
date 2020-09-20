//
// Created by User on 19.09.2020.
//

#ifndef MYPROJECT_UTILITY_H
#define MYPROJECT_UTILITY_H
namespace game {
template<class... Ts>
struct overloaded : Ts... {
	using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;// not needed as of c++20

}// namespace game
#endif// MYPROJECT_UTILITY_H
