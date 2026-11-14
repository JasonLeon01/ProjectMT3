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

local function formatTemplate(key)
    return string.strip(string.pformat(LOC(key), { word = "" }))
end

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
    self._confirmKeys = {}
    for _, key in ipairs(Input.getConfirmKeys()) do
        if key.kind ~= Engine.ActionKind.MouseButton and key.kind ~= Engine.ActionKind.TouchTap then
            self._confirmKeys[#self._confirmKeys + 1] = key
        end
    end
    self:watch(messages, "networkReady", Controller.refreshStatus)
    self:watch(messages, "checking", Controller.refreshStatus)
    self:watch(messages, "submitting", Controller.refreshStatus)
end

function Controller:bind()
    self.ui.controls.ConfirmButton:setCanReceiveFocus(false)
    self.ui.controls.ConfirmButton:addConfirmCallback(self:bindCallback(Controller.confirm))
    self.ui.controls.ConfirmButton:addClickCallback(function (_button, kwargs)
        if kwargs.button == nil or kwargs.button == sf.Mouse.Button.Left then
            self:confirm()
        end
    end)
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
    self:_onCapturedTouchReset()
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

function Controller:onKeyDown(_kwargs)
    if Input.isActionTriggered(Input.getCancelKeys(), true) then
        self:onReturn()
        return
    end
    if self._busy then return end
    if Input.isActionTriggered(self._confirmKeys, true) then
        self:confirm()
        return
    end
    if self.host:_handleDirectionalAction(Direction.UP, Input.getUpKeys()) then return end
    if self.host:_handleDirectionalAction(Direction.DOWN, Input.getDownKeys()) then return end
    if self.host:_handleDirectionalAction(Direction.LEFT, Input.getLeftKeys()) then return end
    self.host:_handleDirectionalAction(Direction.RIGHT, Input.getRightKeys())
end

function Controller:onDirectionalKey(direction)
    if self._busy then return true end
    if self.host.index == 0 or self.host.index == 1 then
        if direction == Direction.UP or direction == Direction.DOWN then
            self:adjustRow(self.host.index, direction == Direction.UP and -1 or 1)
            return true
        end
        if direction == Direction.LEFT or direction == Direction.RIGHT then
            self.host:changeSelection(direction == Direction.LEFT and 0 or 1)
            return true
        end
    end
    return false
end

function Controller:adjustRow(index, step)
    if index == 0 then
        self:changeTemplate(step)
    elseif index == 1 then
        self:changeWord(step)
    end
end

function Controller:getSelectionLayoutRect(index)
    self.ui.controls.MessageList:applyPositions()
    local text = index == 0 and self.ui.controls.Template or self.ui.controls.Word
    local row = index == 0 and self.ui.controls.TemplateRow or self.ui.controls.WordRow
    local bounds = text:getTransform():transformRect(text:getLocalBounds())
    bounds.position = bounds.position - sf.Vector2f.new(8, 4)
    bounds.size = bounds.size + sf.Vector2f.new(16, 8)
    return row:getTransform():transformRect(bounds)
end

function Controller:_getItemHitAbsoluteBounds(index)
    local row = index == 0 and self.ui.controls.TemplateRow or self.ui.controls.WordRow
    return row:getAbsoluteBounds()
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
    if Input.getMouseScrolledWheel() ~= sf.Mouse.Wheel.Vertical then return end
    local delta = -(kwargs.delta or 0)
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

function Controller:_shouldCaptureTouch(position)
    return self:getChoiceRowAt(position) ~= nil
end

function Controller:_onCapturedTouchBegan(position)
    if self._busy then return end
    self._touchOrigin = position:copy()
    self._touchRow = self:getChoiceRowAt(position)
    self._touchConsumed = false
    if self._touchRow ~= nil then self.host:setPointerIndex(self._touchRow) end
end

function Controller:_handleCapturedTouchDrag(position)
    if self._busy or self._touchOrigin == nil or self._touchRow == nil or self._touchConsumed then return true end
    local delta = (position - self._touchOrigin) / Engine.GetScale()
    if math.abs(delta.y) >= 32 and math.abs(delta.y) > math.abs(delta.x) then
        self._touchConsumed = true
        self:adjustRow(self._touchRow, delta.y < 0 and 1 or -1)
    elseif math.abs(delta.x) >= 32 then
        self._touchConsumed = true
    end
    return true
end

function Controller:_handleCapturedTouchTap(_position)
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
    self:setText("Template", formatTemplate(templateKey))
    self:setText("Word", LOC(wordKey))
    self:setText("TemplatePreviousLabel", formatTemplate(Config.TEMPLATES[(self._template - 2) % #Config.TEMPLATES + 1]))
    self:setText("TemplateNextLabel", formatTemplate(Config.TEMPLATES[self._template % #Config.TEMPLATES + 1]))
    self:setText("WordPreviousLabel", LOC(Config.WORDS[(self._word - 2) % #Config.WORDS + 1]))
    self:setText("WordNextLabel", LOC(Config.WORDS[self._word % #Config.WORDS + 1]))
    self:setText("PublishLabel", LOC("MESSAGES_PUBLISH"))
    self.view:reflow()
    self:refreshStatus()
end

function Controller:refreshStatus()
    local enabled = self._messages.networkReady and not self._messages.checking
        and not self._messages.submitting and not self._busy
    self.ui.controls.ConfirmButton:setActive(enabled)
    self.ui.controls.ConfirmButton:setColour(enabled and sf.Color.new(255, 255, 255) or sf.Color.new(140, 140, 140))
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
    self:refreshStatus()
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
