local Ui = require("Internal.UIBase.Ui")
local IconTexture = require("Internal.UIBase.IconTexture")
local View = require("Internal.UI.Parts.WindowBottle.BottleRow")

---@class (partial) Source.Windows.WindowBottle.BottleRow.Controller
local Controller = {}

function Controller:bind()
    self._colours = {}
    for _, name in ipairs({ "Icon", "Multiply", "Count" }) do
        self._colours[name] = self.ui.controls[name]:getColour():copy()
    end
end

function Controller:refresh()
    IconTexture.Apply(self, "Icon", self.model.iconTexture)
    self:setText("Count", tostring(self.model.count))
    for _, name in ipairs({ "Icon", "Multiply", "Count" }) do
        local colour = self._colours[name]:copy()
        colour.a = self.model.count > 0 and 255 or 160
        self:setProperty(name, "colour", colour)
    end
end

return Ui.Define(View, Controller)
