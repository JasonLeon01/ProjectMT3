'use strict';

const upstream = { owner: 'JasonLeon01', repo: 'Ludork' };
const packageJobName = 'Package Windows';
const allowedEvents = new Set(['push', 'schedule', 'workflow_dispatch']);

async function* successfulRuns(github, repository, workflow) {
  for await (const page of github.paginate.iterator(
    github.rest.actions.listWorkflowRuns,
    { ...repository, workflow_id: workflow, branch: 'main', status: 'success', per_page: 100 },
  )) {
    for (const run of page.data) {
      if (run.status === 'completed' && run.conclusion === 'success'
          && run.head_branch === 'main' && allowedEvents.has(run.event)
          && run.head_repository?.full_name === `${repository.owner}/${repository.repo}`) {
        yield run;
      }
    }
  }
}

async function checkProject({ github, context, core }) {
  if (context.ref !== 'refs/heads/main') {
    throw new Error('Run this workflow on main; packages always use the selected main commit.');
  }
  let previous = null;
  for await (const run of successfulRuns(github, context.repo, 'package-windows.yml')) {
    // A rerun must not use its own previous attempt as its baseline.
    if (run.id === context.runId) continue;
    const jobs = await github.paginate(github.rest.actions.listJobsForWorkflowRun, {
      ...context.repo, run_id: run.id, filter: 'latest', per_page: 100,
    });
    if (jobs.some(job => job.name === packageJobName && job.conclusion === 'success')) {
      previous = run;
      break;
    }
  }
  const shouldBuild = previous?.head_sha !== context.sha;
  core.setOutput('project_sha', context.sha);
  core.setOutput('should_build', String(shouldBuild));
  await core.summary.addHeading('Windows package decision').addTable([
    [{ data: 'Input', header: true }, { data: 'Value', header: true }],
    ['Project commit', context.sha],
    ['Last successful package commit', previous?.head_sha ?? 'None'],
    ['Last successful package run', previous?.html_url ?? 'None'],
    ['Decision', shouldBuild ? 'Build using latest successful Ludork tools' : 'Skip: project commit unchanged'],
  ]).write();
}

async function selectLudork({ github, core }) {
  let selected;
  for await (const run of successfulRuns(github, upstream, 'export-editor.yml')) {
    selected = run;
    break;
  }
  if (!selected) throw new Error('No successful Ludork Export Editor run on main was found.');
  const name = `Ludork-windows-x64-${selected.head_sha}`;
  const artifacts = await github.paginate(github.rest.actions.listWorkflowRunArtifacts, {
    ...upstream, run_id: selected.id, per_page: 100,
  });
  const matches = artifacts.filter(artifact => artifact.name === name);
  if (matches.length !== 1 || matches[0].expired
      || Date.parse(matches[0].expires_at) <= Date.now()) {
    throw new Error(`Latest successful Ludork run ${selected.id} has no unique, unexpired ${name} artifact. No older run will be used.`);
  }
  const artifact = matches[0];
  core.setOutput('ludork_sha', selected.head_sha);
  core.setOutput('ludork_run_id', String(selected.id));
  core.setOutput('ludork_artifact_id', String(artifact.id));
  await core.summary.addHeading('Ludork toolchain').addTable([
    [{ data: 'Input', header: true }, { data: 'Value', header: true }],
    ['Commit', selected.head_sha],
    ['Run', selected.html_url],
    ['Artifact', name],
    ['Artifact ID', String(artifact.id)],
  ]).write();
}

module.exports = { checkProject, selectLudork };
