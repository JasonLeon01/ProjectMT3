---@meta Source.Utils.Enemy.DefeatSpawns

local DefeatSpawns = {}

---@param enemy Source.MapActors.Enemy
---@param scene Source.Gameplay.GameplayScene
---@return Engine.Actor[] spawnActors
---@return Source.MapActors.Item[] droppedActors
---@return string | nil layerName
function DefeatSpawns.Prepare(enemy, scene) end

---@param scene     Source.Gameplay.GameplayScene
---@param actor     Engine.Actor
---@param layerName string
function DefeatSpawns.Spawn(scene, actor, layerName) end

return DefeatSpawns
