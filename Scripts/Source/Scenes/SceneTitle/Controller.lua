local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local SourceSystem = require("Source.System")
local EventKey = require("Enums.EventKey")
local WindowCommand = require("Source.Windows.WindowCommand")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.Title")

---@class Source.Scenes.SceneTitle.Controller
local SceneTitleController = {}

SceneTitleController.refreshEvents = { EventKey.LocaleChanged }

function SceneTitleController:bind()
    self._commandModels = {
        {
            localeKey = "TITLE_START",
            callback = function ()
                self.model:startGame()
            end
        },
        {
            localeKey = "TITLE_CONTINUE",
            callback = function ()
                self.model:openLoad()
            end
        },
        {
            localeKey = "TITLE_CONFIG",
            callback = function ()
                self.model:toggleConfig()
            end
        },
        {
            localeKey = "TITLE_EXIT",
            callback = function ()
                self.model:exitGame()
            end
        }
    }
    self._windowCommand = self:createChild("CommandPanel", WindowCommand, self._commandModels)
    local savePath = Engine.getSavePath()
    self._windowCommand:selectIndex(os.path.isdir(savePath) and bool(os.listdir(savePath)) and 1 or 0)
end

function SceneTitleController:refresh()
    local backgroundTexture = assert(
        GlobalCore.TextureManager.load(SourceSystem.GetTitleBackgroundFile(), false, nil, true)
    )
    self.ui.controls.Background:setTexture(backgroundTexture, false)
    self:setText("GameTitle", SourceSystem.GetTitle())
    self._windowCommand:refreshRows()
end

function SceneTitleController:getCommandWindow()
    return self._windowCommand
end

return Ui.Define(View, SceneTitleController)
