'use strict';

module.exports = require('./sync-upstream.cjs').createSync({
  sourceRoot: 'Game/Engine',
  targetRoot: 'Engine',
  statePath: '.github/ludork-engine-sync.json',
  excludedDirectories: ['ThirdParty'],
});
