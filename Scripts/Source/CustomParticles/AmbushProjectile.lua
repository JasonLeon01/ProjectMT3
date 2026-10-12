local Engine = require("Engine")
local GlobalCore = require("GlobalCore")

local AmbushProjectile = {}

local SPEED = 320 / 0.3
local IMAGE_DIRECTION_OFFSET = 45

function AmbushProjectile.Emit(gameMap, texturePath, startPosition, target, onHit)
    local system = gameMap:getParticleSystem()
    local scene = assert(gameMap:getScene(), "Ambush projectile requires an owning scene")
    ---@cast scene Source.Gameplay.GameplayScene
    local position = startPosition:copy()
    local halfCell = Engine.GetCellSize() * 0.5
    local finished = false

    local function isValid()
        return gameMap:getScene() == scene and GlobalCore.SceneManager.getScene() == scene
            and scene:getGameMap() == gameMap and gameMap:getPlayer() == target
            and not target:isDestroyed() and target:getMap() == gameMap and target.attributes.HP > 0
    end

    ---@param particle Engine.Particle
    ---@param hit boolean
    local function finish(particle, hit)
        finished = true
        particle.info.color = sf.Color.Transparent
        system:addUpdateFlag(particle)
        -- Remove and settle outside ParticleSystem's update iteration.
        local eventName = "AmbushProjectile.finish." .. tostring(particle)
        Engine.once(eventName, function ()
            local attached = particle:getParent() == system
            particle:destroy()
            if hit and attached and isValid() then
                onHit()
            end
            gameMap:untrackProjectile(particle)
        end)
        Engine.post(eventName)
    end

    local function update(deltaTime, _countTime, particle)
        ---@cast particle Engine.Particle
        if finished then
            return
        end
        if not isValid() then
            finish(particle, false)
            return
        end
        local targetPosition = target:getPosition() + sf.Vector2f.new(halfCell, halfCell)
        local offset = targetPosition - position
        local distance = math.sqrt(offset.x * offset.x + offset.y * offset.y)
        if distance <= SPEED * deltaTime then
            finish(particle, true)
            return
        end
        position = position + offset * (SPEED * deltaTime / distance)
        particle.info.position = gameMap:worldToMapViewPosition(position)
        particle.info.rotation = sf.degrees(math.deg(math.atan(offset.y, offset.x)) + IMAGE_DIRECTION_OFFSET)
        system:addUpdateFlag(particle)
    end

    local info = Engine.ParticleInfo.new()
    info.position = gameMap:worldToMapViewPosition(position)
    info.scale = sf.Vector2f.new(1, 1)
    info.color = sf.Color.White
    local particle = Engine.Particle.new(system, update, 0, texturePath, info)
    -- addParticle builds the first frame's vertices from the current transform.
    update(0, 0, particle)
    system:addParticle(particle)
    gameMap:trackProjectile(particle)
    return particle
end

return AmbushProjectile
