local Locale = require("Source.Locale.Core")

---@type fun(value: string): string
local LOC = Locale.ApplyStringLocaleFormat

---@class Source.Configs.Messages
local Messages = {}

---@type string[]
Messages.TEMPLATES = {
    "MESSAGES_TEMPLATE_AHEAD", "MESSAGES_TEMPLATE_BEWARE", "MESSAGES_TEMPLATE_SEEK", "MESSAGES_TEMPLATE_REMEMBER",
    "MESSAGES_TEMPLATE_NEED"
}
---@type string[]
Messages.WORDS = {
    "MESSAGES_WORD_ENEMY", "MESSAGES_WORD_AMBUSH", "MESSAGES_WORD_TRAP", "MESSAGES_WORD_POISON",
    "MESSAGES_WORD_WEAKNESS", "MESSAGES_WORD_MECHANISM", "MESSAGES_WORD_PASSAGE", "MESSAGES_WORD_STAIRS",
    "MESSAGES_WORD_KEY", "MESSAGES_WORD_POTION", "MESSAGES_WORD_GEM", "MESSAGES_WORD_CHEST", "MESSAGES_WORD_SHOP",
    "MESSAGES_WORD_WEAPON", "MESSAGES_WORD_ARMOUR", "MESSAGES_WORD_TELEPORTER"
}
Messages.LAYER = "messages"
Messages.BLUEPRINT = "Data.Blueprints.Actors.BP_PlayerMessage"

function Messages.IsAllowed(templateKey, wordKey)
    return table.contains(Messages.TEMPLATES, templateKey) and table.contains(Messages.WORDS, wordKey)
end

function Messages.Format(templateKey, wordKey)
    assert(Messages.IsAllowed(templateKey, wordKey), "Unknown message phrase")
    return string.pformat(LOC(templateKey), { word = LOC(wordKey) })
end

return Messages
