---@meta Source.Gameplay.SpecialAbilities.VampireAbility

---@class Source.Gameplay.SpecialAbilities.VampireAbility: GlobalCore.GameplayAbility
---@field new        fun(magnitude: number): Source.Gameplay.SpecialAbilities.VampireAbility
---@field _magnitude number
local VampireAbility = {}

---@param damage number
---@param magnitude number
---@return integer
function VampireAbility.CalculateHealing(damage, magnitude) end

---@param magnitude number
function VampireAbility:init(magnitude) end

---@param abilitySystem GlobalCore.AbilitySystemComponent
---@param eventData     GlobalCore.GameplayEventData
---@return GlobalCore.GameplayAbilityResult
function VampireAbility:activate(abilitySystem, eventData) end

return VampireAbility
