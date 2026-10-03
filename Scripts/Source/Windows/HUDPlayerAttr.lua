local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local State = require("Enums.GeneralData.State")
local Item = require("Enums.GeneralData.Item")
local Data = require("Source.Data")
local EventKey = require("Enums.EventKey")
local GeneralDataTypes = require("Source.Configs.GeneralDataTypes")
local Effects = require("Source.Gameplay.Effects")
local LocaleCore = require("Source.Locale.Core")
local NumberFormat = require("Source.Utils.NumberFormat")
local IconTexture = require("Internal.UIBase.IconTexture")
local PlayerStateRowController = require("Source.Windows.HUDPlayerAttr.PlayerStateRow.Controller")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.PlayerAttrHUD")
local GameplayConstants = require("Source.Configs.GameplayConstants")

---@type fun(value: string): string
local LOC = LocaleCore.ApplyStringLocaleFormat
local Animation = GlobalCore.Animation
local Canvas = Engine.Canvas
local ToShortNumber = NumberFormat.ToShortNumber
local createStateSignature = tuple
local createSignature = tuple
---@cast createStateSignature fun(values: string[]): tuple<string>
---@cast createSignature fun(...: any): tuple<any>

local _BREATH_ANIM_DURATION = 1.2
local _AVATAR_STEP = 8
local _SWIPE_DISTANCE = 8
local Input = Engine.Input

local function getStateSignature(states)
    ---@type string[]
    local stateIDs = {}
    for index, state in ipairs(states) do
        stateIDs[index] = state.ID
    end
    return createStateSignature(stateIDs)
end

local function getStateDisplaySignature(states, language)
    ---@type string[]
    local values = { language }
    for _, state in ipairs(states) do
        local stateData = Data.GetGeneralStateData(state.ID)
        values[#values + 1] = state.ID
        values[#values + 1] = state.icon or stateData.icon or ""
        values[#values + 1] = state.name or ""
    end
    return createStateSignature(values)
end

local function stateSignatureMatches(currentSignature, rowCount, signature)
    return currentSignature == signature and rowCount == #signature
end

---@class Source.Windows.PlayerAttrHUD.Controller
local Controller = {}

Controller.refreshEvents = { EventKey.LocaleChanged, EventKey.AbilitySystemChanged, EventKey.PlayerChanged }

function Controller:init(inst, openMenuCallback, switchPlayerCallback)
    self._inst = inst
    self._player = inst:getPlayer()
    self._openMenuCallback = openMenuCallback
    self._switchPlayerCallback = switchPlayerCallback
    self._avatarSize = self.ui.controls["Avatar"]:getSize():copy()
    self._namePosition = self.ui.controls["PlayerName"]:getPosition():copy()
    self._avatarWidth = self._avatarSize.x
    self._touchStartPosition = nil
    self._touchDragging = false
    self._stateSignature = nil
    self._stateDisplaySignature = nil
    self._language = ""
    self._headerSignature = nil
    self._combatSignature = nil
    self._hpSignature = nil
    self._statSignature = nil
    self._magicSignature = nil
    self._stackSignature = nil
    self._breathSignature = nil
    self._breathAnimElapsed = 0
    self._breathColours = {}
    self._progressSignature = nil
    self._keySignature = nil
    self._states = self:createCollection(self.ui.controls["StateHost"], PlayerStateRowController)
end

function Controller:setInstance(inst)
    self:_resetAvatarTouch()
    self._inst = inst
    self._player = inst:getPlayer()
    self:refreshAvatars()
    self:refresh()
end

---@param payload Source.Configs.EventChangePayload | { language: string } | nil
function Controller:refreshFromEvent(payload)
    if payload ~= nil and payload.owner ~= nil and payload.owner ~= self:getPlayer() then
        return
    end
    super(Controller, self).refreshFromEvent(payload)
end

function Controller:onTick(deltaTime)
    self:_layoutAvatars()
    if LUDORK_MOBILE then
        self:_updateAvatarTouch()
    end
    self._breathAnimElapsed = self._breathAnimElapsed + deltaTime
    if self._breathAnimElapsed >= _BREATH_ANIM_DURATION then
        self:playBreathAnimation()
    end
end

function Controller:ready()
    self:_layoutAvatars()
end

function Controller:getPlayer()
    return self._player
end

function Controller:openMenu()
    if self._openMenuCallback ~= nil then
        self._openMenuCallback()
    end
end

function Controller:refreshAvatars()
    local keys = self._inst:getPlayerKeys()
    local avatars = self.ui.controls["Avatar"]
    avatars:setCount(#keys)
    self._avatarWidth = self._avatarSize.x + _AVATAR_STEP * (#keys - 1)
    avatars:setSpacing(sf.Vector2f.new(_AVATAR_STEP - self._avatarSize.x, 0))
    for index = 1, #keys do
        local avatar = avatars:get(index)
        assert(Class.isInstance(avatar, Engine.Button), "Avatar template must be an Engine.Button")
        ---@cast avatar Engine.Button
        avatar:setColour(sf.Color.new(255, 255, 255, index == #keys and 255 or 128))
        local texture = self._inst:getPlayerByIndex(#keys - index):getTexture()
        avatar:setVisible(texture ~= nil)
        if texture ~= nil then
            local textureSize = texture:getSize()
            avatar:setTexture(texture, true)
            avatar:setTextureRect(
                sf.IntRect.new(
                    0, 0, math.max(1, math.floor(textureSize.x / 4)), math.max(1, math.floor(textureSize.y / 4))
                )
            )
        end
        avatar:addClickCallback(self:bindCallback(Controller.openMenu))
        if LUDORK_MOBILE then
            -- The HUD owns the whole gesture, including taps, instead of each overlapping button.
            avatar:setTouchHitBounds(Engine.ToFloatRect(0, 0, 0, 0))
        end
    end
    self._headerSignature = nil
    self:_layoutAvatars()
end

function Controller:_layoutAvatars()
    -- View reflow restores authored slots; keep the dynamic party layout and double reflection.
    local avatars = self.ui.controls["Avatar"]
    avatars:setScale(sf.Vector2f.new(-1, 1))
    for _, avatar in ipairs(avatars:getChildren()) do
        local size = avatar:getSize()
        avatar:setScale(sf.Vector2f.new(-self._avatarSize.x / size.x, self._avatarSize.y / size.y))
    end
    avatars:setSize(sf.Vector2f.new(self._avatarWidth, self._avatarSize.y))
    avatars:setOrigin(sf.Vector2f.new(self._avatarWidth, 0))
    self.ui.controls["PlayerName"]:setPosition(
        self._namePosition + sf.Vector2f.new(self._avatarWidth - self._avatarSize.x, 0)
    )
end

function Controller:_onPartyChanged(payload)
    if payload.instance == self._inst then
        self:_resetAvatarTouch()
        self:refreshAvatars()
        self:refresh()
    end
end

function Controller:_resetAvatarTouch()
    if self._touchStartPosition ~= nil then
        Input.cancelTouchGesture()
    end
    self._touchStartPosition = nil
    self._touchDragging = false
end

function Controller:_updateAvatarTouch()
    if Input.isTouchBlocked() or not self.host:getActive() then
        self:_resetAvatarTouch()
        return
    end
    local bounds = self.ui.controls["Avatar"]:getAbsoluteBounds()
    if Input.isTouchBegan(false) then
        local position = Input.getTouchBeganPosition()
        if position ~= nil and bounds:contains(Engine.ToVector2f(position)) then
            self._touchStartPosition = Engine.ToVector2f(position)
            self._touchDragging = false
            Input.isTouchBegan(true)
        end
    end
    if self._touchStartPosition == nil then
        return
    end
    self._touchDragging = self._touchDragging or Input.isTouchDragged()
    if Input.isTouchEnded() then
        local position = Input.getTouchEndedPosition()
        local startPosition = self._touchStartPosition
        local dragging = self._touchDragging
        local tap = Input.isTouchTap(false)
        self:_resetAvatarTouch()
        if position == nil then
            return
        end
        local endPosition = Engine.ToVector2f(position)
        local delta = (endPosition - startPosition) / GlobalCore.Display.getScale()
        if dragging and delta.x * delta.x > _SWIPE_DISTANCE * _SWIPE_DISTANCE and delta.x * delta.x > delta.y * delta.y then
            if self._switchPlayerCallback ~= nil then
                self._switchPlayerCallback(delta.x > 0)
            end
        elseif tap and not dragging and bounds:contains(endPosition) then
            self:openMenu()
        end
    elseif not Input.isTouchActive() then
        self:_resetAvatarTouch()
    end
end

function Controller:bind()
    self:refreshAvatars()
    self:setProperty("PlainText", "visible", not LUDORK_MOBILE)
    self:subscribe(EventKey.PartyChanged, self:bindCallback(Controller._onPartyChanged))
    for _, kind in ipairs({ "Lit", "Dim" }) do
        local colours = {}
        for index = 1, self:getBreathBox(kind):getCount() do
            colours[index] = self:getBreathCanvas(kind, index):getColour():copy()
        end
        self._breathColours[kind] = colours
    end
    self:playBreathAnimation()
end

---@param states    Source.Configs.GeneralDataTypes.StateAttributeSet[]
---@param signature tuple<string>
function Controller:_rebuildStateRows(states, signature)
    self._states:clear()
    self._stateDisplaySignature = nil
    self._stateSignature = signature
    for _ in ipairs(states) do
        self._states:add({ iconTexture = nil, name = "" })
    end
end

---@param states Source.Configs.GeneralDataTypes.StateAttributeSet[]
function Controller:_updateStateRows(states)
    local x = 0.0
    for index, state in ipairs(states) do
        local stateData = Data.GetGeneralStateData(state.ID)
        local iconPath = state.icon or stateData.icon or ""
        local texture = IconTexture.Load(iconPath)
        local row = assert(self._states.items[index])
        row.model.iconTexture = texture
        row.model.name = state.name
        local rowRoot = row:prepare()
        rowRoot:setPosition(sf.Vector2f.new(x, rowRoot:getPosition().y))
        x = x + row:getWidth()
    end
end

function Controller:refreshStates(language)
    local states = {}
    for _, stateID in ipairs(Effects.GetStateIDs(self:getPlayer())) do
        states[#states + 1] = GeneralDataTypes.Create("State", stateID, Data.GetGeneralStateData(stateID))
    end
    local signature = getStateSignature(states)
    local rebuild = not stateSignatureMatches(self._stateSignature, #self._states.items, signature)
    if rebuild then
        self:_rebuildStateRows(states, signature)
    end
    local displaySignature = getStateDisplaySignature(states, language or LocaleCore.GetLanguage())
    if not rebuild and self._stateDisplaySignature == displaySignature then
        return false
    end
    self._stateDisplaySignature = displaySignature
    if bool(states) then
        self:_updateStateRows(states)
    end
    return true
end

function Controller:refresh()
    local layoutDirty = false
    local language = LocaleCore.GetLanguage()
    local localeChanged = self._language ~= language
    local gameMap = self:getPlayer():getMap()
    local mapName = ""
    if gameMap ~= nil then
        ---@cast gameMap GameMap
        mapName = tostring(gameMap.mapName)
    end
    local playerName = self:getPlayer():getDisplayName()
    local headerSignature = createSignature(language, mapName, playerName)
    if self._headerSignature ~= headerSignature then
        self._headerSignature = headerSignature
        self._language = language
        self:setText("MapName", LOC(tostring(mapName)))
        self:setText(
            "PlayerName",
            Engine.TextLayout.fitPlainText(
                playerName,
                math.max(
                    0,
                    self.ui.controls["Canvas"]:getSize().x - self._namePosition.x - self._avatarWidth + self._avatarSize.x
                ), self.ui.controls["PlayerName"]
            )
        )
        self:setText("HpLabel", LOC("HP"))
        self:setText("AtkLabel", LOC("ATK"))
        self:setText("DefLabel", LOC("DEF"))
        self:setText("MagicLabel", LOC("MAGIC"))
        self:setText("BreathLabel", LOC("BREATH"))
        self:setText("ExpLabel", LOC("EXP"))
        self:setText("GoldLabel", LOC("GOLD"))
        layoutDirty = true
    end

    local abilitySystem = self:getPlayer():getAbilitySystemComponent()
    local combatSignature = createSignature(self:getPlayer().attributes, abilitySystem:getRevision())
    local refreshStateRows = localeChanged
    if self._combatSignature ~= combatSignature then
        self._combatSignature = combatSignature
        refreshStateRows = true

        local hpSignature = createSignature(self:getPlayer().attributes.HP, self:getPlayer().attributes.MAXHP)
        if self._hpSignature ~= hpSignature then
            self._hpSignature = hpSignature
            self:setText(
                "HpValue",
                "#default#" .. tostring(ToShortNumber(self:getPlayer().attributes.HP)) .. "/#max#"
                    .. tostring(ToShortNumber(self:getPlayer().attributes.MAXHP)) .. "#default#"
            )
            self.ui.controls["HpBar"]:setProgress(
                self:getPlayer().attributes.MAXHP > 0 and self:getPlayer().attributes.HP
                        / self:getPlayer().attributes.MAXHP or 0.0
            )
            layoutDirty = true
        end

        local statSignature = createSignature(self:getPlayer().attributes.ATK, self:getPlayer().attributes.DEF)
        if self._statSignature ~= statSignature then
            self._statSignature = statSignature
            self:setText("AtkValue", tostring(ToShortNumber(self:getPlayer().attributes.ATK)))
            self:setText("DefValue", tostring(ToShortNumber(self:getPlayer().attributes.DEF)))
            layoutDirty = true
        end

        local weakStacks = abilitySystem:getActiveEffectStacks(GameplayConstants.STATE_PREFIX .. State.Weak)
        local poisonStacks = abilitySystem:getActiveEffectStacks(GameplayConstants.STATE_PREFIX .. State.Poisoned)
        local stackSignature = createSignature(weakStacks, poisonStacks)
        if self._stackSignature ~= stackSignature then
            self._stackSignature = stackSignature
            local debuffString = weakStacks > 0 and "(-" .. tostring(weakStacks) .. ")" or ""
            self:setText("AtkDebuff", debuffString)
            self:setText("DefDebuff", debuffString)
            self:setText("HpPoison", poisonStacks > 0 and "(" .. tostring(poisonStacks) .. ")" or "")
            layoutDirty = true
        end

        local magic = self:getPlayer():getAttr("MAGIC")
        local magicSignature = createSignature(magic)
        if self._magicSignature ~= magicSignature then
            self._magicSignature = magicSignature
            self:setText("MagicValue", tostring(ToShortNumber(magic)))
            layoutDirty = true
        end

        local breathSignature = createSignature(
            self:getPlayer().attributes.breath, self:getPlayer().attributes.breathLimit
        )
        if self._breathSignature ~= breathSignature then
            self._breathSignature = breathSignature
            self:refreshBreath()
            layoutDirty = true
        end

        local progressSignature = createSignature(
            self:getPlayer().attributes.LEVEL, self:getPlayer().attributes.EXP, self:getPlayer().attributes.GOLD
        )
        if self._progressSignature ~= progressSignature then
            self._progressSignature = progressSignature
            self:setText("Level", "Lv. " .. tostring(self:getPlayer().attributes.LEVEL))
            self:setText("ExpValue", tostring(ToShortNumber(self:getPlayer().attributes.EXP)))
            self:setText("GoldValue", tostring(ToShortNumber(self:getPlayer().attributes.GOLD)))
            layoutDirty = true
        end
    end

    local keyYCount = self:getPlayer():getItemCount(Item.KEY_Y)
    local keyBCount = self:getPlayer():getItemCount(Item.KEY_B)
    local keyRCount = self:getPlayer():getItemCount(Item.KEY_R)
    local keySignature = createSignature(keyYCount, keyBCount, keyRCount)
    if self._keySignature ~= keySignature then
        self._keySignature = keySignature
        self:setText(
            "ItemCounts",
            "#Yellow#" .. string.format("%02d", keyYCount) .. "#default#  #Blue#" .. string.format("%02d", keyBCount)
                .. "#default#  #Red#" .. string.format("%02d", keyRCount) .. "#default#"
        )
        layoutDirty = true
    end

    if refreshStateRows then
        self:refreshStates(language)
    end
    if layoutDirty then
        self.view:reflow()
        self:_layoutAvatars()
    end
end

function Controller:refreshBreath()
    local attributes = self:getPlayer().attributes
    local unit = math.floor(attributes.breathLimit / 6)
    local fraction = unit > 0 and attributes.breath % unit / unit or 0
    local bar = self.ui.controls["BreathBar"]
    ---@cast bar Engine.ProgressBar
    bar:setProgress(fraction)
    for _, kind in ipairs({ "Lit", "Dim" }) do
        for index = 1, self:getBreathBox(kind):getCount() do
            local lit = unit > 0 and attributes.breath >= unit * index
            local canvas = self:getBreathCanvas(kind, index)
            canvas:setVisible(true)
            local colour = self._breathColours[kind][index]
            canvas:setColour((lit == (kind == "Lit")) and colour or sf.Color.Transparent)
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
    assert(Class.isInstance(canvas, Canvas), "Breath template must be an Engine.Canvas")
    ---@cast canvas Engine.Canvas
    return canvas
end

function Controller:playBreathAnimation()
    self._breathAnimElapsed = 0
    for _, kind in ipairs({ "Lit", "Dim" }) do
        local data = Data.GetAnimation("BattleBreath" .. kind)
        for index = 1, self:getBreathBox(kind):getCount() do
            local canvas = self:getBreathCanvas(kind, index)
            canvas:clearAnims()
            local preview = canvas:getChildren()[1]
            local previewCenterX = preview:getTransform():transformRect(preview:getLocalBounds()):getCenter().x
            local animation = Animation.new(data, false)
            animation:setPosition(sf.Vector2f.new(previewCenterX, canvas:getSize().y / 2))
            canvas:addAnim(animation)
            preview:setVisible(false)
        end
    end
end

function Controller:dispose()
    self:_resetAvatarTouch()
    for _, kind in ipairs({ "Lit", "Dim" }) do
        for index = 1, self:getBreathBox(kind):getCount() do
            self:getBreathCanvas(kind, index):clearAnims()
        end
    end
    super(Controller, self).dispose()
end

return Ui.DefineWindow(View, Controller, Canvas)
