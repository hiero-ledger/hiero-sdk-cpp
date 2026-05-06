# Release Process

Releases follow semantic versioning (`vX.Y.Z`) and are published approximately every two weeks from the `main` branch.

## Checklist

### 1. Pre-release

- [ ] Confirm all PRs targeted for this release are merged to `main`.
- [ ] Open a PR that bumps the version in [`CMakeLists.txt`](CMakeLists.txt) (line 2):
  ```cmake
  project(hiero-sdk-cpp VERSION X.Y.Z ...)
  ```
- [ ] Merge the version bump PR.

### 2. Tag the release

From the version bump merge commit on `main`:

```bash
git checkout main && git pull
git tag vX.Y.Z
git push origin vX.Y.Z
```

### 3. Build and package artifacts

The Linux build runs automatically on every merged PR. Windows and macOS builds must be triggered manually.

**Trigger Windows and macOS builds:**

1. Go to [Actions → ZXC: Build Library](https://github.com/hiero-ledger/hiero-sdk-cpp/actions/workflows/zxc-build-library.yaml).
2. Click **Run workflow**, check both **run-windows-builds** and **run-macos-builds**, and target the `main` branch at the version bump commit.
3. Wait for the run to complete.

**Download all three artifacts** from the respective workflow runs:

- `hapi-library-Linux-{shortSHA}` — from the version bump PR's automatic CI run
- `hapi-library-Windows-{shortSHA}` — from the manual workflow run above
- `hapi-library-macOS-{shortSHA}` — from the manual workflow run above

Each artifact contains a `package/` directory whose install path is structured as `{BuildType}/{OS}/{Arch}/` (e.g. `Release/Linux/x86_64/`, `Release/Windows/AMD64/`, `Release/Darwin/arm64/`). Merging all three produces a single complete tree with no conflicts.

**Merge and package:**

```bash
SHORT_SHA=$(git rev-parse --short=8 HEAD)
VERSION=X.Y.Z
MERGED=hiero-sdk-cpp-v${VERSION}-${SHORT_SHA}

mkdir -p ${MERGED}
cp -r hapi-library-Linux-${SHORT_SHA}/*  ${MERGED}/
cp -r hapi-library-Windows-${SHORT_SHA}/* ${MERGED}/
cp -r hapi-library-macOS-${SHORT_SHA}/*  ${MERGED}/

tar -czf ${MERGED}.tar.gz ${MERGED}/
sha256sum ${MERGED}.tar.gz > ${MERGED}.tar.gz.sha256
```

Artifact naming convention: `hiero-sdk-cpp-vX.Y.Z-{8-char-sha}.tar.gz`

### 4. Draft the GitHub release

1. Go to [Releases → Draft a new release](https://github.com/hiero-ledger/hiero-sdk-cpp/releases/new).
2. Choose the `vX.Y.Z` tag and set the previous tag for the comparison range.
3. Click **Generate release notes** to auto-populate.
4. **Keep only the `## New Contributors` block and the `**Full Changelog**` link** from the auto-generated text — delete everything else.
5. Write the remaining sections manually above the New Contributors block using the [SDK release notes template](https://github.com/hiero-ledger/sdk-collaboration-hub/blob/main/templates/release-notes-template.md).

> Omit any section that has no entries for this release.

### 5. Attach artifacts

Upload both files produced in step 3:

- `hiero-sdk-cpp-vX.Y.Z-{shortSHA}.tar.gz`
- `hiero-sdk-cpp-vX.Y.Z-{shortSHA}.tar.gz.sha256`

### 6. Publish

- Check **Set as the latest release**.
- Click **Publish release**.
