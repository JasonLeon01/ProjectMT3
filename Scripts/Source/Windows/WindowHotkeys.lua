local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Locale = require("Source.Locale.Core")
local GameSystem = require("Source.System")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.WindowHotkeys")

local Input = Engine.Input
local LOC = Locale.ApplyStringLocaleFormat

---@class Source.Windows.WindowHotkeys.Controller
local Controller = {}

Controller.windowOptions = { hidden = true, focusable = true }
Controller.ENTRIES = {
    "HOTKEY_MENU", "HOTKEY_ENEMY_BOOK", "HOTKEY_TELEPORT", "HOTKEY_SAVE", "HOTKEY_LOAD", "HOTKEY_ITEM", "HOTKEY_EQUIP",
    "HOTKEY_QUICK_SAVE", "HOTKEY_QUICK_LOAD", "HOTKEY_MAGIC_DOOR", "HOTKEY_BOTTLE", "HOTKEY_SWITCH_PLAYER"
}

function Controller:init(onClose)
    self._onClose = onClose
end

function Controller:refreshLocale()
    self.ui.controls["Rows"]:setCount(#Controller.ENTRIES)
    for index, key in ipairs(Controller.ENTRIES) do
        local text = self.ui.controls["Rows"]:get(index)
        assert(Class.isInstance(text, Engine.PlainText), "Hotkey row must be plain text")
        ---@cast text Engine.PlainText
        text:setString(
            Engine.TextLayout.wrapPlainText(LOC("{" .. key .. "}"), self.ui.controls["Rows"]:getSize().x, text)
        )
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
