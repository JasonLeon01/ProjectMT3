local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Render = require("Global.Utils.Render")
local Data = require("Source.Data")
local Battle = require("Source.Configs.Battle")
local Locale = require("Source.Locale.Core")
local WindowBase = require("Source.Windows.Base.WindowBase")
local Ui = require("Source.UIBase.Ui")
local View = require("Source.UI.WindowBattle")
local BattleTextParticle = require("Source.CustomParticles.BattleTextParticle")
local Effects = require("Source.Gameplay.Effects")
local SpecialAbilities = require("Source.Gameplay.SpecialAbilities")
local PoisonedAbility = require("Source.Gameplay.SpecialAbilities.PoisonedAbility")
local VampireAbility = require("Source.Gameplay.SpecialAbilities.VampireAbility")
local Special = require("Source.Configs.GeneralEnum").Special

---@type fun(value: string): string
local LOC = Locale.ApplyStringLocaleFormat
local Input = Engine.Input
local Animation = GlobalCore.Animation

---@class Source.Windows.WindowBattle.Controller
local Controller = {}
Controller.windowOptions = { hidden = true, focusable = true }

---@param actor Source.Player.Player
---@param slot  string
---@param skills table<string, Source.Configs.Battle.Skill | nil>
---@return Source.Configs.Battle.Skill | nil
---@return string
local function resolveEquipSkill(actor, slot, skills)
    local equipID = actor:getEquipInfo(slot)
    if not bool(equipID) then
        return nil, ""
    end
    ---@cast equipID string
    local skill = skills[equipID]
    if skill == nil then
        return nil, ""
    end
    local animationKey = Data.GetGeneralEquipData(equipID).AnimationKey
    return skill, bool(animationKey) and animationKey or ""
end

---@param actor  Source.Player.Player | Source.Enemy
---@param player boolean
---@return Source.Windows.WindowBattle.BattlerState
local function createState(actor, player)
    local attributes = actor.attributes
    local states = Effects.GetStateStacks(actor)
    local abilitySystem = actor:getAbilitySystemComponent()
    ---@type Source.Configs.Battle.Rule
    local rule = player and assert(Battle.players[actor.ID], "Missing player battle config: " .. actor.ID)
        or Battle.enemy
    ---@type Source.Configs.Battle.Skill | nil
    local attackSkill = nil
    ---@type Source.Configs.Battle.Skill | nil
    local defenseSkill = nil
    local attackSkillAnimationKey = ""
    local defenseSkillAnimationKey = ""
    if player then
        ---@cast actor Source.Player.Player
        attackSkill, attackSkillAnimationKey = resolveEquipSkill(actor, "weapon", Battle.attackSkills)
        defenseSkill, defenseSkillAnimationKey = resolveEquipSkill(actor, "shield", Battle.defenseSkills)
    end
    return {
        HP = player and attributes.HP or attributes.MAXHP,
        MAXHP = attributes.MAXHP,
        ATK = (not player and SpecialAbilities.GetMagnitude(abilitySystem, Special.Ambush) ~= nil)
            and attributes.ATK * 2 or attributes.ATK,
        DEF = attributes.DEF,
        MAGIC = player and attributes.MAGIC or 0,
        breath = player and attributes.breath or 0,
        breathLimit = attributes.breathLimit,
        fatigue = 0,
        animationKey = attributes.ANIMATION_KEY,
        CritAnimationKey = attributes.CritAnimationKey,
        attackSkillAnimationKey = attackSkillAnimationKey,
        defenseSkillAnimationKey = defenseSkillAnimationKey,
        crit = rule.crit,
        attackSkill = attackSkill,
        defenseSkill = defenseSkill,
        isPlayer = player,
        poisoned = states.Poisoned or 0,
        weak = states.Weak or 0,
        addedStates = { Poisoned = 0, Weak = 0 },
        poisoning = SpecialAbilities.GetMagnitude(abilitySystem, Special.Poisoning) or 0,
        weaken = SpecialAbilities.GetMagnitude(abilitySystem, Special.Weaken) or 0,
        vampire = SpecialAbilities.GetMagnitude(abilitySystem, Special.Vampire) or 0,
        mucus = SpecialAbilities.GetMagnitude(abilitySystem, Special.Mucus) or 0,
        thunder = math.trunc(tonumber(SpecialAbilities.GetMagnitude(abilitySystem, Special.Thunder)) or 0),
        sureKill = SpecialAbilities.GetMagnitude(abilitySystem, Special.SureKill) ~= nil,
        berserk = SpecialAbilities.GetMagnitude(abilitySystem, Special.Berserk) ~= nil,
        hard = SpecialAbilities.GetMagnitude(abilitySystem, Special.Hard) ~= nil,
        magic = SpecialAbilities.GetMagnitude(abilitySystem, Special.Magic) ~= nil,
        compete = SpecialAbilities.GetMagnitude(abilitySystem, Special.Compete) ~= nil,
        first = SpecialAbilities.GetMagnitude(abilitySystem, Special.First) ~= nil,
        deathCurse = SpecialAbilities.GetMagnitude(abilitySystem, Special.DeathCurse) ~= nil,
        armorBreak = SpecialAbilities.GetMagnitude(abilitySystem, Special.ArmorBreak) ~= nil,
        burn = SpecialAbilities.GetMagnitude(abilitySystem, Special.Burn) ~= nil,
        auraField = SpecialAbilities.GetMagnitude(abilitySystem, Special.AuraField) ~= nil,
        frost = SpecialAbilities.GetMagnitude(abilitySystem, Special.Frost) ~= nil,
        burnHits = 0,
        frozenTurns = 0,
        hitCount = math.max(1, SpecialAbilities.GetMagnitude(abilitySystem, Special.MultiHit) or 1)
    }
end

function Controller:init(scene)
    self._scene = scene
    self._generation = 0
    self._running = false
    self._criticalSelected = false
    self._attackSkillSelected = false
    self._defenseSkillSelected = false
    self._retreatRequested = false
    self._watchStops = {}
    self._particles = self.ui.controls["Content"]:getParticleSystem()
end

function Controller:bind()
    local critical = self:bindCallback(Controller.requestCritical)
    local attackSkill = self:bindCallback(Controller.requestAttackSkill)
    local defenseSkill = self:bindCallback(Controller.requestDefenseSkill)
    local retreat = self:bindCallback(Controller.requestRetreat)
    for name, action in pairs({
        AttackSkillButton = attackSkill,
        CriticalButton = critical,
        DefenseSkillButton = defenseSkill,
        RetreatButton = retreat
    }) do
        local button = self.ui.controls[name]
        ---@cast button Engine.FunctionalBase
        button:addClickCallback(action)
        button:addConfirmCallback(action)
        button:addKeyDownCallback(self:bindCallback(Controller.onKeyDown))
    end
    self.ui.controls["RetreatButton"]:setTouchHitBounds(Engine.ToFloatRect(0, -10, 128, 44))
    self:watch(self, "_criticalSelected", Controller.refreshCritical)
    self:watch(self, "_attackSkillSelected", Controller.refreshCritical)
    self:watch(self, "_defenseSkillSelected", Controller.refreshCritical)
    self:watch(self, "_retreatRequested", Controller.refreshCritical)
    self:watch(self, "_running", Controller.refreshCritical)
end

function Controller:refresh()
    self:refreshLocale()
end

function Controller:setBattleText(name, text)
    local control = assert(self.ui.controls[name])
    ---@cast control Engine.PlainText
    if name:match("Name$") then
        text = Engine.TextLayout.fitPlainText(text, 96, control)
    end
    self:setText(name, text)
    self.view:reflow()
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
    self:setText("RetreatButton", LOC("BATTLE_RETREAT"))
    if self._playerActor ~= nil and self._enemyActor ~= nil then
        self:setBattleText("PlayerName", self._playerActor:getDisplayName())
        self:setBattleText("EnemyName", LOC(self._enemyActor.attributes.name))
    end
end

function Controller:open(player, enemy, onFinished)
    self:cancel()
    self._playerActor, self._enemyActor = player, enemy
    self._player = createState(player, true)
    local enemyState = createState(enemy, false)
    self._enemy = enemyState
    self._onFinished = onFinished
    self._criticalSelected = false
    self._attackSkillSelected = false
    self._defenseSkillSelected = false
    self._retreatRequested = false
    self._running = true
    self:observeState(self._player, "Player")
    self:observeState(self._enemy, "Enemy")
    self:setPortrait("PlayerPortrait", player)
    self:setPortrait("EnemyPortrait", enemy)
    self:refreshLocale()
    local rect = self._scene:getGameMap():getMapViewRect()
    local size = self.host:getSize()
    local bounds = Engine.ToFloatRect(rect.position.x, rect.position.y, rect.size.x, rect.size.y)
    local position = bounds:getCenter() - sf.Vector2f.new(size.x, size.y) / 2
    self.host:setPosition(sf.Vector2f.new(math.floor(position.x), math.floor(position.y)))
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
    self:schedule(Battle.attackInterval + Battle.attackExtraDelay, function ()
        self:beginTurn(not enemyState.first)
    end)
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
        visual.texture, frameRect, visual.scale, bool(visual.animatable), visual.switchInterval or 0.2,
        visual.shaderPath or "", visual.hue or 0
    )
end

function Controller:observeState(state, side)
    ---@param controller Source.Windows.WindowBattle.Controller
    ---@param value      integer | nil
    ---@param oldValue   integer | Class.MissingValue | nil
    local function onHPChanged(controller, value, oldValue)
        -- HP exists before binding; immediate=false excludes the initial notification.
        ---@cast value integer
        ---@cast oldValue integer
        controller:onHPChanged(side, value - oldValue)
    end
    self._watchStops[#self._watchStops + 1] = self:watch(state, "HP", onHPChanged, false)
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

function Controller:onHPChanged(side, delta)
    if delta == 0 or not self._running then return end
    local portrait = assert(self.ui.controls[side .. "Portrait"])
    ---@cast portrait Engine.CharacterView
    -- CharacterView bounds and the Content particle system share local coordinates.
    local bounds = portrait:getGlobalBounds()
    local point = sf.Vector2f.new(
        math.lerp(bounds.position.x, bounds.position.x + bounds.size.x, math.random()),
        math.lerp(bounds.position.y, bounds.position.y + bounds.size.y, math.random())
    )
    BattleTextParticle.Emit(self._particles, point, delta)
end

---@diagnostic disable-next-line: unused, Shared Controller action mutation.
function Controller:changeHP(state, delta)
    local before = state.HP
    state.HP = math.trunc(math.clamp(before + delta, 0, state.MAXHP))
    return state.HP - before
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
function Controller:calculateDamage(attacker, defender, critical, attackSkill)
    local atk = attacker.ATK
    if attacker.compete then
        atk = math.max(atk, defender.ATK)
    end
    local originalDef = defender.DEF
    local def = attacker.magic and 0 or defender.DEF
    if attacker.armorBreak then
        def = math.floor(def / 2)
    end
    if defender.hard then
        def = math.max(def, atk - 1)
    end
    local base = math.max(0, atk - def)
    ---@type number
    local damage = base
    if attackSkill and base > 0 then
        local skill = assert(attacker.attackSkill, "Attack skill is missing")
        damage = skill.apply(base, attacker, defender)
        assert(math.isFinite(damage) and damage >= 0, "Battle attack skill must return finite non-negative damage")
    elseif critical and base > 0 then
        damage = attacker.crit(base, attacker, defender)
        assert(math.isFinite(damage) and damage >= 0, "Battle crit must return finite non-negative damage")
    end
    if attacker.deathCurse and atk >= originalDef then
        damage = damage * 2
    end
    if attacker.burn and damage > 0 then
        damage = damage * (2 ^ attacker.burnHits)
    end
    damage = math.max(0, math.round(damage * math.max(0, 1 - attacker.fatigue / 100)))
    if not defender.isPlayer and defender.auraField
        and (defender.breath <= 0 or defender.breath >= defender.breathLimit) then
        damage = 0
    end
    return damage, base
end

function Controller:skillBreathCost(state)
    return math.floor(state.breathLimit / 6)
end

function Controller:canAffordSkill(state)
    local cost = self:skillBreathCost(state)
    return cost > 0 and state.breath >= cost
end

function Controller:canCritical(attacker, defender)
    local cost = attacker.isPlayer and self:skillBreathCost(attacker) or attacker.breathLimit
    return cost > 0 and attacker.breath >= cost and self:calculateDamage(attacker, defender, true) > 0
end

function Controller:canAttackSkill(attacker, defender)
    return attacker.isPlayer and attacker.attackSkill ~= nil and self:canAffordSkill(attacker)
        and self:calculateDamage(attacker, defender, false, true) > 0
end

function Controller:canDefenseSkill(state)
    return state.isPlayer and state.defenseSkill ~= nil and self:canAffordSkill(state)
end

function Controller:requestCritical()
    if self._running and not self._retreatRequested and not self._criticalSelected
        and self:canCritical(assert(self._player), assert(self._enemy)) then
        self._criticalSelected = true
        self._attackSkillSelected = false
        self._defenseSkillSelected = false
    end
end

function Controller:requestAttackSkill()
    if self._running and not self._retreatRequested and not self._attackSkillSelected
        and self:canAttackSkill(assert(self._player), assert(self._enemy)) then
        self._attackSkillSelected = true
        self._criticalSelected = false
        self._defenseSkillSelected = false
    end
end

function Controller:requestDefenseSkill()
    if self._running and not self._retreatRequested and not self._defenseSkillSelected
        and self:canDefenseSkill(assert(self._player)) then
        self._defenseSkillSelected = true
        self._criticalSelected = false
        self._attackSkillSelected = false
    end
end

function Controller:requestRetreat()
    if self._running then
        self._retreatRequested = true
        self._criticalSelected = false
        self._attackSkillSelected = false
        self._defenseSkillSelected = false
    end
end

function Controller:onKeyDown(_kwargs)
    if not self._running then return end
    if Input.getKeyPressed(sf.Keyboard.Key.Q, true) then
        self:requestRetreat()
    elseif Input.getKeyPressed(sf.Keyboard.Key.C, true) then
        self:requestCritical()
    elseif Input.getKeyPressed(sf.Keyboard.Key.Z, true) then
        self:requestAttackSkill()
    elseif Input.getKeyPressed(sf.Keyboard.Key.X, true) then
        self:requestDefenseSkill()
    end
end

function Controller:beginTurn(playerTurn, remainingHits)
    if self._retreatRequested then
        self:finish("retreat")
        return
    end
    local player = self._player
    local enemy = self._enemy
    if player == nil or enemy == nil then
        return
    end
    if playerTurn and player.frozenTurns > 0 then
        player.frozenTurns = player.frozenTurns - 1
        self:schedule(Battle.attackInterval + Battle.attackExtraDelay, function ()
            self:beginTurn(false)
        end)
        return
    end
    local attacker = playerTurn and player or enemy
    local defender = playerTurn and enemy or player
    local hits = playerTurn and 1 or (remainingHits or attacker.hitCount)
    local attackSkill = false
    local critical = false
    if playerTurn then
        if self._attackSkillSelected and self:canAttackSkill(attacker, defender) then
            attackSkill = true
        elseif self._criticalSelected and self:canCritical(attacker, defender) then
            critical = true
        end
        self._attackSkillSelected = false
        self._criticalSelected = false
    else
        critical = self:canCritical(attacker, defender)
    end
    if attackSkill then
        self:performAttack(attacker, defender, false, hits, true)
    elseif critical then
        self:criticalAttack(attacker, defender, hits)
    else
        self:normalAttack(attacker, defender, hits)
    end
end

function Controller:normalAttack(attacker, defender, remainingHits)
    self:performAttack(attacker, defender, false, remainingHits)
end

function Controller:criticalAttack(attacker, defender, remainingHits)
    self:performAttack(attacker, defender, true, remainingHits)
end

function Controller:performAttack(attacker, defender, critical, remainingHits, attackSkill)
    local damage, base = self:calculateDamage(attacker, defender, critical, attackSkill)
    local defenseSkill = false
    if not attacker.isPlayer and self._defenseSkillSelected and damage > 0 and self:canDefenseSkill(defender) then
        local skill = assert(defender.defenseSkill, "Defense skill is missing")
        local applied = skill.apply(damage, attacker, defender)
        assert(math.isFinite(applied) and applied >= 0, "Battle defense skill must return finite non-negative damage")
        damage = math.max(0, math.round(applied))
        defenseSkill = true
        self._defenseSkillSelected = false
    end
    local key
    if base == 0 then
        key = "09_datie"
    elseif damage == 0 and not defenseSkill then
        key = "08_miss"
    elseif attackSkill then
        key = bool(attacker.attackSkillAnimationKey) and attacker.attackSkillAnimationKey or attacker.animationKey
    elseif defenseSkill then
        key = bool(defender.defenseSkillAnimationKey) and defender.defenseSkillAnimationKey or attacker.animationKey
    elseif critical then
        key = attacker.CritAnimationKey
    else
        key = attacker.animationKey
    end
    local animation = Animation.new(Data.GetAnimation(key), false)
    local portrait = defender.isPlayer and self.ui.controls.PlayerPortrait or self.ui.controls.EnemyPortrait
    local position = portrait:getGlobalBounds():getCenter()
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
        self:receiveAttack(attacker, defender, damage, critical, attackSkill, defenseSkill)
    end
    if hitTime <= 0 then
        hit()
    else
        self:schedule(hitTime, hit)
    end
    local visualWait = math.max(hitTime, animation:getVisualDuration())
    local chain = remainingHits - 1 > 0
    ---@type number
    local delay = visualWait
    if not chain then
        delay = delay + Battle.attackInterval + Battle.attackExtraDelay
    end
    self:schedule(delay, function ()
        if defender.HP <= 0 then
            self:finish(defender.isPlayer and "lose" or "win")
        elseif chain then
            self:beginTurn(false, remainingHits - 1)
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

function Controller:receiveAttack(attacker, defender, damage, critical, attackSkill, defenseSkill)
    if defenseSkill and defender.isPlayer then
        local skill = assert(defender.defenseSkill, "Defense skill is missing")
        defender.breath = defender.breath - self:skillBreathCost(defender)
        defender.fatigue = defender.fatigue + skill.fatigue
    end
    if damage > 0 then
        if attackSkill then
            local skill = assert(attacker.attackSkill, "Attack skill is missing")
            attacker.breath = attacker.breath - self:skillBreathCost(attacker)
            attacker.fatigue = attacker.fatigue + skill.fatigue
        elseif critical then
            attacker.breath = attacker.isPlayer and attacker.breath - self:skillBreathCost(attacker) or 0
            attacker.fatigue = attacker.fatigue + Battle.criticalFatigue
            if attacker.thunder > 0 and defender.isPlayer then
                defender.breath = math.max(0, defender.breath - math.floor(defender.breathLimit / 3))
                defender.fatigue = defender.fatigue + attacker.thunder
            end
            if attacker.frost and defender.isPlayer then
                defender.frozenTurns = 1
            end
        else
            local player = assert(self._player)
            local defense = attacker.isPlayer and defender.DEF or player.DEF
            local gain = player.ATK > 0 and math.round(defense / player.ATK * 6) or 0
            self:addBreath(attacker, gain)
        end
        if not (attacker.thunder > 0 and defender.isPlayer) then
            self:addBreath(defender, math.round(damage / (defender.isPlayer and 10 or 3)))
        end
    end
    if damage <= 0 then return end
    local lostHP = -self:changeHP(defender, -damage)
    if defender.HP > 0 then
        self:changeHP(defender, -PoisonedAbility.CalculateDamage(damage, defender.poisoned))
    end
    self:changeHP(attacker, VampireAbility.CalculateHealing(lostHP, attacker.vampire))
    self:applyAttackStates(attacker, defender)
    if not attacker.isPlayer then
        defender.fatigue = defender.fatigue + attacker.mucus
    end
    if attacker.burn then
        attacker.burnHits = attacker.burnHits + 1
    end
    if critical and attacker.berserk then
        self:changeHP(attacker, -math.floor(attacker.HP / 2))
    end
end

---@diagnostic disable-next-line: unused, Shared Controller action mutation.
function Controller:applyAttackStates(attacker, defender)
    defender.poisoned = defender.poisoned + attacker.poisoning
    defender.weak = defender.weak + attacker.weaken
    defender.addedStates.Poisoned = defender.addedStates.Poisoned + attacker.poisoning
    defender.addedStates.Weak = defender.addedStates.Weak + attacker.weaken
    -- The snapshot already includes pre-existing Weak modifiers.
    defender.ATK = math.max(0, defender.ATK - attacker.weaken)
    defender.DEF = math.max(0, defender.DEF - attacker.weaken)
end

function Controller:refreshActionButton(name, selected, enabled)
    local button
    local selectedImage
    if name == "CriticalButton" then
        button = self.ui.controls.CriticalButton
        selectedImage = self.ui.controls.CriticalButtonSelected
    elseif name == "AttackSkillButton" then
        button = self.ui.controls.AttackSkillButton
        selectedImage = self.ui.controls.AttackSkillButtonSelected
    else
        button = self.ui.controls.DefenseSkillButton
        selectedImage = self.ui.controls.DefenseSkillButtonSelected
    end
    button:setVisible(not selected)
    button:setActive(enabled == true)
    button:setColour(enabled and sf.Color.White or sf.Color.new(128, 128, 128, 255))
    selectedImage:setVisible(selected == true)
end

function Controller:refreshCritical()
    local player = self._player
    local enemy = self._enemy
    if not self._running or self._retreatRequested or player == nil or enemy == nil then
        self:refreshActionButton("CriticalButton", false, false)
        self:refreshActionButton("AttackSkillButton", false, false)
        self:refreshActionButton("DefenseSkillButton", false, false)
        self.ui.controls["RetreatButton"]:setActive(self._running and not self._retreatRequested)
        return
    end
    self:refreshActionButton(
        "CriticalButton",
        self._criticalSelected,
        not self._criticalSelected and self:canCritical(player, enemy)
    )
    self:refreshActionButton(
        "AttackSkillButton",
        self._attackSkillSelected,
        not self._attackSkillSelected and self:canAttackSkill(player, enemy)
    )
    self:refreshActionButton(
        "DefenseSkillButton",
        self._defenseSkillSelected,
        not self._defenseSkillSelected and self:canDefenseSkill(player)
    )
    self.ui.controls["RetreatButton"]:setActive(true)
end

function Controller:refreshBreath(side, state)
    local unit = math.floor(state.breathLimit / 6)
    local fraction = state.isPlayer and (unit > 0 and state.breath % unit / unit or 0)
        or (state.breathLimit > 0 and state.breath / state.breathLimit or 0)
    local bar = self.ui.controls[side .. "BreathBar"]
    ---@cast bar Engine.ProgressBar
    bar:setProgress(fraction)
    if state.isPlayer then
        for _, kind in ipairs({ "Lit", "Dim" }) do
            for index = 1, self:getBreathBox(kind):getCount() do
                local lit = unit > 0 and state.breath >= unit * index
                local canvas = self:getBreathCanvas(kind, index)
                canvas:setVisible(true)
                canvas:setColour((lit == (kind == "Lit")) and sf.Color.White or sf.Color.Transparent)
            end
        end
    end
end

function Controller:getBreathBox(kind)
    local box = assert(self.ui.controls["Breath" .. kind])
    assert(Class.isInstance(box, Engine.WrapBox), "Breath group must be an Engine.WrapBox")
    ---@cast box Engine.WrapBox
    return box
end

function Controller:getBreathCanvas(kind, index)
    local canvas = self:getBreathBox(kind):get(index)
    assert(Class.isInstance(canvas, Engine.Canvas), "Breath template must be an Engine.Canvas")
    ---@cast canvas Engine.Canvas
    return canvas
end

function Controller:playBreathAnimation()
    for _, kind in ipairs({ "Lit", "Dim" }) do
        local data = Data.GetAnimation("BattleBreath" .. kind)
        for index = 1, self:getBreathBox(kind):getCount() do
            local canvas = self:getBreathCanvas(kind, index)
            canvas:clearAnims()
            local animation = Animation.new(data, false)
            animation:setPosition(sf.Vector2f.new(8, 10))
            canvas:addAnim(animation)
            local preview = canvas:getChildren()[1]
            preview:setVisible(false)
        end
    end
    self:schedule(1.2, function () self:playBreathAnimation() end)
end

function Controller:finish(result)
    local callback = self._onFinished
    local player = assert(self._player)
    local hp, breath, addedStates = player.HP, player.breath, player.addedStates
    self:cancel()
    if callback ~= nil then callback(result, hp, breath, addedStates) end
end

function Controller:cancel()
    self._generation = self._generation + 1
    self._running = false
    self._criticalSelected = false
    self._attackSkillSelected = false
    self._defenseSkillSelected = false
    self._onFinished = nil
    for _, stop in ipairs(self._watchStops) do
        stop()
    end
    self._watchStops = {}
    self.ui.controls["Content"]:clearAnims()
    self._particles:clear()
    for _, kind in ipairs({ "Lit", "Dim" }) do
        for index = 1, self:getBreathBox(kind):getCount() do
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
