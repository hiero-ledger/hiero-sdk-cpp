// SPDX-License-Identifier: Apache-2.0
//
// bot-on-pr-review-labels.js
//
// Triggered by workflow_run after "Bot - On PR Review" completes.
// Downloads the recorder artifact, reconstructs context, and delegates to
// bot-on-pr-review.js to apply the correct status label.
 
const fs = require('fs');

module.exports = async ({ github, context }) => {
  const data = JSON.parse(
    fs.readFileSync('review-event.json', 'utf8')
  );
 
  if (data.draft) {
    return;
  } 

  const prNumber = data.pr_number;
  const reviewState = data.review_state;

  const { data: pr } = await github.rest.pulls.get({
    owner: context.repo.owner,
    repo: context.repo.repo,
    pull_number: prNumber,
  });
 
  context.eventName = 'pull_request_review';

  context.payload = {
    pull_request: pr,
    review: {
      state: reviewState,
    },
  };

  const bot = require('./bot-on-pr-review.js');
  await bot({ github, context });
}; 
