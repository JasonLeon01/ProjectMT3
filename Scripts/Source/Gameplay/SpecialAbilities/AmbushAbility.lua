local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local GeneralEnum = require("Source.Configs.GeneralEnum")
local GameplayConstants = require("Source.Configs.GameplayConstants")

local GameplayAbility = GlobalCore.GameplayAbility
local GameplayAbilityResult = GlobalCore.GameplayAbilityResult
local Special = GeneralEnum.Special

---@class (partial) Source.Gameplay.SpecialAbilities.AmbushAbility
local AmbushAbility = {}

function AmbushAbility:init()
    GameplayAbility.init(self, {})
    self.id = GameplayConstants.SPECIAL_PREFIX .. Special.Ambush
    self.triggerTags = {
        GameplayConstants.COMBAT_RESOLVE_ATTACK_EVENT, GameplayConstants.MOVEMENT_QUERY_HAZARD_EVENT
    }
end

---@diagnostic disable-next-line: unused, Gameplay Ability override intentionally ignores its receiver
function AmbushAbility:activate(_abilitySystem, eventData)
    if eventData.eventTag == GameplayConstants.COMBAT_RESOLVE_ATTACK_EVENT then
        eventData.payload.value = eventData.payload.value * 2
        return assert(GameplayAbilityResult.Success("AttackResolved", eventData.payload))
    end
    local MovementSpecials = require("Source.MovementSpecials")
    local player = eventData.target
    local enemy = eventData.instigator
    local playerPosition = eventData.payload.playerPosition
    local enemyPosition = eventData.payload.enemyPosition
    local gameMap = player ~= nil and player:getMap() or nil
    local aligned = playerPosition ~= nil and enemyPosition ~= nil
        and (playerPosition.x == enemyPosition.x or playerPosition.y == enemyPosition.y)
        and Engine.ManhattanDistance(playerPosition, enemyPosition) > 0
    local active = aligned and gameMap ~= nil
        and MovementSpecials.HasClearLine(gameMap, player, playerPosition, enemyPosition)
    local damage = 0
    if active then
        damage = math.trunc(math.max(0, enemy:getAttr("ATK") - player:getAttr("DEF")))
    end
    return assert(GameplayAbilityResult.Success("MovementHazard", {
            active = active,
            special = Special.Ambush,
            magnitude = nil,
            damage = damage
        }))
end

return class(AmbushAbility, GameplayAbility)
