local Engine = require("Engine")
local Data = require("Source.Data")

local BattleTextParticle = {}
local DURATION = 0.25
local HEALING_RISE = 16

function BattleTextParticle.Emit(system, position, delta)
    local healing = delta > 0
    local config = Data.GetPlainTextConfig(healing and "Global/HealingText" or "Global/DamageText")
    local startPosition = position:copy()
    local function update(_deltaTime, countTime, particle)
        ---@cast particle Engine.TextParticle
        if countTime >= DURATION then
            local parent = assert(particle:getParent())
            parent:removeText(particle)
            return
        end
        local progress = countTime / DURATION
        if healing then
            particle:setPosition(startPosition + sf.Vector2f.new(0, -HEALING_RISE * progress))
        else
            local scale = math.lerp(2, 1, progress)
            particle:setScale(sf.Vector2f.new(scale, scale))
        end
    end
    local particle = Engine.TextParticle.new(system, update, 0, tostring(math.abs(delta)), config, true)
    local bounds = particle:getLocalBounds()
    particle:setOrigin(bounds.position + bounds.size / 2)
    particle:setPosition(startPosition)
    update(0, 0, particle)
    system:addText(particle)
    return particle
end

return BattleTextParticle
