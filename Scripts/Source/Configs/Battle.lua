local Battle = {}

---@class Source.Configs.Battle.Rule
---@field crit fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@class Source.Configs.Battle.Skill
---@field apply fun(value: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

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

---@type table<string, Source.Configs.Battle.Skill | nil>
Battle.defenseSkills = {
    HuiMu = {
        apply = function (damage, attacker, defender)
            damage = math.max(0, damage)
            if damage == 0 then
                return 0
            end
            local atk, _, _, def = Battle.GetRealAttrInfo(attacker, defender)
            local basic = 0.2
            local k = 15
            local X = k * (defender.MAGIC / def) * (math.max(0, atk - def) / atk)
            local extra = 0.2 * X / (1 + X)
            return math.round(damage * (1 - (basic + extra)))
        end
    }
}

Battle.criticalFatigue = 5
Battle.startDelay = 0.3
Battle.attackInterval = 0.12
Battle.attackExtraDelay = 0.10

return Battle
