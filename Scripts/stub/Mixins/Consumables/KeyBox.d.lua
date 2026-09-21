---@meta Mixins.Consumables.KeyBox
---@brief
---
---@class (partial) Mixins.Consumables.KeyBox: Engine.Actor
---@field plus  integer
---@field getSE string
local KeyBox = {}

---@param other Engine.Actor[]
function KeyBox:onCollision(other) end

return KeyBox
