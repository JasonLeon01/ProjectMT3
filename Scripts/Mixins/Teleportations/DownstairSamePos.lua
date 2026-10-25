local Player = require("Source.MapActors.Player")

---@class (partial) Mixins.Teleportations.DownstairSamePos
local DownstairSamePos = {}

function DownstairSamePos:onOverlap(other)
    local gameMap = assert(self:getMap(), "DownstairSamePos requires an owning map")
    ---@cast gameMap GameMap
    local player = Player.MeetPlayer(other, gameMap:getPlayer())
    if player == nil then
        return
    end
    local scene = assert(gameMap:getScene(), "DownstairSamePos requires an owning scene")
    ---@cast scene Source.Gameplay.GameplayScene
    scene:requestFloorStep(self, -1, true)
end

return DownstairSamePos
