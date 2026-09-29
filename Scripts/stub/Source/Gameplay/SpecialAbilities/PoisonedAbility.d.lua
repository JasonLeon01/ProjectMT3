---@meta Source.Gameplay.SpecialAbilities.PoisonedAbility

---@class Source.Gameplay.SpecialAbilities.PoisonedAbility: GlobalCore.GameplayAbility
---@field new fun(): Source.Gameplay.SpecialAbilities.PoisonedAbility
local PoisonedAbility = {}

---@param damage number
---@param stacks integer
---@return integer
function PoisonedAbility.CalculateDamage(damage, stacks) end

function PoisonedAbility:init() end

---@param abilitySystem GlobalCore.AbilitySystemComponent
---@param eventData     GlobalCore.GameplayEventData
---@return GlobalCore.GameplayAbilityResult
function PoisonedAbility:activate(abilitySystem, eventData) end

return PoisonedAbility
