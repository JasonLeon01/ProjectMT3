'use strict';

const upstream = { owner: 'JasonLeon01', repo: 'Ludork' };
const allowedEvents = new Set(['push', 'schedule', 'workflow_dispatch']);
const shaPattern = /^[0-9a-f]{40}$/;

function requireSha(value) {
  if (!shaPattern.test(value ?? '')) throw new Error(`Invalid commit/tree SHA: ${value}`);
  return value;
}
function isCompletedMainRun(run, repository) {
  return run.status === 'completed' && run.head_branch === 'main' && allowedEvents.has(run.event)
    && run.head_repository?.full_name === `${repository.owner}/${repository.repo}`;
}

async function selectArtifact(github, head, platform = 'windows-x64') {
  const target = {
    'windows-x64': { job: 'Windows x64', workflow: 'export-editor-windows.yml' },
    'macos-arm64': { job: 'macOS ARM64', workflow: 'export-editor-macos.yml' },
  }[platform];
  if (!target) throw new Error(`Unsupported Ludork artifact platform: ${platform}`);
  const workflows = new Set(['export-editor.yml', target.workflow].map(file => `.github/workflows/${file}`));
  // Repository-wide pagination interleaves dual- and single-platform runs newest first.
  // The target platform can be usable even when the other platform failed.
  for await (const page of github.paginate.iterator(github.rest.actions.listWorkflowRunsForRepo, {
    ...upstream, branch: 'main', status: 'completed', per_page: 100,
  })) {
    for (const run of page.data) {
      if (!isCompletedMainRun(run, upstream) || !workflows.has(run.path)) continue;
      const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
        ...upstream, run_id: run.id, filter: 'latest', per_page: 100,
      });
      if (!jobs.some(job => (job.name === target.job || job.name.endsWith(` / ${target.job}`))
          && job.conclusion === 'success')) continue;
      const name = `Ludork-editor-${platform}-${requireSha(run.head_sha)}`;
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
  throw new Error(`No Ludork Export Editor ${platform} run on main has a successful platform job and a unique, unexpired editor artifact (checked main ${head}).`);
}

module.exports = { upstream, requireSha, selectArtifact };
