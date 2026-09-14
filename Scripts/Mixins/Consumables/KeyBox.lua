local Pickup = require("Source.Pickup")

---@class (partial) Mixins.Consumables.KeyBox
local KeyBox = {}

KeyBox.plus = 1
KeyBox.getSE = ""

function KeyBox:onCollision(other)
    local parentCollision = super().onCollision
    Pickup.HandleCollision(self, other, parentCollision, function (player)
        local count = self.plus
        if count <= 0 then
            count = 1
        end
        player:addItem("KEY_Y", count)
        player:addItem("KEY_B", count)
        player:addItem("KEY_R", count)
    end)
end

return KeyBox
