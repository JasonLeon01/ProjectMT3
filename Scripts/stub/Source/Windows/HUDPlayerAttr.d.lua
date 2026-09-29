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
---@field _avatarTexture         sf.Texture | nil
---@field _avatarRect            sf.IntRect | nil
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

--- Construct a player attribute HUD bound to the given player instance.
---
--- - @param player  Target player whose attributes are displayed on this HUD
--- - @param openMenuCallback Callback invoked when the player avatar is clicked
---@param player           Source.MapActors.Player.Player
---@param openMenuCallback function | nil
function Controller:init(player, openMenuCallback) end

--- Rebind the player whose values are displayed by this HUD.
---
--- - @param player Target player.
---@param player Source.MapActors.Player.Player
function Controller:setPlayer(player) end

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
