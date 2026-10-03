'use strict';

const { execFileSync } = require('node:child_process');
const { createSync } = require('./sync-upstream.cjs');

const projectEnums = {
  'Game/Scripts/Enums': {
    directories: ['GeneralData'],
    files: ['GeneralDataKey.lua', 'EventKey.lua'],
  },
  'Game/Scripts/stub/Enums': {
    directories: ['GeneralData'],
    files: ['GeneralDataKey.d.lua', 'EventKey.d.lua'],
  },
};

const targets = [
  { stateKey: 'engine', sourceRoot: 'Game/Engine', targetRoot: 'Engine', excludedDirectories: ['ThirdParty'] },
  { stateKey: 'global', sourceRoot: 'Game/Scripts/Global', targetRoot: 'Scripts/Global' },
  { stateKey: 'internal', sourceRoot: 'Game/Scripts/Internal', targetRoot: 'Scripts/Internal' },
  // Enums may not exist yet in the merged baseline or the project's HEAD.
  { stateKey: 'enums', sourceRoot: 'Game/Scripts/Enums', targetRoot: 'Scripts/Enums', allowMissingRoot: true },
  { stateKey: 'globalStub', sourceRoot: 'Game/Scripts/stub/Global', targetRoot: 'Scripts/stub/Global' },
  { stateKey: 'internalStub', sourceRoot: 'Game/Scripts/stub/Internal', targetRoot: 'Scripts/stub/Internal' },
].map(config => {
  const localEnums = projectEnums[config.sourceRoot];
  return {
    ...config,
    sync: createSync({
      ...config,
      excludedDirectories: [...(config.excludedDirectories ?? []), ...(localEnums?.directories ?? [])],
      excludedFiles: [...(config.excludedFiles ?? []), ...(localEnums?.files ?? [])],
      statePath: '.github/ludork-sync.json',
    }),
  };
});

function synchronize({ project, upstream, eventName, now = Date.now() }) {
  if (!['schedule', 'workflow_dispatch'].includes(eventName)) throw new Error(`Unsupported event: ${eventName}`);
  if (!Number.isFinite(now)) throw new Error('Invalid sync start time.');
  const upstreamSha = execFileSync('git', ['-C', upstream, 'rev-parse', '--verify', 'refs/heads/main^{commit}'])
    .toString().trim();
  if (eventName === 'schedule' && !targets.some(({ sync }) => sync.hasRecentChanges(upstream, upstreamSha, now))) {
    return { upstreamSha, reconcilePr: false, results: [] };
  }

  // Once any target activates the schedule, rebuild all cumulative diffs so
  // updating the shared PR cannot drop older pending changes from other targets.
  const results = targets.map(({ stateKey, targetRoot, sync }) => ({
    stateKey, targetRoot, ...sync.synchronize({ project, upstream, upstreamSha }),
  }));
  return { upstreamSha, reconcilePr: true, results };
}

async function run({ core, context }) {
  const result = synchronize({
    project: process.env.GITHUB_WORKSPACE,
    upstream: process.env.LUDORK_SYNC_REPOSITORY,
    eventName: context.eventName,
    now: Number(process.env.SYNC_STARTED_AT) * 1000,
  });
  core.setOutput('reconcile_pr', String(result.reconcilePr));
  core.setOutput('upstream_sha', result.upstreamSha);
  core.summary.addHeading('Ludork Engine, Global, Internal and Enums sync (including Lua stubs)');
  for (const target of result.results) {
    core.setOutput(`${target.stateKey}_baseline_sha`, target.baseline);
    core.summary.addHeading(target.targetRoot, 3).addTable([
      [{ data: 'Input', header: true }, { data: 'Value', header: true }],
      ['Upstream SHA', result.upstreamSha],
      ['Merged baseline', target.baseline],
      ['Changed files', String(target.changedFiles)],
      ['Decision', target.reason],
    ]);
  }
  if (!result.reconcilePr) core.summary.addRaw('No effective Engine, Global, Internal, Enums or configured Lua stub commits in the last four hours.');
  await core.summary.write();
}

module.exports = { synchronize, run };
