local Battle = {}

---@class Source.Configs.Battle.Rule
---@field crit fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@class Source.Configs.Battle.Skill
---@field apply fun(value: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

Battle.huiRenMultiplierCeil = 2

---@type table<string, Source.Configs.Battle.Rule | nil>
Battle.players = {
    Bravor = {
        crit = function (damage, _attacker, _defender)
            return damage * 2
        end
    },
    Princess = {
        crit = function (damage, attacker, _defender)
            damage = math.max(0, damage)
            local k = 12
            local basic = 1.75
            local X = k * (attacker.MAGIC / attacker.ATK) * (_defender.DEF / attacker.ATK)
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
        apply = function (damage, attacker, _defender)
            damage = math.max(0, damage)
            local k = 13
            local basic = 1.85
            local X = k * (attacker.MAGIC / attacker.ATK) * ((attacker.ATK - _defender.DEF) / attacker.ATK)
            local extra = 2.15 * X / (1 + X)
            return math.floor(damage * (basic + extra))
        end
    }
}

---@type table<string, Source.Configs.Battle.Skill | nil>
Battle.defenseSkills = {
    HuiMu = {
        apply = function (damage, _attacker, defender)
            damage = math.max(0, damage)
            if damage == 0 then
                return 0
            end
            local basic = 0.2
            local k = 15
            local X = k * (defender.MAGIC / defender.DEF) * ((_attacker.ATK - defender.DEF) / _attacker.ATK)
            local extra = 0.2 * X / (1 + X)
            return math.round(damage * (1 - (basic + extra)))
        end
    }
}

Battle.criticalFatigue = 5
Battle.attackInterval = 0.12
Battle.attackExtraDelay = 0.10

return Battle
