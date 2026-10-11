---@meta Source.Configs.Tutorial

---@class Source.Configs.Tutorial.Entry
---@field rect sf.IntRect             Fixed rectangle in logical game-screen coordinates.
---@field text string | fun(): string Unresolved locale source, evaluated on display and input-device changes.

---@type table<string, Source.Configs.Tutorial.Entry>
local Tutorial = {}

return Tutorial
