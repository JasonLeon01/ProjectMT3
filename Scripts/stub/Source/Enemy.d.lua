---@meta Source.Enemy

---@class Source.Enemy: Source.ConditionalActor, Source.Battler.Battler
---@field ID                         string
---@field DefeatShatterEffectEnabled boolean
---@field attributes                 Source.Configs.GeneralDataTypes.EnemyAttributeSet
---@field childActorComp             Source.Components.ChildActorComponent
---@field afterBattleVarChanges      table<string, { [1]: string, [2]: any }>
---@field private _defeatFinalising  boolean
---@field private _defeatFinalised   boolean
---@field new                        fun(texture?: sf.Texture, rect?: sf.IntRect, tag?: string): Source.Enemy
local Enemy = {}

---@param texture? sf.Texture
---@param rect?    sf.IntRect
---@param tag?     string
function Enemy:init(texture, rect, tag) end

---@param other Engine.Actor[]
function Enemy:onCollision(other) end

function Enemy:onDefeat() end

---@param player Source.Player.Player
---@param scene Source.Gameplay.GameplayScene
---@param hp integer
---@param breath integer
---@param addedStates table<string, integer>
function Enemy:completeBattle(player, scene, hp, breath, addedStates) end

return Enemy
