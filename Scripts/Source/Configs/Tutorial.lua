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
        text = (function ()
            if LUDORK_MOBILE then
                return ""
            end
            return "{TM_04}"
        end)()
    },
    TM_05 = {
        rect = sf.IntRect.new(160, 80, 320, 64),
        text = (function ()
            if LUDORK_MOBILE then
                return "{TM_05_M}"
            end
            return "{TM_05}"
        end)()
    },
    TM_06 = {
        rect = sf.IntRect.new(16, 16, 32, 32),
        text = (function ()
            if LUDORK_DESKTOP then
                return "{TM_06_D}"
            end
            return "{TM_06}"
        end)()
    },
    TM_07 = {
        rect = sf.IntRect.new(16, 16, 32, 32),
        text = (function ()
            if LUDORK_MOBILE then
                return ""
            end
            return "{TM_07}"
        end)()
    }
}

return Tutorial
