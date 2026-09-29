# CG 配乐：The Spire and the Unfinished Oath

依据：现有 `cg.mp4` 的十段画面、`script_op_cg.txt` 的英文旁白、`File 01.wav` 至 `File 10.wav` 的实际采样长度，以及 `build_cg.py` 的 30 fps 帧取整和淡出规则。音乐方向是配乐设计建议；音频检查依据时长、波形电平和停顿，不包含逐词听写校验。

原片为 **65.500 秒**。片头新增 **1.000 秒黑屏、无旁白、有音乐**，新片总长 **66.500 秒 / 1995 帧**。所有下列时间均为加停顿后的成片绝对时间，BGM 从 0 秒开始。片头停顿暂定，修改后需同步调整后续配乐节点。

最终对齐后的配乐文件名：**`cg_bgm.wav`**，放在 `Assets/Videos/`，与合成脚本同目录。建议交付为 48 kHz 双声道 WAV，66.500 秒，文件开头不得留静音。脚本也可解码其他采样率，但改格式时应同步修改 `BGM_FILENAME`。

## Suno 使用方式与精度

开启 **Instrumental**。将下面的 Style Prompt 填入风格/音乐描述；如果界面允许填写结构提示，将后面的 Timeline Prompt 一并作为结构描述使用。不要把现有旁白正文放进歌词框，以免生成演唱或重复旁白。

时间戳表达的是目标配乐节点，**不能保证 Suno 一次生成就精确到秒或帧**。先选情绪走向合适的版本，再按本文件的帧时间码剪辑、替换段落或局部伸缩。Suno 官方介绍了结构标签、分段替换/裁剪，以及 Studio 的时间伸缩；本文件中的自定义带时间标签不是官方逐帧控制语法。

- [Suno：创建与结构提示](https://suno.com/hub/how-to-make-a-song)
- [Suno：Song Editor 分段编辑与淡入淡出](https://help.suno.com/en/articles/6141505)
- [Suno：Studio 音频片段与时间伸缩](https://help.suno.com/en/articles/13670977)

## Style Prompt（可直接复制）

```text
Instrumental orchestral underscore for a narrated medieval dark-fantasy RPG opening, target 66.5 seconds, through-composed cinematic miniature. Start instantly with ONE second of heroic brass, timpani and cymbal impact; settle by 1s into quiet fragile hope. Warm strings, harp and woodwinds for rebuilding; noble restrained horns for a fallen hero's memorial; gentle village warmth. At 23.467s drain the warmth into ominous low strings. At 27.333s introduce a descending tower motif; at 31.800s a heavier impact launches urgent string ostinatos, low brass and war drums. At 43.067s desperate defenders recall the heroic motif in minor. At 56.567s pull back to intimate cello and a tense pulse. At 62.867s sparse celesta and unresolved strings suggest a princess's secret resolve; decay to silence by 66.500s. Clear space for English narration, restrained midrange, strong dynamic contrast. No vocals, choir, speech, lyrics, pop beat, EDM drop, repetitive chorus or triumphant ending.
```

## Timeline Prompt（分段意图，可复制）

```text
[Instrumental cinematic underscore; target duration 66.500 seconds]
[Absolute cue times below are arrangement targets; all instruments only]

[Opening hit | 00:00.000–00:01.000]
Immediate heroic orchestral attack on the first instant: bright brass, timpani and a short cymbal bloom. One compact exclamation, no lead-in or slow build. Reduce energy during the last quarter-second, landing softly at 00:01.000.

[Fragile recovery | 00:01.000–00:08.600]
Ruined farmland and a wounded kingdom rebuilding. Quiet warm strings, sparse harp and a small woodwind motif; hope carries traces of grief. Spacious phrasing beneath narration, no heavy drums.

[The hero remembered | 00:08.600–00:16.967]
A stone hero stands above the ruined town square. Introduce a noble, restrained French-horn phrase supported by soft strings. Remember the opening's heroic color with reverence, keeping it intimate.

[Peace returns | 00:16.967–00:23.467]
Golden light over a busy medieval town. Gentle woodwinds and flowing harp, warmer harmony, modest lift. Thin the upper strings and darken the last second to prepare the cut to black.

[Fate turns | 00:23.467–00:27.333]
Black screen: mercy is withdrawn. Warm instruments drop away at the cut; a quiet low-string drone and distant bass pulse remain. A short dissonant rise prepares the tower's reveal.

[The descending spire | 00:27.333–00:31.800]
A tower hangs in storm clouds above the land. Dark low-brass accent at the reveal, then descending strings and controlled tension. Save the heaviest impact for the next cue.

[Impact and invasion | 00:31.800–00:38.033]
The tower crashes down as a terrifying legion invades. The score's strongest impact at 00:31.800, then urgent low-string ostinatos, war drums and short brass accents. Powerful but leave the narrator clearly audible.

[The realm falls | 00:38.033–00:43.067]
Burning streets and devastated lands. Continue the pulse, darken the harmony, add a descending lament over the attack rhythm. Dread and loss rather than celebration.

[The last defense | 00:43.067–00:56.567]
The royal army holds a ruined palace gate. Bring back the heroic motif in minor, strained horns, restrained martial percussion and restless strings. Sustain desperate resistance; gradually erode the strength near the end. No victory cadence.

[The king's command | 00:56.567–00:62.867]
The king sends the guard away with the princess. Drop the large drums and brass; intimate cello, low strings and a subdued heartbeat pulse. Duty, fear, tenderness and urgency.

[The princess's secret | 00:62.867–00:66.500]
Black screen and an unexpected decision. Remove the martial pulse at the cut; sparse celesta and high suspended strings suggest quiet resolve and unanswered possibility. After about 00:65.700 let only the tail remain. Fade from 00:66.000 to silence at 00:66.500, unresolved and without a final triumphant hit.
```

## 精确剪辑时间表

帧时间码格式为 **分:秒:帧（30 fps）**，区间包含起点、不包含终点。小数秒四舍五入至毫秒；剪辑以帧时间码为准。画面段落长度包含其淡出，不能仅将 WAV 时长直接相加。

| 成片时间（秒） | 帧时间码 | 画面 / 旁白起句 | 配乐动作 |
| --- | --- | --- | --- |
| 0.000–1.000 | 00:00:00–00:01:00 | 新增黑屏，无旁白 | 第一瞬间激昂铜管与定音鼓；0.750 起收束，1.000 前转柔 |
| 1.000–8.600 | 00:01:00–00:08:18 | op1；“Six months…” | 废墟中的复苏；柔和弦乐、竖琴、少量木管 |
| 8.600–16.967 | 00:08:18–00:16:29 | op2；“To honour the hero…” | 英雄雕像；圆号克制地呈现纪念主题 |
| 16.967–23.467 | 00:16:29–00:23:14 | op3；“Peace…” | 暖色城镇；和声稍明亮，22.467 起逐渐抽离温暖 |
| 23.467–27.333 | 00:23:14–00:27:10 | 黑屏；“Yet fate…” | 转折处撤去旋律，低弦与不安的低频脉冲 |
| 27.333–31.800 | 00:27:10–00:31:24 | op4；“On that dark day…” | 云中高塔；低铜管短击，下行音型蓄力 |
| 31.800–38.033 | 00:31:24–00:38:01 | op5；“A terrifying legion…” | 高塔落地；全片最重的一击，随后进入侵攻节奏 |
| 38.033–43.067 | 00:38:01–00:43:02 | op6；“Within moments…” | 城镇燃烧；延续节奏，加入悲剧性下行旋律 |
| 43.067–56.567 | 00:43:02–00:56:17 | op7；“The royal army…” | 宫门死守；英雄主题转小调，后半逐渐显出败势 |
| 56.567–62.867 | 00:56:17–01:02:26 | op8；“In desperation…” | 国王托付；骤降为大提琴与低弦，保留紧迫感 |
| 62.867–66.500 | 01:02:26–01:06:15 | 黑屏；“But the princess…” | 撤鼓，钟琴与悬置弦乐；秘密决意，未解决式收尾 |

优先锁定 **0.000 首击、1.000 收束、23.467 转暗、27.333 高塔显现、31.800 落地、56.567 国王托付、62.867 公主悬念、66.500 结束**。不要让侵攻高潮提前覆盖重建与纪念画面。

## 1 秒开头的备选生成提示

若整曲生成无法做到“立即激昂、1 秒内收住”，单独生成以下短音头，剪成 **1.000 秒**；主体音乐按上表从成片 1.000 秒处接入，总长仍为 66.500 秒。不要把整段激昂序奏硬塞进这一秒。

```text
Instrumental one-shot orchestral logo sting for a medieval fantasy story. Immediate heroic brass and timpani attack at the very first instant, short cymbal bloom, compact triumphant energy, rapidly decaying into a soft warm string bed within one second. No pickup, no silence before the hit, no long crescendo, no vocals or choir. Designed to transition into quiet reflective narration underscore.
```

这是短音头的生成意图，仍需手工裁剪并处理尾音。拼接时在接缝周围使用短交叉淡化，保持主体起点为成片 **1.000 秒**，不要因重叠使时间轴提前。

## 合成与音量

`build_cg.py` 已增加 `INTRO_HOLD_SECONDS = 1.0` 和 `BGM_FILENAME = "cg_bgm.wav"`，开头黑屏无旁白，配乐从第一帧开始。放好最终对齐的音乐后运行合成脚本即可混入。

- 音乐缺失时会明确提示，仍可输出带 1 秒停顿的旁白版。音乐不足 66.500 秒时会报错，避免结尾突然失去配乐；超过部分会截去，不自动循环或伸缩。时间节点需在交付音乐前对齐。
- 原始旁白各段峰值约 **−23 至 −19 dBFS**，整段 RMS 约 **−41 至 −35 dBFS**（含停顿）。这是波形电平，不是 LUFS。不要将常见高响度音乐导出直接以原音量叠加。
- 当前片头音乐增益 **0.18（约 −14.9 dB）**，旁白段 **0.035（约 −29.1 dB）**，比初版旁白段配乐提高约 **2.9 dB**；0.750–1.000 秒平滑退到旁白配乐音量。这是混音增益，不是响度匹配结果，最终平衡以试听为准。
- 原旁白保持原增益；配乐最后 **0.500 秒**淡出。若总混音峰值超过 0.99，整体同比缩小以避免削波。脚本不负责自动识别或移动音乐的叙事节点。

本次只准备提示词、时间表和合成入口；尚未生成 `cg_bgm.wav`。
