local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local Logging = require("Global.Utils.Logging")
local Config = require("Source.Configs.Messages")
local Data = require("Source.Data")
local Locale = require("Source.Locale.Core")
local MapPath = require("Source.Utils.MapPath")

local Server = GlobalCore.Server
local NetworkState = GlobalCore.ServerNetworkState
local ErrorCode = GlobalCore.ServerErrorCode
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
    self.networkReady = false
    self.checking = false
    self.submitting = false
end

function MapMessages:cancel()
    self._generation = self._generation + 1
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
        if self._operations[index]:getStatus() ~= "pending" then
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
    self:track(Server.CheckConnectionAsync(function (result)
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
                assert(self._map):prependBillboardLayer(Config.LAYER)
                self:loadMessages()
            elseif notify then
                self:tip("MESSAGES_NETWORK_BAD")
            end
            if onCompleted ~= nil then onCompleted(self.networkReady) end
        end))
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
    self:track(Server.ListAccountsAsync(5, self._category, function (result)
            assert(result)
            if not self:isCurrent(generation) or not self.networkReady then return end
            if not result.ok then
                self._loadStarted = false
                Logging.warning("Failed to list map messages for %s: %s", self._mapFile, result.message)
                return
            end
            Logging.info("Received %d map message accounts for %s", #result.accounts, self._mapFile)
            for index, account in ipairs(result.accounts) do
                if index > 5 then break end
                self:track(Server.ReadFieldAsync(account, self._category, "message", function (read)
                        assert(read)
                        if not self:isCurrent(generation) or not self.networkReady or not read.ok or not read.found then return end
                        local record = self:validate(read.value)
                        if record ~= nil then self:spawn(account, record) end
                    end))
            end
        end))
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
    if #self._category > 64 then
        self:tip("MESSAGES_FAILED")
        onAccepted()
        return
    end
    self.submitting = true
    local generation = self._generation
    local position = self._scene.player:getMapPosition()
    local record = {
        templateKey = templateKey,
        wordKey = wordKey,
        mapFile = self._mapFile,
        x = position.x,
        y = position.y
    }
    self:checkConnection(function (ready)
        if not ready then
            self.submitting = false
            onAccepted()
            return
        end
        local account = "mt3msg_"
        for _ = 1, 8 do
            account = account .. string.format("%04x", math.random(0, 65535))
        end
        self:spawn(account, record)
        onAccepted()
        self:track(Server.AcquireWriteTokenAsync(account, function (result)
                assert(result)
                if not self:isCurrent(generation) then return end
                if not result.ok then
                    self:finishSubmit(account, result)
                    return
                end
                self:track(
                    Server.WriteFieldAsync(assert(result.token), self._category, "message", record, function (written)
                        assert(written)
                        if self:isCurrent(generation) then self:finishSubmit(account, written) end
                    end)
                )
            end))
    end, true)
end

function MapMessages:finishSubmit(account, result)
    self.submitting = false
    if result.ok then
        self:tip("MESSAGES_SENT")
        return
    end
    local entry = self._entries[account]
    if entry ~= nil then
        entry.actor:destroy()
        self._entries[account] = nil
    end
    if result.code == ErrorCode.RateLimited then
        self:tip("MESSAGES_RATE_LIMITED", { seconds = assert(result.retryAfterSeconds) })
    else
        self:tip("MESSAGES_FAILED")
    end
end

return class(MapMessages)
