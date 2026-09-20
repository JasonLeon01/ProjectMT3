# Windows 每日打包

`package-windows.yml` 每日 UTC+8 16:30（UTC 08:30）运行，也可在 Actions 的 **Package Windows → Run workflow** 中选择 `main` 手动运行。GitHub 定时调度可能排队延迟；工作流需要位于默认分支 `main`。

## 启用

在本仓库 **Settings → Secrets and variables → Actions** 添加 Secret `LUDORK_ACTIONS_TOKEN`：使用能访问 `JasonLeon01/Ludork`、具有 **Actions: Read-only** 权限的 fine-grained token。本仓库历史查询使用自动提供的 `GITHUB_TOKEN`，不需要内容写权限。

## 打包和去重

- 只比较 ProjectMT3 的提交 hash。找到本工作流最近一次工作流成功、且 `Package Windows` job 实际成功的运行，以其 `head_sha` 为基准；相同则跳过，否则构建。不要随意更改该 job 的显示名称。
- 跳过、失败和取消不更新基准；首次运行或历史被删除后重新构建。去重不依赖 artifact，因此游戏包过期不会单独触发重建。查询失败直接报错。
- 需要构建时，选择 Ludork `main` 上最新成功完成的 `export-editor.yml` 运行，排除 PR。下载其 `Ludork-windows-x64-<commit>`；产物缺失、过期或下载失败时报错，不回退到旧运行。
- Ludork 只提供工具。项目的 Engine、Application 和资源直接使用本仓库选定提交，通过随包 `tools/pack_project.bat` 编译 Windows x64 Release；不升级项目，不额外启用 Lua 编译、加密或 ldpak。
- 成功上传的 `ProjectMT3-windows-x64-<项目hash>` 保留 **7 天**，包含游戏目录与 `build-info.json`。该 JSON 记录项目提交、Ludork 提交及上游运行和产物 ID。

首次启用后手动运行，确认下载、完整编译和上传成功，再下载游戏包验证启动。脚本和工作流静态检查不能替代托管构建及游戏运行验收。
