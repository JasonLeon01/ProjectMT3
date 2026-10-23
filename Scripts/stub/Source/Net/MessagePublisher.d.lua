---@meta Source.Net.MessagePublisher

---@class Source.Net.MessagePublisher.Result
---@field ok                boolean
---@field code              string
---@field message           string
---@field retryAfterSeconds integer | nil

---@class Source.Net.MessagePublisher.Listener
---@field token       integer
---@field onChanged   fun(submitting: boolean)
---@field onCompleted fun(account: string, result: Source.Net.MessagePublisher.Result)

---@class Source.Net.MessagePublisher.Pending
---@field owner         integer
---@field account       string
---@field category      string
---@field record        Source.SceneComponents.MessageRecord
---@field sent          boolean
---@field reconciling   boolean
---@field readRequested boolean
---@field readBlocked   boolean
---@field nextReadAt    number
---@field lastReadError string | nil
---@field eventToken    integer | nil
---@field errorToken    integer | nil
---@field operation     Engine.AsyncOperation | nil
---@field readOperation Engine.AsyncOperation | nil

local MessagePublisher = {}

--- An unresolved publish remains pending across map and scene changes.
---@return boolean
function MessagePublisher.IsPending() end

--- Retry an unresolved read after a successful connection check, respecting server retry intervals.
function MessagePublisher.Reconcile() end

--- Attach the active map without retaining callbacks from a previous map.
--- State changes reach the active map; completion reaches only the publishing map.
---@param onChanged   fun(submitting: boolean)
---@param onCompleted fun(account: string, result: Source.Net.MessagePublisher.Result)
---@return integer
function MessagePublisher.Attach(onChanged, onCompleted) end

--- Release a map's callbacks without cancelling or replaying its accepted publish.
---@param token integer
function MessagePublisher.Detach(token) end

--- Reserve the single publish slot and send once on the next operation update.
--- Indeterminate writes are confirmed by reading their exact field, never by replaying the RPC.
---@param owner    integer
---@param account  string
---@param category string
---@param record   Source.SceneComponents.MessageRecord
---@return boolean
function MessagePublisher.Publish(owner, account, category, record) end

return MessagePublisher
