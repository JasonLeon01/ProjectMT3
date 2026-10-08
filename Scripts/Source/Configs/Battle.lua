local Battle = {}

---@class Source.Configs.Battle.Rule
---@field crit fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@class Source.Configs.Battle.Skill
---@field apply fun(value: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number
---@field conversion? fun(state: Source.Windows.WindowBattle.BattlerState): integer, integer
---@field immuneAttackEffects? boolean
---@field reflectCriticalEffects? boolean

Battle.huiRenMultiplierCeil = 2

---@param attacker Source.Windows.WindowBattle.BattlerState
---@param defender Source.Windows.WindowBattle.BattlerState
---@return integer
---@return integer
---@return integer
---@return integer
function Battle.GetRealAttrInfo(attacker, defender)
    local aa = attacker.ATK
    local ad = attacker.DEF
    local da = defender.ATK
    local dd = defender.DEF
    if attacker.compete then
        aa = math.max(aa, da)
    end
    if defender.hard then
        dd = math.max(dd, aa - 1)
    end
    if attacker.armorBreak then
        dd = math.floor(dd / 2)
    end
    if attacker.magic then
        dd = 0
    end
    return aa, ad, da, dd
end

---@type table<string, Source.Configs.Battle.Rule | nil>
Battle.players = {
    Bravor = {
        crit = function (damage, _attacker, _defender)
            return damage * 2
        end
    },
    Princess = {
        crit = function (damage, attacker, defender)
            damage = math.max(0, damage)
            local atk, _, _, def = Battle.GetRealAttrInfo(attacker, defender)
            local k = 12
            local basic = 1.75
            local X = k * (attacker.MAGIC / atk) * (def / atk)
            local extra = 1.25 * X / (1 + X)
            return math.floor(damage * (basic + extra))
        end
    }
}

Battle.enemy = {
    crit = function (damage, attacker, _defender)
        if attacker.berserk then
            return damage * 5
        end
        if attacker.sureKill then
            return damage * 3
        end
        return damage * 2
    end
}

---@type table<string, Source.Configs.Battle.Skill | nil>
Battle.attackSkills = {
    YuRen = {
        apply = function (damage)
            return math.floor(damage * 1.5)
        end,
        conversion = function (state)
            local tier = math.min(math.floor(math.max(state.MAGIC, 0) / 10), 7)
            local defense = math.max(state.DEF, 0)
            -- Combine 8% * (1.10 + 0.25 * tier / 7) and 8% - 3% * tier / 7
            -- before dividing, so exact integer boundaries do not round up by one.
            return math.floor(defense * (154 + 5 * tier) / 1750), math.ceil(defense * (56 - 3 * tier) / 700)
        end
    },
    HuiRen = {
        apply = function (damage, attacker, defender)
            damage = math.max(0, damage)
            local atk, _, _, def = Battle.GetRealAttrInfo(attacker, defender)
            local k = 13
            local basic = 1.85
            local X = k * (attacker.MAGIC / atk) * (math.max(0, atk - def) / atk)
            local extra = 2.15 * X / (1 + X)
            return math.floor(damage * (basic + extra))
        end
    }
}

---@param basic number
---@param maximumExtra number
---@return fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number
local function magicDefense(basic, maximumExtra)
    return function (damage, attacker, defender)
        damage = math.max(0, damage)
        if damage == 0 then
            return 0
        end
        local atk, _, _, def = Battle.GetRealAttrInfo(attacker, defender)
        local k = 15
        ---@type number
        local ratio = 0
        if defender.MAGIC > 0 and atk > def then
            if def <= 0 then
                ratio = 1
            else
                local X = k * (defender.MAGIC / def) * (math.max(0, atk - def) / atk)
                ratio = X / (1 + X)
            end
        end
        local extra = maximumExtra * ratio
        return math.round(damage * (1 - (basic + extra)))
    end
end

---@type table<string, Source.Configs.Battle.Skill | nil>
Battle.defenseSkills = {
    HuiMu = {
        apply = magicDefense(0.2, 0.2)
    },
    JingMu = {
        apply = magicDefense(0.15, 0.15),
        immuneAttackEffects = true,
        reflectCriticalEffects = true
    }
}

Battle.skillFatigueLimit = 70
Battle.criticalFatigue = 5
Battle.startDelay = 0.3
Battle.attackInterval = 0.12
Battle.attackExtraDelay = 0.10

return Battle
