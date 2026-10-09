local Item = require("Enums.GeneralData.Item")
local Player = require("Source.MapActors.Player")
local Effects = require("Source.Gameplay.Effects")

---@class (partial) Mixins.Barriers.Barrier
local Barrier = {}

function Barrier:onOverlap(other)
    if self:isDestroyed() then
        return
    end
    local gameMap = assert(self:getMap(), "Barrier requires an owning map")
    ---@cast gameMap GameMap
    local player = Player.MeetPlayer(other, gameMap:getPlayer())
    if player == nil then
        return
    end
    local scene = assert(gameMap:getScene(), "Barrier requires an owning scene")
    ---@cast scene Source.Gameplay.GameplayScene
    local hasEternalIce = player:hasItem(Item.EternalIce)
    local variables = scene:getGameInstance():getVariables()
    local damage = variables[hasEternalIce and "burnWhenHasSnow" or "burnInMap"]
    if damage == nil then
        damage = hasEternalIce and 10 or 50
    end
    ---@cast damage integer
    Effects.ApplyInstantModifier(player, "Blueprint.Damage", "HP", "Add", -damage)
    assert(self.emitterComp ~= nil, "Barrier requires emitterComp")
    local emitter = assert(self.emitterComp:getEmitter(), "Barrier emitter is unavailable")
    emitter:restart()
    emitter:emit("Ignition", 1)
    emitter:emit("Flames", 24)
    emitter:emit("Embers", 16)
    gameMap:addDamageText(tostring(damage), player:getPosition(), player)
    if hasEternalIce then
        scene:recordDestroyedActor(self)
        self:destroy()
    end
end

return Barrier
