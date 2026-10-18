---@meta Source.Gameplay.SpecialAbilities.AmbushAbility

---@class Source.Gameplay.SpecialAbilities.AmbushAbility: GlobalCore.GameplayAbility
---@field _projectileTexture string
---@field new                fun(projectileImage: string): Source.Gameplay.SpecialAbilities.AmbushAbility
local AmbushAbility = {}

---@param projectileImage string
function AmbushAbility:init(projectileImage) end

---@param abilitySystem GlobalCore.AbilitySystemComponent
---@param eventData     GlobalCore.GameplayEventData
---@return GlobalCore.GameplayAbilityResult
--- QueryHazardBounds conservatively covers the player's entire map, preserving
--- unlimited row/column range. With no map there are no active hazard cells.
--- QueryHazard retains clear-line checks and raw ATK minus DEF movement damage;
--- ResolveAttack retains its separate doubled combat attack.
function AmbushAbility:activate(abilitySystem, eventData) end

return AmbushAbility
