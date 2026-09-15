---@meta

---@class Source.Windows.WindowBattle.BattlerState
---@field HP               integer
---@field MAXHP            integer
---@field poisoned         integer
---@field weak             integer
---@field addedStates      table<string, integer>
---@field poisoning        integer
---@field weaken           integer
---@field vampire          number
---@field ATK              integer
---@field DEF              integer
---@field breath           integer
---@field breathLimit      integer
---@field fatigue          integer
---@field animationKey     string
---@field CritAnimationKey string
---@field isPlayer         boolean
---@field crit             fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@alias Source.Windows.WindowBattle.Result "win"|"lose"|"retreat"
---@alias Source.Windows.WindowBattle.Finished fun(result: Source.Windows.WindowBattle.Result, hp: integer, breath: integer, addedStates: table<string, integer>)

---@class Source.Windows.WindowBattle.Controller: Source.UIBase.UiController
---@field host              Source.Windows.WindowBattle
---@field ui                Source.UI.WindowBattle
---@field _scene            Source.Gameplay.GameplayScene
---@field _generation       integer
---@field _running          boolean
---@field _criticalSelected boolean
---@field _retreatRequested boolean
---@field _watchStops       (fun())[]
---@field _particles        Engine.ParticleSystem
---@field _playerActor      Source.Player.Player | nil
---@field _enemyActor       Source.Enemy | nil
---@field _player           Source.Windows.WindowBattle.BattlerState | nil
---@field _enemy            Source.Windows.WindowBattle.BattlerState | nil
---@field _onFinished       Source.Windows.WindowBattle.Finished | nil
local Controller = {}

---@param scene Source.Gameplay.GameplayScene
function Controller:init(scene) end
---@param player     Source.Player.Player
---@param enemy      Source.Enemy
---@param onFinished Source.Windows.WindowBattle.Finished
function Controller:open(player, enemy, onFinished) end
---@param name string
---@param text string
function Controller:setBattleText(name, text) end
---@param name  string
---@param actor Engine.Actor
function Controller:setPortrait(name, actor) end
---@param state Source.Windows.WindowBattle.BattlerState
---@param side  "Player" | "Enemy"
function Controller:observeState(state, side) end
---@param side  "Player" | "Enemy"
---@param delta integer
function Controller:onHPChanged(side, delta) end
---@param state Source.Windows.WindowBattle.BattlerState
---@param delta integer
---@return integer
function Controller:changeHP(state, delta) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
function Controller:applyAttackStates(attacker, defender) end
---@param delay  number
---@param action fun()
function Controller:schedule(delay, action) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@param critical boolean
---@return integer damage
---@return integer base
function Controller:calculateDamage(attacker, defender, critical) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@return boolean
function Controller:canCritical(attacker, defender) end
---@param playerTurn boolean
function Controller:beginTurn(playerTurn) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
function Controller:normalAttack(attacker, defender) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
function Controller:criticalAttack(attacker, defender) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@param critical boolean
function Controller:performAttack(attacker, defender, critical) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@param damage   integer
---@param critical boolean
function Controller:receiveAttack(attacker, defender, damage, critical) end
---@param state  Source.Windows.WindowBattle.BattlerState
---@param amount integer
function Controller:addBreath(state, amount) end
---@param side  "Player" | "Enemy"
---@param state Source.Windows.WindowBattle.BattlerState
function Controller:refreshBreath(side, state) end
---@param kind  "Lit" | "Dim"
---@param index integer
---@return Engine.Canvas
function Controller:getBreathCanvas(kind, index) end
---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end
---@param result Source.Windows.WindowBattle.Result
function Controller:finish(result) end

function Controller:requestCritical() end
function Controller:requestRetreat() end
function Controller:cancel() end
function Controller:refreshCritical() end
function Controller:playBreathAnimation() end
function Controller:refreshLocale() end
function Controller:bind() end
function Controller:refresh() end
function Controller:dispose() end
