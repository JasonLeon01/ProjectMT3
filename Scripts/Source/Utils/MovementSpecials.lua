local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Special = require("Enums.GeneralData.Special")
local ActorTree = require("Global.ActorTree")
local Effects = require("Source.Gameplay.Effects")
local MotaBattleAbility = require("Source.Gameplay.MotaBattleAbility")
local GameplayScene = require("Source.Gameplay.GameplayScene")
local GameplayConstants = require("Source.Configs.GameplayConstants")
local AmbushProjectile = require("Source.CustomParticles.AmbushProjectile")

local GameplayAbilityResult = GlobalCore.GameplayAbilityResult
local GameplayEventData = GlobalCore.GameplayEventData

local MovementSpecials = {}
local AMBUSH_HIT_DURATION = 0.2
local AMBUSH_HIT_PARTICLES = 16

---@param x integer
---@param y integer
---@return tuple<integer>
local function positionKey(x, y)
    return tuple(x, y)
end

---@param gameMap      GameMap
---@param actor        Engine.Actor
---@param fromPosition sf.Vector2i
---@param toPosition   sf.Vector2i
---@return boolean
function MovementSpecials.HasClearLine(gameMap, actor, fromPosition, toPosition)
    if fromPosition.x ~= toPosition.x and fromPosition.y ~= toPosition.y then
        return false
    end
    if fromPosition.x == toPosition.x and fromPosition.y == toPosition.y then
        return true
    end
    local step = sf.Vector2i.new(math.sign(toPosition.x - fromPosition.x), math.sign(toPosition.y - fromPosition.y))
    ---@cast step sf.Vector2i
    local position = fromPosition + step
    while position.x ~= toPosition.x or position.y ~= toPosition.y do
        if not gameMap:isPassable(actor, position) then
            return false
        end
        position = position + step
    end
    return true
end

---@param enemy          Source.MapActors.Enemy
---@param playerPosition sf.Vector2i
---@param scene          Source.Gameplay.GameplayScene
local function doBlockadeRetreat(enemy, playerPosition, scene)
    local enemyPosition = enemy:getMapPosition()
    local offset = sf.Vector2i.new(
        math.sign(enemyPosition.x - playerPosition.x), math.sign(enemyPosition.y - playerPosition.y)
    )
    ---@cast offset sf.Vector2i
    local moved = enemy:MapMove(offset)
    local newPosition = moved and enemyPosition + offset or enemyPosition
    ---@cast newPosition sf.Vector2i
    if moved then
        scene:recordActorPosition(enemy, newPosition)
    end
    local instance = scene:getGameInstance()
    local tag = enemy.tag or enemy.ID
    instance:setVariable("Blockade_" .. tag .. "_X", newPosition.x)
    instance:setVariable("Blockade_" .. tag .. "_Y", newPosition.y)
end

local function queryEnemy(enemy, player, playerPosition, previewContext)
    if enemy:isDestroyed() or not enemy:isVisibleInHierarchy() then
        return {}
    end
    local enemyPosition = enemy:getMapPosition()
    local damage
    if previewContext == nil then
        damage = MotaBattleAbility.CalculateDamagePerRound(enemy, player)
    else
        assert(previewContext.player == player, "Movement preview context belongs to another player")
        damage = assert(previewContext.damageByEnemy[enemy], "Movement preview context is missing an enemy")
    end
    local eventData = GameplayEventData.new(enemy, player, GameplayConstants.MOVEMENT_QUERY_HAZARD_EVENT, {
        distance = Engine.ManhattanDistance(playerPosition, enemyPosition),
        damagePerRound = damage,
        playerPosition = playerPosition,
        enemyPosition = enemyPosition
    })
    return enemy:getAbilitySystemComponent():handleGameplayEvent(eventData)
end

function MovementSpecials.CreatePreviewContext(enemies, player)
    local damageByEnemy = {}
    for _, enemy in ipairs(enemies) do
        damageByEnemy[enemy] = MotaBattleAbility.CalculateDamagePerRound(enemy, player)
    end
    return { player = player, damageByEnemy = damageByEnemy }
end

---@param enemies        Source.MapActors.Enemy[]
---@param player         Source.MapActors.Player.Player
---@param playerPosition sf.Vector2i
---@param ignoredEnemies Source.MapActors.Enemy[] | nil
---@param previewContext Source.Utils.MovementSpecials.PreviewContext | nil
---@return GlobalCore.GameplayAbilityResult
function MovementSpecials.Preview(enemies, player, playerPosition, ignoredEnemies, previewContext)
    ---@type table<Source.MapActors.Enemy, boolean> | nil
    local ignoredEnemySet = nil
    if bool(ignoredEnemies) then
        ---@cast ignoredEnemies - nil
        ignoredEnemySet = ActorTree.ToSet(ignoredEnemies)
    end
    local totalDamage = 0
    local sources = {}
    ---@type { enemy: Source.MapActors.Enemy, damage: integer } []
    local flankEnemies = {}
    for _, enemy in ipairs(enemies) do
        for _, result in ipairs(queryEnemy(enemy, player, playerPosition, previewContext)) do
            if result.code == "MovementHazard" and result.data.active then
                if result.data.special == Special.Flank then
                    flankEnemies[#flankEnemies + 1] = { enemy = enemy, damage = result.data.damage }
                elseif ignoredEnemySet == nil or not ignoredEnemySet[enemy] then
                    totalDamage = totalDamage + result.data.damage
                    sources[#sources + 1] = {
                        enemy = enemy,
                        special = result.data.special,
                        damage = result.data.damage,
                        projectileTexture = result.data.projectileTexture
                    }
                end
            end
        end
    end
    ---@type dict<tuple<integer>, { enemy: Source.MapActors.Enemy, damage: integer }>
    local positionMap = dict()
    for _, candidate in ipairs(flankEnemies) do
        local enemyPosition = candidate.enemy:getMapPosition()
        local offsetX = enemyPosition.x - playerPosition.x
        local offsetY = enemyPosition.y - playerPosition.y
        ---@cast offsetX integer
        ---@cast offsetY integer
        local key = positionKey(offsetX, offsetY)
        positionMap[key] = candidate
    end
    local flankPairs = { { positionKey(-1, 0), positionKey(1, 0) }, { positionKey(0, -1), positionKey(0, 1) } }
    for _, pair in ipairs(flankPairs) do
        local first = positionMap:get(pair[1])
        local second = positionMap:get(pair[2])
        if first ~= nil and second ~= nil then
            for _, candidate in ipairs({ first, second }) do
                if ignoredEnemySet == nil or not ignoredEnemySet[candidate.enemy] then
                    totalDamage = totalDamage + candidate.damage
                    sources[#sources + 1] = {
                        enemy = candidate.enemy,
                        special = Special.Flank,
                        damage = candidate.damage
                    }
                end
            end
        end
    end
    return assert(GameplayAbilityResult.Success("MovementHazard", {
            damage = totalDamage,
            sources = sources,
            playerPosition = playerPosition:copy()
        }))
end

function MovementSpecials.CollectEnemies(gameMap)
    local Enemy = require("Source.MapActors.Enemy")

    ---@type Source.MapActors.Enemy[]
    local enemies = {}
    for _, actor in ipairs(gameMap:getAllActors()) do
        if Class.isInstance(actor, Enemy) and not actor:isDestroyed() and actor:isVisibleInHierarchy() then
            ---@cast actor Source.MapActors.Enemy
            local abilitySystem = actor:getAbilitySystemComponent()
            if abilitySystem:hasMatchingGameplayTag(GameplayConstants.MOVEMENT_HAZARD_TAG) then
                enemies[#enemies + 1] = actor
            end
        end
    end
    return enemies
end

---@param scene Source.Gameplay.GameplayScene
---@param gameMap GameMap
---@param player Source.MapActors.Player.Player
---@param damage integer
---@param sources Source.Utils.MovementSpecials.DangerSource[]
---@param visualDuration number
local function applyDamage(scene, gameMap, player, damage, sources, visualDuration)
    local source = assert(sources[1], "Movement damage requires at least one source")
    local eventData = GameplayEventData.new(
        source.enemy, player, "Event.Movement.HazardDamage", { sources = sources }
    )
    Effects.ApplyInstantModifier(player, "Movement.HazardDamage", "HP", "Add", -damage, eventData)
    gameMap:addDamageText(tostring(damage), player:getPosition(), player)
    local battleHP = scene:applyExternalBattleDamage(player, damage)
    if player.attributes.HP <= 0 or battleHP ~= nil and battleHP <= 0 then
        player:setAttr("HP", 0)
        scene:requestGameOver(player, visualDuration)
    end
end

---@param scene Source.Gameplay.GameplayScene
---@param gameMap GameMap
---@param player Source.MapActors.Player.Player
---@param source Source.Utils.MovementSpecials.DangerSource
local function launchAmbush(scene, gameMap, player, source)
    local halfCell = Engine.GetCellSize() * 0.5
    local startPosition = source.enemy:getPosition() + sf.Vector2f.new(halfCell, halfCell)
    AmbushProjectile.Emit(gameMap, assert(source.projectileTexture), startPosition, player, function ()
        local component = assert(player.emitterComp, "Player requires an ambush hit emitter component")
        local emitter = assert(component:getEmitter(), "Player ambush hit emitter is unavailable")
        emitter:emit("Flash", 1)
        emitter:emit("Hit", AMBUSH_HIT_PARTICLES)
        applyDamage(scene, gameMap, player, source.damage, { source }, AMBUSH_HIT_DURATION)
    end)
end

function MovementSpecials.Commit(player, pathPositions)
    local gameMap = player:getMap()
    ---@type Source.MapActors.Enemy[]
    local enemies = {}
    if gameMap ~= nil then
        enemies = MovementSpecials.CollectEnemies(gameMap)
    end
    if gameMap == nil or not bool(enemies) then
        return assert(GameplayAbilityResult.Success("NoMovementHazard", { damage = 0, sources = {} }))
    end
    local totalDamage = 0
    local allSources = {}
    local blockadeRetreats = {}
    local previewContext = MovementSpecials.CreatePreviewContext(enemies, player)
    for _, playerPosition in ipairs(pathPositions) do
        local result = MovementSpecials.Preview(enemies, player, playerPosition, nil, previewContext)
        totalDamage = totalDamage + result.data.damage
        for _, source in ipairs(result.data.sources) do
            allSources[#allSources + 1] = source
            if source.special == Special.Blockade then
                blockadeRetreats[#blockadeRetreats + 1] = {
                    enemy = source.enemy,
                    playerPosition = playerPosition:copy()
                }
            end
        end
    end
    if totalDamage <= 0 and not bool(blockadeRetreats) then
        return assert(GameplayAbilityResult.Success("NoMovementHazard", { damage = 0, sources = allSources }))
    end
    local scene = gameMap:getScene()
    assert(
        Class.isInstance(scene, GameplayScene), "Movement hazard settlement requires a GameplayScene on its owning map"
    )
    ---@cast scene Source.Gameplay.GameplayScene
    for _, retreat in ipairs(blockadeRetreats) do
        doBlockadeRetreat(retreat.enemy, retreat.playerPosition, scene)
    end
    if totalDamage <= 0 then
        return assert(GameplayAbilityResult.Success("NoMovementHazard", { damage = 0, sources = allSources }))
    end
    local animationLength = 0.0
    local playerPosition = player:getPosition()
    local seenEnemies = {}
    local instantDamage = 0
    local instantSources = {}
    for _, source in ipairs(allSources) do
        if source.special == Special.Ambush then
            if source.damage > 0 then
                launchAmbush(scene, gameMap, player, source)
            end
        else
            instantDamage = instantDamage + source.damage
            instantSources[#instantSources + 1] = source
            if not seenEnemies[source.enemy] then
                seenEnemies[source.enemy] = true
                local animation = source.enemy:playAttackAnimationAt(scene, playerPosition)
                if animation ~= nil then
                    animationLength = math.max(animationLength, animation:getVisualDuration())
                end
            end
        end
    end
    if instantDamage > 0 then
        applyDamage(scene, gameMap, player, instantDamage, instantSources, animationLength)
    end
    return assert(GameplayAbilityResult.Success("MovementHazardCommitted", {
            damage = totalDamage,
            sources = allSources
        }))
end

function MovementSpecials.NotifyPlayerMovementFinished(player, pathPositions)
    if not bool(pathPositions) then
        pathPositions = { player:getMapPosition() }
    end
    return MovementSpecials.Commit(player, pathPositions)
end

return MovementSpecials
