---@meta Source.Gameplay.SpecialAbilities.AmbushAbility

---@class Source.Gameplay.SpecialAbilities.AmbushAbility: GlobalCore.GameplayAbility
---@field _projectileTexture string
---@field new fun(projectileImage: string): Source.Gameplay.SpecialAbilities.AmbushAbility
local AmbushAbility = {}

---@param projectileImage string
function AmbushAbility:init(projectileImage) end

---@param abilitySystem GlobalCore.AbilitySystemComponent
---@param eventData     GlobalCore.GameplayEventData
---@return GlobalCore.GameplayAbilityResult
function AmbushAbility:activate(abilitySystem, eventData) end

return AmbushAbility
