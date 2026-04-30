const fs = require('fs');

module.exports = async ({ github, context }) => {
  const data = JSON.parse(
    fs.readFileSync('review-event.json', 'utf8')
  );

  const prNumber = data.pr_number;
  const reviewState = data.review_state;

  const { data: pr } = await github.rest.pulls.get({
    owner: context.repo.owner,
    repo: context.repo.repo,
    pull_number: prNumber,
  });

  context.payload = {
    pull_request: pr,
    review: {
      state: reviewState,
    },
  };

  const bot = require('./bot-on-pr-review.js');
  await bot({ github, context });
};