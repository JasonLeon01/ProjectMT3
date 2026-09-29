---@meta

---@class Source.Windows.WindowMagicDoor.Controller: Internal.UIBase.UiController
---@field ui       Internal.UI.WindowMagicDoor
---@field host     Source.Windows.WindowMagicDoor
---@field _player  Source.MapActors.Player.Player
---@field _onClose fun()
local Controller = {}

---@param player  Source.MapActors.Player.Player
---@param onClose fun()
function Controller:init(player, onClose) end

---@param player Source.MapActors.Player.Player
function Controller:setPlayer(player) end

---@param mapRect sf.IntRect
function Controller:open(mapRect) end

function Controller:close() end

---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end
