---@meta Source.Windows.WindowBottle.BottleRow.Controller

---@class Source.Windows.WindowBottle.BottleRow.Controller.Model
---@field iconTexture sf.Texture | nil
---@field count       integer

---@class Source.Windows.WindowBottle.BottleRow.Controller: Internal.UIBase.UiController
---@field ui       Internal.UI.Parts.WindowBottle.BottleRow
---@field model    Source.Windows.WindowBottle.BottleRow.Controller.Model
---@field new      fun(model: Source.Windows.WindowBottle.BottleRow.Controller.Model): Source.Windows.WindowBottle.BottleRow.Controller
---@field _colours table<string, sf.Color>
local Controller = {}

function Controller:bind() end

function Controller:refresh() end

return Controller
