local Player = require("Source.MapActors.Player")

---@class (partial) Mixins.Actors.BravorCrystal
local BravorCrystal = {}

local shatterParticleCount = 64
local destroyDelay = 1.1

function BravorCrystal:onCreate()
    super().onCreate()
    self.dissipating = false
end

function BravorCrystal:onCollision(other)
    if self:isDestroyed() or bool(self.dissipating) then
        return
    end
    local gameMap = self:getMap()
    if gameMap == nil then
        return
    end
    ---@cast gameMap GameMap
    local player = Player.MeetPlayer(other, gameMap:getPlayer())
    if player == nil then
        return
    end
    local NodeFunctionUtils = require("GlobalFunctions.Utils")
    NodeFunctionUtils.SetGameVariable("bravorshow", true)
    assert(self.emitterComp ~= nil, "BravorCrystal requires emitterComp")
    local bounds = self:getLocalBounds()
    self.emitterComp.anchor = sf.Vector2f.new(0, 0)
    self.emitterComp.offset = sf.Vector2f.new(
        bounds.position.x + bounds.size.x * 0.5, bounds.position.y + bounds.size.y * 0.5
    )
    local emitter = self.emitterComp:getEmitter()
    assert(emitter ~= nil, "BravorCrystal emitter is unavailable")
    emitter:emit("Shatter", shatterParticleCount)
    self:setTextureRect(sf.IntRect.new(0, 0, 0, 0))
    self.dissipating = true
    local scene = gameMap:getScene()
    assert(scene ~= nil, "BravorCrystal requires an owning scene")
    ---@cast scene Source.Scenes.SceneMap.SceneMap
    scene:recordDestroyedActor(self)
    scene:addTimer(destroyDelay, function ()
        if not self:isDestroyed() then
            self:destroy()
        end
    end)
    super().onCollision(other)
end

return BravorCrystal
