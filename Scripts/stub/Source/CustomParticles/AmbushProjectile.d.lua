---@meta Source.CustomParticles.AmbushProjectile

local AmbushProjectile = {}

---@param gameMap GameMap
---@param texturePath string
---@param startPosition sf.Vector2f World-space centre of the source.
---@param target Source.MapActors.Player.Player
---@param onHit fun()
---@return Engine.Particle
function AmbushProjectile.Emit(gameMap, texturePath, startPosition, target, onHit) end

return AmbushProjectile
