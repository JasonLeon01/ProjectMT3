local MapPath = require("Source.Utils.MapPath")
local GameplayConstants = require("Source.Configs.GameplayConstants")
---@type { Special: Source.Configs.GeneralEnum.Special }
local GeneralEnum = require("Source.Configs.GeneralEnum")

local TransferRules = {
    regions = { "SECRET" },
    maps = { "MT3_025" }
}

---@param gameMap GameMap
---@return boolean
function TransferRules.IsForbidden(gameMap)
    local scene = gameMap:getScene()
    ---@cast scene Source.Gameplay.GameplayScene
    local instance = scene:getGameInstance()
    if table.contains(TransferRules.regions, instance:getCurrentRegion()) then
        return true
    end
    if table.contains(TransferRules.maps, MapPath.WithoutExtension(instance:getCurrentMapPath())) then
        return true
    end

    local Enemy = require("Source.MapActors.Enemy")
    for _, actor in ipairs(gameMap:getAllActors()) do
        if Class.isInstance(actor, Enemy) and not actor:isDestroyed() and actor:isVisibleInHierarchy()
            and gameMap:isActorVisibleOnMap(actor) then
            ---@cast actor Source.MapActors.Enemy
            if actor:getAbilitySystemComponent():hasMatchingGameplayTag(
                GameplayConstants.SPECIAL_PREFIX .. GeneralEnum.Special.EvilEye
            ) then
                return true
            end
        end
    end
    return false
end

return TransferRules
