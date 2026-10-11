local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Pool = require("Global.Pool")

local MapInput = {}

function MapInput.GetMapPosition(gameMap, positionX, positionY)
    local scale = GlobalCore.Display.getScale()
    if scale > 0.0 then
        positionX = math.floor(positionX / scale)
        positionY = math.floor(positionY / scale)
    end
    local mapViewRect = gameMap:getMapViewRect()
    local mapViewPixel = Pool.Get("sf.Vector2i", sf.Vector2i, { x = positionX, y = positionY })
    if not mapViewRect:contains(mapViewPixel) then
        Pool.Put("sf.Vector2i", mapViewPixel)
        return nil
    end
    mapViewPixel.x = mapViewPixel.x - mapViewRect.position.x
    mapViewPixel.y = mapViewPixel.y - mapViewRect.position.y
    local camera = assert(gameMap:getCamera(), "Map input requires a camera")
    local worldPos = camera:mapPixelToCoords(mapViewPixel)
    Pool.Put("sf.Vector2i", mapViewPixel)
    return sf.Vector2i.new(math.floor(worldPos.x / Engine.GetCellSize()), math.floor(worldPos.y / Engine.GetCellSize()))
end

return MapInput
