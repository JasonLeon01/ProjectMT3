# ProjectMT3

本项目使用 Ludork 复刻相邻目录 Mota 中的游戏。Mota 是机制和原始素材的参考，运行时、UI、输入与数据生成遵循 Ludork 的规范。

- 禁止自行调用 git add、commit、rebase；保留已有工作区改动。
- 精确围绕需求修改，不修改无关内容；尽可能复用逻辑，完成后清理临时测试代码。
- UI 使用现有窗口背景和边框、声明式资产及手写 Controller；原版图标和气息素材保持尺寸与位置。
- 资产按现有目录规范归类：动画图片放 Assets/Animations，UI 图标放 Assets/Icons，静态系统 UI 纹理放 Assets/System；动画定义放 Data/Animations。
- 战斗逻辑放在窗口 Controller，拆分动作，onKeyDown 响应按键，Timer 推进时序，watch 响应状态变化。
- 战斗使用临时血量和气息，胜利写回、撤退丢弃、死亡进入 Game Over。
- crit 是 Lua Config 中的伤害回调；会心动画由 General Data 的 critanimationkey 指定。成功会心增加 5 点疲劳，下次攻击生效。
- 暂不加入魔力、技能、V 调息及战斗 special；保留地图 special、重生和现有预估伤害，移除战后中毒与衰弱附加。
