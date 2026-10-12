local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Special = require("Enums.GeneralData.Special")
local GameplayConstants = require("Source.Configs.GameplayConstants")

local GameplayAbility = GlobalCore.GameplayAbility
local GameplayAbilityResult = GlobalCore.GameplayAbilityResult

---@class (partial) Source.Gameplay.SpecialAbilities.AmbushAbility
local AmbushAbility = {}

function AmbushAbility:init(projectileImage)
    GameplayAbility.init(self, {})
    assert(Class.isInstance(projectileImage, "string"), "Ambush special requires a projectile image filename")
    -- Any-valued General Data can store string literals with their quotes.
    local first = projectileImage:sub(1, 1)
    if first == '"' or first == "'" then
        projectileImage = Engine.Eval(projectileImage, {})
    end
    assert(
        Class.isInstance(projectileImage, "string") and bool(projectileImage),
        "Ambush special requires a non-empty projectile image filename"
    )
    self._projectileTexture = "/Game/Assets/System/" .. projectileImage
    self.id = GameplayConstants.SPECIAL_PREFIX .. Special.Ambush
    self.triggerTags = { GameplayConstants.COMBAT_RESOLVE_ATTACK_EVENT, GameplayConstants.MOVEMENT_QUERY_HAZARD_EVENT }
end

---@diagnostic disable-next-line: unused, Gameplay Ability override intentionally ignores its receiver
function AmbushAbility:activate(_abilitySystem, eventData)
    if eventData.eventTag == GameplayConstants.COMBAT_RESOLVE_ATTACK_EVENT then
        eventData.payload.value = eventData.payload.value * 2
        return assert(GameplayAbilityResult.Success("AttackResolved", eventData.payload))
    end
    local MovementSpecials = require("Source.Utils.MovementSpecials")
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
            projectileTexture = self._projectileTexture,
            damage = damage
        }))
end

return class(AmbushAbility, GameplayAbility)
