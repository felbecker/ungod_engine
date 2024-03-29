#include <boost/test/unit_test.hpp>
#include "ungod/base/World.h"
#include "ungod/application/Application.h"
#include "ungod/test/mainTest.h"

BOOST_AUTO_TEST_SUITE(InputTest)

BOOST_AUTO_TEST_CASE( entity_mouse_events_test )
{
    //these bools are checked in the end, they must be true as a result of the following code
    bool mouseEnter = false;
    bool mouseClick = false;
    bool mouseExit = false;

    //initialize stuff
    sf::RenderWindow window;
    window.create(sf::VideoMode(640, 640), "test");
    ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
	ungod::WorldGraphNode& node = state.getWorldGraph().createNode(state, "nodeid", "nodefile");
	node.setSize({ 800,600 });
	ungod::World* world = node.addWorld();

    //create an entity with a collider that shall trigger some events
    ungod::Entity e = world->create(ungod::BaseComponents<ungod::TransformComponent, ungod::RigidbodyComponent<>>());
    world->getTransformHandler().setPosition(e, {25, 25});
    world->getMovementRigidbodyHandler().addCollider(e, ungod::makeRotatedRect({10,10}, {50, 50}));
    world->getQuadTree().insert(e);

    //shift the windows view to be sure things still work with a non default view
    window.setView( sf::View( sf::FloatRect( {20, 20}, {640, 640} ) ) );

    //catch and evaluate signals
    world->getInputEventHandler().onMouseEnter( [&mouseEnter] (ungod::Entity e) { mouseEnter = true; } );
    world->getInputEventHandler().onMouseClick( [&mouseClick] (ungod::Entity e) { mouseClick = true; } );
    world->getInputEventHandler().onMouseExit( [&mouseExit] (ungod::Entity e) { mouseExit = true; } );

    //simulate a "mouse enter"
    sf::Event event;
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = 50;
    event.mouseMove.y = 50;

    world->handleInput(event, window);

    BOOST_CHECK(mouseEnter);

    //simulate a "missing mouse click"
    event.type = sf::Event::MouseButtonPressed;
    event.mouseMove.x = 375;
    event.mouseMove.y = 375;

    world->handleInput(event, window);

    BOOST_CHECK(!mouseClick);

    //simulate a "mouse click"
    event.type = sf::Event::MouseButtonPressed;
    event.mouseButton.x = 50;
    event.mouseButton.y = 50;

    world->handleInput(event, window);

    BOOST_CHECK(mouseClick);

    //simulate a "mouse exit"
    event.type = sf::Event::MouseMoved;
    event.mouseMove.x = 775;
    event.mouseMove.y = 775;

    world->handleInput(event, window);

    BOOST_CHECK(mouseExit);

	world->destroy(e); //queue entity for destruction
	world->update(20.0f, {}, {}); //destroys entity in queue
}

BOOST_AUTO_TEST_SUITE_END()
