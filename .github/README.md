# Windows 与 AOS 打包

`package-game.yml` 在推送到 `main` 时自动运行；仅修改 `.github/**` 时不触发自动构建，同时修改该目录之外的文件时仍正常构建。也可在 Actions 的 **Package Game → Run workflow** 中选择 `main` 手动运行。每次运行固定本项目提交，Windows 与 AOS 两个 job 并行构建；不生成 MT3 macOS 游戏包。

## 上游工具与产物

默认使用 GitHub 自动提供的 `GITHUB_TOKEN` 查询和下载公开的 Ludork 产物。如果跨仓库下载权限不足，可配置能访问 `JasonLeon01/Ludork`、具有 **Actions: Read-only** 权限的 `LUDORK_ACTIONS_TOKEN`；两个平台均优先使用该 Secret。

两部分共用 `package-upstream.cjs` 的产物选择逻辑：优先检查 Ludork `main` HEAD 的成功 `export-editor.yml` 运行，再向前查找更早的成功运行。必须存在实际成功的平台 job，以及名称精确匹配、唯一且未过期的产物；跳过没有生成包的运行、缺失或过期产物，全部不可用才报错。

| MT3 产物 | 构建环境 | Ludork 工具来源 |
| --- | --- | --- |
| Windows x64 游戏目录 | `windows-2022` | `Windows x64` job 的 `Ludork-windows-x64-<上游提交>` |
| AOS / Android ARM64 签名 APK | `macos-15` Apple Silicon | `macOS ARM64` job 的 `Ludork-macos-arm64-<上游提交>` DMG |

Ludork 只提供工具。项目的 Engine、Application、资源和 Android 模板均使用本仓库选定提交，不用上游模板替换项目源码；两个平台均采用默认 Release 打包，不额外启用 Lua 编译、加密或 ldpak。

## Windows

沿用现有工具、构建和状态缓存。每次推送或手动运行都会打包选定提交；最近一次成功工作流中的 `Package Windows` job 用于定位缓存基线，不按相同提交跳过手动重试。保持 workflow 文件名及该 job 名称稳定，以保留历史查找。

`tools/pack_project.bat` 完成构建后检查运行程序、资源目录和 DLL；成功上传及缓存保存完成后才发布新的缓存状态。`ProjectMT3-windows-x64-<项目提交>` 保留 **7 天**，包含游戏目录与 `build-info.json`。

## AOS / Android

AOS job 下载 Ludork DMG，只读挂载并用 `ditto` 提取 `Ludork.app/Contents/Resources/tools`，保留工具的执行权限后卸载镜像。复用其中的 `pack_android.sh` 及 ScriptTools，不构建 macOS 游戏。

构建环境按 Ludork 的 Android 打包契约准备：Android Studio 及其 JBR、SDK Platform 36、Build Tools 36.0.0、稳定 NDK r27 或更新版本，以及系统 CMake。CI 确保安装 NDK `27.3.13750724`，打包器从已有安装中选择最高的完整稳定版本；需要时补装 Rosetta。runner 环境参见 [GitHub macOS ARM64 镜像清单](https://github.com/actions/runner-images/blob/main/images/macos/macos-15-arm64-Readme.md)。

使用仓库已有的四个 Secrets：

| Secret | 用途 |
| --- | --- |
| `AOS_ALIAS` | keystore 内的签名密钥 alias |
| `AOS_KEY_PASSWORD` | 密钥密码 |
| `AOS_KEY_STORE_PASSWORD` | keystore 密码 |
| `AOS_SIGNING_KEY` | keystore 文件内容的 Base64 编码 |

签名材料仅在打包步骤注入。keystore 解码到 runner 临时目录，密码按“keystore 密码、密钥密码”的顺序通过标准输入传给 Ludork 打包器，不写入命令参数或产物。脚本退出时清理临时密钥，workflow 的 `always()` 步骤再做一次清理；缺失 Secret、签名或校验失败直接失败，不回退到未签名 APK。

打包器负责 APK 签名、签名验证、资源及原生库检查。只上传一个 `*-android-arm64-v8a-signed.apk` 和 `build-info.json`，产物名为 `ProjectMT3-aos-arm64-v8a-<项目提交>`，保留 **7 天**。元数据记录项目提交、Engine tree、实际选中的 Ludork 提交/运行/产物 ID 及 APK SHA-256。AOS 当前每次进行完整打包，不复用 Windows 的原生构建缓存。

首次托管运行需确认下载、Android 工具安装、完整编译、签名验证和上传成功，再安装到 Android 设备验证启动。脚本和工作流的本地检查不替代托管构建、真实密钥签名或设备验收。

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
