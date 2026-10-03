local GlobalCore = require("GlobalCore")
local Special = require("Enums.GeneralData.Special")
local GameplayConstants = require("Source.Configs.GameplayConstants")

local GameplayAbility = GlobalCore.GameplayAbility
local GameplayAbilityResult = GlobalCore.GameplayAbilityResult

---@class (partial) Source.Gameplay.SpecialAbilities.VampireAbility
local VampireAbility = {}

---@param damage    number
---@param magnitude number
---@return integer
function VampireAbility.CalculateHealing(damage, magnitude)
    return math.floor(damage * magnitude)
end

---@param magnitude number
function VampireAbility:init(magnitude)
    GameplayAbility.init(self, {})
    self.id = GameplayConstants.SPECIAL_PREFIX .. Special.Vampire
    self.triggerTags = { GameplayConstants.BATTLE_RULES_EVENT }
    self._magnitude = magnitude
end

function VampireAbility:activate(_abilitySystem, eventData)
    eventData.payload.vampireHealing = VampireAbility.CalculateHealing(eventData.payload.counterDamage, self._magnitude)
    return assert(GameplayAbilityResult.Success("BattleRulesResolved", eventData.payload))
end

return class(VampireAbility, GameplayAbility)
