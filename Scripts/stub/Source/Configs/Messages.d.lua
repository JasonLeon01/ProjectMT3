---@meta Source.Configs.Messages

---@class Source.Configs.Messages
---@field TEMPLATES string[]
---@field WORDS     string[]
---@field LAYER     string
---@field BLUEPRINT string
local Messages = {}

---@param templateKey string
---@param wordKey     string
---@return boolean
function Messages.IsAllowed(templateKey, wordKey) end

---@param templateKey string
---@param wordKey     string
---@return string
function Messages.Format(templateKey, wordKey) end

return Messages
