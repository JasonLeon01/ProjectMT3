# ProjectMT3

This project recreates the game in the sibling Mota directory using Ludork. Mota provides the reference mechanics and original assets; runtime behavior, UI, input, and data generation follow Ludork conventions.

- Do not run `git add`, `git commit`, or `git rebase` on your own initiative. Preserve existing working tree changes.
- Limit changes to the request and leave unrelated content untouched. Reuse existing logic where possible, and remove temporary test code when finished.
- Do not reimplement native features in Lua. Before using or adding a standard math function, inspect the current engine API, stubs, and native implementation to confirm its parameters, return types, and boundary semantics, then reuse it directly.
- Use existing window backgrounds and borders, declarative UI assets, and handwritten Controllers. Preserve the original dimensions and positions of icons and breath assets.
- Follow the existing asset layout: animation images in `Assets/Animations`, UI icons in `Assets/Icons`, static system UI textures in `Assets/System`, and animation definitions in `Data/Animations`.
- Keep battle logic in the window Controller and split it into actions. Handle keys in `onKeyDown`, advance timing with `Timer`, and react to state changes with `watch`.
- Battles use temporary HP and breath values. Write them back on victory, discard them on retreat, and enter Game Over on death.
- `crit` is the damage callback in Lua Config. General Data's `CritAnimationKey` selects the critical-hit animation. A successful critical hit adds 5 fatigue, taking effect on the next attack.
- Battle specials include `DeathCurse`, `Frost`, `Ambush`, `Burn`, `ArmorBreak`, `Thunder`, and `AuraField`. Post-battle specials include `Reborn`, `Ember`, `Glacial`, `BurstFlame`, and `Snowland`. Ambush doubles melee attack only in the opening battle panel and damage estimates; the handbook and encyclopedia show the original attack. Preserve map specials and existing damage estimates.
- Poison and weakness gain stacks after a valid attack resolves. Keep them on victory and discard them on retreat. Each poison stack deals 1 additional HP damage with separate floating text. Life steal uses only the HP actually removed by the attack itself, excludes poison damage, and cannot heal above `MAXHP`.
