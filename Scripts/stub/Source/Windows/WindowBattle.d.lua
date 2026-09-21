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
---@field mucus            integer
---@field thunder          integer
---@field sureKill         boolean
---@field berserk          boolean
---@field hard             boolean
---@field magic            boolean
---@field compete          boolean
---@field first            boolean
---@field deathCurse       boolean
---@field armorBreak       boolean
---@field burn             boolean
---@field auraField        boolean
---@field frost            boolean
---@field burnHits         integer
---@field frozenTurns      integer
---@field hitCount         integer
---@field ATK              integer
---@field DEF              integer
---@field MAGIC            integer
---@field breath           integer
---@field breathLimit      integer
---@field fatigue          integer
---@field animationKey     string
---@field CritAnimationKey string
---@field attackSkillAnimationKey string
---@field defenseSkillAnimationKey string
---@field attackSkillFatiguePlus integer
---@field defenseSkillFatiguePlus integer
---@field attackSkillBreathMinus integer
---@field defenseSkillBreathMinus integer
---@field isPlayer         boolean
---@field crit             fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number
---@field attackSkill      Source.Configs.Battle.Skill | nil
---@field defenseSkill     Source.Configs.Battle.Skill | nil

---@alias Source.Windows.WindowBattle.Result "win"|"lose"|"retreat"
---@alias Source.Windows.WindowBattle.Finished fun(result: Source.Windows.WindowBattle.Result, hp: integer, breath: integer, addedStates: table<string, integer>)

---@class Source.Windows.WindowBattle.Controller: Source.UIBase.UiController
---@field host              Source.Windows.WindowBattle
---@field ui                Source.UI.WindowBattle
---@field _scene            Source.Gameplay.GameplayScene
---@field _generation       integer
---@field _running          boolean
---@field _criticalSelected boolean
---@field _attackSkillSelected boolean
---@field _defenseSkillSelected boolean
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
---@param attacker     Source.Windows.WindowBattle.BattlerState
---@param defender     Source.Windows.WindowBattle.BattlerState
---@param critical     boolean
---@param attackSkill? boolean
---@return integer damage
---@return integer base
function Controller:calculateDamage(attacker, defender, critical, attackSkill) end
---@param state Source.Windows.WindowBattle.BattlerState
---@param units? integer
---@return integer
function Controller:skillBreathCost(state, units) end
---@param state Source.Windows.WindowBattle.BattlerState
---@param cost  integer
---@return boolean
function Controller:canAffordSkill(state, cost) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@return boolean
function Controller:canCritical(attacker, defender) end
---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@return boolean
function Controller:canAttackSkill(attacker, defender) end
---@param state Source.Windows.WindowBattle.BattlerState
---@return boolean
function Controller:canDefenseSkill(state) end
---@param playerTurn     boolean
---@param remainingHits  integer | nil
function Controller:beginTurn(playerTurn, remainingHits) end
---@param attacker       Source.Windows.WindowBattle.BattlerState
---@param defender       Source.Windows.WindowBattle.BattlerState
---@param remainingHits  integer
function Controller:normalAttack(attacker, defender, remainingHits) end
---@param attacker       Source.Windows.WindowBattle.BattlerState
---@param defender       Source.Windows.WindowBattle.BattlerState
---@param remainingHits  integer
function Controller:criticalAttack(attacker, defender, remainingHits) end
---@param attacker       Source.Windows.WindowBattle.BattlerState
---@param defender       Source.Windows.WindowBattle.BattlerState
---@param critical       boolean
---@param remainingHits  integer
---@param attackSkill?   boolean
function Controller:performAttack(attacker, defender, critical, remainingHits, attackSkill) end
---@param attacker      Source.Windows.WindowBattle.BattlerState
---@param defender      Source.Windows.WindowBattle.BattlerState
---@param damage        integer
---@param critical      boolean
---@param attackSkill?  boolean
---@param defenseSkill? boolean
function Controller:receiveAttack(attacker, defender, damage, critical, attackSkill, defenseSkill) end
---@param state  Source.Windows.WindowBattle.BattlerState
---@param amount integer
function Controller:addBreath(state, amount) end
---@param side  "Player" | "Enemy"
---@param state Source.Windows.WindowBattle.BattlerState
function Controller:refreshBreath(side, state) end
---@param kind "Lit" | "Dim"
---@return Engine.WrapBox
function Controller:getBreathBox(kind) end
---@param kind  "Lit" | "Dim"
---@param index integer
---@return Engine.Canvas
function Controller:getBreathCanvas(kind, index) end
---@param kwargs Engine.UiInputEventArguments
function Controller:onKeyDown(kwargs) end
---@param result Source.Windows.WindowBattle.Result
function Controller:finish(result) end

function Controller:requestCritical() end
function Controller:requestAttackSkill() end
function Controller:requestDefenseSkill() end
function Controller:requestRetreat() end
function Controller:cancel() end
---@param name     string
---@param selected boolean
---@param enabled  boolean | nil
function Controller:refreshActionButton(name, selected, enabled) end
function Controller:refreshCritical() end
function Controller:playBreathAnimation() end
function Controller:refreshLocale() end
function Controller:bind() end
function Controller:refresh() end
function Controller:dispose() end
