'use strict';

const upstream = { owner: 'JasonLeon01', repo: 'Ludork' };
const allowedEvents = new Set(['push', 'schedule', 'workflow_dispatch']);
const shaPattern = /^[0-9a-f]{40}$/;

function requireSha(value) {
  if (!shaPattern.test(value ?? '')) throw new Error(`Invalid commit/tree SHA: ${value}`);
  return value;
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

async function selectArtifact(github, head, platform = 'windows-x64') {
  const jobName = { 'windows-x64': 'Windows x64', 'macos-arm64': 'macOS ARM64' }[platform];
  if (!jobName) throw new Error(`Unsupported Ludork artifact platform: ${platform}`);
  // Scheduled exports can skip platform packaging entirely. Prefer the checked HEAD, then
  // walk earlier successful runs until an actual downloadable package is found.
  const inspected = new Set();
  for (const filter of [{ head_sha: head }, {}]) {
    for await (const run of successfulRuns(github, upstream, 'export-editor.yml', filter)) {
      if ((filter.head_sha && run.head_sha !== head) || inspected.has(run.id)) continue;
      inspected.add(run.id);
      const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
        ...upstream, run_id: run.id, filter: 'latest', per_page: 100,
      });
      if (!jobs.some(job => job.name === jobName && job.conclusion === 'success')) continue;
      const name = `Ludork-${platform}-${requireSha(run.head_sha)}`;
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
  throw new Error(`No successful Ludork ${platform} run on main has a unique, unexpired package (checked HEAD ${head} and earlier runs).`);
}

module.exports = { upstream, shaPattern, requireSha, successfulRuns, selectArtifact };
