---@meta

---
--- Shows the player's avatar, current map name, level, states, HP bar with value, stat values, and breath slots.
---@class Source.Windows.PlayerAttrHUD.Controller: Internal.UIBase.UiController
---@field host                   Source.Windows.PlayerAttrHUD
---@field _player                Source.MapActors.Player.Player
---@field _openMenuCallback      function | nil
---@field ui                     Internal.UI.PlayerAttrHUD
---@field refreshEvents          string[]
---@field _stateSignature        tuple<string> | nil
---@field _stateDisplaySignature tuple<string> | nil
---@field _inst                  Source.GameInstance.GameInstance
---@field _switchPlayerCallback  fun(right: boolean): boolean | nil
---@field _avatarWidth           number
---@field _avatarSize            sf.Vector2f
---@field _namePosition          sf.Vector2f
---@field _touchStartPosition    sf.Vector2f | nil
---@field _touchDragging         boolean
---@field _language              string
---@field _headerSignature       tuple<any> | nil
---@field _combatSignature       tuple<any> | nil
---@field _hpSignature           tuple<any> | nil
---@field _statSignature         tuple<any> | nil
---@field _magicSignature        tuple<any> | nil
---@field _stackSignature        tuple<any> | nil
---@field _breathSignature       tuple<any> | nil
---@field _breathAnimElapsed     number
---@field _breathColours         table<string, sf.Color[]>
---@field _progressSignature     tuple<any> | nil
---@field _keySignature          tuple<any> | nil
---@field _states                Internal.UIBase.UiCollection<Source.Windows.HUDPlayerAttr.PlayerStateRow.Controller>
local Controller = {}

--- Construct the party HUD with menu and player-switch callbacks.
---@param inst                 Source.GameInstance.GameInstance
---@param openMenuCallback     function | nil
---@param switchPlayerCallback fun(right: boolean): boolean | nil
function Controller:init(inst, openMenuCallback, switchPlayerCallback) end

--- Rebind the party and its primary player, including after loading a save.
---@param inst Source.GameInstance.GameInstance
function Controller:setInstance(inst) end

function Controller:refreshAvatars() end
function Controller:ready() end
function Controller:_layoutAvatars() end
---@param payload { instance: Source.GameInstance.GameInstance }
function Controller:_onPartyChanged(payload) end
function Controller:_resetAvatarTouch() end
function Controller:_updateAvatarTouch() end

--- Ignore Ability System and player events from other battlers, then refresh the HUD.
---
--- - @param payload EventBus payload. Locale events have no owner; player and ability events include `owner`.
---@param payload Source.Configs.EventKeys.ChangePayload | { language: string } | nil
function Controller:refreshFromEvent(payload) end

--- Advance the breath-slot animation clock and replay the loop when it elapses.
---
--- - @param deltaTime  Elapsed frame time in seconds
---@param deltaTime number
function Controller:onTick(deltaTime) end

---@return Source.MapActors.Player.Player
function Controller:getPlayer() end

function Controller:openMenu() end

function Controller:bind() end

---@param language string | nil
---@return boolean
function Controller:refreshStates(language) end

function Controller:refresh() end

---@param kind "Lit" | "Dim"
---@return Engine.WrapBox
function Controller:getBreathBox(kind) end

---@param kind  "Lit" | "Dim"
---@param index integer
---@return Engine.Canvas
function Controller:getBreathCanvas(kind, index) end

function Controller:refreshBreath() end

function Controller:playBreathAnimation() end

function Controller:dispose() end
