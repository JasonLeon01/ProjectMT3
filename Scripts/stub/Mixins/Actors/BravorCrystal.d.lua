---@meta Mixins.Actors.BravorCrystal
---@class (partial) Mixins.Actors.BravorCrystal: Engine.Actor
---@field dissipating boolean
local BravorCrystal = {}

function BravorCrystal:onCreate() end

---@param other Engine.Actor[]
function BravorCrystal:onCollision(other) end

return BravorCrystal
