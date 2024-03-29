/*
* This file is part of the ungod - framework.
* Copyright (C) 2016 Felix Becker - fb132550@uni-greifswald.de
*
* This software is provided 'as-is', without any express or
* implied warranty. In no event will the authors be held
* liable for any damages arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute
* it freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented;
*    you must not claim that you wrote the original software.
*    If you use this software in a product, an acknowledgment
*    in the product documentation would be appreciated but
*    is not required.
*
* 2. Altered source versions must be plainly marked as such,
*    and must not be misrepresented as being the original software.
*
* 3. This notice may not be removed or altered from any
*    source distribution.
*/

#include "ungod/script/registration/RegisterInput.h"
#include "ungod/script/registration/RegistrationDetail.h"
#include "ungod/base/Input.h"

namespace ungod
{
    class Entity;

    namespace scriptRegistration
    {
        void registerInput(ScriptStateBase& state)
        {
            state.registerEnum<InputManager::InputType>("InputType",
                                     {{"MouseInput", InputManager::InputType::MOUSE_INPUT},
                                     {"KeyboardInput", InputManager::InputType::KEYBOARD_INPUT}});

            state.registerUsertype<InputManager>("InputManager",
                                                       "loadBindings", &InputManager::loadBindings,
                                                       "addBinding", &InputManager::addBinding,
                                                       "clearBindings", &InputManager::clearBindings);

            state.registerEnum<sf::Mouse::Button>("MouseButtons",
                                     {{"Left", sf::Mouse::Button::Left},
                                     {"Right", sf::Mouse::Button::Right}});

            state.registerEnum<sf::Keyboard::Key>("Keys",
                                     {{"A", sf::Keyboard::A}, {"B", sf::Keyboard::B}, {"C", sf::Keyboard::C}, {"D", sf::Keyboard::D},
                                     {"E", sf::Keyboard::E}, {"F", sf::Keyboard::F}, {"G", sf::Keyboard::G}, {"H", sf::Keyboard::H},
                                     {"I", sf::Keyboard::I}, {"J", sf::Keyboard::J}, {"K", sf::Keyboard::K}, {"L", sf::Keyboard::L},
                                     {"M", sf::Keyboard::M}, {"N", sf::Keyboard::N}, {"P", sf::Keyboard::P}, {"Q", sf::Keyboard::Q},
                                     {"R", sf::Keyboard::R}, {"S", sf::Keyboard::S}, {"T", sf::Keyboard::T}, {"U", sf::Keyboard::U},
                                     {"V", sf::Keyboard::V}, {"W", sf::Keyboard::W}, {"X", sf::Keyboard::X}, {"Y", sf::Keyboard::Y},
                                     {"Z", sf::Keyboard::Z},
                                     {"Num0", sf::Keyboard::Num0}, {"Num1", sf::Keyboard::Num1}, {"Num2", sf::Keyboard::Num2}, {"Num3", sf::Keyboard::Num3},
                                     {"Num4", sf::Keyboard::Num4}, {"Num5", sf::Keyboard::Num5}, {"Num6", sf::Keyboard::Num6}, {"Num7", sf::Keyboard::Num7},
                                     {"Num8", sf::Keyboard::Num8}, {"Num9", sf::Keyboard::Num9},
                                     {"LAlt", sf::Keyboard::LAlt}, {"RAlt", sf::Keyboard::RAlt},
                                     {"Space", sf::Keyboard::Space}, {"Tab", sf::Keyboard::Tab},
                                     {"LControl", sf::Keyboard::LControl}, {"Rcontrol", sf::Keyboard::RControl},
                                     {"Right", sf::Keyboard::Right}, {"Left", sf::Keyboard::Left}, {"Up", sf::Keyboard::Up}, {"Down", sf::Keyboard::Down},
                                     {"F1", sf::Keyboard::F1}, {"F2", sf::Keyboard::F2}, {"F3", sf::Keyboard::F3}, {"F4", sf::Keyboard::F4},
                                     {"F5", sf::Keyboard::F5}, {"F6", sf::Keyboard::F6}, {"F7", sf::Keyboard::F7}, {"F8", sf::Keyboard::F8},
                                     {"F9", sf::Keyboard::F9}, {"F10", sf::Keyboard::F10}, {"F11", sf::Keyboard::F11}, {"F12", sf::Keyboard::F12}} );
        }
    }
}
