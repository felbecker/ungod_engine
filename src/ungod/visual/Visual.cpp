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

#include "ungod/visual/Visual.h"
#include "ungod/base/World.h"
#include "ungod/visual/Camera.h"

namespace ungod
{
    VisualsComponent::VisualsComponent() : mVisible(false), mOpacity(1.0f), mHideForCamera(false) {}


    bool VisualsComponent::isVisible() const
    {
        return mVisible;
    }


    sf::Texture* VisualsComponent::getTexture()
    {
        return mImage.get();
    }


    bool VisualsComponent::isLoaded() const
    {
        return mImage.isLoaded();
    }


    float VisualsComponent::getOpacity() const
    {
        return mOpacity;
    }


    bool VisualsComponent::isHiddenForCamera() const
    {
        return mHideForCamera;
    }


    std::string VisualsComponent::getFilePath() const
    {
        return mImage.getFilePath();
    }


    VisualAffectorComponent::VisualAffectorComponent() : mCallback(nullptr), mActive(false) {}

    void VisualAffectorComponent::setActive(bool active)
    {
        mActive = active;
    }

    bool VisualAffectorComponent::isActive() const
    {
        return mActive;
    }


    std::string SpriteMetadataComponent::getFilePath() const
    {
        return mMeta.getFilePath();
    }


    sf::IntRect SpriteMetadataComponent::getTexrect(const std::string& key) const
    {
        sf::IntRect texrect{};
        auto node = mMeta.getNodeWithKey(key);
        if (node)
        {
            auto result = node.getAttributes<int, int, int, int>(
                        {"pos_x", 0}, {"pos_y", 0}, {"width", 0}, {"height", 0} );
            texrect.left = std::get<0>(result);
            texrect.top = std::get<1>(result);
            texrect.width = std::get<2>(result);
            texrect.height = std::get<3>(result);
        }
        return texrect;
    }

    bool BigSpriteComponent::isLoaded() const
    {
        return mBigImage.isLoaded();
    }

    bool BigSpriteComponent::isVisible() const
    {
        return mVisible;
    }

    std::string BigSpriteComponent::getFilePath() const
    {
        return mBigImage.getFilePath();
    }


    void VisualsManager::initTextureRects(Entity e, std::size_t num)
    {
        e.modify<VertexArrayComponent>().mVertices.initRects(num);
        e.modify<VertexArrayComponent>().mKeys.resize(num);
    }

    void VisualsManager::setSpriteTextureRect(Entity e, SpriteComponent& sprite, const sf::FloatRect& rect)
    {
        sprite.mSprite.setTextureRect(rect);
        sprite.mKey = std::string{};
        mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(sprite.getSprite().getBounds()));
    }

    void VisualsManager::setSpritePosition(Entity e, SpriteComponent& sprite, const sf::Vector2f& position)
    {
        sprite.mSprite.setPosition(position);
        mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(sprite.getSprite().getBounds()));
    }

    void VisualsManager::setArrayTextureRect(VertexArrayComponent& vertices, const sf::FloatRect& rect, std::size_t index)
    {
        vertices.mVertices.setTextureRect(rect, index);
        if (index < vertices.mKeys.size())
            vertices.mKeys[index] = std::string{};
    }

    void VisualsManager::setTextureRectPosition(Entity e, const sf::Vector2f& position, std::size_t index)
    {
        VertexArrayComponent& vertices = e.modify<VertexArrayComponent>();
        vertices.mVertices.setRectPosition(position, index);
        //emit signal
        mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(vertices.mVertices.getBounds()));
    }

    void VisualsManager::setPoints(Entity e, VertexArrayComponent& vertices, std::size_t index, const sf::Vector2f& p1,
                                   const sf::Vector2f& p2, const sf::Vector2f& p3, const sf::Vector2f& p4)
    {
        vertices.mVertices.setPoints(p1, p2, p3, p4, index);
        //emit signal
        mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(vertices.mVertices.getBounds()));
    }


   const sf::Vector2f& VisualsManager::getPoint(Entity e, std::size_t rectIndex, std::size_t pointIndex)
   {
        return e.get<VertexArrayComponent>().getVertices().getPoint(rectIndex, pointIndex);
   }

    std::size_t VisualsManager::newTextureRect(VertexArrayComponent& vertices, const sf::FloatRect& rect)
    {
        std::size_t index = vertices.mVertices.newTextureRect(rect);
        return index;
    }

    std::size_t VisualsManager::newTextureRect(Entity e, VertexArrayComponent& vertices, VisualsComponent& vis)
    {
        std::size_t index = newTextureRect(vertices, {0,0,0,0});
        if (vis.isLoaded())
        {
            setArrayTextureRect(vertices, sf::FloatRect{0.0f,0.0f, (float)vis.getTexture()->getSize().x, (float)vis.getTexture()->getSize().y}, index);
            setTextureRectPosition(e, {0,0}, index);
        }
        return index;
    }

    void VisualsManager::loadTexture(VisualsComponent& visuals, const std::string& imageID, std::function<void(VisualsComponent&)> callback)
    {
        visuals.mImage.load(imageID, ASYNC);
        visuals.mVisible = true;
        visuals.mImage.get([this, &visuals, callback](sf::Texture& texture)
          {
              callback(visuals);
          });
    }

    void VisualsManager::loadTexture(VisualsComponent& visuals, const std::string& imageID, const LoadPolicy policy)
    {
        visuals.mImage.load(imageID, policy);
        visuals.mVisible = true;
    }

    void VisualsManager::loadMetadata(Entity e, const std::string& metaID)
    {
        SpriteMetadataComponent& data = e.modify<SpriteMetadataComponent>();
        data.mMeta.load(metaID, SYNC);
    }

    void VisualsManager::setSpriteTextureRect(Entity e, SpriteComponent& sprite, const SpriteMetadataComponent& data, const std::string& key)
    {
        auto node = data.mMeta.getNodeWithKey(key);
        if (node)
        {
            sprite.mSprite.setTextureRect(node);
            sprite.mKey = key;
            mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(sprite.mSprite.getBounds()));
        }
        else
        {
            Logger::warning(key);
            Logger::warning(" not found in meta file: ");
            Logger::warning(data.mMeta.getFilePath());
            Logger::endl();
        }
    }

    void VisualsManager::setArrayTextureRect(Entity e, std::size_t index, const std::string& key)
    {
        SpriteMetadataComponent& data = e.modify<SpriteMetadataComponent>();
        auto node = data.mMeta.getNodeWithKey(key);
        if (node)
        {
            e.modify<VertexArrayComponent>().mVertices.setTextureRect(node, index);
            if (index >= e.modify<VertexArrayComponent>().mKeys.size())
            {
                e.modify<VertexArrayComponent>().mKeys.resize(index+1);
            }
            e.modify<VertexArrayComponent>().mKeys[index] = key;
            mContentsChangedSignal.emit(e, static_cast<sf::IntRect>(e.modify<VertexArrayComponent>().mVertices.getBounds()));
        }
        else
        {
            Logger::warning(key);
            Logger::warning(" not found in meta file: ");
            Logger::warning(data.mMeta.getFilePath());
            Logger::endl();
        }
    }


    std::size_t VisualsManager::newTextureRect(Entity e, const std::string& key)
    {
        auto index = newTextureRect(e, sf::FloatRect{0.0f,0.0f,0.0f,0.0f});
        setArrayTextureRect(e, index, key);
        return index;
    }


    void VisualsManager::setVisible(Entity e, bool visible)
    {
        e.modify<VisualsComponent>().mVisible = visible;
        mVisibilityChangedSignal(e, visible);
    }


    void VisualsManager::setHideForCamera(Entity e, bool hideForCamera)
    {
        e.modify<VisualsComponent>().mHideForCamera = hideForCamera;
    }


    void VisualsManager::bindSpriteToAnimation(SpriteComponent& sprite, AnimationComponent& animation)
    {
        animation.mVertices = sprite.mSprite.getVertices();
    }

    void VisualsManager::bindArrayToAnimation(VertexArrayComponent& vertices, AnimationComponent& animation, std::size_t index)
    {
        animation.mVertices = &vertices.mVertices.getVertexArray()[0];
    }

    bool VisualsManager::setAnimationState(Entity e, const std::string& key)
    {
        return setAnimationState(e, e.get<SpriteMetadataComponent>(), e.modify<AnimationComponent>(), key);
    }


    bool VisualsManager::setAnimationState(Entity e, const std::string& key, std::size_t multiAnimationIndex)
    {
        return setAnimationState(e, e.get<SpriteMetadataComponent>(), e.modify<MultiAnimationComponent>().getComponent(multiAnimationIndex), key);
    }


    bool VisualsManager::setAnimationState(Entity e, const SpriteMetadataComponent& data, AnimationComponent& animation, const std::string& key)
    {
        if (!animation.mVertices)
            return false;
        if (animation.mAnimation.isRunning())
        {
            mAnimationStopSignal(e, animation.mAnimation.getKey());
        }
        bool r = animation.mAnimation.startAnimation(data.mMeta, key, animation.mVertices);
        if (r)
        {
            mContentsChangedSignal(e, animation.getAnimation().getBounds());
        }
        mAnimationStartSignal(e, key);
        return r;
    }


    bool VisualsManager::newAnimationState(Entity e, const std::string& key)
    {
        auto index = newTextureRect(e, sf::FloatRect{0.0f,0.0f,0.0f,0.0f});
        bindArrayToAnimation(e, index);
        return setAnimationState(e, key);
    }

    void VisualsManager::setRunning(Entity e, bool running)
    {
        e.modify<AnimationComponent>().mAnimation.setRunning(running);
    }


    void VisualsManager::setRunning(Entity e, bool running, std::size_t animationIndex)
    {
        e.modify<MultiAnimationComponent>().getComponent(animationIndex).mAnimation.setRunning(running);
    }


    void VisualsManager::setAnimationSpeed(Entity e, float speed)
    {
        e.modify<AnimationComponent>().mAnimation.setSpeed(speed);
    }


    void VisualsManager::setAnimationSpeed(Entity e, float speed, std::size_t animationIndex)
    {
        e.modify<MultiAnimationComponent>().getComponent(animationIndex).mAnimation.setSpeed(speed);
    }


    void VisualsManager::setSpriteColor(SpriteComponent& sprite, const sf::Color& color)
    {
        sprite.mSprite.setColor(color);
    }


    void VisualsManager::setArrayRectColor(VertexArrayComponent& vertices, const sf::Color& color, std::size_t index)
    {
        vertices.mVertices.setRectColor(color, index);
    }

    void VisualsManager::setOpacity(Entity e, float opacity)
    {
        e.modify<VisualsComponent>().mOpacity = opacity;
        componentOpacitySet(e, opacity);
    }


    void VisualsManager::componentOpacitySet(Entity e, float opacity)
    {
        if (e.has<SpriteComponent>())
        {
            e.modify<SpriteComponent>().mSprite.setOpacity(opacity);
        }
        if (e.has<MultiSpriteComponent>())
        {
            MultiSpriteComponent& multisprite = e.modify<MultiSpriteComponent>();
            for (unsigned i = 0; i < multisprite.getComponentCount(); ++i)
            {
                multisprite.getComponent(i).mSprite.setOpacity(opacity);
            }
        }
        if (e.has<VertexArrayComponent>())
        {
            VertexArrayComponent& vertices = e.modify<VertexArrayComponent>();
            for (unsigned i = 0; i < vertices.mVertices.textureRectCount(); ++i)
            {
                vertices.mVertices.setRectOpacity(opacity, i);
            }
        }
    }


    void VisualsManager::onContentsChanged(const std::function<void(Entity, const sf::IntRect&)>& callback)
    {
        mContentsChangedSignal.connect(callback);
    }

    void VisualsManager::onVisibilityChanged(const std::function<void(Entity, bool)>& callback)
    {
        mVisibilityChangedSignal.connect(callback);
    }

    void VisualsManager::onAnimationStart(const std::function<void(Entity, const std::string&)>& callback)
    {
        mAnimationStartSignal.connect(callback);
    }

    void VisualsManager::onAnimationStop(const std::function<void(Entity, const std::string&)>& callback)
    {
        mAnimationStopSignal.connect(callback);
    }


    sf::Vector2f VisualsManager::getLowerBound(Entity e)
    {
        return getLowerBoundInternal(e, [] (const Sprite& sprite) -> sf::FloatRect { return sprite.getBounds(); });
    }


    sf::Vector2f VisualsManager::getUpperBound(Entity e)
    {
        return getUpperBoundInternal(e, [] (const Sprite& sprite) -> sf::FloatRect { return sprite.getBounds(); });
    }


    sf::Vector2f VisualsManager::getUntransformedLowerBound(Entity e)
    {
        return getLowerBoundInternal(e, [] (const Sprite& sprite) -> sf::FloatRect { return sprite.getUntransformedBounds(); });
    }


    sf::Vector2f VisualsManager::getUntransformedUpperBound(Entity e)
    {
        return getUpperBoundInternal(e, [] (const Sprite& sprite) -> sf::FloatRect { return sprite.getUntransformedBounds(); });
    }



    sf::Vector2f VisualsManager::getLowerBoundInternal(Entity e, const std::function<sf::FloatRect(const Sprite&)>& boundsGetter)
    {
        sf::Vector2f lowerBounds(0,0);
        if (e.has<VertexArrayComponent>())
        {
            const VertexArrayComponent& vert = e.get<VertexArrayComponent>();
            for (unsigned i = 2; i < vert.mVertices.getVertexArray().getVertexCount(); i += 4)  //iterate only over the bottom-right points
            {
                lowerBounds.x = std::max(vert.mVertices.getVertexArray()[i].position.x, lowerBounds.x);
                lowerBounds.y = std::max(vert.mVertices.getVertexArray()[i].position.y, lowerBounds.y);
            }
        }
        if (e.has<SpriteComponent>())
        {
            const SpriteComponent& sprite = e.get<SpriteComponent>();
            sf::FloatRect bounds = boundsGetter(sprite.getSprite());
            lowerBounds.x = std::max(bounds.left + bounds.width, lowerBounds.x);
            lowerBounds.y = std::max(bounds.top + bounds.height, lowerBounds.y);
        }
        if (e.has<MultiSpriteComponent>())
        {
            const MultiSpriteComponent& multisprite = e.get<MultiSpriteComponent>();
            for (unsigned i = 0; i < multisprite.getComponentCount(); ++i)
            {
                sf::FloatRect bounds = boundsGetter(multisprite.getComponent(i).getSprite());
                lowerBounds.x = std::max(bounds.left + bounds.width, lowerBounds.x);
                lowerBounds.y = std::max(bounds.top + bounds.height, lowerBounds.y);
            }
        }
        return lowerBounds;
    }


    sf::Vector2f VisualsManager::getUpperBoundInternal(Entity e, const std::function<sf::FloatRect(const Sprite&)>& boundsGetter)
    {
        sf::Vector2f upperBounds(0,0);
        if (e.has<VertexArrayComponent>())
        {
            const VertexArrayComponent& vert = e.get<VertexArrayComponent>();
            for (unsigned i = 0; i < vert.mVertices.getVertexArray().getVertexCount(); i += 4)  //iterate only over the top-left points
            {
                upperBounds.x = std::min(vert.mVertices.getVertexArray()[i].position.x, upperBounds.x);
                upperBounds.y = std::min(vert.mVertices.getVertexArray()[i].position.y, upperBounds.y);
            }
        }
        if (e.has<SpriteComponent>())
        {
            const SpriteComponent& sprite = e.get<SpriteComponent>();
            sf::FloatRect bounds = boundsGetter(sprite.getSprite());
            upperBounds.x = std::min(bounds.left, upperBounds.x);
            upperBounds.y = std::min(bounds.top, upperBounds.y);
        }
        if (e.has<MultiSpriteComponent>())
        {
            const MultiSpriteComponent& multisprite = e.get<MultiSpriteComponent>();
            for (unsigned i = 0; i < multisprite.getComponentCount(); ++i)
            {
                sf::FloatRect bounds = boundsGetter(multisprite.getComponent(i).getSprite());
                upperBounds.x = std::min(bounds.left, upperBounds.x);
                upperBounds.y = std::min(bounds.top, upperBounds.y);
            }
        }
        return upperBounds;
    }


    void VisualsManager::moveVisuals(Entity e, const sf::Vector2f& vec)
    {
        if (e.has<VertexArrayComponent>())
        {
            VertexArrayComponent& vert = e.modify<VertexArrayComponent>();
            for (unsigned i = 0; i < vert.mVertices.getVertexArray().getVertexCount(); i += 4)  //iterate only over the top-left points
            {
                vert.mVertices.moveRect(vec, i);
            }
        }
        if (e.has<SpriteComponent>())
        {
            SpriteComponent& sprite = e.modify<SpriteComponent>();
            sprite.mSprite.move(vec);
        }
        if (e.has<MultiSpriteComponent>())
        {
            MultiSpriteComponent& multisprite = e.modify<MultiSpriteComponent>();
            for (unsigned i = 0; i < multisprite.getComponentCount(); ++i)
            {
                multisprite.getComponent(i).mSprite.move(vec);
            }
        }
    }

    void VisualsManager::setRotation(Entity e, SpriteComponent& sprite, float rotation)
    {
        sprite.mSprite.setRotation(rotation);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(sprite.mSprite.getBounds()));
    }

    void VisualsManager::setScale(Entity e, SpriteComponent& sprite, const sf::Vector2f& scale)
    {
        sprite.mSprite.setScale(scale);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(sprite.mSprite.getBounds()));
    }

    void VisualsManager::setOrigin(Entity e, SpriteComponent& sprite, const sf::Vector2f& origin)
    {
        sprite.mSprite.setOrigin(origin);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(sprite.mSprite.getBounds()));
    }


    void VisualsManager::setAffectorCallback(VisualAffectorComponent& affector, const VisualAffectorComponentCallback& callback)
    {
        affector.mCallback = callback;
        affector.mActive = true;
    }

    void VisualsManager::flipVertexX(VertexArrayComponent& vertices)
    {
        vertices.mFlipX = !vertices.mFlipX;
        vertices.mVertices.flipX();
    }

    void VisualsManager::flipVertexY(VertexArrayComponent& vertices)
    {
        vertices.mFlipY = !vertices.mFlipY;
        vertices.mVertices.flipY();
    }

    void VisualsManager::flipSpriteX(SpriteComponent& sprite)
    {
        sprite.mFlipX = !sprite.mFlipX;
        sprite.mSprite.flipX();
    }

    void VisualsManager::flipSpriteY(SpriteComponent& sprite)
    {
        sprite.mFlipY = !sprite.mFlipY;
        sprite.mSprite.flipY();
    }

    void VisualsManager::loadBigTexture(Entity e, BigSpriteComponent& bigSprite, const std::string& filepath, LoadPolicy policy)
    {
        bigSprite.mBigImage.load(filepath, policy);
        bigSprite.mBigImage.get([this, &bigSprite, e](sf::BigTexture& btxt)
          {
              if (e.valid())
              {
                bigSprite.mVisible = true;
                setBigSpriteTexture(e, bigSprite);
              }
          });
    }

    void VisualsManager::setBigSpriteTexture(Entity e, BigSpriteComponent& bigSprite)
    {
        if (bigSprite.mBigSprite.getBigTexture() == bigSprite.mBigImage.get())
            return;
        bigSprite.mBigSprite.setTexture(*bigSprite.mBigImage.get());
        mContentsChangedSignal(e, static_cast<sf::IntRect>(bigSprite.mBigSprite.getGlobalBounds()));
    }

    void VisualsManager::setBigSpriteVisibility(BigSpriteComponent& bigSprite, bool visible)
    {
        bigSprite.mVisible = visible;
    }

    void VisualsManager::setBigSpritePosition(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& position)
    {
        bigSprite.mBigSprite.setPosition(position);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(bigSprite.mBigSprite.getGlobalBounds()));
    }

    void VisualsManager::setBigSpriteColor(BigSpriteComponent& bigSprite, const sf::Color& color)
    {
        bigSprite.mBigSprite.setColor(color);
    }

    void VisualsManager::setBigSpriteScale(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& scale)
    {
        bigSprite.mBigSprite.setScale(scale);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(bigSprite.mBigSprite.getGlobalBounds()));
    }

    void VisualsManager::setBigSpriteOrigin(Entity e, BigSpriteComponent& bigSprite, const sf::Vector2f& origin)
    {
        bigSprite.mBigSprite.setOrigin(origin);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(bigSprite.mBigSprite.getGlobalBounds()));
    }

    void VisualsManager::setBigSpriteRotation(Entity e, BigSpriteComponent& bigSprite, const float angle)
    {
        bigSprite.mBigSprite.setRotation(angle);
        mContentsChangedSignal(e, static_cast<sf::IntRect>(bigSprite.mBigSprite.getGlobalBounds()));
    }
}
