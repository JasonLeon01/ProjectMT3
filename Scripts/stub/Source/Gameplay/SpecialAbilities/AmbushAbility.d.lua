---@meta Source.Gameplay.SpecialAbilities.AmbushAbility

---@class Source.Gameplay.SpecialAbilities.AmbushAbility: GlobalCore.GameplayAbility
---@field new fun(): Source.Gameplay.SpecialAbilities.AmbushAbility
local AmbushAbility = {}

function AmbushAbility:init() end

---@param abilitySystem GlobalCore.AbilitySystemComponent
---@param eventData     GlobalCore.GameplayEventData
---@return GlobalCore.GameplayAbilityResult
function AmbushAbility:activate(abilitySystem, eventData) end

return AmbushAbility
