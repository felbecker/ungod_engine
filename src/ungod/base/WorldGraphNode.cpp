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

#include "ungod/base/WorldGraph.h"
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/serialization/SerialGraph.h"
#include "ungod/serialization/SerialRenderLayer.h"


namespace ungod
{
    WorldGraphNode::WorldGraphNode(WorldGraph& wg, unsigned index, ScriptedGameState& gamestate, const std::string& identifier, const std::string& datafile)
        : mWorldGraph(wg), 
        mIndex(index), 
        mLoadingInProcess(false),
        mIsLoaded(false), 
        mGamestate(gamestate), 
        mData(),
        mLayers(), 
        mIdentifier(identifier), 
        mDataFile(datafile)
    {
        setSize({ DEFAULT_SIZE , DEFAULT_SIZE });
    }

    void WorldGraphNode::load()
    {
        mData.load(mDataFile, LoadPolicy::ASYNC);
        mLoadingInProcess = true;
		Logger::info("Started loading of node:", getIdentifier());
    }

    void WorldGraphNode::unload()
    {
        mData.getWait(); //wait if loading is not ready
        save();
        mLayers.clearEverything();
        mIsLoaded = false;
		Logger::info("Node unloaded:", getIdentifier());
    }

	void WorldGraphNode::save()
	{
		SerializationContext context;
		context.serializeRootObject(mLayers, mGamestate);
		context.save(mDataFile);
	}

	sf::FloatRect WorldGraphNode::getBounds() const
	{
		return { mLayers.getPosition().x, mLayers.getPosition().y , mLayers.getSize().x , mLayers.getSize().y };
	}

    World* WorldGraphNode::addWorld(unsigned i)
    {
        return static_cast<World*>(mLayers.registerLayer(RenderLayerPtr{new World()}, i));
    }

    World* WorldGraphNode::addWorld()
    {
        return addWorld((unsigned)mLayers.getVector().size());
    }

    World* WorldGraphNode::getWorld(unsigned i) const
    {
        if (i < mLayers.getVector().size())
            return static_cast<World*>(mLayers.getVector()[i].first.get());
        else
            return nullptr;
    }

    bool WorldGraphNode::render(sf::RenderTarget& target, sf::RenderStates states) const
    {
        return mLayers.render(target, mGamestate.getCamera(), states);
    }

    bool WorldGraphNode::renderDebug(sf::RenderTarget& target, sf::RenderStates states,
                     bool bounds, bool texrects, bool colliders, bool audioemitters, bool lightemitters) const
    {
        return mLayers.renderDebug(target, mGamestate.getCamera(), states, bounds, texrects, colliders, audioemitters, lightemitters);
    }

    void WorldGraphNode::update(float delta)
    {
        if (mLoadingInProcess)
            mLoadingInProcess = !tryInit();
        mLayers.update(delta, mGamestate.getCamera());
    }

    void WorldGraphNode::handleInput(const sf::Event& event, const sf::RenderTarget& target)
    {
        mLayers.handleInput(event, mGamestate.getApp().getWindow());
    }

    void WorldGraphNode::handleCustomEvent(const CustomEvent& event)
    {
        mLayers.handleCustomEvent(event);
    }

	void WorldGraphNode::setPosition(const sf::Vector2f& pos)
	{
		mLayers.setPosition(pos);
		mWorldGraph.notifyBoundsChanged(this);
	}

	void WorldGraphNode::move(const sf::Vector2f& offset)
	{
		mLayers.setPosition(mLayers.getPosition() + offset);
		mWorldGraph.notifyBoundsChanged(this);
	}

	void WorldGraphNode::setSize(const sf::Vector2f& size)
	{
		mLayers.setSize(size);
		mWorldGraph.notifyBoundsChanged(this);
	}


	void WorldGraphNode::moveLayerUp(unsigned i)
	{
		mLayers.moveLayerUp(i);
	}

	void WorldGraphNode::moveLayerDown(unsigned i)
	{
		mLayers.moveLayerDown(i);
	}

	void WorldGraphNode::setActive(unsigned i, bool active)
	{
		mLayers.setActive(i, active);
	}

    bool WorldGraphNode::tryInit()
    {
        if (mData.isLoaded())
        {
            mIsLoaded = true;
            //init loaded worlds
            for (unsigned i = 0; i < mData.get().container.getVector().size(); i++)
            {
                World* world = static_cast<World*>(mLayers.registerLayer(mData.get().container.getVector()[i].first, mLayers.getVector().size()));
                world->init(mGamestate, &mData.get().memory);
            }
            mData.drop(); //we can drop the asset, it is no longer required
            Logger::info("Loaded node:", getIdentifier());
            return true;
        }
        else
            return false;
    }

    void SerialBehavior<WorldGraphNode>::serialize(const WorldGraphNode& data, MetaNode serializer, SerializationContext& context)
    {
        context.serializeProperty("id", data.mIdentifier, serializer);
        context.serializeProperty("file", data.mDataFile, serializer);
		context.serializeProperty("x", data.mLayers.getPosition().x, serializer);
		context.serializeProperty("y", data.mLayers.getPosition().y, serializer);
		context.serializeProperty("w", data.mLayers.getSize().x, serializer);
		context.serializeProperty("h", data.mLayers.getSize().y, serializer);

    }

    void DeserialBehavior<WorldGraphNode>::deserialize(WorldGraphNode& data, MetaNode deserializer, DeserializationContext& context)
    {
        //world graph nodes are deserialized in sleeping state
        auto result = deserializer.getAttributes<std::string, std::string, float, float, float, float>(
			{"id", ""}, {"file", ""}, { "x", 0.0f }, { "y", 0.0f }, { "w", 0.0f }, { "h", 0.0f });
        data.mIdentifier = std::get<0>(result);
        data.mDataFile = std::get<1>(result);
		data.setPosition({ std::get<2>(result) , std::get<3>(result) });
		data.setSize({ std::get<4>(result) , std::get<5>(result) });
    }
}

namespace quad
{
    Vector2f ElementTraits<ungod::WorldGraphNode*>::getPosition(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().left, w->getBounds().top);
    }

    Vector2f ElementTraits<ungod::WorldGraphNode*>::getSize(ungod::WorldGraphNode const* w)
    {
        return Vector2f(w->getBounds().width, w->getBounds().height);
    }

	uint64_t ElementTraits<ungod::WorldGraphNode*>::getID(ungod::WorldGraphNode const* e)
    {
		return e->getIndex();
    }
}
