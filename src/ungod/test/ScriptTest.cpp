#include <boost/test/unit_test.hpp>
#include "ungod/script/EntityBehavior.h"
#include "ungod/base/World.h"
#include "ungod/script/Registration.h"
#include "ungod/application/Application.h"
#include "ungod/application/ScriptedMenuState.h"
#include "ungod/application/ScriptedGameState.h"
#include "ungod/test/mainTest.h"
#include <chrono>
#include <thread>
#include "ungod/test/mainTest.h"

BOOST_AUTO_TEST_CASE( script_test )
{
    using namespace ungod;

    //script test
    {
        script::State state;
        Script script;
        script.load("test_data/simple_script_test.lua", script::StateRef(state));

        BOOST_REQUIRE(script.isLoaded());

        script.get()->run();

        auto env = state.get<Optional<script::Environment>>("a");

        BOOST_REQUIRE(env);

        BOOST_CHECK_EQUAL("test", env->get_or<std::string>(0, "nope"));
    }

    //test correct error handling if the script was not found or had errors
    {
        script::State state;

        //code below is required not to crash
        {
            Script script;
            script.load("test_data/not_existing_script.lua", script::StateRef(state));

            BOOST_REQUIRE(!script.isLoaded());
        }

        //shall not crash and print correct error notes
        {
            Script script;
            script.load("test_data/incorrect_script.lua", script::StateRef(state));

            BOOST_REQUIRE(script.isLoaded());
            BOOST_REQUIRE(!script.isValid());

            //fine so far, now trouble is incoming D:
            //if we woudl run an invalid script, we would crash
            //gladly we checked the valid flag before x)
           // script.runScript();
        }
    }

    //invokable script test
    {
        int testme = 4;

        script::State state;
        InvokeableScript script;
        script.load("test_data/invokable_script_test.lua", "main", script::StateRef(state));

        BOOST_REQUIRE(script.isLoaded());

        script.runScript();
        script.invokeCallback(testme);

        auto testmeplus1 = state.get<Optional<int>>("testmeplus1");

        BOOST_CHECK(testmeplus1);
        BOOST_CHECK_EQUAL(*testmeplus1, testme + 1);
    }

    //callback invoker test
    {
        int testme = 0;
        enum Funcs { ON_INIT, ON_EXIT };
        CallbackInvoker callbacks({"onInit", "onExit",});
        callbacks.loadScript("test_data/callback_invoker_test.lua");
        callbacks.execute(ON_INIT, testme);
        auto test1 = callbacks.getEnvironment().get<Optional<int>>("test1");
        BOOST_CHECK(test1);
        BOOST_CHECK(*test1 == 2);
        callbacks.execute(ON_EXIT, *test1);
        auto test2 = callbacks.getEnvironment().get<Optional<int>>("test2");
        BOOST_CHECK(test2);
        BOOST_CHECK(*test2 == 15);
    }

    //init a simple manager with 2 callback methods
    //define both in the script and set flags on invocation, check flags afterwards
    {
        //some handy enums for the callback methods
        enum Funcs { ON_CREATE, ON_INIT, ON_UNCHECK, ON_SC, ON_SD, ON_NOT_DEFINED };

        BehaviorManager<> bm( {"onCreate", "onInit", "onUncheck", "onStaticConstr", "onStaticDestr", "onNotDefined"}, ON_CREATE, ON_INIT, ON_UNCHECK, ON_SC, ON_SD );
        bm.loadBehavior("test_data/global_test.lua");

        {
            script::Environment instance = bm.makeInstanceEnvironment(0);
            ungod::BehaviorPtr<> behavior = bm.makeBehavior("global_test", instance);

            BOOST_REQUIRE(behavior);

            //static constr called?
            auto staticconstr = behavior->getStaticVariable<int>("constr");
            BOOST_REQUIRE(staticconstr);
            BOOST_CHECK_EQUAL(37, *staticconstr);

            {
                //static constr NOT called twice when creating multiple instances? --- sheeeesh that would be sucky
                script::Environment instance2 = bm.makeInstanceEnvironment(1);
                ungod::BehaviorPtr<> behavior2 = bm.makeBehavior("global_test", instance2);
                staticconstr = behavior2->getStaticVariable<int>("constr");
                BOOST_REQUIRE(staticconstr);
                BOOST_CHECK_EQUAL(37, *staticconstr);

                //is called automatically
                //behavior->execute(ON_INIT);

                auto tcheck = behavior->getVariable<bool>("check1");

                BOOST_REQUIRE(tcheck);
                BOOST_REQUIRE(*tcheck);

                behavior->execute(ON_UNCHECK);

                tcheck = behavior->getVariable<bool>("check1");
                auto tcheck2 = behavior->getVariable<bool>("check2");

                BOOST_REQUIRE(tcheck);
                BOOST_REQUIRE(tcheck2);
                BOOST_REQUIRE(!(*tcheck));
                BOOST_REQUIRE(*tcheck2);

                //call to a method that is not defined shall be ignored
                behavior->execute(ON_NOT_DEFINED);
            }

            //okay nice so far, behavior2 is out of scope now, but behavior isnt, so deref must have been counted, but still no static Destr so far
            auto staticdestr = behavior->getStaticVariable<int>("destr");
            BOOST_REQUIRE(!staticdestr);
        }

        //both behaviors out of scope, test whether static destr is performed after the SECOND deref
        auto staticdestr = bm.getStaticVariable<int>("global_test", "destr");
        BOOST_REQUIRE(staticdestr);
        BOOST_CHECK_EQUAL(256, *staticdestr);
    }

    {
        enum Funcs { ON_CREATE, ON_INIT, ON_UNCHECK, ON_SC, ON_SD, ON_EVENT };

        BehaviorManager<> bm( {"onCreate", "onInit", "onUncheck", "onStaticConstr", "onStaticDestr", "onEvent"}, ON_CREATE, ON_INIT, ON_UNCHECK, ON_SC, ON_SD );
        bm.loadBehavior("test_data/state_test.lua");
        script::Environment instance = bm.makeInstanceEnvironment(0);
        StateBehaviorPtr<> behavior = bm.makeStateBehavior("state_test", instance);

        BOOST_REQUIRE(behavior);

        //check whether the initial state is active
        auto check = behavior->getStateVariable<bool>("state1", "check");

        BOOST_REQUIRE(check);
        BOOST_REQUIRE(*check);

        //run an event (switches state in script)
        behavior->execute(ON_EVENT);

        //check whether the "state closed" method was properly called (unchecks check)
        check = behavior->getStateVariable<bool>("state1", "check");

        BOOST_REQUIRE(check);
        BOOST_REQUIRE(!(*check));

        //check whether the new state was properly activated
        auto check2 = behavior->getStateVariable<bool>("state2", "check_state2");

        BOOST_REQUIRE(check2);
        BOOST_REQUIRE(*check2);

        //run another event (switches back to the previous state)
        behavior->execute(ON_EVENT);

        //check the flags again, state1 must be active, state2 inactive
        check = behavior->getStateVariable<bool>("state1", "check");
        check2 = behavior->getStateVariable<bool>("state2", "check_state2");
        BOOST_REQUIRE(check);
        BOOST_REQUIRE(*check);
        BOOST_REQUIRE(check2);
        BOOST_REQUIRE(!(*check2));
    }

    {
        ungod::ScriptedGameState state(EmbeddedTestApp::getApp(), 0);
        ungod::World world(&state);
        world.instantiate( EmbeddedTestApp::getApp(), "resource/unshadowShader.vert", "resource/unshadowShader.frag", "resource/lightOverShapeShader.vert", "resource/lightOverShapeShader.frag", "resource/penumbraTexture.png");
        world.initSpace(-5000,-5000,10000,10000);
        world.getBehaviorManager().loadBehaviorScript("test_data/entity_behavior.lua");
        world.getBehaviorManager().loadBehaviorScript("test_data/entity_behavior2.lua");
        Entity e = world.create(BaseComponents<TransformComponent, EntityBehaviorComponent, EntityUpdateTimer>(), "entity_behavior");
        Entity e2 = world.create(BaseComponents<TransformComponent, EntityBehaviorComponent, EntityUpdateTimer>(), "entity_behavior2");
        world.getQuadTree().insert(e);
        world.getQuadTree().insert(e2);

        auto globaltest = world.getBehaviorManager().getGlobalVariable<int>("test");
        BOOST_REQUIRE(globaltest);
        BOOST_CHECK_EQUAL(2, *globaltest);

        auto check_update = e.get<EntityBehaviorComponent>().getStateVariable<bool>("normal", "check_update");
        auto check_creation = e.get<EntityBehaviorComponent>().getStateVariable<bool>("normal", "check_creation");

        BOOST_REQUIRE(check_update);
        BOOST_REQUIRE(!(*check_update));

        BOOST_REQUIRE(check_creation);
        BOOST_REQUIRE(*check_creation);

        std::this_thread::sleep_for(std::chrono::milliseconds(300));

        world.update(20.0f, {0,0}, {800,600});

        check_update = e.get<EntityBehaviorComponent>().getStateVariable<bool>("normal", "check_update");

        BOOST_REQUIRE(check_update);
        BOOST_REQUIRE(*check_update);
		world.destroy(e); //queue entity for destruction
		world.destroy(e2); //queue entity for destruction
		world.update(20.0f, {}, {}); //destroys entity in queue
    }
}

BOOST_AUTO_TEST_CASE( input_script_test )
{
    using namespace ungod;

    {
    ScriptedMenuState menustate(EmbeddedTestApp::getApp(), 0, "test_data/input_test.lua");

    menustate.init();

    auto check = menustate.getEnvironment().get<Optional<bool>>("check");
    auto check2 = menustate.getEnvironment().get<Optional<bool>>("check2");

    BOOST_REQUIRE(check);
    BOOST_REQUIRE(!(*check));
    BOOST_REQUIRE(check2);
    BOOST_REQUIRE(!(*check2));

    //emulate key press events
    sf::Event event;
    event.type = sf::Event::KeyPressed;
    event.key.code = sf::Keyboard::A;
    menustate.handleEvent(event);

    check = menustate.getEnvironment().get<Optional<bool>>("check");

    BOOST_REQUIRE(check);
    BOOST_REQUIRE(*check);

    event.type = sf::Event::KeyReleased;
    event.key.code = sf::Keyboard::A;
    menustate.handleEvent(event);

    check2 = menustate.getEnvironment().get<Optional<bool>>("check2");

    BOOST_REQUIRE(check2);
    BOOST_REQUIRE(*check2);
    }
	
    {
        /*ScriptedGameState gamestate(EmbeddedTestApp::getApp(), 0, "test_data/input_test2.lua");

        gamestate.init();

        auto hovered = gamestate.getEnvironment().get<Optional<bool>>("hovered");
        auto exit = gamestate.getEnvironment().get<Optional<bool>>("exit");
        auto click = gamestate.getEnvironment().get<Optional<bool>>("click");
        auto release = gamestate.getEnvironment().get<Optional<bool>>("release");

        BOOST_REQUIRE(hovered);
        BOOST_REQUIRE(!(*hovered));
        BOOST_REQUIRE(exit);
        BOOST_REQUIRE(!(*exit));
        BOOST_REQUIRE(click);
        BOOST_REQUIRE(!(*click));
        BOOST_REQUIRE(release);
        BOOST_REQUIRE(!(*release));

        sf::Event event;
        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = 45;
        event.mouseMove.y = 45;
		gamestate.update(20.0f);
        gamestate.handleEvent(event);

        hovered = gamestate.getEnvironment().get<Optional<bool>>("hovered");

        BOOST_REQUIRE(hovered);
        BOOST_REQUIRE(*hovered);

        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.x = 45;
        event.mouseButton.y = 45;
        event.mouseButton.button = sf::Mouse::Left;
        gamestate.handleEvent(event);

        click = gamestate.getEnvironment().get<Optional<bool>>("click");

        BOOST_REQUIRE(click);
        BOOST_REQUIRE(*click);

        event.type = sf::Event::MouseButtonPressed;
        event.mouseButton.x = 5;
        event.mouseButton.y = 5;
        event.mouseButton.button = sf::Mouse::Left;
        gamestate.handleEvent(event);

        release = gamestate.getEnvironment().get<Optional<bool>>("release");

        BOOST_REQUIRE(release);
        BOOST_REQUIRE(*release);

        event.type = sf::Event::MouseMoved;
        event.mouseMove.x = 5;
        event.mouseMove.y = 5;
        gamestate.handleEvent(event);

        exit = gamestate.getEnvironment().get<Optional<bool>>("exit");

        BOOST_REQUIRE(exit);
        BOOST_REQUIRE(*exit);*/
    }
}
