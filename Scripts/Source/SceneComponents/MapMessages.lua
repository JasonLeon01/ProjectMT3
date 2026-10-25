local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Logging = require("Global.Utils.Logging")
local Config = require("Source.Configs.Messages")
local Data = require("Global.Data")
local Locale = require("Source.Locale.Core")
local MapPath = require("Global.Utils.MapPath")
local Net = require("Global.Net")
local MessagePublisher = require("Source.Net.MessagePublisher")
require("Global.Net.Storage")

local Server = GlobalCore.Server
local NetworkState = GlobalCore.ServerNetworkState
local MAX_CONNECTION_ROUND_TRIP_MS = 500
local CONNECTION_RETRY_DELAYS = { 1.0, 3.0 }
---@type fun(value: string): string
local LOC = Locale.ApplyStringLocaleFormat

---@class Source.SceneComponents.MapMessages
local MapMessages = {}

function MapMessages:init(scene)
    self._scene = scene
    self._generation = 0
    self._checkRevision = 0
    self._loadStarted = false
    self._operations = {}
    self._entries = {}
    self._map = nil
    self._mapFile = ""
    self._category = ""
    self._publisherToken = nil
    self.networkReady = false
    self.checking = false
    self.submitting = false
end

function MapMessages:cancel()
    self._generation = self._generation + 1
    if self._publisherToken ~= nil then
        MessagePublisher.Detach(self._publisherToken)
        self._publisherToken = nil
    end
    self._loadStarted = false
    for _, operation in ipairs(self._operations) do
        operation:cancel()
    end
    for _, entry in pairs(self._entries) do
        entry.actor:destroy()
    end
    self._operations = {}
    self._entries = {}
    self._map = nil
    self.networkReady = false
    self.checking = false
    self.submitting = false
end

function MapMessages:track(operation)
    for index = #self._operations, 1, -1 do
        if self._operations[index]:getStatus() ~= Engine.AsyncOperation.PENDING then
            table.remove(self._operations, index)
        end
    end
    self._operations[#self._operations + 1] = operation
end

function MapMessages:isCurrent(generation)
    return self._map ~= nil and self._generation == generation and self._scene:getGameMap() == self._map
end

function MapMessages:tip(key, args)
    self._scene:addCommonTip(string.pformat(LOC(key), args or {}))
end

function MapMessages:checkConnection(onCompleted, notify)
    self._checkRevision = self._checkRevision + 1
    self.checking = true
    local generation = self._generation
    local revision = self._checkRevision
    local operation = Server.CheckConnectionAsync(function (result)
        assert(result)
        ---@diagnostic disable-next-line: preferred-local-alias, reject results from superseded connection probes
        if not self:isCurrent(generation) or self._checkRevision ~= revision then return end
        -- The engine's Normal cutoff is 300 ms; messages allow up to 500 ms.
        self.networkReady = result.ok and result.state ~= NetworkState.Disconnected
            and result.roundTripMs ~= nil and result.roundTripMs <= MAX_CONNECTION_ROUND_TRIP_MS
        self.checking = false
        Logging.info(
            "Map messages connection for %s: ready=%s, roundTripMs=%s, code=%s", self._mapFile,
            tostring(self.networkReady), tostring(result.roundTripMs), tostring(result.code)
        )
        if self.networkReady then
            MessagePublisher.Reconcile()
            assert(self._map):prependBillboardLayer(Config.LAYER)
            self:loadMessages()
        elseif notify then
            self:tip("MESSAGES_NETWORK_BAD")
        end
        if onCompleted ~= nil then onCompleted(self.networkReady) end
    end)
    if operation == nil then
        self.checking = false
        self.networkReady = false
        if notify then
            self:tip("MESSAGES_NETWORK_BAD")
        end
        if onCompleted ~= nil then
            onCompleted(false)
        end
        return
    end
    self:track(operation)
end

function MapMessages:checkBackgroundConnection(attempt)
    local generation = self._generation
    self:checkConnection(function (ready)
        local delay = CONNECTION_RETRY_DELAYS[attempt]
        if ready or delay == nil then return end
        local revision = self._checkRevision
        -- A transient slow first probe must not disable messages for the entire map visit.
        -- User-initiated checks supersede these retries; publishing is never retried.
        self:track(self._scene:addTimer(delay, function ()
                ---@diagnostic disable-next-line: preferred-local-alias, reject retries superseded by a newer probe
                if not self:isCurrent(generation) or self._checkRevision ~= revision then return end
                self:checkBackgroundConnection(attempt + 1)
            end, false
            ))
    end, false)
end

function MapMessages:beginMap(gameMap, mapFile)
    self:cancel()
    self._map = gameMap
    self._mapFile = MapPath.Normalise(mapFile)
    self._category = "messages:" .. self._mapFile
    local generation = self._generation
    self._publisherToken = MessagePublisher.Attach(function (submitting)
        if self:isCurrent(generation) then
            self.submitting = submitting
        end
    end,
        function (account, result)
            if self:isCurrent(generation) then
                self:finishSubmit(account, result)
            end
        end)
    self.submitting = MessagePublisher.IsPending()
    if #self._category > 64 then return end
    self:checkBackgroundConnection(1)
end

function MapMessages:validate(value)
    if not Class.isInstance(value, "table") then return nil end
    if value.mapFile ~= self._mapFile or not Class.isInstance(value.templateKey, "string")
        or not Class.isInstance(value.wordKey, "string") or not Config.IsAllowed(value.templateKey, value.wordKey) then
        return nil
    end
    if not math.isFinite(value.x) or not math.isFinite(value.y)
        or math.tointeger(value.x) == nil or math.tointeger(value.y) == nil then
        return nil
    end
    local size = assert(self._map):getSize()
    if value.x < 0 or value.y < 0 or value.x >= size.x or value.y >= size.y then return nil end
    return {
        templateKey = value.templateKey,
        wordKey = value.wordKey,
        mapFile = self._mapFile,
        x = assert(math.tointeger(value.x)),
        y = assert(math.tointeger(value.y))
    }
end

function MapMessages:loadMessages()
    if self._loadStarted then return end
    self._loadStarted = true
    local generation = self._generation
    local operation = Net.GetProxy("Storage", { query = "accounts", sampleCount = 5, categoryFilter = self._category }, function (
        result
    )
        if not self:isCurrent(generation) or not self.networkReady then return end
        if not result.ok then
            self._loadStarted = false
            Logging.warning("Failed to list map messages for %s: %s", self._mapFile, result.message)
            return
        end
        local proxy = assert(result.proxy)
        ---@cast proxy Global.Net.StorageProxy
        local accounts = assert(proxy:GetAccounts())
        Logging.info("Received %d map message accounts for %s", #accounts, self._mapFile)
        for index, account in ipairs(accounts) do
            if index > 5 then break end
            local readOperation = Net.GetProxy(
                "Storage",
                {
                    query = "field",
                    account = account,
                    category = self._category,
                    field = "message"
                },
                function (read)
                    if not self:isCurrent(generation) or not self.networkReady or not read.ok then return end
                    local fieldProxy = assert(read.proxy)
                    ---@cast fieldProxy Global.Net.StorageProxy
                    if not fieldProxy:IsFound() then return end
                    local record = self:validate(fieldProxy:GetFieldValue())
                    if record ~= nil then self:spawn(account, record) end
                end
            )
            if readOperation ~= nil then
                self:track(readOperation)
            else
                self._loadStarted = false
                self.networkReady = false
            end
        end
    end)
    if operation == nil then
        self._loadStarted = false
        self.networkReady = false
        return
    end
    self:track(operation)
end

function MapMessages:spawn(account, record)
    if self._entries[account] ~= nil then return end
    local actor = assert(Data.GenActorFromClassPath(Config.BLUEPRINT, "message_" .. account))
    actor:setMapPosition(sf.Vector2i.new(record.x, record.y))
    assert(actor.billboardComp).items = {
        Engine.BillboardItem.new({
            kind = "text",
            text = Config.Format(record.templateKey, record.wordKey),
            fontSize = 12
        })
    }
    -- These actors belong to the map session, never to persistent actor records.
    assert(self._map):spawnTransientActor(actor, Config.LAYER)
    self._entries[account] = { actor = actor, record = record }
end

function MapMessages:refreshLocale()
    for _, entry in pairs(self._entries) do
        assert(entry.actor.billboardComp).items = {
            Engine.BillboardItem.new({
                kind = "text",
                text = Config.Format(entry.record.templateKey, entry.record.wordKey),
                fontSize = 12
            })
        }
    end
end

function MapMessages:submit(templateKey, wordKey, onAccepted)
    if self.submitting or self._map == nil or not Config.IsAllowed(templateKey, wordKey) then return end
    if MessagePublisher.IsPending() then
        self.submitting = true
        self:tip("MESSAGES_SENDING")
        onAccepted()
        return
    end
    if #self._category > 64 then
        self:tip("MESSAGES_FAILED")
        onAccepted()
        return
    end
    self.submitting = true
    local generation = self._generation
    local position = self._scene:getPlayer():getMapPosition()
    local record = {
        templateKey = templateKey,
        wordKey = wordKey,
        mapFile = self._mapFile,
        x = position.x,
        y = position.y
    }
    self:checkConnection(function (ready)
        if not ready then
            self.submitting = MessagePublisher.IsPending()
            onAccepted()
            return
        end
        local account = "mt3msg_"
        for _ = 1, 8 do
            account = account .. string.format("%04x", math.random(0, 65535))
        end
        self:spawn(account, record)
        if not MessagePublisher.Publish(assert(self._publisherToken), account, self._category, record) then
            if self:isCurrent(generation) then
                local entry = self._entries[account]
                if entry ~= nil then
                    entry.actor:destroy()
                    self._entries[account] = nil
                end
                self.submitting = MessagePublisher.IsPending()
                self:tip("MESSAGES_SENDING")
            end
        end
        onAccepted()
    end, true)
end

function MapMessages:finishSubmit(account, result)
    if result.ok then
        self:tip("MESSAGES_SENT")
        return
    end
    local entry = self._entries[account]
    if entry ~= nil then
        entry.actor:destroy()
        self._entries[account] = nil
    end
    if result.code == "RateLimited" then
        self:tip("MESSAGES_RATE_LIMITED", { seconds = assert(result.retryAfterSeconds) })
    else
        self:tip("MESSAGES_FAILED")
    end
end

return class(MapMessages)
