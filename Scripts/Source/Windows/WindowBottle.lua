local GlobalCore = require("GlobalCore")
local PlayerChangeKind = require("Enums.PlayerChangeKind")
local Data = require("Source.Data")
local EventKey = require("Enums.EventKey")
local Item = require("Enums.GeneralData.Item")
local GameSystem = require("Source.System")
local IconTexture = require("Internal.UIBase.IconTexture")
local Ui = require("Internal.UIBase.Ui")
local View = require("Internal.UI.WindowBottle")
local BottleRowController = require("Source.Windows.WindowBottle.BottleRow.Controller")
local WindowSelectable = require("Internal.UIBase.WindowSelectable")

local AudioManager = GlobalCore.AudioManager
local ITEM_IDS = { Item.Bottle150, Item.Bottle400 }

---@class Source.Windows.WindowBottle.Controller
local Controller = {}

Controller.windowOptions = { hidden = true, list = "BottleList" }

function Controller:init(player, onClose)
    self._player = player
    self._onClose = onClose
    self._rows = self:createCollection(self.ui.controls["BottleList"], BottleRowController)
    for _, itemID in ipairs(ITEM_IDS) do
        local row = self._rows:add({
            iconTexture = IconTexture.Load(Data.GetGeneralItemData(itemID).icon),
            count = player:getItemCount(itemID)
        })
        ---@cast row Source.Windows.WindowBottle.BottleRow.Controller
        row.ui.root:addConfirmCallback(self:bindCallback(Controller.useSelectedItem))
    end
    self._rows:layout()
end

function Controller:bind()
    self:subscribe(EventKey.PlayerChanged, self:bindCallback(Controller.onPlayerChanged))
end

function Controller:onPlayerChanged(payload)
    if payload.owner == self._player and payload.kind == PlayerChangeKind.Inventory then
        self:refreshItems()
    end
end

function Controller:setPlayer(player)
    self._player = player
    self:refreshItems()
end

function Controller:refreshItems()
    for index, itemID in ipairs(ITEM_IDS) do
        local row = assert(self._rows.items[index])
        row.model.iconTexture = IconTexture.Load(Data.GetGeneralItemData(itemID).icon)
        row.model.count = self._player:getItemCount(itemID)
        row:prepare()
    end
    self._rows:layout()
end

function Controller:open(mapRect)
    local size = self.host:getSize()
    self.host:setPosition(
        sf.Vector2f.new(mapRect.position.x + mapRect.size.x - size.x, mapRect.position.y + mapRect.size.y - size.y)
    )
    self:refreshItems()
    self.view:reflow()
    self.host:resetSelection()
    self.host:setVisible(true)
    self.host:setActive(true)
    self.host:requestKeyboardFocusAtCursor()
end

function Controller:close()
    if not self.host:getVisible() then
        return
    end
    self.host:setActive(false)
    self.host:setVisible(false)
    self._onClose()
end

function Controller:onReturn()
    AudioManager.playSound(GameSystem.GetCancelSE())
    self:close()
end

function Controller:onTick(deltaTime)
    WindowSelectable.onTick(self.host, deltaTime)
end

function Controller:useSelectedItem()
    if self.host.index == nil then
        return
    end
    local itemID = ITEM_IDS[self.host.index + 1]
    if itemID == nil then
        return
    end
    if self._player:getItemCount(itemID) <= 0 then
        AudioManager.playSound(GameSystem.GetBuzzerSE())
        return
    end
    AudioManager.playSound(GameSystem.GetDecisionSE())
    local result = self._player:activateItem(itemID)
    assert(result.ok, "Item Ability failed: " .. tostring(result.code))
    self:refreshItems()
end

return Ui.DefineWindow(View, Controller, WindowSelectable)
