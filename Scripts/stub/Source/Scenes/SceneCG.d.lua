---@meta Source.Scenes.SceneCG

---@class Source.Scenes.SceneCG: GlobalCore.SceneBase
---@field new fun(): Source.Scenes.SceneCG
---@field private _mapEntryTask AsyncioTask | nil
local Scene = {}

function Scene:onEnter() end

function Scene:onQuit() end

return Scene
