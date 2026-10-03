local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Data = require("Source.Data")
local Item = require("Enums.GeneralData.Item")
local GameSystem = require("Source.System")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.WindowMagicDoor")

local Input = Engine.Input
local AudioManager = GlobalCore.AudioManager
local ITEM_ID = Item.SDoor

---@class Source.Windows.WindowMagicDoor.Controller
local Controller = {}

Controller.windowOptions = { hidden = true, focusable = true }

function Controller:init(player, onClose)
    self._player = player
    self._onClose = onClose
end

function Controller:setPlayer(player)
    self._player = player
end

function Controller:open(mapRect)
    local size = self.host:getSize()
    self.host:setPosition(
        sf.Vector2f.new(mapRect.position.x + mapRect.size.x - size.x, mapRect.position.y + mapRect.size.y - size.y)
    )
    self:setProperty("Icon", "texture", Data.GetGeneralItemData(ITEM_ID).icon)
    self:setText("Count", tostring(self._player:getItemCount(ITEM_ID)))
    self.view:reflow()
    self.host:setVisible(true)
    self.host:setActive(true)
    self.host:requestKeyboardFocus()
end

function Controller:close()
    if not self.host:getVisible() then
        return
    end
    self.host:setActive(false)
    self.host:setVisible(false)
    self._onClose()
end

function Controller:onKeyDown(_kwargs)
    if not self.host:getVisible() then
        return
    end
    if Input.isActionTriggered(Input.getCancelKeys(), true) then
        AudioManager.playSound(GameSystem.GetCancelSE())
        self:close()
    elseif Input.isActionTriggered(Input.getConfirmKeys(), true) then
        self.host:setActive(false)
        if self._player:hasItem(ITEM_ID) then
            AudioManager.playSound(GameSystem.GetDecisionSE())
            local result = self._player:activateItem(ITEM_ID)
            assert(result.ok, "Item Ability failed: " .. tostring(result.code))
        end
        self:close()
    end
end

return Ui.DefineWindow(View, Controller)
