---@meta

---@class Source.Windows.WindowHotkeys.Controller: Internal.UIBase.UiController
---@field ui       Internal.UI.WindowHotkeys
---@field host     Source.Windows.WindowHotkeys
---@field _onClose fun()
---@field ENTRIES  string[]
local Controller = {}

---@param onClose fun()
function Controller:init(onClose) end

---@param mapRect sf.IntRect
function Controller:open(mapRect) end

function Controller:refreshLocale() end
function Controller:close() end

---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end

---@param kwargs Engine.UiInputEventArguments
---@return boolean
function Controller:onMouseButtonDown(kwargs) end

function Controller:onReturn() end
