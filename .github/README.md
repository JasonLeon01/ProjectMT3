# Windows 打包

`package-windows.yml` 在推送到 `main` 时自动运行；仅修改 `.github/workflows/**` 时不触发自动构建，同时修改项目文件时仍正常构建。也可在 Actions 的 **Package Windows → Run workflow** 中选择 `main` 手动运行。

## 启用

默认使用 GitHub 自动提供的 `GITHUB_TOKEN` 查询和下载公开的 Ludork 产物，不因未配置自定义 Secret 而提前终止。本仓库历史查询也使用该 token，不需要内容写权限。

如果实际下载返回权限错误，可在本仓库 **Settings → Secrets and variables → Actions** 添加可选 Secret `LUDORK_ACTIONS_TOKEN`：使用能访问 `JasonLeon01/Ludork`、具有 **Actions: Read-only** 权限的 fine-grained token。配置后，上游查询与下载均优先使用它。公开产物列表可匿名读取，但下载接口仍需要认证；自动 token 的跨仓库下载需由实际 CI 验证。

## 打包和去重

- 只比较 ProjectMT3 的提交 hash。找到本工作流最近一次工作流成功、且 `Package Windows` job 实际成功的运行，以其 `head_sha` 为基准；相同则跳过，否则构建。不要随意更改该 job 的显示名称。
- 跳过、失败和取消不更新基准；首次运行或历史被删除后重新构建。去重不依赖 artifact，因此游戏包过期不会单独触发重建。查询失败直接报错。
- 需要构建时，选择 Ludork `main` 上最新成功完成、且 `Windows x64` job 实际成功的 `export-editor.yml` 运行，排除 PR。上游因近期无变化而跳过打包时，工作流仍会成功但没有产物；这类运行会被忽略，继续使用更早的实际 Windows 打包。下载其 `Ludork-windows-x64-<commit>`；这次实际打包的产物缺失、过期或下载失败时报错，不回退到更旧的打包。
- Ludork 只提供工具。项目的 Engine、Application 和资源直接使用本仓库选定提交，通过随包 `tools/pack_project.bat` 编译 Windows x64 Release；不升级项目，不额外启用 Lua 编译、加密或 ldpak。
- 成功上传的 `ProjectMT3-windows-x64-<项目hash>` 保留 **7 天**，包含游戏目录与 `build-info.json`。该 JSON 记录项目提交、Ludork 提交及上游运行和产物 ID。

首次启用后手动运行，确认下载、完整编译和上传成功，再下载游戏包验证启动。脚本和工作流静态检查不能替代托管构建及游戏运行验收。

# Ludork Engine 与 Global 增量同步

`sync-ludork.yml` 在 **Sync Ludork Engine and Global → Run workflow** 手动运行，或每四小时运行一次（北京时间 00:17、04:17、08:17、12:17、16:17、20:17）。工作流必须先进入默认分支；GitHub 定时调度可能延迟。

## 启用和 PR

在 **Settings → Actions → General → Workflow permissions** 开启 **Allow GitHub Actions to create and approve pull requests**。工作流使用自带的 `GITHUB_TOKEN`，声明 `contents: write` 和 `pull-requests: write`，不需要额外 Secret；`LUDORK_ACTIONS_TOKEN` 仍仅用于原有打包流程。

同步从本项目默认分支出发，在固定分支 `codex/sync-ludork` 创建或更新一个 PR，统一包含 Engine、Global 和基准记录，等待人工合并。该分支由工作流维护，请勿在其中添加其他修改。PR 记录两个来源基准 SHA、统一的目标 SHA 和运行链接；同步流程不自动合并或构建，合并到 `main` 后按打包工作流的路径规则触发构建。

## 文件与基准

| 项目 | Engine | Global |
| --- | --- | --- |
| 上游目录 | `Game/Engine/` | `Game/Scripts/Global/` |
| 本项目目录 | `Engine/`（排除 `ThirdParty/`） | `Scripts/Global/`（包含全部子目录） |
| `.github/ludork-sync.json` 中的基准字段 | `engine.upstreamSha` | `global.upstreamSha` |

- 来源固定为 `JasonLeon01/Ludork` 的 `main`，每次只获取一次上游历史并固定同一个来源 SHA。两个原有提交 SHA 原值迁入同一个 JSON，各自记录已合并的同步位置，不强行对齐基准。
- 同步范围始终由 Ludork 自己的两个版本决定：各自已同步 SHA 与本次 SHA 之间对应目录的变化，不通过比较项目目录与上游目录来发现待同步文件。文件内容、执行权限和符号链接的上游变化均参与判断。
- 只处理已合并基准至最新上游 SHA 之间新增、修改或删除的文件；重命名按删除旧路径和新增新路径处理。修改文件使用完整上游版本，不合并文件内部的项目改动；上游未修改的文件保留项目版本。只对这些候选文件检查项目是否已经相同，以避免无意义写入。
- 每次从默认分支的已合并基准重新计算，不使用未合并 PR 中的 SHA 作为起点。因此后续运行更新同一 PR 时会保留两部分此前未合并的累计修改。某部分没有实际文件差异时不推进其基准；两部分均无差异时不产生空提交或新 PR，PR action 会关闭差异已消失的旧 PR。
- 上游获取失败、目录缺失、基准缺失、非法或不再是上游祖先时同步失败，不降级为双方目录对齐或全目录替换。基准更新与对应文件修改一起进入 PR，由工作流维护。
- `Engine/ThirdParty/`、`Scripts/GlobalFunctions/`、`Scripts/Internal/`、`Scripts/Source/` 均不属于同步范围。

## 定时判断与验收

定时运行以任务开始时刻为截止点，检查此前四小时（包含边界）内上游 `main` 第一父链上的提交，以提交者时间判断。提交相对第一父提交的变化涉及 `Game/Engine/`（排除 `ThirdParty/`）或 `Game/Scripts/Global/`，任一满足即启动统一同步，因此合并引入的有效修改也能识别。仅 ThirdParty 或其他目录修改、无有效近期提交时跳过；手动运行忽略时间条件。

四小时窗口只决定是否启动；一旦启动，两部分均覆盖各自已合并基准之后的全部累计差异，避免仅一部分近期变化时遗漏另一部分的待合并修改。调度延迟或失败造成的漏跑可手动补同步。同步流程不修改 Binaries 或 Lua stub。

首次启用后手动检查 PR 中的文件范围及两个基准 SHA，再次运行确认复用同一 PR，合并后确认后续同步从各自新基准继续。本地夹具及语法检查不替代托管 Actions 的权限、PR 创建和更新验收，也不代表编译或游戏运行通过。
