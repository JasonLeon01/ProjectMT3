local _METADATA = {
    Teleporter = {
        moduleReturn = true,
        attrs = {
            "Offset",
            "stairSE",
            "transitionName",
            "transitionTime"
        },
        bases = {
            { "Source.ConditionalActor", "ConditionalActor" }
        },
        Offset = {
            type = "sf.Vector2i",
            default = { 0, 0 }
        },
        stairSE = {
            type = "string",
            default = "",
            Meta = {
                PathVars = "/Game/Assets/Sounds",
                ConfigVars = { "Audio", "stairSE" }
            }
        },
        transitionName = {
            type = "string",
            default = "",
            Meta = {
                PathVars = "/Game/Assets/Transitions"
            }
        },
        transitionTime = {
            type = "float",
            default = 0.5
        },
        goUpstairs = {
            type = "function",
            parameters = {},
            ["return"] = {},
            ExecSplit = {
                "default",
                default = "nil"
            }
        },
        goDownstairs = {
            type = "function",
            parameters = {},
            ["return"] = {},
            ExecSplit = {
                "default",
                default = "nil"
            }
        },
        goToMap = {
            type = "function",
            parameters = {
                "mapPath",
                "position",
                "record",
                mapPath = "string",
                position = "sf.Vector2i",
                record = "bool"
            },
            default = {
                [3] = true
            },
            ["return"] = {},
            ExecSplit = {
                "default",
                default = "nil"
            },
            Meta = {
                Transfer = {
                    { "position", "mapPath" }
                }
            }
        },
        Meta = {
            PathVars = {
                { "stairSE", "/Game/Assets/Sounds" },
                { "transitionName", "/Game/Assets/Transitions" }
            },
            ConfigVars = {
                { "stairSE", "Audio", "stairSE" }
            }
        }
    }
}

return _METADATA
