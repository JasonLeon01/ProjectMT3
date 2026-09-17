local Logging = require("Global.Utils.Logging")
local Data = require("Source.Data")
---@type { Special: Source.Configs.GeneralEnum.Special }
local GeneralEnum = require("Source.Configs.GeneralEnum")
local Item = require("Source.Item")
local SpecialAbilities = require("Source.Gameplay.SpecialAbilities")

local Special = GeneralEnum.Special

local DefeatSpawns = {}

local AREA_OFFSETS = {
    sf.Vector2i.new(0, 0),
    sf.Vector2i.new(-1, -1),
    sf.Vector2i.new(0, -1),
    sf.Vector2i.new(1, -1),
    sf.Vector2i.new(-1, 0),
    sf.Vector2i.new(1, 0),
    sf.Vector2i.new(-1, 1),
    sf.Vector2i.new(0, 1),
    sf.Vector2i.new(1, 1)
}

local SPAWN_KINDS = {
    { special = Special.Reborn, area = false, skipPlayer = false, allowPlayer = false },
    { special = Special.Ember, area = false, skipPlayer = false, allowPlayer = false },
    { special = Special.Glacial, area = false, skipPlayer = false, allowPlayer = false },
    { special = Special.BurstFlame, area = true, skipPlayer = false, allowPlayer = true },
    { special = Special.Snowland, area = true, skipPlayer = true, allowPlayer = false }
}

---@class EnemyDefeatSpawnContext
---@field gameMap      GameMap
---@field layerName    string
---@field originalTag  string
---@field position     sf.Vector2i
---@field reservedTags table<string, boolean>

---@param enemy Source.Enemy
---@param scene Source.Gameplay.GameplayScene
---@return EnemyDefeatSpawnContext
local function createContext(enemy, scene)
    local gameMap = scene:getGameMap()
    local layerName = assert(gameMap:getActorLayer(enemy), "Defeated enemy is not on a map layer")
    local originalTag = enemy:getMapTag()
    assert(bool(originalTag), "Defeated enemy requires a non-empty map-placement tag")
    return {
        gameMap = gameMap,
        layerName = layerName,
        originalTag = originalTag,
        position = copy(enemy:getMapPosition()),
        reservedTags = {}
    }
end

---@param context EnemyDefeatSpawnContext
---@param suffix  string
---@return string
local function reserveTag(context, suffix)
    local baseTag = context.originalTag .. "_" .. suffix
    local mapTag = baseTag
    local tagSuffix = 2
    while context.gameMap:getActorByTag(mapTag) ~= nil or context.reservedTags[mapTag] do
        mapTag = baseTag .. "_" .. tostring(tagSuffix)
        tagSuffix = tagSuffix + 1
    end
    context.reservedTags[mapTag] = true
    return mapTag
end

---@param context       EnemyDefeatSpawnContext
---@param blueprintPath string
---@param kind          string
---@param tagSuffix     string
---@param position      sf.Vector2i
---@return Engine.Actor
local function prepareActor(context, blueprintPath, kind, tagSuffix, position)
    assert(
        Class.isInstance(blueprintPath, "string") and bool(blueprintPath),
        "Enemy " .. kind .. " requires a Blueprint class path"
    )
    local actor = assert(
        Data.GenActorFromClassPath(blueprintPath), "Enemy " .. kind .. " Blueprint class not found: " .. blueprintPath
    )
    actor:setMapTag(reserveTag(context, tagSuffix))
    actor:setMapPosition(copy(position))
    return actor
end

---@param blueprintPath string
---@param position      sf.Vector2i
---@return string
local function createDropMapTag(blueprintPath, position)
    local prefix = blueprintPath:gsub("^Data%.Blueprints%.", ""):gsub("%.", "_")
    return prefix .. "_default_" .. tostring(position.x) .. "_" .. tostring(position.y)
end

---@param context       EnemyDefeatSpawnContext
---@param blueprintPath string
---@param offset        sf.Vector2i
---@return Source.Item | nil
local function prepareDrop(context, blueprintPath, offset)
    assert(
        Class.isInstance(blueprintPath, "string") and bool(blueprintPath), "Enemy drop requires a Blueprint class path"
    )
    local resolvedPath = Data.ResolveClassPath(blueprintPath)
    local itemClass = assert(Data.GetClass(resolvedPath), "Enemy drop Blueprint class not found: " .. resolvedPath)
    assert(Class.isSubclass(itemClass, Item), "Enemy drop Blueprint must derive from Source.Item: " .. resolvedPath)
    local position = context.position + offset
    local mapTag = createDropMapTag(resolvedPath, position)
    if context.gameMap:getActorByTag(mapTag) ~= nil or context.reservedTags[mapTag] then
        Logging.warning(
            "Skipping enemy drop %s at (%d, %d): map tag already exists: %s", resolvedPath, position.x, position.y,
            mapTag
        )
        return nil
    end
    local actor = assert(
        Data.GenActorFromClassPath(resolvedPath), "Enemy drop Blueprint class not found: " .. resolvedPath
    )
    ---@cast actor Source.Item
    context.reservedTags[mapTag] = true
    actor:setMapTag(mapTag)
    actor:setMapPosition(position)
    return actor
end

---@param gameMap  GameMap
---@param position sf.Vector2i
---@return boolean
local function inBounds(gameMap, position)
    local size = gameMap:getSize()
    return position.x >= 0 and position.y >= 0 and position.x < size.x and position.y < size.y
end

---@param gameMap GameMap
---@param position sf.Vector2i
---@param ignored Engine.Actor
---@return boolean
local function hasOtherActor(gameMap, position, ignored)
    for _, actor in ipairs(gameMap:getActorsByPosition(position)) do
        if actor ~= ignored and not actor:isDestroyed() and actor:isVisibleInHierarchy() then
            return true
        end
    end
    return false
end

---@param player   Engine.Actor | nil
---@param position sf.Vector2i
---@return boolean
local function isPlayerOn(player, position)
    if player == nil then
        return false
    end
    local playerPosition = player:getMapPosition()
    return position.x == playerPosition.x and position.y == playerPosition.y
end

---@param context     EnemyDefeatSpawnContext
---@param enemy       Source.Enemy
---@param player      Engine.Actor | nil
---@param position    sf.Vector2i
---@param isOrigin    boolean
---@param skipPlayer  boolean
---@param allowPlayer boolean
---@return boolean
local function canPlace(context, enemy, player, position, isOrigin, skipPlayer, allowPlayer)
    if not inBounds(context.gameMap, position) then
        return false
    end
    if skipPlayer and isPlayerOn(player, position) then
        return false
    end
    if isOrigin then
        return true
    end
    if isPlayerOn(player, position) then
        return allowPlayer
    end
    if hasOtherActor(context.gameMap, position, enemy) then
        return false
    end
    return player == nil or context.gameMap:isPassable(player, position)
end

function DefeatSpawns.Prepare(enemy, scene)
    local abilitySystem = enemy:getAbilitySystemComponent()
    ---@type { special: string, area: boolean, skipPlayer: boolean, allowPlayer: boolean, path: string }[]
    local kinds = {}
    for _, kind in ipairs(SPAWN_KINDS) do
        local path = SpecialAbilities.GetMagnitude(abilitySystem, kind.special)
        if Class.isInstance(path, "string") and bool(path) then
            kinds[#kinds + 1] = {
                special = kind.special,
                area = kind.area,
                skipPlayer = kind.skipPlayer,
                allowPlayer = kind.allowPlayer,
                path = path
            }
        end
    end
    local drops = deepcopy(enemy.attributes.drops)
    if not bool(kinds) and not bool(drops) then
        return {}, {}, nil
    end
    local context = createContext(enemy, scene)
    local player = context.gameMap:getPlayer()
    local spawnActors = {}
    for _, kind in ipairs(kinds) do
        local offsets = kind.area and AREA_OFFSETS or { sf.Vector2i.new(0, 0) }
        for _, offset in ipairs(offsets) do
            local position = context.position + offset
            local isOrigin = offset.x == 0 and offset.y == 0
            if canPlace(context, enemy, player, position, isOrigin, kind.skipPlayer, kind.allowPlayer) then
                local tagSuffix = kind.special == Special.Reborn and "Reborn"
                    or (kind.special .. "_" .. tostring(position.x) .. "_" .. tostring(position.y))
                spawnActors[#spawnActors + 1] = prepareActor(
                    context, kind.path, kind.special .. " special", tagSuffix, position
                )
            end
        end
    end
    local droppedActors = {}
    for _, dropPath in ipairs(table.orderedStringKeys(drops)) do
        local actor = prepareDrop(context, dropPath, drops[dropPath])
        if actor ~= nil then
            droppedActors[#droppedActors + 1] = actor
        end
    end
    return spawnActors, droppedActors, context.layerName
end

function DefeatSpawns.Spawn(scene, actor, layerName)
    scene:getGameMap():spawnActor(actor, layerName)
    scene:recordAddedActor(actor)
end

return DefeatSpawns
