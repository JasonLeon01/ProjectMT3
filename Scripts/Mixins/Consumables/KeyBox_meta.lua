local _METADATA = {
    KeyBox = {
        attrs = {
            "plus",
            "getSE",
        },
        plus = {
            type = "int",
            default = 1,
        },
        getSE = {
            type = "string",
            default = "",
            Meta = {
                PathVars = "/Game/Assets/Sounds",
                ConfigVars = { "Audio", "getSE" },
            },
        },
        Meta = {
            PathVars = {
                { "getSE", "/Game/Assets/Sounds" },
            },
            ConfigVars = {
                { "getSE", "Audio", "getSE" },
            },
        },
    },
}

return _METADATA
