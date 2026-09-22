'use strict';

const fs = require('node:fs');
const path = require('node:path');
const crypto = require('node:crypto');
const { execFileSync } = require('node:child_process');

const upstream = { owner: 'JasonLeon01', repo: 'Ludork' };
const prefix = 'projectmt3-windows-v1';
const allowedEvents = new Set(['push', 'schedule', 'workflow_dispatch']);
const buildDirectories = ['build', 'bin', 'Intermediate'];
const toolFiles = [
  'tools/pack_project.bat', 'tools/build_cpp.bat', 'tools/build_standalone.bat',
  'tools/ScriptTools/ScriptTools.exe', 'tools/luac.exe', 'tools/gnu-make/gnumake.exe',
];
const shaPattern = /^[0-9a-f]{40}$/;
const cachePattern = new RegExp(`^${prefix}-[a-z0-9-]+$`);
function temp(name) {
  const root = path.resolve(process.env.RUNNER_TEMP);
  const result = path.resolve(root, name);
  if (!result.startsWith(`${root}${path.sep}`)) throw new Error(`Path outside runner temp: ${name}`);
  return result;
}
const stateFile = () => temp('projectmt3-state/state.json');
const candidateFile = () => temp('projectmt3-candidate.json');
const manifestFile = () => temp('projectmt3-build-state/files.json');
const toolsDirectory = () => temp('ludork');
const hash = value => crypto.createHash('sha256').update(value).digest('hex');
const readJson = file => JSON.parse(fs.readFileSync(file, 'utf8').replace(/^\uFEFF/, ''));
function writeJson(file, value) {
  fs.mkdirSync(path.dirname(file), { recursive: true });
  fs.writeFileSync(file, `${JSON.stringify(value)}\n`);
}
function execute(command, args, cwd = process.env.GITHUB_WORKSPACE) {
  return execFileSync(command, args, {
    cwd, encoding: 'utf8', maxBuffer: 64 * 1024 * 1024, timeout: 600_000,
    env: { ...process.env, GIT_TERMINAL_PROMPT: '0' },
  });
}
function requireSha(value) {
  if (!shaPattern.test(value ?? '')) throw new Error(`Invalid commit/tree SHA: ${value}`);
  return value;
}
function invocation(context) {
  return `${context.runId}-${process.env.GITHUB_RUN_ATTEMPT}`;
}
function readPrevious(core) {
  try {
    const state = readJson(stateFile());
    if (state.version !== 1 || !shaPattern.test(state.project_sha)
        || !shaPattern.test(state.engine_hash) || !shaPattern.test(state.ludork_sha)
        || !/^\d+$/.test(state.ludork_run_id) || !/^\d+$/.test(state.ludork_artifact_id)
        || !cachePattern.test(state.tools_key) || !cachePattern.test(state.build_key)
        || !/^[0-9a-f]{64}$/.test(state.environment_hash)) {
      throw new Error('Invalid cache state');
    }
    return state;
  } catch (error) {
    if (error.code !== 'ENOENT') core.warning(`Ignoring unusable package state: ${error.message}`);
    return null;
  }
}

async function* successfulRuns(github, repository, workflow, extra = {}) {
  for await (const page of github.paginate.iterator(github.rest.actions.listWorkflowRuns, {
    ...repository, workflow_id: workflow, branch: 'main', status: 'success', per_page: 100, ...extra,
  })) {
    for (const run of page.data) {
      if (run.status === 'completed' && run.conclusion === 'success'
          && run.head_branch === 'main' && allowedEvents.has(run.event)
          && run.head_repository?.full_name === `${repository.owner}/${repository.repo}`) yield run;
    }
  }
}

async function checkProject({ github, context, core }) {
  if (context.ref !== 'refs/heads/main') throw new Error('Run this workflow on main.');
  core.setOutput('project_sha', requireSha(context.sha));
  // Every push/dispatch builds its selected commit, including deliberate retries.
  for await (const run of successfulRuns(github, context.repo, 'package-windows.yml')) {
    if (run.id === context.runId) continue;
    const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
      ...context.repo, run_id: run.id, filter: 'latest', per_page: 100,
    });
    if (jobs.some(job => job.name === 'Package Windows' && job.conclusion === 'success')) {
      core.setOutput('previous_state_key', `${prefix}-state-${run.id}-${run.run_attempt}`);
      core.info(`Previous successful package: ${run.html_url}`);
      return;
    }
  }
}

function restoreState({ core }) {
  const previous = readPrevious(core);
  core.setOutput('tools_key', previous?.tools_key ?? '');
}

function changedScripts(base, head, repository) {
  requireSha(base);
  requireSha(head);
  if (base === head) return [];
  // Without rename detection both old and new extensions are examined. Git tree
  // diffs have no REST compare API file-count truncation.
  return execute('git', ['-C', repository, 'diff', '--name-only', '--no-renames', '-z', base, head])
    .split('\0').filter(file => /\.(py|bat|sh)$/i.test(file));
}

async function selectArtifact(github, head) {
  // Scheduled exports can skip Windows entirely. Prefer the checked HEAD, then
  // walk earlier successful runs until an actual downloadable package is found.
  const inspected = new Set();
  for (const filter of [{ head_sha: head }, {}]) {
    for await (const run of successfulRuns(github, upstream, 'export-editor.yml', filter)) {
      if ((filter.head_sha && run.head_sha !== head) || inspected.has(run.id)) continue;
      inspected.add(run.id);
      const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
        ...upstream, run_id: run.id, filter: 'latest', per_page: 100,
      });
      if (!jobs.some(job => job.name === 'Windows x64' && job.conclusion === 'success')) continue;
      const name = `Ludork-windows-x64-${requireSha(run.head_sha)}`;
      const artifacts = await github.paginate(github.rest.actions.listWorkflowRunArtifacts, {
        ...upstream, run_id: run.id, per_page: 100,
      });
      const matches = artifacts.filter(artifact => artifact.name === name);
      if (matches.length !== 1 || matches[0].expired || !(Date.parse(matches[0].expires_at) > Date.now())) continue;
      return {
        ludork_sha: run.head_sha, ludork_run_id: String(run.id), ludork_artifact_id: String(matches[0].id),
      };
    }
  }
  throw new Error(`No successful Ludork Windows run on main has a unique, unexpired package (checked HEAD ${head} and earlier runs).`);
}

async function selectLudork({ github, core }) {
  const previous = readPrevious(core);
  const { data } = await github.rest.repos.getBranch({ ...upstream, branch: 'main' });
  const head = requireSha(data.commit.sha); // Freeze HEAD once for this run.
  const cached = process.env.TOOLS_CACHE_HIT === 'true' && previous
    && toolFiles.every(file => fs.existsSync(path.join(toolsDirectory(), file))
      && fs.statSync(path.join(toolsDirectory(), file)).isFile());
  let scripts = [];
  if (cached && previous.ludork_sha !== head) {
    const repository = temp('ludork-compare.git');
    execute('git', ['init', '--bare', repository]);
    execute('git', ['-C', repository, 'fetch', '--no-tags', '--depth=1', '--filter=blob:none',
      'https://github.com/JasonLeon01/Ludork.git', previous.ludork_sha, head]);
    scripts = changedScripts(previous.ludork_sha, head, repository);
  }
  let reuse = Boolean(cached && scripts.length === 0);
  let reason = !cached ? 'Tool cache missing or invalid'
    : scripts.length ? `Upstream scripts changed (${scripts.length} paths)` : 'No upstream script changes';
  const selected = reuse ? previous : await selectArtifact(github, head);
  if (!reuse && selected.ludork_sha !== head) {
    reason += `; no available HEAD package, selected earlier commit ${selected.ludork_sha}`;
  }
  if (!reuse && cached && selected.ludork_sha === previous.ludork_sha
      && selected.ludork_run_id === previous.ludork_run_id
      && selected.ludork_artifact_id === previous.ludork_artifact_id) {
    reuse = true;
    reason += '; selected package already cached';
  }
  if (!reuse) {
    // This fixed child of RUNNER_TEMP is never the user's project directory.
    fs.rmSync(toolsDirectory(), { recursive: true, force: true });
  }
  const candidate = {
    version: 1, project_sha: requireSha(process.env.PROJECT_SHA),
    engine_hash: requireSha(execute('git', ['rev-parse', `${process.env.PROJECT_SHA}:Engine`]).trim()),
    ludork_sha: selected.ludork_sha, ludork_run_id: selected.ludork_run_id,
    ludork_artifact_id: selected.ludork_artifact_id, ludork_checked_sha: head,
    tools_key: reuse ? previous.tools_key : `${prefix}-tools-${selected.ludork_sha}-${selected.ludork_artifact_id}`,
    tools_cache_hit: reuse, tools_reason: reason,
  };
  writeJson(candidateFile(), candidate);
  for (const field of ['ludork_sha', 'ludork_run_id', 'ludork_artifact_id', 'tools_key']) core.setOutput(field, candidate[field]);
  core.setOutput('download', String(!reuse));
  core.info(`${reason}; using Ludork ${candidate.ludork_sha}; checked main ${head}`);
}

function environmentHash() {
  const environment = {};
  for (const name of ['ImageOS', 'ImageVersion', 'VCToolsVersion', 'VCToolsInstallDir',
    'WindowsSDKVersion', 'WindowsSdkDir', 'CMAKE_GENERATOR', 'GITHUB_WORKSPACE', 'RUNNER_TEMP']) {
    if (!process.env[name]) throw new Error(`Missing compiler environment: ${name}`);
    environment[name] = process.env[name];
  }
  environment.cmake = execute('cmake', ['--version']).trim();
  environment.ninja = execute('ninja', ['--version']).trim();
  environment.compiler = execute('where.exe', ['cl.exe']).trim();
  environment.compiler_version = execute('pwsh', ['-NoProfile', '-Command',
    '(Get-Item -LiteralPath (Get-Command cl.exe).Source).VersionInfo.FileVersion']).trim();
  environment.configuration = 'Release';
  return hash(JSON.stringify(environment));
}

function buildReason(previous, candidate) {
  if (!previous) return 'No previous successful cache state';
  if (previous.engine_hash !== candidate.engine_hash) return 'Engine tree changed';
  if (previous.tools_key !== candidate.tools_key) return 'Ludork tool version changed';
  if (previous.environment_hash !== candidate.environment_hash) return 'Compiler environment or workspace changed';
  return '';
}

function selectBuild({ core, context }) {
  const candidate = readJson(candidateFile());
  const previous = readPrevious(core);
  candidate.environment_hash = environmentHash();
  candidate.build_reason = buildReason(previous, candidate);
  candidate.build_key = `${prefix}-build-${invocation(context)}`;
  writeJson(candidateFile(), candidate);
  core.setOutput('restore_key', candidate.build_reason ? '' : previous.build_key);
  core.setOutput('save_key', candidate.build_key);
  core.setOutput('state_key', `${prefix}-state-${invocation(context)}`);
}

function projectPath(relative) {
  if (typeof relative !== 'string' || !relative || relative.includes('\\')
      || relative.split('/').some(part => part === '..' || part === '' || part === '.')) {
    throw new Error(`Invalid cached relative path: ${relative}`);
  }
  const root = path.resolve(process.env.GITHUB_WORKSPACE);
  const result = path.resolve(root, relative);
  if (!result.startsWith(`${root}${path.sep}`)) throw new Error(`Path outside workspace: ${relative}`);
  return result;
}

function fileStamp(relative, digest = false) {
  const file = projectPath(relative);
  const info = fs.statSync(file, { bigint: true });
  return {
    path: relative, size: Number(info.size),
    // Windows FILETIME preserves the 100 ns resolution Ninja uses. tar may not.
    time: String(info.mtimeNs / 100n + 116444736000000000n),
    ...(digest ? { hash: hash(fs.readFileSync(file)) } : {}),
  };
}

async function prepareBuild({ core }) {
  const candidate = readJson(candidateFile());
  candidate.build_cache_hit = false;
  let timestamps = [];
  if (!candidate.build_reason && process.env.BUILD_CACHE_HIT === 'true') {
    try {
      const manifest = readJson(manifestFile());
      const previous = readPrevious(core);
      if (manifest.build_key !== previous.build_key || !Array.isArray(manifest.sources)
          || !Array.isArray(manifest.outputs) || !manifest.outputs.length) throw new Error('Invalid build manifest');
      for (const required of ['build/CMakeCache.txt', 'build/build.ninja', 'bin/Release/Main.exe']) {
        if (!fs.existsSync(projectPath(required))) throw new Error(`Missing ${required}`);
      }
      for (const entry of [...manifest.outputs, ...manifest.sources]) {
        projectPath(entry.path);
        if (!/^\d+$/.test(entry.time) || !Number.isSafeInteger(entry.size) || entry.size < 0) {
          throw new Error('Invalid file timestamp record');
        }
      }
      for (const entry of manifest.outputs) {
        if (!buildDirectories.some(dir => entry.path.startsWith(`${dir}/`))
            || fs.statSync(projectPath(entry.path)).size !== entry.size) throw new Error(`Invalid cached output: ${entry.path}`);
        timestamps.push(entry);
      }
      // Only touch files that are still tracked and byte-identical to the saved
      // input; changed/new sources keep their fresh checkout timestamps.
      const tracked = new Set(execute('git', ['ls-files', '-z']).split('\0'));
      for (const entry of manifest.sources) {
        if (tracked.has(entry.path) && fs.existsSync(projectPath(entry.path))
            && hash(fs.readFileSync(projectPath(entry.path))) === entry.hash) timestamps.push(entry);
      }
      const file = temp('projectmt3-timestamps.json');
      writeJson(file, timestamps);
      execute('pwsh', ['-NoProfile', '-File', path.join(process.env.GITHUB_WORKSPACE,
        '.github/scripts/package-windows-timestamps.ps1'), '-ManifestPath', file,
      '-ProjectDirectory', process.env.GITHUB_WORKSPACE]);
      core.info(`Restored ${timestamps.length} input/output timestamps.`);
      candidate.build_cache_hit = true;
      candidate.build_reason = 'Compatible previous build restored';
    } catch (error) {
      timestamps = [];
      candidate.build_reason = `Build cache invalid: ${error.message}`;
      core.warning(candidate.build_reason);
    }
  }
  if (!candidate.build_cache_hit) {
    candidate.build_reason ||= 'Build cache missing';
    for (const directory of buildDirectories) {
      // Resolve and validate every fixed target before any recursive deletion.
      fs.rmSync(projectPath(directory), { recursive: true, force: true });
    }
  }
  writeJson(candidateFile(), candidate);
  core.exportVariable('PACKAGE_BUILD_INFO', candidateFile());
  await core.summary.addHeading('Windows package inputs and cache decision').addTable([
    [{ data: 'Input', header: true }, { data: 'Value', header: true }],
    ['Project commit', candidate.project_sha], ['Engine tree', candidate.engine_hash],
    ['Ludork used', candidate.ludork_sha], ['Ludork main checked', candidate.ludork_checked_sha],
    ['Tools', candidate.tools_reason], ['Build', candidate.build_reason],
  ]).write();
}

function snapshotBuild({ core }) {
  const candidate = readJson(candidateFile());
  const sources = execute('git', ['ls-files', '-z']).split('\0').filter(Boolean)
    .filter(file => fs.existsSync(projectPath(file)) && fs.statSync(projectPath(file)).isFile())
    .map(file => fileStamp(file, true));
  const outputs = [];
  function visit(directory) {
    for (const entry of fs.readdirSync(projectPath(directory), { withFileTypes: true })) {
      const relative = `${directory}/${entry.name}`;
      // Only regular files need timestamps; never follow dependency symlinks.
      if (entry.isSymbolicLink()) continue;
      if (entry.isDirectory()) visit(relative);
      else if (entry.isFile()) outputs.push(fileStamp(relative));
      else throw new Error(`Unsupported build output: ${relative}`);
    }
  }
  for (const directory of buildDirectories) visit(directory);
  writeJson(manifestFile(), { build_key: candidate.build_key, sources, outputs });
  core.info(`Snapshot: ${sources.length} source files, ${outputs.length} build outputs.`);
}

function publishState() {
  // Called only after package upload and cache save steps. Cache-service misses
  // on a later restore are safe: they cause a fresh download / clean build.
  writeJson(stateFile(), readJson(candidateFile()));
}

module.exports = {
  checkProject, restoreState, selectLudork, selectBuild, prepareBuild, snapshotBuild, publishState,
  changedScripts, selectArtifact, buildReason,
};
