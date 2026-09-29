'use strict';

const fs = require('node:fs');
const path = require('node:path');
const { execFileSync } = require('node:child_process');

const fourHours = 4 * 60 * 60;

function createSync({ sourceRoot, targetRoot, statePath, stateKey, excludedDirectories = [] }) {
  function git(directory, ...args) {
    return execFileSync('git', ['-C', directory, ...args], { maxBuffer: 128 * 1024 * 1024 });
  }

  function included(name) {
    return !excludedDirectories.some(directory => name === directory || name.startsWith(`${directory}/`));
  }

  function tree(directory, revision, root) {
    const entries = new Map();
    const type = git(directory, 'cat-file', '-t', `${revision}:${root}`).toString().trim();
    if (type !== 'tree') throw new Error(`Missing directory: ${revision}:${root}`);
    for (const record of git(directory, 'ls-tree', '-rz', revision, '--', `${root}/`).toString().split('\0')) {
      if (!record) continue;
      const tab = record.indexOf('\t');
      const [mode, kind, sha] = record.slice(0, tab).split(' ');
      const name = record.slice(tab + 1).slice(root.length + 1);
      if (!included(name)) continue;
      if (kind !== 'blob' || !['100644', '100755', '120000'].includes(mode)) {
        throw new Error(`Unsupported ${targetRoot} entry: ${name} (${mode}, ${kind})`);
      }
      entries.set(name, { mode, sha });
    }
    return entries;
  }

  function equal(a, b) {
    return a?.sha === b?.sha && a?.mode === b?.mode;
  }

  function changedPaths(before, after) {
    return [...new Set([...before.keys(), ...after.keys()])].filter(name => !equal(before.get(name), after.get(name)));
  }

  function hasRecentChanges(upstream, sha, now) {
    const cutoff = Math.floor(now / 1000) - fourHours;
    const end = Math.floor(now / 1000);
    // Do not use --since: commit dates need not be monotonic. Inspect the full
    // first-parent chain and compare merges against their first parent as well.
    const history = git(upstream, 'log', '--first-parent', '--format=%H %ct %P', sha).toString().trim();
    for (const line of history.split('\n')) {
      const [commit, timestamp, parent] = line.split(' ');
      if (Number(timestamp) < cutoff || Number(timestamp) > end) continue;
      const args = parent
        ? ['diff', '--name-only', '--no-renames', '-z', parent, commit]
        : ['diff-tree', '--root', '--no-commit-id', '--name-only', '-r', '-z', commit];
      const names = git(upstream, ...args, '--', `${sourceRoot}/`).toString().split('\0');
      if (names.some(name => name.startsWith(`${sourceRoot}/`) && included(name.slice(sourceRoot.length + 1)))) {
        return true;
      }
    }
    return false;
  }

  function checkedPath(project, name) {
    const parts = name.split('/');
    if (!name || parts.some(part => !part || part === '.' || part === '..' || part.includes('\\'))) {
      throw new Error(`Unsafe ${targetRoot} path: ${name}`);
    }
    let current = project;
    // Check every parent, including Scripts for the nested Scripts/Global target.
    // Never follow a project symlink while writing or deleting a synced file.
    for (const part of [...targetRoot.split('/'), ...parts.slice(0, -1)]) {
      current = path.join(current, part);
      let stat;
      try {
        stat = fs.lstatSync(current);
      } catch (error) {
        if (error.code !== 'ENOENT') throw error;
      }
      if (stat && !stat.isDirectory()) throw new Error(`Sync parent is not a directory: ${current}`);
    }
    return path.join(project, targetRoot, ...parts);
  }

  function pruneEmptyParents(project, filename) {
    const root = path.join(project, targetRoot);
    let directory = path.dirname(filename);
    while (directory !== root && fs.readdirSync(directory).length === 0) {
      fs.rmdirSync(directory);
      directory = path.dirname(directory);
    }
  }

  function synchronize({ project, upstream, upstreamSha: sha }) {
    project = path.resolve(project);
    upstream = path.resolve(upstream);
    const latest = tree(upstream, sha, sourceRoot);
    const stateFile = path.join(project, statePath);
    if (!fs.existsSync(stateFile)) throw new Error('Missing sync baseline; set upstreamSha to the Ludork commit already synced into this project.');
    const state = JSON.parse(fs.readFileSync(stateFile, 'utf8'));
    const baseline = state[stateKey]?.upstreamSha;
    if (typeof baseline !== 'string' || !/^[0-9a-f]{40}$/.test(baseline)) throw new Error('Invalid upstreamSha in sync state.');
    git(upstream, 'merge-base', '--is-ancestor', baseline, sha);
    const result = { upstreamSha: sha, baseline, changedFiles: 0, shouldUpdatePr: false };

    const local = tree(project, 'HEAD', targetRoot);
    // The update set comes exclusively from two Ludork revisions. Project-only
    // differences must never introduce candidates for synchronization.
    const before = tree(upstream, baseline, sourceRoot);
    const candidates = changedPaths(before, latest);
    // Work from the default branch on every run, never from the pending PR's state.
    const changes = candidates.filter(name => !equal(local.get(name), latest.get(name)));

    // Remove only changed tracked files, before creating any replacements. This
    // also supports file/directory transitions without recursively deleting dirs.
    for (const name of changes) {
      if (!local.has(name)) continue;
      const filename = checkedPath(project, name);
      fs.unlinkSync(filename);
      pruneEmptyParents(project, filename);
    }
    for (const name of changes) {
      const entry = latest.get(name);
      if (!entry) continue;
      const filename = checkedPath(project, name);
      fs.mkdirSync(path.dirname(filename), { recursive: true });
      const contents = git(upstream, 'cat-file', 'blob', entry.sha);
      if (entry.mode === '120000') fs.symlinkSync(contents.toString(), filename);
      else {
        fs.writeFileSync(filename, contents, { flag: 'wx', mode: entry.mode === '100755' ? 0o755 : 0o644 });
        fs.chmodSync(filename, entry.mode === '100755' ? 0o755 : 0o644);
      }
    }
    const shouldUpdatePr = changes.length > 0;
    if (shouldUpdatePr) {
      fs.mkdirSync(path.dirname(stateFile), { recursive: true });
      state[stateKey] = { ...state[stateKey], upstreamSha: sha };
      fs.writeFileSync(stateFile, `${JSON.stringify(state, null, 2)}\n`);
    }
    return {
      ...result, changedFiles: changes.length, shouldUpdatePr,
      // Run PR reconciliation even with no diff: an older pending PR may now be
      // obsolete because upstream reverted its changes or the base caught up.
      reconcilePr: true,
      reason: shouldUpdatePr ? `Upstream ${targetRoot} changes ready for PR.` : 'No effective file differences.',
    };
  }

  return { synchronize, hasRecentChanges };
}

module.exports = { createSync };
