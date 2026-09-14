local _METADATA = {
    Gem = {
        attrs = {
            "ATTR_key",
            "plus",
            "ATTR_key2",
            "plus2",
            "getSE"
        },
        ATTR_key = {
            type = "string",
            default = ""
        },
        plus = {
            type = "int",
            default = 0
        },
        ATTR_key2 = {
            type = "string",
            default = ""
        },
        plus2 = {
            type = "int",
            default = 0
        },
        getSE = {
            type = "string",
            default = "",
            Meta = {
                PathVars = "/Game/Assets/Sounds",
                ConfigVars = { "Audio", "getSE" }
            }
        },
        Meta = {
            PathVars = {
                { "getSE", "/Game/Assets/Sounds" }
            },
            ConfigVars = {
                { "getSE", "Audio", "getSE" }
            }
        }
    }
}

return _METADATA
