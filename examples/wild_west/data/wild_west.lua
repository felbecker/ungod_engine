--arguments:
-- static is a table shared between all gamestates (but not entities)
-- state is the underlying gamestate object with methods to access e.g. the world graph
---
-- the application can be accessed e.g. via ungod.addGameState(id, scriptfile) to add a new gamestate or ungod.quitApplication()
-- the ungod table is shared between all gamestates and entities and can be used for global communication
function wild_west.onInit(static, state)

    --create a global table
    --all entities and other gamestates can access this
    ungod.wild_west = {}
    w = ungod.wild_west

    print("hello wild west world...")

    --create a new node of size 1000x1000 in the world graph and add a world to it
    w.node = state:worldGraph():createNode(state, "wild_west", "wild_west.world")
    w.node:setSize(Vec2f.new(1000,1000))
    state:worldGraph():activateNode("wild_west")
    w.world = w.node:addWorld()

    --create an entity using the behavior script "miner" and add it to the world so it can be updated
    w.world:behavior():loadBehaviorScript("data/miner.lua")
    w.miner = w.world:createEntity("miner", {name = "Bob"})
    w.miner:add():UpdateTimer()
    w.world:add(w.miner)

    --create another entity using the behavior script "wife"
    w.world:behavior():loadBehaviorScript("data/wife.lua")
    w.wife = w.world:createEntity("wife", {name = "Elsa", husband = w.miner})
    w.wife:add():UpdateTimer()
    w.world:add(w.wife)

end

function wild_west.onClose(static, state)

    print("... and goodbye wild west world")

end
