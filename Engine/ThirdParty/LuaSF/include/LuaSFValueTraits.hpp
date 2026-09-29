#pragma once

#include <LuaGlue/LuaGlue.hpp>
#include <SFML/Audio/Music.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Glsl.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Network/IpAddress.hpp>
#include <SFML/System/Angle.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/VideoMode.hpp>

template <> struct lua_glue::StructTraits<sf::Angle> : lua_glue::IndependentValue<sf::Angle> {};
template <> struct lua_glue::StructTraits<sf::Color> : lua_glue::IndependentValue<sf::Color> {};
template <> struct lua_glue::StructTraits<sf::ContextSettings> : lua_glue::IndependentValue<sf::ContextSettings> {};
template <> struct lua_glue::StructTraits<sf::Event> : lua_glue::IndependentValue<sf::Event> {};
template <> struct lua_glue::StructTraits<sf::Event::Closed> : lua_glue::IndependentValue<sf::Event::Closed> {};
template <> struct lua_glue::StructTraits<sf::Event::FocusGained> : lua_glue::IndependentValue<sf::Event::FocusGained> {};
template <> struct lua_glue::StructTraits<sf::Event::FocusLost> : lua_glue::IndependentValue<sf::Event::FocusLost> {};
template <> struct lua_glue::StructTraits<sf::Event::JoystickButtonPressed> : lua_glue::IndependentValue<sf::Event::JoystickButtonPressed> {};
template <> struct lua_glue::StructTraits<sf::Event::JoystickButtonReleased> : lua_glue::IndependentValue<sf::Event::JoystickButtonReleased> {};
template <> struct lua_glue::StructTraits<sf::Event::JoystickConnected> : lua_glue::IndependentValue<sf::Event::JoystickConnected> {};
template <> struct lua_glue::StructTraits<sf::Event::JoystickDisconnected> : lua_glue::IndependentValue<sf::Event::JoystickDisconnected> {};
template <> struct lua_glue::StructTraits<sf::Event::JoystickMoved> : lua_glue::IndependentValue<sf::Event::JoystickMoved> {};
template <> struct lua_glue::StructTraits<sf::Event::KeyPressed> : lua_glue::IndependentValue<sf::Event::KeyPressed> {};
template <> struct lua_glue::StructTraits<sf::Event::KeyReleased> : lua_glue::IndependentValue<sf::Event::KeyReleased> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseButtonPressed> : lua_glue::IndependentValue<sf::Event::MouseButtonPressed> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseButtonReleased> : lua_glue::IndependentValue<sf::Event::MouseButtonReleased> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseEntered> : lua_glue::IndependentValue<sf::Event::MouseEntered> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseLeft> : lua_glue::IndependentValue<sf::Event::MouseLeft> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseMoved> : lua_glue::IndependentValue<sf::Event::MouseMoved> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseMovedRaw> : lua_glue::IndependentValue<sf::Event::MouseMovedRaw> {};
template <> struct lua_glue::StructTraits<sf::Event::MouseWheelScrolled> : lua_glue::IndependentValue<sf::Event::MouseWheelScrolled> {};
template <> struct lua_glue::StructTraits<sf::Event::Resized> : lua_glue::IndependentValue<sf::Event::Resized> {};
template <> struct lua_glue::StructTraits<sf::Event::SensorChanged> : lua_glue::IndependentValue<sf::Event::SensorChanged> {};
template <> struct lua_glue::StructTraits<sf::Event::TextEntered> : lua_glue::IndependentValue<sf::Event::TextEntered> {};
template <> struct lua_glue::StructTraits<sf::Event::TouchBegan> : lua_glue::IndependentValue<sf::Event::TouchBegan> {};
template <> struct lua_glue::StructTraits<sf::Event::TouchEnded> : lua_glue::IndependentValue<sf::Event::TouchEnded> {};
template <> struct lua_glue::StructTraits<sf::Event::TouchMoved> : lua_glue::IndependentValue<sf::Event::TouchMoved> {};
template <> struct lua_glue::StructTraits<sf::IpAddress> : lua_glue::IndependentValue<sf::IpAddress> {};
template <> struct lua_glue::StructTraits<sf::Music::Span<sf::Time>> : lua_glue::IndependentValue<sf::Music::Span<sf::Time>> {};
template <> struct lua_glue::StructTraits<sf::Rect<float>> : lua_glue::IndependentValue<sf::Rect<float>> {};
template <> struct lua_glue::StructTraits<sf::Rect<int>> : lua_glue::IndependentValue<sf::Rect<int>> {};
template <> struct lua_glue::StructTraits<sf::Time> : lua_glue::IndependentValue<sf::Time> {};
template <> struct lua_glue::StructTraits<sf::Transform> : lua_glue::IndependentValue<sf::Transform> {};
template <> struct lua_glue::StructTraits<sf::Vector2<bool>> : lua_glue::IndependentValue<sf::Vector2<bool>> {};
template <> struct lua_glue::StructTraits<sf::Vector2<float>> : lua_glue::IndependentValue<sf::Vector2<float>> {};
template <> struct lua_glue::StructTraits<sf::Vector2<int>> : lua_glue::IndependentValue<sf::Vector2<int>> {};
template <> struct lua_glue::StructTraits<sf::Vector2<unsigned int>> : lua_glue::IndependentValue<sf::Vector2<unsigned int>> {};
template <> struct lua_glue::StructTraits<sf::Vector3<bool>> : lua_glue::IndependentValue<sf::Vector3<bool>> {};
template <> struct lua_glue::StructTraits<sf::Vector3<float>> : lua_glue::IndependentValue<sf::Vector3<float>> {};
template <> struct lua_glue::StructTraits<sf::Vector3<int>> : lua_glue::IndependentValue<sf::Vector3<int>> {};
template <> struct lua_glue::StructTraits<sf::Vector3<unsigned int>> : lua_glue::IndependentValue<sf::Vector3<unsigned int>> {};
template <> struct lua_glue::StructTraits<sf::Vertex> : lua_glue::IndependentValue<sf::Vertex> {};
template <> struct lua_glue::StructTraits<sf::VideoMode> : lua_glue::IndependentValue<sf::VideoMode> {};
template <> struct lua_glue::StructTraits<sf::priv::Matrix<3, 3>> : lua_glue::IndependentValue<sf::priv::Matrix<3, 3>> {};
template <> struct lua_glue::StructTraits<sf::priv::Matrix<4, 4>> : lua_glue::IndependentValue<sf::priv::Matrix<4, 4>> {};
template <> struct lua_glue::StructTraits<sf::priv::Vector4<bool>> : lua_glue::IndependentValue<sf::priv::Vector4<bool>> {};
template <> struct lua_glue::StructTraits<sf::priv::Vector4<float>> : lua_glue::IndependentValue<sf::priv::Vector4<float>> {};
template <> struct lua_glue::StructTraits<sf::priv::Vector4<int>> : lua_glue::IndependentValue<sf::priv::Vector4<int>> {};
