local Engine = require("Engine")

local InputPresentation = {}

function InputPresentation.GetDevice()
    return Engine.Input.getInputHintDevice()
end

function InputPresentation.ResolveText(keyboard, gamepad, touch)
    local device = InputPresentation.GetDevice()
    if device == Engine.InputHintDevice.Gamepad then return gamepad end
    if device == Engine.InputHintDevice.Touch then return touch end
    return keyboard
end

return InputPresentation
