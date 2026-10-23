---@meta Source.SceneComponents.MapMessages

---@class Source.SceneComponents.MessageRecord
---@field templateKey string
---@field wordKey     string
---@field mapFile     string
---@field x           integer
---@field y           integer

---@class Source.SceneComponents.MapMessages
---@field new             fun(scene: Source.Scenes.SceneMap.SceneMap): Source.SceneComponents.MapMessages
---@field _scene          Source.Scenes.SceneMap.SceneMap
---@field _generation     integer
---@field _checkRevision  integer
---@field _loadStarted    boolean
---@field _operations     Engine.AsyncOperation[]
---@field _entries        table<string, { actor: Engine.Actor, record: Source.SceneComponents.MessageRecord }>
---@field _map            GameMap | nil
---@field _mapFile        string
---@field _category       string
---@field _publisherToken integer | nil
---@field networkReady    boolean
---@field checking        boolean
---@field submitting      boolean
local MapMessages = {}

---@param scene Source.Scenes.SceneMap.SceneMap
function MapMessages:init(scene) end
function MapMessages:cancel() end
---@param operation Engine.AsyncOperation
function MapMessages:track(operation) end
---@param generation integer
---@return boolean
function MapMessages:isCurrent(generation) end
---@param key   string
---@param args? table<string, any>
function MapMessages:tip(key, args) end
---@param onCompleted fun(ready: boolean) | nil
---@param notify      boolean
function MapMessages:checkConnection(onCompleted, notify) end
---@param attempt integer
function MapMessages:checkBackgroundConnection(attempt) end
---@param gameMap GameMap
---@param mapFile string
function MapMessages:beginMap(gameMap, mapFile) end
---@param value any
---@return Source.SceneComponents.MessageRecord | nil
function MapMessages:validate(value) end
function MapMessages:loadMessages() end
---@param account string
---@param record  Source.SceneComponents.MessageRecord
function MapMessages:spawn(account, record) end
function MapMessages:refreshLocale() end
---@param templateKey string
---@param wordKey     string
---@param onAccepted  fun()
function MapMessages:submit(templateKey, wordKey, onAccepted) end
---@param account string
---@param result  Source.Net.MessagePublisher.Result
function MapMessages:finishSubmit(account, result) end
return MapMessages
