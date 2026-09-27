# 五平台加密打包与干净构建

`package-game.yml` 在推送到 `main` 时运行；仅修改 `.github/**` 时不自动触发。它先固定项目提交与上游工具身份，再并行调用 Windows、Android、HarmonyOS、macOS、iOS 五个可复用工作流；一个平台构建失败不会取消其他平台。

各平台也有独立的手动入口，在 **Actions → 对应工作流 → Run workflow** 选择 `main`：

| 工作流 | 文件 | 运行范围 |
| --- | --- | --- |
| Package Game | `package-game.yml` | 五个平台 |
| Package Windows | `package-windows.yml` | Windows x64 |
| Package AOS ARM64 | `package-android.yml` | Android ARM64 |
| Package HOS ARM64 | `package-harmony.yml` | HarmonyOS Mobile / OpenGL ES ARM64 签名 HAP |
| Package macOS ARM64 | `package-macos.yml` | macOS ARM64 签名、公证 |
| Publish iOS TestFlight | `package-ios.yml` | iOS 构建、签名及内部 TestFlight 发布 |

只验证 iOS 时运行 **Publish iOS TestFlight**，不会启动其他平台。新工作流文件需要先提交到默认分支，GitHub 才会显示手动入口。修改 CI 后请启动新运行；重试旧运行仍使用旧提交。

`package-inputs.yml` 共用项目与上游产物选择逻辑。总入口将已固定的输入传给子工作流，不重新选择上游；独立运行只选择对应工具平台，Windows 使用 x64 工具，其他四个平台使用 macOS ARM64 工具。

所有平台均使用 Release 编译和 `--dev --encrypt-data --compile-lua --use-ldpak`：

- Data JSON 加密为 `.ldc`；Lua 使用 `luac -s` 编译成 `.luac` 并移除源文件。Lua 字节码不是密码加密。
- 资源分别写入 `Assets.ldpak`、`Data.ldpak`、`Scripts.ldpak`。ldpak 是归档格式，本身不增加加密。
- shader 和存档格式保持原设置，不新增这两项加密。
- CI 检查归档结构、CRC、编译脚本入口及加密数据文件，拒绝遗留 Lua 源码、原始 Data JSON 或散放资源目录。

## 干净构建与工具来源

每个 job 使用新的 GitHub 托管 runner 和选定提交的 checkout，打包前清理项目 `build/`、`bin/`、`Intermediate/`、`Cache/`、`EditorCache/` 及对应临时产物目录。清理脚本仅允许在 CI 的指定 checkout 和 runner 临时目录运行。

**Actions 仅缓存 Android Studio 和 DevEco Studio 应用目录**。Android 保留原有 key `android-studio-macos-15-arm64-v1`；DevEco 缓存 `DevEco-Studio.app`（含其自带 SDK 和构建工具），key 为 `deveco-studio-macos-15-arm64-v1-<HOS_DEVECO_SHA256>`，只使用完整 key 匹配。不恢复或保存 Ludork 工具、项目构建产物、状态、独立 Android SDK/NDK、Gradle、Ruby gem 或项目依赖缓存。Android 使用全新的临时 `GRADLE_USER_HOME`，关闭 Gradle build/configuration cache。runner 镜像预装工具正常使用；旧 Windows 远程缓存不会再被读取或续存，由 GitHub 过期回收，也可在 Actions → Caches 手动删除。

共用 `package-upstream.cjs` 选择 Ludork `main` 上最新可用的 **Export Editor** 产物：兼容双平台和对应单平台工作流；只接受可信事件、目标平台 job 成功、名称精确匹配且唯一未过期的产物。另一平台失败不排除目标平台已成功的产物，不回退到带模板的 **Export Package**。Windows 使用 x64 工具；Android、HarmonyOS、macOS、iOS 下载同一份选定的 ARM64 DMG，各自只读挂载并提取工具，不跨 job 复用下载目录。

项目 Engine、Application、资源和移动端模板始终来自本仓库。默认使用 `GITHUB_TOKEN` 读取上游公开产物；跨仓库权限不足时配置可读取 `JasonLeon01/Ludork`、具有 **Actions: Read-only** 权限的 `LUDORK_ACTIONS_TOKEN`。

| Job | 环境 | GitHub 产物，保留 7 天 |
| --- | --- | --- |
| Package Windows | `windows-2022` | `ProjectMT3-windows-x64-<提交>`：游戏目录、构建记录 |
| Package AOS ARM64 | `macos-15` ARM64 | `ProjectMT3-aos-arm64-v8a-<提交>`：签名 APK、构建记录 |
| Package HOS ARM64 | `macos-15` ARM64，指定版本的 DevEco Studio | `ProjectMT3-hos-arm64-v8a-<提交>`：签名 HAP、构建记录 |
| Package macOS ARM64 | `macos-15` ARM64 | `ProjectMT3-macos-arm64-<提交>`：已签名公证 `.app` 的 ZIP、构建记录 |
| Publish iOS TestFlight | `macos-15` ARM64，已安装的最新稳定 Xcode 26 / iPhoneOS 26 SDK | `ProjectMT3-ios-testflight-<提交>`：`testflight.txt`、构建记录，绝不上传 IPA |

`build-info.json` 记录项目/Engine 提交、Ludork 提交/运行/产物 ID、构建选项、平台和干净构建状态；APK/HAP/ZIP 还记录 SHA-256。Package Game 的完整运行串行排队，单次运行内五平台并行。每个平台另有固定的并发组，独立运行与总入口调用共用该组并排队执行；iOS 的输入选择、构建号分配、上传和处理等待均在同一并发组内，避免两个入口同时分配构建号。父、子工作流使用不同的并发组，避免互相等待。

## GitHub Secrets

进入本仓库 **Settings → Secrets and variables → Actions → Secrets → New repository secret**，按下表填写。密钥文件不能提交到仓库；密码填写原文，不要做 Base64。p12 必须包含证书对应的私钥，建议设置非空导出密码。

| Secret | 内容 / 用途 |
| --- | --- |
| `AOS_ALIAS` | 现有 Android keystore alias |
| `AOS_KEY_PASSWORD` | 现有 Android 密钥密码 |
| `AOS_KEY_STORE_PASSWORD` | 现有 Android keystore 密码 |
| `AOS_SIGNING_KEY` | 现有 Android keystore 的 Base64 |
| `HOS_ALIAS` | HarmonyOS P12 中的密钥别名 |
| `HOS_KEY_PASSWORD` | HarmonyOS 密钥密码原文 |
| `HOS_KEY_STORE_PASSWORD` | HarmonyOS P12 密钥库密码原文；相同时与上一项填写相同值 |
| `HOS_SIGNING_KEY` | HarmonyOS `.p12` 文件的 Base64 |
| `HOS_SIGNING_CERTIFICATE` | 与 P12 私钥匹配的 `.cer` 证书链文件的 Base64 |
| `HOS_PROVISIONING_PROFILE` | 与证书和应用包名匹配的已签名 `.p7b` Profile 的 Base64 |
| `APPLE_TEAM_ID` | 10 位 Apple 开发者团队 ID，不是 Issuer ID |
| `APP_STORE_CONNECT_KEY_ID` | App Store Connect 团队 API 密钥 ID |
| `APP_STORE_CONNECT_ISSUER_ID` | 同一个 API 密钥的 Issuer ID |
| `APP_STORE_CONNECT_PRIVATE_KEY` | 下载的 `.p8` 文件的 Base64，macOS 公证和 iOS 上传共用 |
| `MACOS_SIGNING_CERTIFICATE` | **Developer ID Application** `.p12` 的 Base64 |
| `MACOS_SIGNING_CERTIFICATE_PASSWORD` | 上述 macOS p12 的导出密码 |
| `IOS_SIGNING_CERTIFICATE` | **Apple Distribution** `.p12` 的 Base64 |
| `IOS_SIGNING_CERTIFICATE_PASSWORD` | 上述 iOS p12 的导出密码 |
| `IOS_PROVISIONING_PROFILE` | 与 iOS 证书和 Bundle ID 匹配的 **App Store Connect** 分发 `.mobileprovision` 的 Base64 |
| `LUDORK_ACTIONS_TOKEN` | 可选，上游 Actions 只读令牌 |

macOS 终端分别执行下列命令，将剪贴板内容粘贴进对应 Secret：

```bash
base64 -i /path/to/certificate.p12 | pbcopy
base64 -i /path/to/AuthKey_KEYID.p8 | pbcopy
base64 -i /path/to/profile.mobileprovision | pbcopy
```

已有一个 p12 不代表两种用途都已具备：在钥匙串中检查证书名称，macOS 必须是 **Developer ID Application**，iOS 必须是 **Apple Distribution**。在 Apple Developer → Certificates, Identifiers & Profiles 创建对应证书，并从钥匙串连同私钥导出 p12。macOS 不使用 Developer ID Installer，也不上传 Mac App Store。

在 **App Store Connect → Users and Access → Integrations → App Store Connect API → Team Keys** 创建 **App Manager** 角色密钥；记录 Key ID、Issuer ID，下载只能下载一次的 p8。此流程使用 API 密钥，不需要 Apple ID 密码、应用专用密码或 CI 中的双重认证。

参考：[GitHub 证书配置](https://docs.github.com/en/actions/how-tos/deploy/deploy-to-third-party-platforms/sign-xcode-applications)、[App Store Connect API](https://developer.apple.com/help/app-store-connect/get-started/app-store-connect-api/)。

## HarmonyOS 首次配置

先让包含 HOS 材料签名功能的 Ludork 代码生成成功的 **Export Editor macOS ARM64** 产物。HOS job 仍复用原有最新可用产物选择规则；若选中的工具不支持 `--sign --certificate --profile`，会明确失败，不能使用旧的自动签名工具。不要把本地 `.tools`、DevEco 安装目录或签名材料提交到本仓库。

在仓库 **Settings → Secrets and variables → Actions** 添加上表六个 HOS 签名 Secrets。文件都先做 Base64，密码和 alias 直接填原文；`.csr` 不需要上传。macOS 可分别复制：

```bash
base64 -i /path/to/signing.p12 | pbcopy
base64 -i /path/to/certificate.cer | pbcopy
base64 -i /path/to/profile.p7b | pbcopy
```

DevEco 安装包使用私有 GitHub Container Registry（GHCR）包保存，ORAS 直接上传完整 ZIP。先在本机登录[华为下载中心](https://developer.huawei.com/consumer/cn/download/)，下载完整的 **Mac ARM** ZIP/DMG，并核对官方 SHA-256。本地此前验证的是 **6.1.1.300 Mac ARM**；更换工具版本后仍需通过工具、SDK 和实际打包检查。

首次上传在本机终端完成。以下以账号 `OWNER`、包名 `deveco-studio` 为例；GHCR 地址中的账号必须小写。无需新建 Git 仓库或 Release。

1. 安装 [ORAS](https://formulae.brew.sh/formula/oras)：`brew install oras`。
2. 创建仅用于本机上传的 [Personal access token (classic)](https://github.com/settings/tokens/new?scopes=write:packages)，勾选 `write:packages`，不需要 `repo`。选择合适的有效期。这是 classic Token，不是 fine-grained Token；不将它保存到 ProjectMT3 的 Secrets。
3. 执行 `oras login ghcr.io --username OWNER`，在 Password 提示处粘贴 Token。
4. 切换到原始 ZIP 所在目录，核对摘要后上传。例如：

```bash
cd /path/to/download-directory
shasum -a 256 devecostudio-mac-arm-26.0.0.851.zip
oras push ghcr.io/owner/deveco-studio:26.0.0.851-macos-arm64 \
  --artifact-type application/vnd.ludork.deveco-installer.v1 \
  devecostudio-mac-arm-26.0.0.851.zip:application/zip
oras logout ghcr.io
```

新建的 GHCR 包默认是 Private，保持私有。GitHub 个人主页 → **Packages → deveco-studio → Package settings → Manage Actions access → Add repository**，添加 **ProjectMT3**，Role 设为 **Read**。这是 Actions 的访问授权；仅填写仓库链接不能替代这一步。CI 使用自动提供的 `GITHUB_TOKEN`，单独 HOS workflow 与 Package Game 的 HOS 调用均声明 `packages: read`，无需新增下载 Secret。参见 [GHCR 认证](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry#authenticating-to-the-container-registry)和[包的 Actions 访问权限](https://docs.github.com/en/packages/learn-github-packages/configuring-a-packages-access-control-and-visibility)。

在 **ProjectMT3 → Settings → Secrets and variables → Actions → Variables** 配置两个变量：

| Variable | 内容 |
| --- | --- |
| `HOS_DEVECO_PACKAGE` | `ghcr.io/owner/deveco-studio`，全部小写，不包含版本标签或 `@sha256:`。 |
| `HOS_DEVECO_SHA256` | 原始完整 ZIP/DMG 的 64 位 SHA-256，使用上面 `shasum` 输出并与官方摘要核对；ZIP 填外层 ZIP 的摘要。**不要填写 `oras push` 输出的 manifest Digest，也不要填写内部 DMG 的摘要。** |

每次上传为原始 ZIP 创建一个版本标签，并保留引用该文件的包版本。CI 直接按文件 SHA-256 获取对应 blob，不依赖可变标签或附件文件名；升级时上传新版本并更新 `HOS_DEVECO_SHA256` 即可。安装脚本支持完整 DMG、包含一个 DMG 的 ZIP 或包含一个 `DevEco-Studio.app` 的 ZIP。

旧 `HOS_DEVECO_DOWNLOAD_URL`、`HOS_DEVECO_REPOSITORY`、`HOS_DEVECO_TAG`、`HOS_DEVECO_ASSET` 变量及 `HOS_TOOLCHAIN_TOKEN` Secret 均不再使用，可删除。现有六个 HOS 签名 Secrets 保持原配置，签名材料不上传到 GHCR。

CI 先按安装包 SHA-256 恢复 DevEco Studio 缓存。命中后校验必需工具、SDK 和 ARM64 架构，直接使用，不访问 GHCR；未命中才安装 ORAS 1.3.4，通过 stdin 传入工作流 Token，按摘要下载、校验并安装，在游戏构建前保存 DevEco 应用目录缓存。ORAS 使用仓库外临时配置，不写入用户登录配置，退出时清理临时目录。通过 `LUDORK_DEVECO_STUDIO` 将工具交给 Ludork，不打开 IDE、不登录华为账号、不连接设备。包无权访问、摘要对应的文件不存在、文件校验不符、工具或 SDK 不完整都会失败；缓存损坏时从 Actions → Caches 删除相应 DevEco 缓存后重跑。

GHCR 是缓存未命中时的持久下载来源；游戏项目仍每次干净构建，签名材料和游戏构建产物均不进入缓存。[GHCR 单层限制为 10 GB，上传超时为 10 分钟](https://docs.github.com/en/packages/working-with-a-github-packages-registry/working-with-the-container-registry#troubleshooting)，3.80 GiB ZIP 完整上传需要约 7 MB/s 的持续上行速度。

当前 `APP_NAME = "ProjectMT3"` 对应 HOS bundle name 为 `com.ludork.projectmt3.76adf92ced`。Profile 必须包含该包名且有效，证书与私钥必须匹配。Debug 和 Release Profile 均可签名；Debug Profile 只授权其中列出的测试设备。`--dev` 是 Ludork 的内测版本标记，与 Profile 类型和 Release 编译优化独立。

手动运行 **Package HOS ARM64**，或运行 **Package Game** 一并打包五个平台。流水线先验证签名材料，再从干净目录编译，导出 Mobile / OpenGL ES、API 22、arm64-v8a 的签名 HAP。Ludork 校验签名和 Profile，CI 另核对 HAP 身份/版本、原生库、运行时 ZIP 摘要及三组 `.ldpak` 中的加密 Data 和编译 Lua。成功只上传签名 HAP 与含 SHA-256 的 `build-info.json`，保留 7 天；不会发布到 AppGallery，也不会安装到设备。

材料只在仓库外 runner 临时目录解码，两个密码通过 stdin 输入打包器。脚本退出时清理，workflow 的 `always()` 步骤再清理签名材料、Ludork 临时工具和 runner 上的 DevEco 副本；已保存的 Actions 缓存继续保留。失败不会降级为未签名包。

## Apple 首次配置

### macOS

配置 Developer ID p12 和 API 密钥后即可运行。CI 验证证书类型、团队和有效期，调用 Ludork 签名、公证并装订票据。使用 `ditto` 保存 `.app` 权限、符号链接及扩展属性，重新解压验证 `codesign`、`stapler` 和 Gatekeeper；失败不降级到临时签名。缺失任一必需凭据会让该 job 明确失败。

### iOS 内部 TestFlight

1. 在 Apple Developer 注册明确的 App ID。当前 `Scripts/Entry.lua` 的 `APP_NAME = "ProjectMT3"`，Ludork 生成的 Bundle ID 是 `com.ludork.<小写团队ID>.projectmt3.76adf92ced`。将 `<小写团队ID>` 替换成 `APPLE_TEAM_ID` 的小写形式；应用名改变会改变此 ID。CI 的 `pack_ios --check` 会核对生成的 ID 和描述文件，错误立即失败。
2. 为此 App ID 和 Apple Distribution 证书生成 **App Store Connect** 分发描述文件，下载并配置 `IOS_PROVISIONING_PROFILE`。开发、Ad Hoc、Enterprise 描述文件均不能替代。
3. 在 App Store Connect → Apps 创建 iOS 应用记录，选择同一 Bundle ID，自行填写显示名、主语言和唯一 SKU。
4. 在应用的 TestFlight → Internal Testing 创建一个内部组；把需要测试的 App Store Connect 用户加入此组。CI 不创建账号、不代发任意邮件，也不提交外测审核。
5. 在仓库 **Settings → Secrets and variables → Actions → Variables → New repository variable** 配置：

| Variable | 值 |
| --- | --- |
| `IOS_TESTFLIGHT_GROUP` | 上一步已有内部测试组的名称，精确匹配 |
| `IOS_USES_NON_EXEMPT_ENCRYPTION` | 确认本应用加密申报后填写 `true` 或 `false`，不能留空 |
| `IOS_ENCRYPTION_EXPORT_COMPLIANCE_CODE` | 上一项为 `true` 时必填：Apple 审批提供的合规代码；为 `false` 时不使用 |

项目启用了资源加密，CI 不擅自判断其申报类别。首次在 App Store Connect 完成适用的加密信息/材料配置，再把结果填入变量。`true` 且没有已批准代码时会拒绝构建，避免每次上传后等待手动补资料。参考：[Apple 加密配置](https://developer.apple.com/help/app-store-connect/manage-app-information/overview-of-export-compliance/)。

6. 手动运行 Publish iOS TestFlight（或通过 Package Game 一起运行五个平台）。iOS 先核对证书、描述文件、应用记录和内部组，通过 App Store Connect 查询所有已有 iOS 构建号，以三段数字格式递增分配，包含失败、已过期构建及仍在转为 Build 的上传记录，避免同小时运行或旧工作流重跑冲突。请勿同时通过其他流水线向此应用上传，否则可能出现构建号竞争。
7. CI 使用 Ludork 打包器编译，在临时包中生成不带透明通道的 1024px AppIcon 和 Xcode 资产目录，补齐构建号、图标和加密申报后重新签名。重签名前将新的临时钥匙串加入搜索列表、确认签名身份有效，退出时恢复原搜索列表并删除临时钥匙串。原游戏图标及仓库资源不变。
8. 使用固定 `fastlane 2.240.1` 和 API 密钥上传，最多等待 45 分钟确认 Apple 处理成功并加入指定内部组。失败、缺少合规信息、处理超时都会使 job 失败；`testflight.txt` 明确记录状态，只有 `ready` 表示验收通过。故障重跑会生成更高的新构建号。

内部测试最多 100 名有应用访问权限的 App Store Connect 用户；通过 Apple 的个人邀请使用 TestFlight，**没有公开邀请码**。`testflight.txt` 保存版本、构建号、状态、管理链接和加入说明。参考：[Apple 内部测试说明](https://developer.apple.com/help/app-store-connect/test-a-beta-version/add-internal-testers/)。

IPA 仅在 runner 的临时目录生成和上传。脚本退出与 workflow 的 `always()` 都清理 IPA、p12、p8、描述文件、临时钥匙串及暂存文件；上传步骤只列出允许公开的 TXT/JSON。首次配置后正常运行无需人工点击；证书/描述文件到期、账号协议及 Apple 服务异常仍需维护。

## 验证边界

本地可执行语法、归档校验、签名输入和模拟上传状态测试，但不能替代 GitHub runner 的真实五平台编译、HOS/Apple 签名、公证和 TestFlight 处理。配置凭据后，以五个 job 的日志和实际设备运行结果验收；iOS 不能仅凭上传成功视为可安装。

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
