local GlobalCore = require("GlobalCore")
local GameplayConstants = require("Source.Configs.GameplayConstants")

local GameplayAbility = GlobalCore.GameplayAbility
local GameplayAbilityResult = GlobalCore.GameplayAbilityResult

---@class (partial) Source.Gameplay.SpecialAbilities.PoisonedAbility
local PoisonedAbility = {}

---@param damage number
---@param stacks integer
---@return integer
function PoisonedAbility.CalculateDamage(damage, stacks)
    return damage > 0 and math.max(0, stacks) or 0
end

function PoisonedAbility:init()
    GameplayAbility.init(self, {})
    self.id = "State.Poisoned.Combat"
    self.triggerTags = { GameplayConstants.COMBAT_RESOLVE_INCOMING_DAMAGE_EVENT }
end

---@diagnostic disable-next-line: unused, Gameplay Ability override intentionally ignores its receiver
function PoisonedAbility:activate(abilitySystem, eventData)
    local stacks = abilitySystem:getActiveEffectStacks("State.Poisoned")
    eventData.payload.value = eventData.payload.value + PoisonedAbility.CalculateDamage(eventData.payload.value, stacks)
    return assert(GameplayAbilityResult.Success("IncomingDamageResolved", eventData.payload))
end

return class(PoisonedAbility, GameplayAbility)
