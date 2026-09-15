local Ui = require("Source.UIBase.Ui")
local View = require("Source.UI.Parts.BattleFloatingText")

---@class Source.Windows.WindowBattle.BattleFloatingText.Controller
local Controller = {}

function Controller:show(parent, position, delta)
    self:attachTo(parent, parent:getSize())
    local name = delta < 0 and "Damage" or "Healing"
    self:setText(name, tostring(math.abs(delta)))
    self.view:reflow()
    local text = self.ui.controls[name]
    local bounds = text:getLocalBounds()
    text:setOrigin(bounds:getCenter())
    text:setPosition(position)
    text:setVisible(true)
    self:playAnimation(name, name)
end

return Ui.Define(View, Controller)
