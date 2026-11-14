---@meta

---@class Source.Windows.WindowInscription.Controller: Internal.UIBase.UiController
---@field host            Source.Windows.WindowInscription
---@field ui              Internal.UI.WindowInscription
---@field _messages       Source.SceneComponents.MapMessages
---@field _onClose        fun()
---@field _template       integer
---@field _word           integer
---@field _busy           boolean
---@field _openGeneration integer
---@field _wheelRow       integer | nil
---@field _wheelAmount    number
---@field _touchOrigin    sf.Vector2f | nil
---@field _touchRow       integer | nil
---@field _touchConsumed  boolean
---@field _confirmKeys    Engine.InputActionKey[]
local Controller = {}

---@param direction string
---@return boolean
function Controller:onDirectionalKey(direction) end
---@param index integer
---@param step  integer
function Controller:adjustRow(index, step) end
---@param index integer
---@return sf.FloatRect
function Controller:getSelectionLayoutRect(index) end
---@param index integer
---@return sf.FloatRect
function Controller:_getItemHitAbsoluteBounds(index) end
---@param position sf.Vector2f
---@return integer | nil
function Controller:getChoiceRowAt(position) end
---@param kwargs Engine.UiInputEventArguments
function Controller:onMouseWheelScrolled(kwargs) end
---@param position sf.Vector2f
---@return boolean
function Controller:_shouldCaptureTouch(position) end
---@param position sf.Vector2f
function Controller:_onCapturedTouchBegan(position) end
---@param position sf.Vector2f
---@return boolean
function Controller:_handleCapturedTouchDrag(position) end
---@param position sf.Vector2f
---@return boolean
function Controller:_handleCapturedTouchTap(position) end
function Controller:_onCapturedTouchReset() end

---@param messages Source.SceneComponents.MapMessages
---@param onClose  fun()
function Controller:init(messages, onClose) end
function Controller:bind() end
---@param mapRect sf.IntRect
function Controller:open(mapRect) end
function Controller:close() end
function Controller:dismiss() end
function Controller:onReturn() end
---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end
---@param step integer
function Controller:changeTemplate(step) end
---@param step integer
function Controller:changeWord(step) end
function Controller:refreshLocale() end
function Controller:refreshStatus() end
function Controller:confirm() end
function Controller:dispose() end
