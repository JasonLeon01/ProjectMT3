local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Locale = require("Source.Locale.Core")
local InputPresentation = require("Source.Utils.InputPresentation")
local GameSystem = require("Source.System")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.WindowHotkeys")

local Input = Engine.Input
local LOC = Locale.ApplyStringLocaleFormat

---@class Source.Windows.WindowHotkeys.Controller
local Controller = {}

Controller.windowOptions = { hidden = true, focusable = true }
Controller.ENTRIES = {
    "HOTKEY_CONTROLS", "HOTKEY_MENU", "MESSAGES_HOTKEY", "HOTKEY_SAVE", "HOTKEY_LOAD", "HOTKEY_ITEM", "HOTKEY_EQUIP",
    "HOTKEY_QUICK_SAVE", "HOTKEY_QUICK_LOAD", "HOTKEY_ENEMY_BOOK", "HOTKEY_TELEPORT", "HOTKEY_BOTTLE",
    "HOTKEY_MAGIC_DOOR", "HOTKEY_SWITCH_PLAYER", "HOTKEY_BATTLE"
}

function Controller:init(onClose)
    self._onClose = onClose
    self._inputDevice = nil
end

function Controller:refreshLocale()
    self._inputDevice = InputPresentation.GetDevice()
    ---@type string[]
    local entries = Controller.ENTRIES
    if self._inputDevice == Engine.InputHintDevice.Gamepad then
        entries = {
            "GAMEPAD_MOVE", "GAMEPAD_MENU", "GAMEPAD_CONTROLS", "MESSAGES_GAMEPAD_HOTKEY", "GAMEPAD_CONFIRM",
            "GAMEPAD_BATTLE"
        }
    elseif self._inputDevice == Engine.InputHintDevice.Touch then
        entries = { "TOUCH_MOVE", "TOUCH_MENU", "TOUCH_CONTROLS", "MESSAGES_TOUCH_HOTKEY", "TOUCH_BATTLE" }
    end
    self.ui.controls["Rows"]:setCount(#entries)
    for index, key in ipairs(entries) do
        local text = self.ui.controls["Rows"]:get(index)
        assert(Class.isInstance(text, Engine.PlainText), "Hotkey row must be plain text")
        ---@cast text Engine.PlainText
        text:setString(
            Engine.TextLayout.wrapPlainText(LOC("{" .. key .. "}"), self.ui.controls["Rows"]:getSize().x, text)
        )
    end
    self.view:reflow()
end

function Controller:onTick(_deltaTime)
    if self.host:getVisible() and self._inputDevice ~= InputPresentation.GetDevice() then
        self:refreshLocale()
        self.ui.controls.Scroll:setScrollOffset(sf.Vector2f.new(0, 0))
    end
end

function Controller:open(mapRect)
    self.host:setPosition(sf.Vector2f.new(mapRect.position.x, mapRect.position.y))
    self:refreshLocale()
    self.ui.controls["Scroll"]:setScrollOffset(sf.Vector2f.new(0, 0))
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
    if self.host:getVisible() and Input.isActionTriggered(Input.getCancelKeys(), true) then
        self:onReturn()
    end
end

function Controller:onMouseButtonDown(kwargs)
    if kwargs.button == sf.Mouse.Button.Right then
        self:onReturn()
        return true
    end
    return false
end

function Controller:onReturn()
    GlobalCore.AudioManager.playSound(GameSystem.GetCancelSE())
    self:close()
end

return Ui.DefineWindow(View, Controller)
