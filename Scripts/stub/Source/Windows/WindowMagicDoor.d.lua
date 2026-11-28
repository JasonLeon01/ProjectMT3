---@meta

---@class Source.Windows.WindowMagicDoor.Controller: Internal.UIBase.UiController
---@field ui       Internal.UI.WindowMagicDoor
---@field host     Source.Windows.WindowMagicDoor
---@field _player  Source.MapActors.Player.Player
---@field _onClose fun()
local Controller = {}

Controller.windowOptions = {}

---@param player  Source.MapActors.Player.Player
---@param onClose fun()
function Controller:init(player, onClose) end

---@param player Source.MapActors.Player.Player
function Controller:setPlayer(player) end

---@param mapRect sf.IntRect
function Controller:open(mapRect) end

function Controller:close() end

---@param deltaTime number
function Controller:onTick(deltaTime) end

---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end

---@param kwargs Engine.UiInputEventArguments
---@return boolean
function Controller:onMouseButtonDown(kwargs) end

function Controller:onReturn() end

function Controller:confirm() end

---@param position sf.Vector2f
---@return boolean
function Controller:_isReturnButtonHit(position) end
