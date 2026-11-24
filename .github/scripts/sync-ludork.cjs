'use strict';

const { execFileSync } = require('node:child_process');
const { createSync } = require('./sync-upstream.cjs');

const projectEnums = {
  'Game/Scripts/Enums': {
    directories: ['GeneralData'],
    files: ['GeneralDataKey.lua', 'Animation.lua', 'Particle.lua', 'EventKey.lua'],
  },
};

const targets = [
  { stateKey: 'engine', sourceRoot: 'Game/Engine', targetRoot: 'Engine', excludedDirectories: ['ThirdParty'] },
  { stateKey: 'application', sourceRoot: 'Game/Application', targetRoot: 'Application' },
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

function synchronize({ project, upstream }) {
  const upstreamSha = execFileSync('git', ['-C', upstream, 'rev-parse', '--verify', 'refs/heads/main^{commit}'])
    .toString().trim();
  // Schedules can be delayed. Rebuild the cumulative PR from the merged
  // baselines on every run, including when upstream has reverted a pending diff.
  const results = targets.map(({ stateKey, targetRoot, sync }) => ({
    stateKey, targetRoot, ...sync.synchronize({ project, upstream, upstreamSha }),
  }));
  return { upstreamSha, results };
}

async function run({ core }) {
  const result = synchronize({
    project: process.env.GITHUB_WORKSPACE,
    upstream: process.env.LUDORK_SYNC_REPOSITORY,
  });
  core.setOutput('upstream_sha', result.upstreamSha);
  core.summary.addHeading('Ludork Engine, Application, Global, Internal and Enums sync (including Lua stubs)');
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
  await core.summary.write();
}

module.exports = { synchronize, run };
