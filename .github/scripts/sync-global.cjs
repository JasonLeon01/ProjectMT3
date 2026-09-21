'use strict';

module.exports = require('./sync-upstream.cjs').createSync({
  sourceRoot: 'Game/Scripts/Global',
  targetRoot: 'Scripts/Global',
  statePath: '.github/ludork-global-sync.json',
});
