local Engine = require("Engine")
local GlobalCore = require("GlobalCore")
local MapInput = require("Source.Utils.MapInput")

local Input = Engine.Input
local Transition = GlobalCore.Transition

---@class Source.SceneComponents.MapMessageGesture
local MapMessageGesture = {}

function MapMessageGesture:init(scene)
    self._scene = scene
    self._map = nil
    self._position = nil
end

function MapMessageGesture:cancel()
    if self._map ~= nil then Input.cancelTouchGesture() end
    self._map = nil
    self._position = nil
end

function MapMessageGesture:update()
    if not LUDORK_MOBILE then return end
    if self._scene:isInputBlocked() or self._scene:_isMapClickMoveBlocked() or self._scene._mapTransferInProgress
        or Transition.isTransitionPending() or Transition.isInTransition() then
        self:cancel()
        return
    end
    local gameMap = self._scene:getGameMap()
    if not Input.isTouchActive() then
        self._map = nil
        self._position = nil
    elseif self._map ~= nil
        and (self._map ~= gameMap or self._position ~= self._scene.player:getMapPosition() or Input.isTouchDragged()) then
        self:cancel()
    end
    if Input.isTouchBegan(false) then
        local position = Input.getTouchBeganPosition()
        if position ~= nil then
            local mapPosition = MapInput.GetMapPosition(gameMap, position.x, position.y)
            if mapPosition ~= nil and mapPosition == self._scene.player:getMapPosition() then
                self._map = gameMap
                self._position = mapPosition
            end
        end
    end
    if self._map == nil or not Input.isTouchLongPressed(false) then return end
    local position = Input.getTouchLongPressPosition()
    if position == nil or MapInput.GetMapPosition(gameMap, position.x, position.y) ~= self._position then
        self:cancel()
        return
    end
    Input.isTouchLongPressed(true)
    self:cancel()
    self._scene:showMessages()
end

return class(MapMessageGesture)
