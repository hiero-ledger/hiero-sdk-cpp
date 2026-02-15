# [Intermediate]: Refactor bot workflows to one script per GitHub event

**Labels:** `intermediate`

---

## Thanks for contributing! 😊

We truly appreciate your time and effort.

This template is here to help you create an **Intermediate-level issue** for contributors who are comfortable with the basics and ready to take on a bit more ownership in the **Hiero C++ SDK** codebase.

These issues usually involve a mix of exploration, reasoning, and implementation, while still having clear goals and boundaries.

> Maintainers can read more about:  
> [Intermediate Issues](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/maintainers/guidelines-intermediate-issues.md)

---

## 🧩 Intermediate Friendly

**Who is this issue for?**

This issue is a good fit for contributors who are already familiar with the Hiero C++ SDK repo and feel comfortable navigating the codebase, including GitHub Actions workflows and the `.github/scripts` bot code.

Intermediate Issues often involve:

- Exploring existing implementations  
- Understanding how different components work together  
- Making thoughtful changes that follow established patterns  

The goal is to support deeper problem-solving while keeping the task clear, focused, and enjoyable to work on.

> [!IMPORTANT]
> ### 🧭 About Intermediate Issues
>
> Intermediate Issues are a great next step for contributors who enjoy digging into the codebase and reasoning about how things work.
>
> These issues often:
> - Involve multiple related files or components  
> - Encourage investigation and understanding of existing behavior  
> - Leave room for thoughtful implementation choices  
> - Stay focused on a clearly defined goal  
>
> Other kinds of contributions — from beginner-friendly tasks to large system-level changes — are just as valuable and use different labels.

---

## 🐞 Problem Description

The repository’s bot automation is split across several workflows and scripts with overlapping responsibilities:

- **Bot - DCO & GPG** (`.github/workflows/bot-dco-gpg.yaml`) runs on `pull_request_target` → `synchronize` only and executes `bot-dco-gpg.js` to verify DCO sign-off and GPG on commits.
- **Bot - On PR** (`.github/workflows/bot-on-pr.yaml`) runs on `pull_request_target` → `opened`, `reopened`, `ready_for_review`. It first runs `bot-dco-gpg.js`, then `bot-on-pr.js`, which handles auto-assign, merge conflict check, and labels (needs review / needs revision).
- **Bot - Assign on Comment** (`.github/workflows/bot-assign-on-comment.yaml`) runs on `issue_comment` → `created` and executes `bot-assign-on-comment.js`.

As a result:

1. **Commit-level checks are duplicated** — DCO/GPG logic runs in both the dedicated DCO/GPG workflow (on synchronize) and inside PR automation (on open/reopen/ready_for_review). Merge conflict checking lives only in `bot-on-pr.js`, so it does not run on every new commit (synchronize).
2. **There is no single place for “what runs on each commit”** — Adding new checks that should run on every push to a PR (e.g. branch naming, file-size checks) would require touching multiple workflows and possibly duplicating logic.
3. **Event-to-script mapping is harder to follow** — Multiple workflows call into overlapping scripts, and the mental model of “one script per event type” is not explicit.

Relevant areas:

- `.github/workflows/bot-dco-gpg.yaml`
- `.github/workflows/bot-on-pr.yaml`
- `.github/workflows/bot-assign-on-comment.yaml`
- `.github/scripts/bot-dco-gpg.js`
- `.github/scripts/bot-on-pr.js`
- `.github/scripts/bot-assign-on-comment.js`
- `.github/scripts/helpers/` (logger, constants, validation, api)

---

## 💡 Expected Outcome

Refactor the bot automation so that there is **one JS script per GitHub event type**, with clear responsibilities:

| Event / trigger              | Workflow (e.g.)       | Script               | Responsibilities                                                                 |
|-----------------------------|------------------------|----------------------|-----------------------------------------------------------------------------------|
| Each push to a PR           | `on-commit.yaml`       | `bot-on-commit.js`   | DCO, GPG, merge conflict check; optionally more checks. Writes outputs for downstream steps when running in a job that has `GITHUB_OUTPUT`. |
| PR opened / reopened / ready | PR automation workflow | (1) `bot-on-commit.js` (2) `bot-on-pr.js` | Reuse commit checks; then assign author and set labels from step 1 outputs.        |
| Issue comment               | `on-comment.yaml` or existing | `bot-on-comment.js`  | `/assign` and any future comment-based behavior.                                 |

Concretely:

1. **Introduce `bot-on-commit.js`**  
   - Consolidate DCO and GPG verification (from current `bot-dco-gpg.js`) and merge conflict checking (from current `bot-on-pr.js`) into a single script.  
   - When `GITHUB_OUTPUT` is set, write at least `dco_gpg_passed` and a merge-conflict result (e.g. `merge_conflict` or equivalent) so a following step can read them.

2. **Introduce `on-commit.yaml`**  
   - Trigger on `pull_request_target` → `synchronize` only.  
   - Run `bot-on-commit.js` so that every new commit pushed to a PR runs the full set of commit-level checks (DCO, GPG, merge conflict).

3. **Refactor PR automation**  
   - Rename or repurpose the current PR automation workflow so it:  
     - Step 1: Runs `bot-on-commit.js` (same script as on-commit).  
     - Step 2: Runs `bot-on-pr.js` with `DCO_GPG_PASSED` and merge-conflict output from step 1.  
   - **`bot-on-pr.js`** should contain only PR-specific behavior: auto-assign author, compute “all passed” from the outputs of step 1, and set labels (needs review / needs revision). No DCO/GPG or merge-conflict logic inside `bot-on-pr.js`.

4. **Retire or repoint the current “Bot - DCO & GPG” workflow**  
   - Either remove `bot-dco-gpg.yaml` and rely on `on-commit.yaml` for synchronize, or make `bot-dco-gpg.yaml` a thin wrapper that runs `bot-on-commit.js` only. No duplicate DCO/GPG implementation.

5. **Comment bot**  
   - Optionally rename `bot-assign-on-comment.js` → `bot-on-comment.js` and the workflow to something like `on-comment.yaml` for consistency. Behavior (e.g. `/assign`) and any future comment handlers stay in this single script.

Existing behavior must be preserved:

- On **synchronize**: all commit checks (DCO, GPG, merge conflict) run via `bot-on-commit.js`.  
- On **opened / reopened / ready_for_review**: same commit checks run first, then PR automation (assign, labels) using those results.  
- Status checks and PR comments (e.g. DCO/GPG failure) should continue to work as they do today.  
- Local test scripts (e.g. `test-on-commit-bot.js`, `test-on-pr-bot.js`) should be updated or added so the new scripts can be tested without making real API calls.

---

## 🧠 Implementation Notes

- **Outputs:** `bot-dco-gpg.js` already writes `dco_gpg_passed` to `GITHUB_OUTPUT` when used from PR automation. `bot-on-commit.js` should do the same and add an output for merge conflict (e.g. `merge_conflict=success|failure` or similar) so `bot-on-pr.js` can read it from the environment (e.g. `MERGE_CONFLICT` set from `steps.on-commit.outputs.merge_conflict`).
- **Check runs and comments:** Preserve the current behavior of creating/updating the “DCO & GPG” check run and posting a PR comment on DCO/GPG failure. This logic should live in `bot-on-commit.js` (or shared helpers used by it), not in `bot-on-pr.js`.
- **Concurrency:** Keep per-PR (and per-issue for comment) concurrency so only one run per PR/issue is active when applicable.
- **Security:** Keep using `pull_request_target` and checking out only the default branch when running bot scripts, so contributor-controlled code is not executed with write tokens.
- **Tests:** Update `test-on-commit-bot.js` and `test-on-pr-bot.js` to exercise the new script names and behavior (e.g. `bot-on-commit.js` and `bot-on-pr.js`). Add or adjust mocks so merge-conflict and output behavior can be tested locally.
- **Naming:** Using `bot-on-commit` and `on-commit` is sufficient; alternative names (e.g. `bot-on-push`) are acceptable if they are used consistently across workflow and script.

---

## ✅ Acceptance Criteria

To help get this change merged smoothly:

- [ ] One script per event: `bot-on-commit.js`, `bot-on-pr.js`, and a single comment script (`bot-on-comment.js` or current name) with responsibilities as described above.  
- [ ] `on-commit.yaml` runs on every push to a PR (synchronize) and executes `bot-on-commit.js`.  
- [ ] PR automation runs `bot-on-commit.js` then `bot-on-pr.js`, with outputs passed from the first step to the second.  
- [ ] DCO, GPG, and merge conflict behavior are unchanged from a user perspective (same checks, same status/comment behavior).  
- [ ] No duplicate DCO/GPG or merge-conflict logic; “Bot - DCO & GPG” workflow is either removed or only calls `bot-on-commit.js`.  
- [ ] Follow existing project conventions (permissions, harden-runner, checkout default branch where required).  
- [ ] Existing or updated local test scripts run and pass for the refactored bots.  
- [ ] All CI checks pass.

---

## 📋 Contribution Guide

To help your contribution go as smoothly as possible, we recommend following these steps:

- [ ] Comment `/assign` to request the issue  
- [ ] Wait for assignment  
- [ ] Fork the repository and create a branch  
- [ ] Set up the project using the instructions in `README.md`  
- [ ] Make the requested changes  
- [ ] Sign each commit using `-s -S`  
- [ ] Push your branch and open a pull request  

Read [Workflow Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/workflow.md) for step-by-step workflow guidance.  
Read [README.md](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/README.md) for setup instructions.

❗ Pull requests **cannot be merged** without `S` and `s` signed commits.  
See the [Signing Guide](https://github.com/hiero-ledger/hiero-sdk-cpp/blob/main/docs/training/signing.md).

---

## 📚 Additional Context or Resources

- Current workflows: `.github/workflows/bot-*.yaml`  
- Current scripts: `.github/scripts/bot-*.js`, `.github/scripts/helpers/`  
- Local tests: `.github/scripts/test-on-commit-bot.js`, `.github/scripts/test-on-pr-bot.js`, `.github/scripts/test-assign-bot.js`  

If you have questions, the community is happy to help:

https://discord.com/channels/905194001349627914/1337424839761465364
