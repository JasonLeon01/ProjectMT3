local Engine = require("Engine")
local Net = require("Global.Net")
local Storage = require("Global.Net.Storage")
local Logging = require("Global.Utils.Logging")

local MessagePublisher = {}
---@type Source.Net.MessagePublisher.Pending | nil
local pending = nil
---@type Source.Net.MessagePublisher.Listener | nil
local listener = nil
local nextListener = 0

local function notifyPending()
    if listener ~= nil then
        listener.onChanged(pending ~= nil)
    end
end

---@param attempt Source.Net.MessagePublisher.Pending
---@param result  Source.Net.MessagePublisher.Result
local function finish(attempt, result)
    if pending ~= attempt then
        return
    end
    pending = nil
    if attempt.operation ~= nil then
        attempt.operation:cancel()
    end
    if attempt.readOperation ~= nil then
        attempt.readOperation:cancel()
    end
    Storage.Unsubscribe(assert(attempt.eventToken))
    Storage.Unsubscribe(assert(attempt.errorToken))
    notifyPending()
    if listener ~= nil and listener.token == attempt.owner then
        listener.onCompleted(attempt.account, result)
    end
end

---@param value    Global.Net.Value
---@param expected Source.SceneComponents.MessageRecord
---@return boolean
local function matchesRecord(value, expected)
    if not Class.isInstance(value, "table") then
        return false
    end
    ---@cast value table<Global.Net.Key, Global.Net.Value>
    return value.mapFile == expected.mapFile and value.templateKey == expected.templateKey
        and value.wordKey == expected.wordKey and value.x == expected.x
        and value.y == expected.y
end

---@param attempt Source.Net.MessagePublisher.Pending
local function readOutcome(attempt)
    attempt.readRequested = false
    attempt.readOperation = Net.GetProxy(
        "Storage",
        {
            query = "field",
            account = attempt.account,
            category = attempt.category,
            field = "message"
        },
        function (result)
            if pending ~= attempt then
                return
            end
            attempt.readOperation = nil
            if not result.ok then
                attempt.readBlocked = result.code == "AuthenticationFailed" or result.code == "Disabled"
                attempt.nextReadAt = perfCounter() + (result.retryAfterSeconds or 0)
                if attempt.lastReadError ~= result.code then
                    attempt.lastReadError = result.code
                    Logging.warning(
                        "Could not confirm map message %s (%s): %s", attempt.account, result.code, result.message
                    )
                end
                return
            end
            local proxy = assert(result.proxy)
            ---@cast proxy Global.Net.StorageProxy
            if proxy:IsFound() and matchesRecord(proxy:GetFieldValue(), attempt.record) then
                finish(attempt, { ok = true, code = "None", message = "" })
            else
                finish(attempt, {
                    ok = false,
                    code = "WriteUnconfirmed",
                    message = "The submitted map message was not found in storage."
                })
            end
        end
    )
end

---@param attempt Source.Net.MessagePublisher.Pending
---@param failure Global.Net.Error
local function recover(attempt, failure)
    if attempt.reconciling then
        return
    end
    attempt.reconciling = true
    attempt.readRequested = true
    attempt.nextReadAt = perfCounter()
    Logging.warning("Map message %s needs confirmation (%s): %s", attempt.account, failure.code, failure.message)
end

function MessagePublisher.IsPending()
    return pending ~= nil
end

function MessagePublisher.Reconcile()
    if pending ~= nil and pending.reconciling and not pending.readBlocked and pending.readOperation == nil
        and perfCounter() >= pending.nextReadAt then
        pending.readRequested = true
    end
end

function MessagePublisher.Attach(onChanged, onCompleted)
    nextListener = nextListener + 1
    listener = { token = nextListener, onChanged = onChanged, onCompleted = onCompleted }
    return nextListener
end

function MessagePublisher.Detach(token)
    if listener ~= nil and listener.token == token then
        listener = nil
    end
end

function MessagePublisher.Publish(owner, account, category, record)
    if pending ~= nil or listener == nil or listener.token ~= owner then
        return false
    end
    local attempt = {
        owner = owner,
        account = account,
        category = category,
        record = copy(record),
        sent = false,
        reconciling = false,
        readRequested = false,
        readBlocked = false,
        nextReadAt = 0
    }
    ---@cast attempt Source.Net.MessagePublisher.Pending
    pending = attempt
    attempt.eventToken = Storage.Subscribe("FieldWritten", function (writtenAccount, writtenCategory, field)
        if pending == attempt and attempt.sent
            and writtenAccount == attempt.account and writtenCategory == attempt.category
            and field == "message" then
            finish(attempt, { ok = true, code = "None", message = "" })
        end
    end)
    attempt.errorToken = Storage.SubscribeError(function (failure)
        if pending ~= attempt or not attempt.sent then
            return
        end
        if failure.functionName == "WriteField" then
            if failure.code == "RateLimited" or failure.code == "InvalidArgument"
                or failure.code == "AuthenticationFailed" or failure.code == "Disabled" then
                finish(attempt, {
                    ok = false,
                    code = failure.code,
                    message = failure.message,
                    retryAfterSeconds = failure.retryAfterSeconds
                })
            else
                recover(attempt, failure)
            end
        elseif failure.functionName == nil then
            recover(attempt, failure)
        end
    end)
    -- A new RPC cannot start inside the previous request's terminal callback.
    attempt.operation = Engine.AsyncOperation.new(function ()
        if pending ~= attempt then
            return
        end
        if not attempt.sent then
            attempt.sent = true
            Storage.CallRpcFunction("WriteField", attempt.account, attempt.category, "message", attempt.record)
        elseif attempt.readRequested and not attempt.readBlocked
            and attempt.readOperation == nil and perfCounter() >= attempt.nextReadAt then
            readOutcome(attempt)
        end
    end)
    notifyPending()
    return true
end

return MessagePublisher
