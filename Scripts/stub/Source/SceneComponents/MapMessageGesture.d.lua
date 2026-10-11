---@meta Source.SceneComponents.MapMessageGesture

---@class Source.SceneComponents.MapMessageGesture
---@field new       fun(scene: Source.Scenes.SceneMap.SceneMap): Source.SceneComponents.MapMessageGesture
---@field _scene    Source.Scenes.SceneMap.SceneMap
---@field _map      GameMap | nil
---@field _position sf.Vector2i | nil
local MapMessageGesture = {}

---@param scene Source.Scenes.SceneMap.SceneMap
function MapMessageGesture:init(scene) end
function MapMessageGesture:cancel() end
function MapMessageGesture:update() end

return MapMessageGesture
