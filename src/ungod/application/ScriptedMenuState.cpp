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

#include "ungod/application/ScriptedMenuState.h"
#include "ungod/application/Application.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/visual/RenderLayer.h"

namespace ungod
{
        constexpr std::array<const char*, ScriptedMenuState::MenuScriptCallbacks::NUM_CALLBACK> ScriptedMenuState::MENU_CALLBACK_IDENTIFIERS;

        ScriptedMenuState::ScriptedMenuState(Application& app, StateID id, const std::string& scriptID) :
            State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(MENU_CALLBACK_IDENTIFIERS), std::end(MENU_CALLBACK_IDENTIFIERS) } ),
            mGui(app.getWindow()),
            mSoundHandler(),
            mCamera(app.getWindow())
        {
            //set up the script behavior
            mScriptCallbacks.loadScript(scriptID);

            mTargetSizeChangedLink = app.onTargetSizeChanged([this, &app](const sf::Vector2u& ts)
            {
                    mGui.setView(sf::View{ sf::FloatRect{0.0f, 0.0f, float(ts.x), float(ts.y)} });
            });

            mScriptStateChangedLink = mApp.onScriptStateChanged([this]()
                {
                    mScriptCallbacks.reload(mApp.getScriptState(), mApp.getGlobalScriptEnv());
                });

            mSoundHandler.init(mApp.getSoundProfileManager(), &mListener);
        }


        void ScriptedMenuState::handleEvent(const sf::Event& curEvent)
        {
            mCamera.handleEvent(curEvent);
            mGui.handleEvent(curEvent);
        }


        void ScriptedMenuState::update(const float delta)
        {
            mSoundHandler.update(delta);
            if (mIntervalTimer.getElapsedTime().asMilliseconds() >= UPDATE_INTERVAL)
            {
                mIntervalTimer.restart();
                mScriptCallbacks.execute(ON_UPDATE, this, UPDATE_INTERVAL);
            }
        }


        void ScriptedMenuState::render(sf::RenderTarget& target, sf::RenderStates states)
        {
            mCamera.renderBegin();
            mGui.draw();
            mCamera.renderEnd();
        }


        void ScriptedMenuState::onCustomEvent(const CustomEvent& event)
        {
            mScriptCallbacks.execute(ON_CUSTOM_EVENT, this, event);
        }


        void ScriptedMenuState::init()
        {
            mScriptCallbacks.execute(ON_INIT, this);
        }


        void ScriptedMenuState::close()
        {
            mScriptCallbacks.execute(ON_CLOSE, this);
        }

        ScriptedMenuState::~ScriptedMenuState()
        {
            mTargetSizeChangedLink.disconnect();
            mTargetSizeChangedLink.disconnect();
        }
}
