local Battle = {}

---@class Source.Configs.Battle.Rule
---@field crit fun(damage: number, attacker: Source.Windows.WindowBattle.BattlerState, defender: Source.Windows.WindowBattle.BattlerState): number

---@type table<string, Source.Configs.Battle.Rule | nil>
Battle.players = {
    Bravor = {
        crit = function (damage, _attacker, _defender)
            return damage * 2
        end
    },
    Princess = {
        crit = function (damage, _attacker, _defender)
            return damage * 2
        end
    }
}

Battle.enemy = {
    crit = function (damage, _attacker, _defender)
        return damage * 2
    end
}

Battle.criticalFatigue = 5
Battle.attackInterval = 0.12
Battle.attackExtraDelay = 0.10

return Battle
