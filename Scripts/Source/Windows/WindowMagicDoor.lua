local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Data = require("Source.Data")
local Item = require("Enums.GeneralData.Item")
local GameSystem = require("Source.System")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.WindowMagicDoor")
local WindowBase = require("Internal.UIBase.WindowBase")

local Input = Engine.Input
local AudioManager = GlobalCore.AudioManager
local ITEM_ID = Item.SDoor

---@class Source.Windows.WindowMagicDoor.Controller
local Controller = {}

Controller.windowOptions = { hidden = true, focusable = true, returnButton = true, content = "Root" }

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

function Controller:onTick(deltaTime)
    WindowBase.onTick(self.host, deltaTime)
    if not self.host:getVisible() or not self.host:getActive() or Input.isTouchBlocked() then
        return
    end
    local tapPosition = Input.getTouchTapPosition()
    if tapPosition == nil then
        return
    end
    local position = Engine.ToVector2f(tapPosition)
    if not sf.FloatRect.contains(self.host:getAbsoluteBounds(), position) then
        return
    end
    if self:_isReturnButtonHit(position) then
        return
    end
    Input.isTouchTap(true)
    Input.isTouchTriggered(true)
    self:confirm()
end

function Controller:onKeyDown(_kwargs)
    if not self.host:getVisible() then
        return
    end
    if Input.isActionTriggered(Input.getCancelKeys(), true) then
        self:onReturn()
    elseif Input.isActionTriggered(Input.getConfirmKeys(), true) then
        self:confirm()
    end
end

function Controller:onMouseButtonDown(kwargs)
    if not self.host:getVisible() then
        return false
    end
    if kwargs.button == sf.Mouse.Button.Right then
        self:onReturn()
        return true
    end
    if kwargs.button == sf.Mouse.Button.Left then
        local position = kwargs.position
        if position == nil then
            position = Engine.ToVector2f(Input.getMousePosition())
        end
        if self:_isReturnButtonHit(position) then
            return false
        end
        self:confirm()
        return true
    end
    return false
end

function Controller:onReturn()
    AudioManager.playSound(GameSystem.GetCancelSE())
    self:close()
end

function Controller:confirm()
    if not self.host:getVisible() then
        return
    end
    self.host:setActive(false)
    if self._player:hasItem(ITEM_ID) then
        AudioManager.playSound(GameSystem.GetDecisionSE())
        local result = self._player:activateItem(ITEM_ID)
        assert(result.ok, "Item Ability failed: " .. tostring(result.code))
    end
    self:close()
end

function Controller:_isReturnButtonHit(position)
    if not self.host:isReturnButtonEnabled() then
        return false
    end
    local returnButton = self.ui.assets.WindowChrome.controls.ReturnButton
    return sf.FloatRect.contains(returnButton:getAbsoluteBounds(), position)
end

return Ui.DefineWindow(View, Controller, WindowBase)
