local InputPresentation = require("Source.Utils.InputPresentation")

---@type table<string, Source.Configs.Tutorial.Entry>
local Tutorial = {
    TM_01 = {
        rect = sf.IntRect.new(16, 16, 32, 32),
        text = "{TM_01}"
    },
    TM_02 = {
        rect = sf.IntRect.new(416, 320, 32, 32),
        text = "{TM_02}"
    },
    TM_03 = {
        rect = sf.IntRect.new(256, 352, 32, 32),
        text = "{TM_03}"
    },
    TM_04 = {
        rect = sf.IntRect.new(512, 448, 128, 32),
        text = function ()
            return InputPresentation.ResolveText("{TM_04}", "{TM_04_G}", "{TM_04_M}")
        end
    },
    TM_05 = {
        rect = sf.IntRect.new(160, 80, 320, 64),
        text = function ()
            return InputPresentation.ResolveText("{TM_05}", "{TM_05_G}", "{TM_05_M}")
        end
    },
    TM_06 = {
        rect = sf.IntRect.new(16, 16, 32, 32),
        text = function ()
            return InputPresentation.ResolveText("{TM_06_D}", "{TM_06}", "{TM_06}")
        end
    },
    TM_07 = {
        rect = sf.IntRect.new(16, 16, 32, 32),
        text = function ()
            return InputPresentation.ResolveText("{TM_07}", "{TM_07_MENU}", "{TM_07_MENU}")
        end
    }
}

return Tutorial
