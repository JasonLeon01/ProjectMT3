local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Render = require("Global.Utils.Render")
local Data = require("Source.Data")
local Battle = require("Source.Configs.Battle")
local Locale = require("Source.Locale.Core")
local WindowBase = require("Source.Windows.Base.WindowBase")
local Ui = require("Source.UIBase.Ui")
local View = require("Source.UI.WindowBattle")

---@type fun(value: string): string
local LOC = Locale.ApplyStringLocaleFormat
local Input = Engine.Input
local Animation = GlobalCore.Animation

---@class Source.Windows.WindowBattle.Controller
local Controller = {}
Controller.windowOptions = { hidden = true, focusable = true }

local function round(value)
    return math.floor(value + 0.5)
end

---@param actor  Source.Player.Player | Source.Enemy
---@param player boolean
---@return Source.Windows.WindowBattle.BattlerState
local function createState(actor, player)
    local attributes = actor.attributes
    return {
        HP = player and attributes.HP or attributes.MAXHP,
        ATK = attributes.ATK,
        DEF = attributes.DEF,
        breath = player and attributes.breath or 0,
        breathLimit = attributes.breathLimit,
        fatigue = 0,
        animationKey = attributes.ANIMATION_KEY,
        critanimationkey = attributes.critanimationkey,
        crit = (player and assert(Battle.players[actor.ID], "Missing player battle config: " .. actor.ID)
            or Battle.enemy).crit,
        isPlayer = player
    }
end

function Controller:init(scene)
    self._scene = scene
    self._generation = 0
    self._running = false
    self._criticalSelected = false
    self._retreatRequested = false
    self._watchStops = {}
end

function Controller:bind()
    local critical = self:bindCallback(Controller.requestCritical)
    local retreat = self:bindCallback(Controller.requestRetreat)
    for name, action in pairs({ CriticalButton = critical, RetreatButton = retreat }) do
        local button = self.ui.controls[name]
        ---@cast button Engine.Button
        button:addClickCallback(action)
        button:addConfirmCallback(action)
        button:addKeyDownCallback(self:bindCallback(Controller.onKeyDown))
    end
    self:watch(self, "_criticalSelected", Controller.refreshCritical)
    self:watch(self, "_retreatRequested", Controller.refreshCritical)
    self:watch(self, "_running", Controller.refreshCritical)
end

function Controller:refresh()
    self:refreshLocale()
end

function Controller:setBattleText(name, text)
    local control = assert(self.ui.controls[name])
    ---@cast control Engine.PlainText
    local centered = name:match("Name$") or name:match("Damage$")
    if name:match("Name$") then
        text = Engine.TextLayout.fitPlainText(text, 96, control)
    end
    self:setText(name, text)
    if centered or name:match("^Player.*Value$") or name:match("^Player.*Label$") and name ~= "PlayerBREATHLabel" then
        local width = Engine.TextLayout.measurePlainText(control, text)
        control:setOrigin(sf.Vector2f.new(centered and width / 2 or width, 0))
    end
end

function Controller:refreshLocale()
    for _, side in ipairs({ "Enemy", "Player" }) do
        for _, attribute in ipairs({ "HP", "ATK", "DEF", "FATIGUE", "BREATH" }) do
            local text = LOC(attribute == "FATIGUE" and "BATTLE_FATIGUE" or attribute)
            self:setBattleText(
                side .. attribute .. "Label", side == "Player" and attribute ~= "BREATH" and ":" .. text or text .. ":"
            )
        end
    end
    self:setText("RetreatLabel", LOC("BATTLE_RETREAT"))
    if self._playerActor ~= nil and self._enemyActor ~= nil then
        self:setBattleText("PlayerName", self._playerActor:getDisplayName())
        self:setBattleText("EnemyName", LOC(self._enemyActor.attributes.name))
    end
end

function Controller:open(player, enemy, onFinished)
    self:cancel()
    self._playerActor, self._enemyActor = player, enemy
    self._player = createState(player, true)
    self._enemy = createState(enemy, false)
    self._onFinished = onFinished
    self._criticalSelected = false
    self._retreatRequested = false
    self._running = true
    self:observeState(self._player, "Player")
    self:observeState(self._enemy, "Enemy")
    self:setPortrait("PlayerPortrait", player)
    self:setPortrait("EnemyPortrait", enemy)
    self:refreshLocale()
    local rect = self._scene:getGameMap():getMapViewRect()
    local size = self.host:getSize()
    self.host:setPosition(
        sf.Vector2f.new(
            math.floor(rect.position.x + (rect.size.x - size.x) / 2),
            math.floor(rect.position.y + (rect.size.y - size.y) / 2)
        )
    )
    self.host:showWithAnimation(
        "FadeIn",
        self:bindCallback(function (controller)
            if controller._running then
                controller.host:setActive(true)
                controller.host:requestKeyboardFocus()
            end
        end)
    )
    self:playBreathAnimation()
    self:schedule(Battle.attackInterval, function () self:beginTurn(true) end)
end

function Controller:setPortrait(name, actor)
    local visual = Render.CaptureActorVisual(actor)
    local frameRect = assert(visual.rect or visual.textureRect)
    if name == "PlayerPortrait" then
        frameRect = Engine.ToIntRect(0, 0, frameRect.size.x, frameRect.size.y)
        visual.animatable = false
    end
    local portrait = assert(self.ui.controls[name])
    ---@cast portrait Engine.CharacterView
    portrait:setCharacter(
        visual.texture, frameRect, visual.scale, bool(visual.animatable),
        visual.switchInterval or 0.2, visual.shaderPath or "", visual.hue or 0
    )
end

function Controller:observeState(state, side)
    for _, field in ipairs({ "HP", "ATK", "DEF", "fatigue" }) do
        self._watchStops[#self._watchStops + 1] = self:watch(state, field, function (controller, value)
            controller:setBattleText(side .. (field == "fatigue" and "FATIGUE" or field) .. "Value", tostring(value))
            controller:refreshCritical()
        end)
    end
    for _, field in ipairs({ "breath", "breathLimit" }) do
        self._watchStops[#self._watchStops + 1] = self:watch(state, field, function (controller)
            controller:refreshBreath(side, state)
            controller:refreshCritical()
        end)
    end
end

function Controller:schedule(delay, action)
    local generation = self._generation
    self._scene:addTimer(
        delay,
        self:bindCallback(function (controller)
            if controller._running and controller._generation == generation then
                action()
            end
        end)
    )
end

---@diagnostic disable-next-line: unused, Shared Controller action calculation.
function Controller:calculateDamage(attacker, defender, critical)
    local base = math.max(0, attacker.ATK - defender.DEF)
    ---@type number
    local damage = base
    if critical and base > 0 then
        damage = attacker.crit(base, attacker, defender)
        assert(
            type(damage) == "number" and damage >= 0 and damage < math.huge,
            "Battle crit must return finite non-negative damage"
        )
    end
    return math.max(0, round(damage * math.max(0, 1 - attacker.fatigue / 100))), base
end

function Controller:canCritical(attacker, defender)
    local cost = attacker.isPlayer and math.floor(attacker.breathLimit / 6) or attacker.breathLimit
    return cost > 0 and attacker.breath >= cost and self:calculateDamage(attacker, defender, true) > 0
end

function Controller:requestCritical()
    if self._running and not self._retreatRequested and not self._criticalSelected
        and self:canCritical(assert(self._player), assert(self._enemy)) then
        self._criticalSelected = true
    end
end

function Controller:requestRetreat()
    if self._running then
        self._retreatRequested = true
        self._criticalSelected = false
    end
end

function Controller:onKeyDown(_kwargs)
    if not self._running then return end
    if Input.getKeyPressed(sf.Keyboard.Key.Q, true) then
        self:requestRetreat()
    elseif Input.getKeyPressed(sf.Keyboard.Key.C, true) then
        self:requestCritical()
    end
end

function Controller:beginTurn(playerTurn)
    if self._retreatRequested then
        self:finish("retreat")
        return
    end
    local attacker = assert(playerTurn and self._player or self._enemy)
    local defender = assert(playerTurn and self._enemy or self._player)
    local critical = (not playerTurn or self._criticalSelected) and self:canCritical(attacker, defender)
    if playerTurn then self._criticalSelected = false end
    if critical then
        self:criticalAttack(attacker, defender)
    else
        self:normalAttack(attacker, defender)
    end
end

function Controller:normalAttack(attacker, defender)
    self:performAttack(attacker, defender, false)
end

function Controller:criticalAttack(attacker, defender)
    self:performAttack(attacker, defender, true)
end

function Controller:performAttack(attacker, defender, critical)
    local damage, base = self:calculateDamage(attacker, defender, critical)
    local key = base == 0 and "09_datie"
        or (damage == 0 and "08_miss" or (critical and attacker.critanimationkey or attacker.animationKey))
    local side = defender.isPlayer and "Player" or "Enemy"
    local animation = Animation.new(Data.GetAnimation(key), false)
    local position = self.ui.controls[side .. "Portrait"]:getPosition() + sf.Vector2f.new(16, 16)
    animation:setPosition(position)
    self.ui.controls["Content"]:addAnim(animation)
    ---@type number
    local hitTime = 0
    for _, tag in ipairs(animation:getAllTimeTags()) do
        if tag.tag == "dmg" then
            hitTime = tag.time
            break
        end
    end
    local function hit()
        self:receiveAttack(attacker, defender, damage, critical)
        self:setBattleText(side .. "Damage", tostring(damage))
        self:playAnimation("Damage", side .. "Damage")
    end
    if hitTime <= 0 then
        hit()
    else
        self:schedule(hitTime, hit)
    end
    self:schedule(math.max(hitTime, animation:getVisualDuration()) + Battle.attackInterval, function ()
        if defender.HP <= 0 then
            self:finish(defender.isPlayer and "lose" or "win")
        else
            self:beginTurn(not attacker.isPlayer)
        end
    end)
end

---@diagnostic disable-next-line: unused, Shared Controller action mutation.
function Controller:addBreath(state, amount)
    local maximum = math.max(0, state.breathLimit - (state.isPlayer and 1 or 0))
    state.breath = math.min(maximum, state.breath + amount)
end

function Controller:receiveAttack(attacker, defender, damage, critical)
    if damage > 0 then
        if critical then
            attacker.breath = attacker.isPlayer and attacker.breath - math.floor(attacker.breathLimit / 6) or 0
            attacker.fatigue = attacker.fatigue + Battle.criticalFatigue
        else
            local player = assert(self._player)
            local defense = attacker.isPlayer and defender.DEF or player.DEF
            local gain = player.ATK > 0 and round(defense / player.ATK * 6) or 0
            self:addBreath(attacker, gain)
        end
        self:addBreath(defender, round(damage / (defender.isPlayer and 10 or 3)))
    end
    defender.HP = math.max(0, defender.HP - damage)
end

function Controller:refreshCritical()
    local enabled = self._running and not self._retreatRequested and not self._criticalSelected and self._player ~= nil
        and self._enemy ~= nil and self:canCritical(self._player, self._enemy)
    local button = self.ui.controls["CriticalButton"]
    button:setActive(enabled == true)
    button:setColour((enabled or self._criticalSelected) and sf.Color.White or sf.Color.new(128, 128, 128, 255))
    local file = self._criticalSelected and "mting-1227.png" or "mting-528.png"
    button:setTexture(assert(GlobalCore.TextureManager.load("/Game/Assets/Icons/" .. file)), true)
    local size = button:getTexture():getSize()
    button:setOrigin(sf.Vector2f.new(size.x / 2, size.y / 2))
    self.ui.controls["RetreatButton"]:setActive(self._running and not self._retreatRequested)
end

function Controller:refreshBreath(side, state)
    local unit = math.floor(state.breathLimit / 6)
    local fraction = state.isPlayer and (unit > 0 and state.breath % unit / unit or 0)
        or (state.breathLimit > 0 and state.breath / state.breathLimit or 0)
    self.ui.controls[side .. "BreathFill"]:setTextureRect(Engine.ToIntRect(0, 4, math.floor(fraction * 80), 4))
    if state.isPlayer then
        for index = 1, 6 do
            local lit = unit > 0 and state.breath >= unit * index
            for _, kind in ipairs({ "Lit", "Dim" }) do
                local canvas = self:getBreathCanvas(kind, index)
                canvas:setVisible(true)
                canvas:setColour((lit == (kind == "Lit")) and sf.Color.White or sf.Color.Transparent)
            end
        end
    end
end

function Controller:getBreathCanvas(kind, index)
    local canvas = assert(self.ui.controls["Breath" .. kind .. index])
    ---@cast canvas Engine.Canvas
    return canvas
end

function Controller:playBreathAnimation()
    for _, kind in ipairs({ "Lit", "Dim" }) do
        local data = Data.GetAnimation("BattleBreath" .. kind)
        for index = 1, 6 do
            local canvas = self:getBreathCanvas(kind, index)
            canvas:clearAnims()
            local animation = Animation.new(data, false)
            animation:setPosition(sf.Vector2f.new(8, 10))
            canvas:addAnim(animation)
            local preview = assert(self.ui.controls["Breath" .. kind .. index .. "Preview"])
            ---@cast preview Engine.Image
            preview:setVisible(false)
        end
    end
    self:schedule(1.2, function () self:playBreathAnimation() end)
end

function Controller:finish(result)
    local callback = self._onFinished
    local player = assert(self._player)
    local hp, breath = player.HP, player.breath
    self:cancel()
    if callback ~= nil then callback(result, hp, breath) end
end

function Controller:cancel()
    self._generation = self._generation + 1
    self._running = false
    self._onFinished = nil
    for _, stop in ipairs(self._watchStops) do
        stop()
    end
    self._watchStops = {}
    self.ui.controls["Content"]:clearAnims()
    for _, side in ipairs({ "Player", "Enemy" }) do
        self:stopAnimation("Damage", side .. "Damage")
        self:setText(side .. "Damage", "")
    end
    for _, kind in ipairs({ "Lit", "Dim" }) do
        for index = 1, 6 do
            self:getBreathCanvas(kind, index):clearAnims()
        end
    end
    self.host:setActive(false)
    self.host:hideImmediate()
    self._playerActor, self._enemyActor = nil, nil
    self._player, self._enemy = nil, nil
end

function Controller:dispose()
    self:cancel()
    super(Controller, self).dispose()
end

return Ui.DefineWindow(View, Controller, WindowBase)
