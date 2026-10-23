local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Config = require("Source.Configs.Messages")
local GameSystem = require("Source.System")
local Locale = require("Source.Locale.Core")
local Ui = require("Internal.UIBase.Ui")
local WindowSelectable = require("Internal.UIBase.WindowSelectable")
local View = require("Internal.UI.WindowInscription")

local Input = Engine.Input
local Direction = Engine.FocusDirection
---@type fun(value: string): string
local LOC = Locale.ApplyStringLocaleFormat

---@class Source.Windows.WindowInscription.Controller
local Controller = {}
Controller.windowOptions = {
    centered = true,
    hidden = true,
    returnButton = true,
    focusable = true,
    list = "MessageList"
}

function Controller:init(messages, onClose)
    self._messages = messages
    self._onClose = onClose
    self._template = 1
    self._word = 1
    self._busy = false
    self._openGeneration = 0
    self._wheelRow = nil
    self._wheelAmount = 0
    self._touchOrigin = nil
    self._touchRow = nil
    self._touchConsumed = false
    self._leftKeys = Input.getLeftKeys()
    self._rightKeys = Input.getRightKeys()
    self._leftKeys[#self._leftKeys + 1] = Engine.InputActionKey.new({
        kind = Engine.ActionKind.JoystickAxis,
        code = sf.Joystick.Axis.U,
        threshold = -10,
        comparison = Engine.AxisComparison.Less
    })
    self._rightKeys[#self._rightKeys + 1] = Engine.InputActionKey.new({
        kind = Engine.ActionKind.JoystickAxis,
        code = sf.Joystick.Axis.U,
        threshold = 10,
        comparison = Engine.AxisComparison.Greater
    })
    self._shiftKeys = {
        Engine.InputActionKey.new({ kind = Engine.ActionKind.Key, code = sf.Keyboard.Key.LShift }),
        Engine.InputActionKey.new({ kind = Engine.ActionKind.Key, code = sf.Keyboard.Key.RShift })
    }
    self:watch(messages, "networkReady", Controller.refreshStatus)
    self:watch(messages, "checking", Controller.refreshStatus)
    self:watch(messages, "submitting", Controller.refreshStatus)
end

function Controller:bind()
    self.ui.controls.ConfirmRow:addConfirmCallback(self:bindCallback(Controller.confirm))
    for _, row in ipairs(self.ui.controls.MessageList:getChildren()) do
        assert(Class.isInstance(row, Engine.Canvas))
        ---@cast row Engine.Canvas
        row:setCanReceiveFocus(false)
    end
end

function Controller:open(_mapRect)
    self._openGeneration = self._openGeneration + 1
    self._template = 1
    self._word = 1
    self._busy = false
    self._wheelRow = nil
    self._wheelAmount = 0
    self:refreshLocale()
    self.host:resetSelection()
    self.host:showWithAnimation("FadeIn", function ()
        self.host:setActive(true)
        self.host:requestKeyboardFocusAtCursor()
    end)
    self._messages:checkConnection(nil, true)
end

function Controller:close()
    if self._busy or not self.host:getVisible() then return end
    self._openGeneration = self._openGeneration + 1
    self.host:setActive(false)
    self.host:hideImmediate()
    self._onClose()
end

function Controller:dismiss()
    self._busy = false
    self:close()
end

function Controller:onReturn()
    if self._busy or not self.host:getVisible() then return end
    GlobalCore.AudioManager.playSound(GameSystem.GetCancelSE())
    self:close()
end

function Controller:onKeyDown(kwargs)
    if Input.isActionTriggered(Input.getCancelKeys(), true) then
        self:onReturn()
        return
    end
    if self._busy then return end
    if self.host.index == 0 or self.host.index == 1 then
        if Input.isActionTriggered(self._leftKeys, true, 0.4, 0.1) then
            self:adjustRow(self.host.index, -1)
            return
        end
        if Input.isActionTriggered(self._rightKeys, true, 0.4, 0.1) then
            self:adjustRow(self.host.index, 1)
            return
        end
    end
    WindowSelectable.onKeyDown(self.host, kwargs)
end

function Controller:onDirectionalKey(direction)
    if self._busy then return true end
    if self.host.index == 0 or self.host.index == 1 then
        if direction == Direction.LEFT or direction == Direction.RIGHT then
            self:adjustRow(self.host.index, direction == Direction.LEFT and -1 or 1)
            return true
        end
    end
    return WindowSelectable.onDirectionalKey(self.host, direction)
end

function Controller:adjustRow(index, step)
    if index == 0 then
        self:changeTemplate(step)
    elseif index == 1 then
        self:changeWord(step)
    end
end

function Controller:getChoiceRowAt(position)
    if self.ui.controls.TemplateRow:getAbsoluteBounds():contains(position) then return 0 end
    if self.ui.controls.WordRow:getAbsoluteBounds():contains(position) then return 1 end
    return nil
end

function Controller:onMouseWheelScrolled(kwargs)
    if self._busy or not self.host:canReceiveFocus() or kwargs.position == nil then return end
    local row = self:getChoiceRowAt(kwargs.position)
    if row == nil then return end
    local horizontal = Input.getMouseScrolledWheel() == sf.Mouse.Wheel.Horizontal
    if not horizontal and not Input.isActionHeld(self._shiftKeys) then return end
    local delta = kwargs.delta or 0
    if not horizontal then delta = -delta end
    if self._wheelRow ~= row then
        self._wheelRow = row
        self._wheelAmount = 0
    end
    self.host:setPointerIndex(row)
    local threshold = 1.0
    if Input.isMouseWheelPrecise() then threshold = 32 * Engine.GetScale() end
    self._wheelAmount = self._wheelAmount + delta
    local steps = math.trunc(self._wheelAmount / threshold)
    if steps ~= 0 then
        self._wheelAmount = self._wheelAmount - steps * threshold
        self:adjustRow(row, steps)
    end
end

function Controller:_onCapturedTouchBegan(position)
    self._touchOrigin = position:copy()
    self._touchRow = self:getChoiceRowAt(position)
    self._touchConsumed = false
    if self._touchRow ~= nil then self.host:setPointerIndex(self._touchRow) end
end

function Controller:_handleCapturedTouchDrag(position)
    if self._busy or self._touchOrigin == nil or self._touchRow == nil or self._touchConsumed then return true end
    local delta = (position - self._touchOrigin) / Engine.GetScale()
    if math.abs(delta.x) >= 32 and math.abs(delta.x) > math.abs(delta.y) then
        self._touchConsumed = true
        self:adjustRow(self._touchRow, delta.x < 0 and 1 or -1)
    elseif math.abs(delta.y) >= 32 then
        self._touchConsumed = true
    end
    return true
end

function Controller:_onCapturedTouchReset()
    self._touchOrigin = nil
    self._touchRow = nil
    self._touchConsumed = false
end

function Controller:changeTemplate(step)
    if self._busy then return end
    self._template = (self._template - 1 + step) % #Config.TEMPLATES + 1
    self:refreshLocale()
end

function Controller:changeWord(step)
    if self._busy then return end
    self._word = (self._word - 1 + step) % #Config.WORDS + 1
    self:refreshLocale()
end

function Controller:refreshLocale()
    local templateKey = assert(Config.TEMPLATES[self._template])
    local wordKey = assert(Config.WORDS[self._word])
    self:setText("TemplateLabel", LOC("MESSAGES_TEMPLATE_LABEL"))
    self:setText("WordLabel", LOC("MESSAGES_WORD_LABEL"))
    self:setText("Template", string.pformat(LOC(templateKey), { word = "…" }))
    self:setText("Word", LOC(wordKey))
    self:setText("Preview", Config.Format(templateKey, wordKey))
    self:setText("PublishLabel", LOC("MESSAGES_PUBLISH"))
    self.view:reflow()
    self:refreshStatus()
end

function Controller:refreshStatus()
    local enabled = self._messages.networkReady and not self._messages.checking and not self._messages.submitting
    self.ui.controls.ConfirmRow:setActive(enabled)
    self.ui.controls.PublishLabel:setColour(enabled and sf.Color.new(255, 255, 255) or sf.Color.new(140, 140, 140))
    self:setText(
        "Status",
        LOC(
            self._messages.submitting and "MESSAGES_SENDING"
                or (self._messages.checking and "MESSAGES_CHECKING"
                    or (self._messages.networkReady and "MESSAGES_READY" or "MESSAGES_OFFLINE"))
        )
    )
end

function Controller:confirm()
    if self._busy or self._messages.submitting or self._messages.checking or not self._messages.networkReady then
        return
    end
    self._busy = true
    local generation = self._openGeneration
    self._messages:submit(assert(Config.TEMPLATES[self._template]), assert(Config.WORDS[self._word]), function ()
        ---@diagnostic disable-next-line: preferred-local-alias, compare the current generation with the callback snapshot
        if self._openGeneration ~= generation then return end
        self._busy = false
        self:close()
    end)
end

function Controller:dispose()
    self._openGeneration = self._openGeneration + 1
    self.host:unmount()
    super(Controller, self).dispose()
end

return Ui.DefineWindow(View, Controller, WindowSelectable)
