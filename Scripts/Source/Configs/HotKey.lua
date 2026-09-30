local SceneMap = require("Source.Scenes.SceneMap")

return {
    [sf.Keyboard.Key.U] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.switchPlayer,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.N] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.showMagicDoor,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.H] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.showHotkeys,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.Escape] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.openMenu,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.D] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = nil,
        FunctionWhenReleased = SceneMap.showEnemyBook
    },
    [sf.Keyboard.Key.F] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = nil,
        FunctionWhenReleased = SceneMap.showFloorTeleporter
    },
    [sf.Keyboard.Key.S] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.openSaveUI,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.L] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.openLoadUI,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.I] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.openItemUI,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.E] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.openEquipUI,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.LBracket] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.quickSave,
        FunctionWhenReleased = nil
    },
    [sf.Keyboard.Key.RBracket] = {
        Scene = SceneMap,
        Filter = { "casual" },
        FunctionWhenPressed = SceneMap.quickLoad,
        FunctionWhenReleased = nil
    }
}
