local Pickup = require("Source.Utils.Pickup")
local Effects = require("Source.Gameplay.Effects")

---@class (partial) Mixins.Consumables.Gem
local Gem = {}

Gem.ATTR_key = ""
Gem.plus = 0
Gem.getSE = ""

local function applyGemModifier(player, attrKey, plus)
    if not bool(attrKey) then
        return
    end
    local schema = assert(
        player.attributes:getAttributeSchema(attrKey), "Gem attribute is not in the player AttributeSet"
    )
    assert(schema.type == "int" or schema.type == "float", "Gem attribute must be numeric")
    Effects.ApplyInstantModifier(player, "Consumable.Gem." .. attrKey, attrKey, "Add", plus)
end

function Gem:onCollision(other)
    local parentCollision = super().onCollision
    Pickup.HandleCollision(self, other, parentCollision, function (player)
        applyGemModifier(player, self.ATTR_key, self.plus)
    end)
end

return Gem
