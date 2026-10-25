---@meta Mixins.Teleportations.DownstairSamePos

---@class (partial) Mixins.Teleportations.DownstairSamePos: Source.MapActors.Teleporter.Teleporter
local DownstairSamePos = {}

---@param other Engine.Actor[]
function DownstairSamePos:onOverlap(other) end

return DownstairSamePos
