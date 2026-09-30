local EventKeys = {
    LocaleChanged = "LocaleChanged",
    AbilitySystemChanged = "AbilitySystemChanged",
    PlayerChanged = "PlayerChanged",
    PartyChanged = "PartyChanged",
    AbilitySystemChangeKind = {
        Attribute = "Attribute",
        State = "State"
    },
    PlayerChangeKind = {
        Inventory = "Inventory",
        Name = "Name",
        Map = "Map"
    }
}

return EventKeys
