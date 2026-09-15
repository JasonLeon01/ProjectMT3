---@meta Source.CustomParticles.BattleTextParticle

local BattleTextParticle = {}

--- Emit one native text particle in the owning Canvas's logical coordinates.
--- Damage and healing use separate TextConfig assets without modifying them.
--- The particle removes itself after 0.25 seconds of Canvas updates.
---@param system   Engine.ParticleSystem
---@param position sf.Vector2f
---@param delta    integer
---@return Engine.TextParticle
function BattleTextParticle.Emit(system, position, delta) end

return BattleTextParticle
