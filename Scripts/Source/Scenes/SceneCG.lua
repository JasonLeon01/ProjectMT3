local GlobalCore = require("GlobalCore")
local Video = require("GlobalFunctions.Video")
local GameInstance = require("Source.GameInstance")

local SceneManager = GlobalCore.SceneManager
local SceneBase = GlobalCore.SceneBase
local Transition = GlobalCore.Transition

---@class Source.Scenes.SceneCG: GlobalCore.SceneBase
local Scene = {}

---@diagnostic disable-next-line: unused
function Scene:onEnter()
    Transition.setTransition(nil, 0.0)
    Video.PlayVideo("/Game/Assets/Videos/cg.mp4", false, true)

    local SceneMap = require("Source.Scenes.SceneMap")
    local nextScene = SceneMap.new()
    nextScene:setInst(GameInstance.new())
    SceneManager.setScene(nextScene)
    -- Native video bypasses the scene canvas; its cached transition background is still the title.
    Transition.requestTransition(nil, 0.0)
end

return class(Scene, SceneBase)
