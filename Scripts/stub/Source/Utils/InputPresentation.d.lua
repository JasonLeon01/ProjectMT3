---@meta Source.Utils.InputPresentation

local InputPresentation = {}

---@return Engine.InputHintDevice
function InputPresentation.GetDevice() end

---@param keyboard string
---@param gamepad  string
---@param touch    string
---@return string
function InputPresentation.ResolveText(keyboard, gamepad, touch) end

return InputPresentation
