local Battle = {}

---@class Source.Configs.Battle.Rule
---@field crit fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@class Source.Configs.Battle.Skill
---@field fatigue integer
---@field apply fun(value: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

Battle.princessCritDirectCap = 100
Battle.princessCritMagicScale = 100

---@type table<string, Source.Configs.Battle.Rule | nil>
Battle.players = {
    Bravor = {
        crit = function (damage, _attacker, _defender)
            return damage * 2
        end
    },
    Princess = {
        crit = function (damage, attacker, defender)
            local magic = math.max(0, attacker.MAGIC)
            local extra = defender.DEF * magic / (Battle.princessCritMagicScale + magic)
            return damage + math.min(damage, Battle.princessCritDirectCap) + extra
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
        fatigue = 6,
        apply = function (base, attacker, _defender)
            local magic = math.max(0, attacker.MAGIC)
            local strike = math.max(0, base)
            return math.round(strike * (1 + (magic + strike) / math.max(1, strike - magic)))
        end
    }
}

---@type table<string, Source.Configs.Battle.Skill | nil>
Battle.defenseSkills = {
    HuiMu = {
        fatigue = 5,
        apply = function (incoming, _attacker, defender)
            local magic = math.max(0, defender.MAGIC)
            local damage = math.max(0, incoming)
            if damage <= 0 then
                return 0
            end
            return math.round(damage * math.max(0, damage - magic) / (damage + magic))
        end
    }
}

Battle.criticalFatigue = 5
Battle.attackInterval = 0.12
Battle.attackExtraDelay = 0.10

return Battle
