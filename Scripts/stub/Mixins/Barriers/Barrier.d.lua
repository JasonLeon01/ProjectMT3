---@meta Mixins.Barriers.Barrier

---@class (partial) Mixins.Barriers.Barrier: Source.MapActors.ConditionalActor
local Barrier = {}

---@param other Engine.Actor[]
function Barrier:onOverlap(other) end

return Barrier
