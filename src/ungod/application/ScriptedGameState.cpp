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

#include "ungod/application/ScriptedGameState.h"
#include "ungod/application/Application.h"
#include "ungod/script/CustomEvent.h"
#include "ungod/base/World.h"
#include "ungod/serialization/DeserialInit.h"
#include <numeric>

namespace ungod
{
    constexpr std::array<const char*, ScriptedGameState::GameScriptCallbacks::NUM_CALLBACK> ScriptedGameState::GAME_CALLBACK_IDENTIFIERS;

    ScriptedGameState::ScriptedGameState(Application& app, StateID id, const std::string& gameScriptID)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mWorldGraph(*this),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(false),
            mDebugColliders(true),
            mDebugAudioEmitters(true),
            mDebugLightEmitters(true),
            mDebugInfo(false),
            mRenderer(mApp),
            mEntityBehaviorManager(mApp),
            mLightManager(mApp)
    {
        runScript(gameScriptID);
    }

    ScriptedGameState::ScriptedGameState(Application& app, StateID id)
            : State(app, id),
            mScriptCallbacks( app.getScriptState(), app.getGlobalScriptEnv(), { std::begin(GAME_CALLBACK_IDENTIFIERS), std::end(GAME_CALLBACK_IDENTIFIERS) } ),
            mGameScriptID(),
            mWorldGraph(*this),
            mRenderDebug(false),
            mDebugBounds(true),
            mDebugTexrects(true),
            mDebugColliders(true),
            mDebugAudioEmitters(true),
            mDebugLightEmitters(true),
            mRenderer(mApp),
            mEntityBehaviorManager(mApp),
            mLightManager(mApp)
    {
    }

    void ScriptedGameState::runScript(const std::string& gameScriptID)
    {
        mGameScriptID = gameScriptID;

        //set up the script behavior
        mScriptCallbacks.loadScript(gameScriptID);

        mScriptStateChangedLink = mApp.onScriptStateChanged([this]()
            {
                mScriptCallbacks.reload(mApp.getScriptState(), mApp.getGlobalScriptEnv());
            });
    }


    void ScriptedGameState::handleEvent(const sf::Event& curEvent)
    {
        if (curEvent.type == sf::Event::KeyPressed)
        {
            if (curEvent.key.code == sf::Keyboard::F2)
                toggleDebugRender(!mRenderDebug);
            if (curEvent.key.code == sf::Keyboard::F3)
                toggleDebugInfo(!mDebugInfo);
        }
        mWorldGraph.handleInput(curEvent, mApp.getWindow());
    }


    void ScriptedGameState::update(const float delta)
    {
        mWorldGraph.update(delta);
        mScriptCallbacks.execute(ON_UPDATE, this, delta);
    }


    void ScriptedGameState::render(sf::RenderTarget& target, sf::RenderStates states)
    {
        mRenderer.resetDrawCalls();

        mWorldGraph.render(target, states);
        if (mRenderDebug)
            mWorldGraph.renderDebug(target, states, mDebugBounds, mDebugTexrects, mDebugColliders, mDebugAudioEmitters, mDebugLightEmitters);

        if (mDebugInfo)
        {
            if (mApp.getDefaultFont().isLoaded())
            {
                float fps = 1.0f / mRenderTimer.restart().asSeconds();
                float smoothFPS = FPS_WINDOW_LAMBDA * fps + (1 - FPS_WINDOW_LAMBDA) * std::accumulate(mLastFps.begin(), mLastFps.end(), 0) / mLastFps.size();
                target.draw(sf::Text("FPS: " + std::to_string((int)std::floor(smoothFPS)), mApp.getDefaultFont().get(), DEBUG_TEXT_SIZE));
                mLastFps.push_back(fps);
                if (mLastFps.size() > FPS_AVG_WINDOW_SIZE)
                    mLastFps.pop_front();

                sf::Text drawCallsTxt("Draw calls: " + std::to_string(mRenderer.getDrawCalls()), mApp.getDefaultFont().get(), DEBUG_TEXT_SIZE);
                drawCallsTxt.setPosition(0, 30);
                target.draw(drawCallsTxt);

                sf::Text absoluteCamPosTxt("Absolute camera position: (" + 
                                        std::to_string(mWorldGraph.getCamera().getCenter().x) + ", " +
                                        std::to_string(mWorldGraph.getCamera().getCenter().y) + ")",
                                        mApp.getDefaultFont().get(), DEBUG_TEXT_SIZE);
                absoluteCamPosTxt.setPosition(0, 60);
                target.draw(absoluteCamPosTxt);

                sf::Text relativeCamPosTxt("", mApp.getDefaultFont().get(), DEBUG_TEXT_SIZE);
                if (mWorldGraph.getActiveNode())
                {
                    relativeCamPosTxt.setString("Relative camera position: (" +
                        std::to_string(mWorldGraph.getCamera().getCenter().x + mWorldGraph.getActiveNode()->getPosition().x) + ", " +
                        std::to_string(mWorldGraph.getCamera().getCenter().y + mWorldGraph.getActiveNode()->getPosition().y) + ") \t Active node:" +
                        mWorldGraph.getActiveNode()->getIdentifier());
                }
                else
                {
                    relativeCamPosTxt.setString("No active node.");
                }
                relativeCamPosTxt.setPosition(0, 90);
                target.draw(relativeCamPosTxt);
            }
        }
    }


    void ScriptedGameState::onCustomEvent(const CustomEvent& event)
    {
        mScriptCallbacks.execute(ON_CUSTOM_EVENT, this, event);
        mWorldGraph.handleCustomEvent(event);
    }


    void ScriptedGameState::save(const std::string& fileid, bool saveNodes)
    {
        SerializationContext context;
        //saves world graph information
        context.serializeRootObject(*this);
        context.save(fileid);
        //saves content of individual nodes
        if (saveNodes)
            mWorldGraph.save(*this);
    }


    void ScriptedGameState::load(const std::string& fileid)
    {
        DeserializationContext context;
        initContext(context);
        context.read(fileid);

        context.deserializeRootObject(*this);
    }


    void ScriptedGameState::init()
    {
        mScriptCallbacks.execute(ON_INIT, this);
    }


    void ScriptedGameState::close()
    {
        mScriptCallbacks.execute(ON_CLOSE, this);
        mWorldGraph.unloadAll();
    }

    ScriptedGameState::~ScriptedGameState()
    {
        mScriptStateChangedLink.disconnect();
    }
}

