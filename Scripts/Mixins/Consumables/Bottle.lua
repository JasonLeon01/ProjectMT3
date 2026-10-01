local Pickup = require("Source.Utils.Pickup")
local Effects = require("Source.Gameplay.Effects")

local BOTTLE_ITEMS = { [150] = "Bottle150", [400] = "Bottle400" }

---@class (partial) Mixins.Consumables.Bottle
local Bottle = {}

Bottle.HP_plus = 0
Bottle.getSE = ""

function Bottle:onCollision(other)
    local parentCollision = super().onCollision
    local SceneFunctions = require("GlobalFunctions.Scene")
    Pickup.HandleCollision(self, other, parentCollision, function (player)
        local itemID = BOTTLE_ITEMS[self.HP_plus]
        if itemID ~= nil and player.attributes.HP + self.HP_plus > player.attributes.MAXHP then
            player:addItem(itemID, 1)
            SceneFunctions.ShowTutorial("TM_06")
            return
        end
        Effects.ApplyInstantModifier(player, "Consumable.Bottle.Heal", "HP", "Add", self.HP_plus)
    end)
end

return Bottle
