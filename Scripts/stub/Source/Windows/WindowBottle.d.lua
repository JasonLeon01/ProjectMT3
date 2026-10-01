---@meta

---@class Source.Windows.WindowBottle.Controller: Internal.UIBase.UiController
---@field ui       Internal.UI.WindowBottle
---@field host     Source.Windows.WindowBottle
---@field _player  Source.MapActors.Player.Player
---@field _onClose fun()
---@field _rows    Internal.UIBase.UiCollection<Source.Windows.WindowBottle.BottleRow.Controller>
local Controller = {}

---@param player  Source.MapActors.Player.Player
---@param onClose fun()
function Controller:init(player, onClose) end

function Controller:bind() end

---@param payload table
function Controller:onPlayerChanged(payload) end

---@param player Source.MapActors.Player.Player
function Controller:setPlayer(player) end

function Controller:refreshItems() end

---@param mapRect sf.IntRect
function Controller:open(mapRect) end

function Controller:close() end

function Controller:onReturn() end

---@param deltaTime number
function Controller:onTick(deltaTime) end

function Controller:useSelectedItem() end
