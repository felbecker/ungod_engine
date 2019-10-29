#ifndef WORLD_ACTION_WRAPPER_H
#define WORLD_ACTION_WRAPPER_H

#include <unordered_map>
#include "ungod/base/World.h"
#include "ungod/content/FloodFill.h"
#include "ungod/utility/ScopedAccessor.h"
#include "ungod/content/TilemapBrush.h"

namespace uedit
{
    class EditorFrame;

    /**\brief A class that wraps a ungod::world object and keeps track of all modifying operations on it in the
    * EditorFrames action manager. */
    class WorldActionWrapper
    {
    public:
        WorldActionWrapper(EditorFrame* eframe);

        EditorFrame* getEditorFrame() { return mEFrame; }

        void setEntityPosition(ungod::Entity e, const sf::Vector2f& pos);
        void setEntityScale(ungod::Entity e, const sf::Vector2f& scale);
        void setEntityBaseLineOffset(ungod::Entity e, const sf::Vector2f& offset);
        void setEntityPlaneStatus(ungod::Entity e, bool plane);

        void startEntityMoveSession(ungod::Entity e);
        void moveEntity(ungod::Entity e, const sf::Vector2f& mv);
        void endEntityMoveSession(ungod::Entity e);

        void setEntityHideForCamera(ungod::Entity e, bool hideForCamera);
        void loadTexture(ungod::Entity e, const std::string& file);
        void loadMetadata(ungod::Entity e, const std::string& file);
        void setSpriteTextureRect(ungod::Entity e, const std::string& key);
        void setMultiSpriteTextureRect(ungod::Entity e, const std::string& key, std::size_t i);
        void setSpritePosition(ungod::Entity e, const sf::Vector2f& pos);
        void setMultiSpritePosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i);
        void setVertexarrayPosition(ungod::Entity e, const sf::Vector2f& pos, std::size_t i);
        void setSpriteScale(ungod::Entity e, const sf::Vector2f& scale);
        void setSpriteRotation(ungod::Entity e, float rota);
        void setMultiSpriteScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i);
        void setMultiSpriteRotation(ungod::Entity e, float rota, std::size_t i);
        void setVertexRectCount(ungod::Entity e, std::size_t numrect);
        void setVertexArrayRect(ungod::Entity e, std::size_t i, const std::string& key);
        void setVertexArrayRectColor(ungod::Entity e, std::size_t i, const sf::Color& color);

        template<std::size_t CONTEXT>
        void addCollider(ungod::Entity e, const sf::Vector2f& upleft, const sf::Vector2f& downright, float rot, int i = -1);
        template<std::size_t CONTEXT>
        void removeCollider(ungod::Entity e, unsigned i);
        template <std::size_t CONTEXT = 0>
        void setColliderDownRight(ungod::Entity e, std::size_t index, const sf::Vector2f& downright);
        template <std::size_t CONTEXT = 0>
        void setColliderUpLeft(ungod::Entity e, std::size_t index, const sf::Vector2f& upleft);
        template <std::size_t CONTEXT = 0>
        void rotateCollider(ungod::Entity e, std::size_t index, float rotation);
        template <std::size_t CONTEXT = 0>
        void setColliderRotation(ungod::Entity e, std::size_t index, float rotation);

        void attachScript(ungod::Entity e, const std::string& name);

        //removes the given entity from the world and marks it for deletion
        //it IS NOT DELETED DIRECTLY, to ensure that the operation is redoable
        void removeEntity(ungod::Entity e);

        //call this to permanently delete removed entities
        //after calling this, redoing a removeEntity operation is undefined bahavior
        void deleteEntities();

        void reserveTileCount(ungod::Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY);
        void setTileData(ungod::Entity e, const sf::Vector2f& position, int id);
        void setTileActive(ungod::Entity e, const sf::Vector2f& position, bool active);
        void setTileData(ungod::Entity e, const sf::Vector2f& position, int id, unsigned material, bool active, bool blocked);
        void loadTiles(ungod::Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap = {});
        void addKey(ungod::Entity e, const std::string& key);

        void reserveWaterTileCount(ungod::Entity e, std::size_t num, const unsigned mapSizeX, const unsigned mapSizeY);
        void setWaterTileData(ungod::Entity e, const sf::Vector2f& position, int id);
        void setWaterTileActive(ungod::Entity e, const sf::Vector2f& position, bool active);
        void setWaterTileData(ungod::Entity e, const sf::Vector2f& position, int id, unsigned material, bool active, bool blocked);
        void loadWaterTiles(ungod::Entity e, const std::string& tileID, const std::string& metaID,
                       unsigned cTileWidth, unsigned cTileHeight,
                       const std::vector<std::string>& keymap = {});
        void loadWaterShaders(ungod::Entity e, const std::string& distortionMap,
                      const std::string& fragmentShader, const std::string& vertexShader);
        void addWaterKey(ungod::Entity e, const std::string& key);



        inline void setLightPosition(ungod::Entity e, const sf::Vector2f& position)
        { setLightPosition(e, e.modify<ungod::LightEmitterComponent>(), position); }
        inline void setMultiLightPosition(ungod::Entity e, const sf::Vector2f& position, std::size_t i)
        { setLightPosition(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), position); }
        void setLightPosition(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& position);

        inline void setLightScale(ungod::Entity e, const sf::Vector2f& scale)
        { setLightScale(e, e.modify<ungod::LightEmitterComponent>(), scale); }
        inline void setMultiLightScale(ungod::Entity e, const sf::Vector2f& scale, std::size_t i)
        { setLightScale(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), scale); }
        void setLightScale(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& scale);


        inline void setLightOrigin(ungod::Entity e, const sf::Vector2f& origin)
        { setLightOrigin(e, e.modify<ungod::LightEmitterComponent>(), origin); }
        inline void setMultiLightOrigin(ungod::Entity e, const sf::Vector2f& origin, std::size_t i)
        { setLightOrigin(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), origin); }
        void setLightOrigin(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Vector2f& origin);

        inline void setLightColor(ungod::Entity e, const sf::Color& color)
        { setLightColor(e, e.modify<ungod::LightEmitterComponent>(), color); }
        inline void setMultiLightColor(ungod::Entity e, const sf::Color& color, std::size_t i)
        { setLightColor(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), color); }
        void setLightColor(ungod::Entity e, ungod::LightEmitterComponent& le, const sf::Color& color);


        inline void setLightRadius(ungod::Entity e, float radius)
        { setLightRadius(e, e.modify<ungod::LightEmitterComponent>(), radius); }
        inline void setMultiLightRadius(ungod::Entity e, float radius, std::size_t i)
        { setLightRadius(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), radius); }
        void setLightRadius(ungod::Entity e, ungod::LightEmitterComponent& le, float radius);

        inline void setShadowExtendMultiplier(ungod::Entity e, float multiplier)
        { setShadowExtendMultiplier(e, e.modify<ungod::LightEmitterComponent>(), multiplier); }
        inline void setMultiShadowExtendMultiplier(ungod::Entity e, float multiplier, std::size_t i)
        { setShadowExtendMultiplier(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), multiplier); }
        void setShadowExtendMultiplier(ungod::Entity e, ungod::LightEmitterComponent& le, float multiplier);

        inline void setLightActive(ungod::Entity e, bool active)
        { setLightActive(e, e.modify<ungod::LightEmitterComponent>(), active); }
        inline void setMultiLightActive(ungod::Entity e, bool active, std::size_t i)
        { setLightActive(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), active); }
        void setLightActive(ungod::Entity e, ungod::LightEmitterComponent& le, bool active);

        inline void loadLightTexture(ungod::Entity e, const std::string& textureID)
        { loadLightTexture(e, e.modify<ungod::LightEmitterComponent>(), textureID); }
        inline void loadMultiLightTexture(ungod::Entity e, const std::string& textureID, std::size_t i)
        { loadLightTexture(e, e.modify<ungod::MultiLightEmitter>().getComponent(i), textureID); }
        void loadLightTexture(ungod::Entity e, ungod::LightEmitterComponent& le, const std::string& textureID);

        void setPoint(ungod::Entity e, const sf::Vector2f& point, std::size_t pointIndex)
        { setPoint(e, e.modify<ungod::ShadowEmitterComponent>(), point, pointIndex); }
        void setMultiPoint(ungod::Entity e, const sf::Vector2f& point, std::size_t pointIndex, std::size_t multiIndex)
        { setPoint(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), point, pointIndex); }
        void setPoint(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, const sf::Vector2f& point, std::size_t pointIndex);

        void setPointCount(ungod::Entity e, std::size_t num)
        { setPointCount(e, e.modify<ungod::ShadowEmitterComponent>(), num); }
        void setMultiPointCount(ungod::Entity e, std::size_t num, std::size_t multiIndex)
        { setPointCount(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), num); }
        void setPointCount(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, std::size_t num);

        void setPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points);
        void setMultiPoints(ungod::Entity e, const std::vector<sf::Vector2f>& points, std::size_t multiIndex);

        void setLightOverShape(ungod::Entity e, bool lightOverShape)
        { setLightOverShape(e, e.modify<ungod::ShadowEmitterComponent>(), lightOverShape); }
        void setMultiLightOverShape(ungod::Entity e, bool lightOverShape, std::size_t multiIndex)
        { setLightOverShape(e, e.modify<ungod::MultiShadowEmitter>().getComponent(multiIndex), lightOverShape); }
        void setLightOverShape(ungod::Entity e, ungod::ShadowEmitterComponent& shadow, bool lightOverShape);

        inline void setAffectorCallback(ungod::Entity e, const std::function<void(float, ungod::LightEmitterComponent&)>& callback)
        { setAffectorCallback(e, e.modify<ungod::LightAffectorComponent>(), e.modify<ungod::LightEmitterComponent>(),callback); }
        inline void setMultiAffectorCallback(ungod::Entity e, const std::function<void(float, ungod::LightEmitterComponent&)>& callback, std::size_t lightIndex)
        { setAffectorCallback(e, e.modify<ungod::LightAffectorComponent>(), e.modify<ungod::MultiLightEmitter>().getComponent(lightIndex), callback); }
        inline void setMultiAffectorCallback(ungod::Entity e, const std::function<void(float, ungod::LightEmitterComponent&)>& callback, std::size_t lightIndex, std::size_t affectorIndex)
        { setAffectorCallback(e, e.modify<ungod::MultiLightAffector>().getComponent(affectorIndex), e.modify<ungod::MultiLightEmitter>().getComponent(lightIndex), callback); }
        void setAffectorCallback(ungod::Entity e, ungod::LightAffectorComponent& affector, ungod::LightEmitterComponent& emitter, const std::function<void(float, ungod::LightEmitterComponent&)>& callback);

        void floodFill(ungod::Entity e, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate);
        void floodFillWater(ungod::Entity e, std::size_t ix, std::size_t iy, const std::vector<int>& replacementIDs, bool activate);

        template<typename EMITTER_DATA, typename ESTIMATOR_DATA, typename ... PARAM>
        void setEmitter(ungod::Entity e, const std::string& emitterkey, const std::string& estimatorkey, PARAM&& ... param);


        template<typename DATA, typename ... PARAM>
        void setPositionDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setVelocityDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setSpawnInterval(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setLifetimeDist(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void addAffector(ungod::Entity e, const std::string& key, PARAM&& ... param);
        template<typename DATA, typename ... PARAM>
        void setTexrectInitializer(ungod::Entity e, const std::string& key, PARAM&& ... param);
        void clearAffectors(ungod::Entity e);
        void setParticleMaxForce(ungod::Entity e, float maxforce);
        void setParticleMaxVelocity(ungod::Entity e, float maxvel);
        void setParticleSpeed(ungod::Entity e, float speed);

        template<typename DATA, typename F>
        void alterPositionDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterVelocityDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterSpawnInterval(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterLifetimeDist(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterTexrectInitializer(ungod::Entity e, const F& func);
        template<typename DATA, typename F>
        void alterAffector(ungod::Entity e, std::size_t i, const F& func);

        void loadAudioEmitterTrack(ungod::Entity e, const std::string& file);
        void setAudioEmitterDistanceCap(ungod::Entity e, float cap);
        void setAudioEmitterActive(ungod::Entity e, bool active);
        void setAudioEmitterVolume(ungod::Entity e, float vol);


        void paintTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, bool connect);
        void eraseTile(ungod::TilemapBrush& brush, const sf::Vector2f& worldpos, int erasingID);
    private:
        EditorFrame* mEFrame;
        std::unordered_map<ungod::Entity, sf::Vector2f> mEntityMovements;
        std::vector<ungod::Entity> mRemovedEntities;
    };
}

#endif // WORLD_ACTION_WRAPPER_H
