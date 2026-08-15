# Parabolic AppImage — Forking & Setup Guide

A complete, step-by-step guide to:

1. Forking the upstream `nickvisionapps/parabolic` repo to your GitHub profile
2. Adding the AppImage build assets (this folder) to your fork
3. Enabling the GitHub Actions workflow that automatically builds the AppImage
4. Triggering builds and downloading the resulting `.AppImage`

Once set up, every push to your fork's `main` branch (and every git tag) will produce a fresh `Parabolic-<version>-<arch>.AppImage` you can download from the Actions artifacts panel or from a GitHub Release.

---

## Table of contents

1. [What you'll get](#1-what-youll-get)
2. [Prerequisites](#2-prerequisites)
3. [Step 1 — Fork the upstream repo](#step-1--fork-the-upstream-repo)
4. [Step 2 — Clone your fork locally](#step-2--clone-your-fork-locally)
5. [Step 3 — Copy the AppImage assets in](#step-3--copy-the-appimage-assets-in)
6. [Step 4 — Commit & push to your fork](#step-4--commit--push-to-your-fork)
7. [Step 5 — Enable GitHub Actions on your fork](#step-5--enable-github-actions-on-your-fork)
8. [Step 6 — Trigger the first build](#step-6--trigger-the-first-build)
9. [Step 7 — Download the AppImage](#step-7--download-the-appimage)
10. [Step 8 — (Optional) Cut a release](#step-8--optional-cut-a-release)
11. [Step 9 — (Optional) Local build on your own machine](#step-9--optional-local-build-on-your-own-machine)
12. [Keeping your fork up to date (auto-sync)](#keeping-your-fork-up-to-date-auto-sync)
13. [Auto-update: in-app Yad updater](#auto-update-in-app-yad-updater)
14. [Troubleshooting](#troubleshooting)

---

## 1. What you'll get

After completing this guide you will have:

- A GitHub fork at `https://github.com/<YOUR_USERNAME>/parabolic` that:
  - Tracks upstream `nickvisionapps/parabolic`
  - Adds three new folders/files on top:
    - `packaging/appimage/` — the build script + AppRun fallback + Yad-based updater
    - `.github/workflows/appimage.yml` — the CI workflow that builds the AppImage
    - `.github/workflows/sync-upstream.yml` — daily cron that merges upstream into your fork
- A GitHub Actions workflow that runs on every push to `main`, every tag matching `v*.*.*`, and on manual dispatch
- The workflow builds the **x86_64 AppImage** (≈171 MB with deno bundled), runs a smoke test, uploads it as a workflow artifact, and (on tag push) attaches it to a GitHub Release
- A **daily auto-sync** that pulls `nickvisionapps/parabolic:main` into your fork — you never need to manually merge upstream
- A **Yad-based in-app updater** bundled inside every AppImage — users run `./Parabolic.AppImage --update` to check for and install new versions via a Yad dialog
- The resulting `Parabolic-<version>-x86_64.AppImage` runs on any Linux with glibc ≥ 2.35 (Ubuntu 22.04+, Debian 12+, Fedora 36+, Arch, openSUSE Leap 15.6+, etc.) — no Flatpak, no Snap, no system packages required.

---

## 2. Prerequisites

- A GitHub account (free)
- Git installed locally (optional, but recommended — you can also do all the steps from the GitHub web UI)
- The files in this folder: `build-appimage.sh`, `AppRun`, `appimage.yml`, `README.md`, `FORKING-GUIDE.md`
- No need for an ARM machine — GitHub hosts both x86_64 and arm64 runners (the latter is in public preview; if it's unavailable for you, you can drop the arm64 matrix entry, see [Troubleshooting](#troubleshooting)).

---

## Step 1 — Fork the upstream repo

1. Go to <https://github.com/nickvisionapps/parabolic> in your browser.
2. Make sure you are **signed in** to your GitHub account.
3. Click the **"Fork"** button in the top-right corner.

   ![Fork button location](https://docs.github.com/assets/images/help/repository/fork_button.jpg)

4. On the "Create a fork" page:
   - **Owner:** select your own username
   - **Repository name:** keep the default `parabolic` (recommended) — you can rename it, but the workflow paths in this guide assume `parabolic`
   - **Copy the `main` branch only:** ✅ check this (cleaner fork)
5. Click **"Create fork"**.

After a few seconds you'll be redirected to `https://github.com/<YOUR_USERNAME>/parabolic`. Your fork now exists.

---

## Step 2 — Clone your fork locally

Open a terminal on your machine and run:

```bash
# Replace <YOUR_USERNAME> with your actual GitHub username
git clone https://github.com/<YOUR_USERNAME>/parabolic.git
cd parabolic

# Add upstream as a remote so you can pull upstream changes later
git remote add upstream https://github.com/nickvisionapps/parabolic.git
git fetch upstream
```

Verify:

```bash
git remote -v
# origin    https://github.com/<YOUR_USERNAME>/parabolic.git (fetch)
# origin    https://github.com/<YOUR_USERNAME>/parabolic.git (push)
# upstream  https://github.com/nickvisionapps/parabolic.git (fetch)
# upstream  https://github.com/nickvisionapps/parabolic.git (push)
```

> **Tip — no local git?** You can skip steps 2-4 and instead upload files directly via the GitHub web UI ("Add file → Upload files"). But the local approach is much faster for re-runs.

---

## Step 3 — Copy the AppImage assets in

From the folder where you have these AppImage build files (`build-appimage.sh`, `AppRun`, `parabolic-updater.sh`, `appimage.yml`, `sync-upstream.yml`), copy them into your local clone:

```bash
# Inside your local clone of the fork
mkdir -p packaging/appimage
mkdir -p .github/workflows

# Adjust the source path to wherever you saved the deliverables
cp /path/to/deliverables/build-appimage.sh  packaging/appimage/
cp /path/to/deliverables/AppRun             packaging/appimage/
cp /path/to/deliverables/parabolic-updater.sh  packaging/appimage/
cp /path/to/deliverables/README.md          packaging/appimage/   # the AppImage README, not this guide
cp /path/to/deliverables/FORKING-GUIDE.md   packaging/appimage/
cp /path/to/deliverables/appimage.yml       .github/workflows/
cp /path/to/deliverables/sync-upstream.yml  .github/workflows/

chmod +x packaging/appimage/build-appimage.sh packaging/appimage/AppRun packaging/appimage/parabolic-updater.sh
```

Your local clone now looks like:

```
parabolic/
├── .github/
│   └── workflows/
│       ├── flatpak.yml        (already here from upstream)
│       ├── macos.yml          (already here from upstream)
│       ├── windows.yml        (already here from upstream)
│       ├── spelling.yml       (already here from upstream)
│       ├── appimage.yml       ← NEW — builds the AppImage
│       └── sync-upstream.yml  ← NEW — daily cron that merges upstream into your fork
└── packaging/
    └── appimage/
        ├── build-appimage.sh   ← NEW
        ├── AppRun              ← NEW
        ├── parabolic-updater.sh ← NEW — Yad-based in-app auto-updater
        ├── README.md           ← NEW
        └── FORKING-GUIDE.md   ← NEW
```

---

## Step 4 — Commit & push to your fork

```bash
git checkout -b add-appimage-build
git add .github/workflows/appimage.yml packaging/appimage/
git commit -m "Add AppImage build script and GitHub Actions workflow

Adds packaging/appimage/build-appimage.sh which clones the upstream
Parabolic source, downloads yt-dlp/ffmpeg/aria2/deno/curl-impersonate
as static binaries, builds the .NET 10 self-contained native binary,
bundles GTK4+libadwaita via linuxdeploy with the gtk plugin, and
produces a portable AppImage for any Linux (glibc >= 2.35).

Adds .github/workflows/appimage.yml which triggers on:
- push to main (paths: packaging/appimage/**)
- tag push matching v*.*.* (creates a Release with the AppImage attached)
- manual dispatch (inputs: parabolic_ref, arch)

The workflow builds x86_64 and aarch64 in parallel, smoke-tests the
extracted AppDir, and uploads the AppImage as a 30-day workflow artifact."

git push origin add-appimage-build
```

Then open a Pull Request on GitHub:

1. Visit `https://github.com/<YOUR_USERNAME>/parabolic/pulls` — GitHub will show a banner offering to open a PR for your just-pushed branch.
2. Click **"Compare & pull request"**.
3. Title it `Add AppImage build` and click **"Create pull request"**.
4. On the same PR page, click **"Merge pull request"** to land it on your `main`.

(You can also fast-forward merge from the CLI: `git checkout main && git merge --ff-only add-appimage-build && git push origin main`.)

---

## Step 5 — Enable GitHub Actions on your fork

Forks have Actions **disabled by default**. Enable them:

1. Go to `https://github.com/<YOUR_USERNAME>/parabolic/actions`
2. If you see a banner that says "Workflows aren't being run on this forked repository", click the **"I understand my workflows, go ahead and enable them"** button.
3. In the left sidebar you should now see a workflow named **"Build AppImage"**.

> **Note:** The other workflows inherited from upstream (`flatpak.yml`, `macos.yml`, `windows.yml`, `spelling.yml`) will also be visible. You can ignore them — they won't fire unless you push changes that match their trigger paths.

---

## Step 6 — Trigger the first build

You have three ways to trigger a build:

### Option A — Push to main (automatic)

Any future commit touching `packaging/appimage/**` or `.github/workflows/appimage.yml` will fire the workflow. For example:

```bash
echo "# AppImage build" >> packaging/appimage/.touch
git add packaging/appimage/.touch
git commit -m "Trigger AppImage build"
git push origin main
```

### Option B — Manual dispatch (recommended for first run)

1. Go to `https://github.com/<YOUR_USERNAME>/parabolic/actions/workflows/appimage.yml`
2. Click the **"Run workflow"** dropdown (top-right).
3. Choose:
   - **Branch:** `main`
   - **parabolic_ref:** `main` (or any tag/SHA you want to build)
   - **arch:** `x86_64` (or `aarch64`)
4. Click the green **"Run workflow"** button.

### Option C — Tag push (creates a Release)

```bash
git tag v2026.5.0-appimage
git push origin v2026.5.0-appimage
```

The workflow will run, build the x86_64 AppImage, and attach it to a GitHub Release named `v2026.5.0-appimage`.

---

## Step 7 — Download the AppImage

1. Watch the run at `https://github.com/<YOUR_USERNAME>/parabolic/actions`
2. When the build finishes (≈ 6–8 minutes), click into the run.
3. Scroll down to the **"Artifacts"** section at the bottom.
4. You'll see the artifact:
   - `Parabolic-<version>-x86_64.AppImage` (~171 MB with default knobs — deno is bundled)
5. Click it to download — it's a zip file containing the AppImage.

Extract and run:

```bash
unzip Parabolic-*.AppImage.zip
chmod +x Parabolic-*.AppImage
./Parabolic-*.AppImage
```

If your file manager doesn't let you double-click AppImages, run them from a terminal as above. On first run you may need to allow execution:

```bash
# Some distros require this for AppImages downloaded via browser
chmod +x Parabolic-*.AppImage
./Parabolic-*.AppImage --appimage-extract-and-run   # alternative if FUSE is unavailable
```

### Tuning AppImage size

The default build produces a **~171 MB** AppImage. The biggest single component is **deno** (~92 MB), which is required by yt-dlp for JS-based extractors (sites like Bilibili, Niconico, and some YouTube age-restricted videos won't download without it). You have several size/feature trade-offs:

**Option A — Skip deno** (~79 MB, ⚠ breaks yt-dlp JS extractors on some sites):
1. Go to Actions → "Build AppImage" → "Run workflow"
2. Toggle `bundle_deno` to `false`
3. Click "Run workflow"
4. ⚠ Only do this if you ONLY download from sites that don't need JS rendering (e.g., plain YouTube videos without age restrictions).

**Option B — Use bleeding-edge BtbN ffmpeg** (~271 MB):
1. Go to Actions → "Build AppImage" → "Run workflow"
2. Pick `btbn` from the `ffmpeg_source` dropdown
3. Click "Run workflow"
4. Use this only if you need a bleeding-edge ffmpeg filter not yet in the stable 7.0 release.

**Option C — Edit defaults permanently** (e.g., always skip deno):
Edit `.github/workflows/appimage.yml`'s `env:` block:
```yaml
env:
  BUNDLE_DENO: "0"           # 0 = skip deno (⚠ breaks yt-dlp JS extractors)
  SKIP_FFPLAY: "0"
  FFMPEG_SOURCE: btbn
```

The full list of knobs is documented in [`README.md`](./README.md#size-reduction-knobs).

---

## Step 8 — (Optional) Cut a release

If you want a permanent, shareable release (not just workflow artifacts):

```bash
# On your local clone
git tag v2026.5.0-appimage
git push origin v2026.5.0-appimage
```

The workflow will:

1. Build both arches in parallel
2. Run the smoke test on each
3. Create a GitHub Release named `v2026.5.0-appimage` with auto-generated release notes
4. Attach both AppImages to the release

You can then share the release URL:

```
https://github.com/<YOUR_USERNAME>/parabolic/releases/tag/v2026.5.0-appimage
```

---

## Step 9 — (Optional) Local build on your own machine

You don't need GitHub Actions — you can build the AppImage locally too.

### Prerequisites (Ubuntu 22.04+)

```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential autoconf automake autotools-dev libtool pkg-config \
  meson ninja-build gettext \
  libglib2.0-dev-bin libglib2.0-dev \
  libgtk-4-dev libadwaita-1-dev \
  libgdk-pixbuf-2.0-dev libgdk-pixbuf2.0-0 \
  desktop-file-utils \
  libssl-dev libcares-dev libxml2-dev zlib1g-dev libsqlite3-dev \
  git curl wget unzip tar

# Install .NET 10 SDK
curl -sSL https://dot.net/v1/dotnet-install.sh | bash /dev/stdin --channel 10.0
export PATH="$HOME/.dotnet:$PATH"

# Install blueprint-compiler (Ubuntu 22.04's is too old)
git clone --depth 1 --branch 0.10.0 https://gitlab.gnome.org/GNOME/blueprint-compiler.git /tmp/bpc
( cd /tmp/bpc && meson setup build --prefix=/usr && ninja -C build && sudo ninja -C build install )
```

### Build

```bash
cd packaging/appimage
chmod +x build-appimage.sh AppRun

# Default build (~171 MB — deno IS bundled, johnvansickle ffmpeg, stripped binaries, no ffplay)
./build-appimage.sh x86_64

# Skip deno (~79 MB, ⚠ breaks yt-dlp JS extractors on some sites)
BUNDLE_DENO=0 ./build-appimage.sh x86_64

# Full-feature build (matches flatpak, ~460 MB)
BUNDLE_DENO=1 SKIP_FFPLAY=0 STRIP_BINARIES=0 REMOVE_PDB=0 FFMPEG_SOURCE=btbn \
    ./build-appimage.sh x86_64

# Reuse downloaded deps for fast re-runs
SKIP_DOWNLOAD=1 ./build-appimage.sh x86_64
```

The script will:

1. Clone Parabolic source to `build/parabolic/`
2. Download all deps to `build/deps/` (cached for re-runs — pass `SKIP_DOWNLOAD=1` to reuse)
3. Run upstream `publish-and-install.sh` to produce the .NET binary
4. Bundle everything into `build/AppDir/` (using symlinks to dedupe ffmpeg/yt-dlp/aria2c/deno between `usr/bin/` and `usr/lib/`)
5. Run linuxdeploy with the GTK plugin to bundle GTK4 + libadwaita
6. Apply post-build size optimizations (strip binaries, remove .pdb files, repackage with appimagetool)
7. Output `Parabolic-<version>-<arch>.AppImage` in the current directory (~171 MB by default)

---

## Keeping your fork up to date (auto-sync)

You DON'T need to manually sync your fork with upstream — that's what `.github/workflows/sync-upstream.yml` is for. It runs **daily at 06:00 UTC** automatically once you enable Actions on your fork (Step 5).

What it does:
1. Fetches `nickvisionapps/parabolic:main`
2. Merges it into your fork's `main` (conflict-free — your fork only ADDS files, doesn't modify upstream)
3. Pushes — which triggers the AppImage build workflow

If upstream hasn't changed since the last sync, no push happens (no spurious rebuild). If a sync merge ever fails (shouldn't, but just in case), the workflow creates a GitHub issue in your fork automatically so you know to investigate.

### Disabling auto-sync

If you want to control when updates land (e.g., review each upstream release before adopting it):
1. Delete `.github/workflows/sync-upstream.yml` from your fork, OR
2. Edit it and comment out the `on: schedule:` block, keeping only `workflow_dispatch:` for manual runs

### Manual sync (if auto-sync is disabled)

```bash
# From your local clone
git fetch upstream
git checkout main
git merge --ff-only upstream/main
git push origin main
```

The push triggers a rebuild automatically.

> **Tip:** If upstream changes break the AppImage build (e.g., a new NuGet dep, a renamed csproj), the workflow will fail with a clear error in the Actions log. Open an issue in your fork and adjust `build-appimage.sh` accordingly.

---

## Auto-update: in-app Yad updater

Every AppImage built by this workflow includes a **Yad-based auto-updater** at `usr/bin/parabolic-updater`. It checks your fork's GitHub Releases for a newer version and offers to download + atomically replace the AppImage.

### How users update

```bash
# Interactive (Yad dialog asking Yes/No)
./Parabolic-*.AppImage --update

# Silent check (exit 0 = up-to-date, exit 2 = update available, exit 3 = error)
./Parabolic-*.AppImage --update --check

# Skip prompt, just download + replace
./Parabolic-*.AppImage --update --force
```

The `--update` flag is intercepted by the launcher script (`usr/bin/org.nickvision.tubeconverter`) before the .NET binary runs. The launcher routes `--update` to the bundled `parabolic-updater`.

### Host requirements

The updater uses these in order of preference (no need to install all):
1. **yad** — preferred; install with `apt install yad` / `dnf install yad` / `pacman -S yad`
2. **zenity** — fallback; pre-installed on most GNOME distros
3. **notify-send + xdg-open** — last resort; just shows a notification with the download URL

If none are installed, the updater falls back to a plain terminal prompt.

### How the updater knows where to check

At build time, the workflow injects your GitHub username into `parabolic-updater.sh` via the `GITHUB_OWNER` env var (which `${{ github.repository_owner }}` resolves to your username). So the AppImage checks:

```
https://api.github.com/repos/<YOUR_USERNAME>/parabolic/releases/latest
```

To verify:
```bash
./Parabolic-*.AppImage --appimage-extract
grep '^PARABOLIC_UPDATE_OWNER=' squashfs-root/usr/bin/parabolic-updater
# Should print: PARABOLIC_UPDATE_OWNER="<your-github-username>"
```

### End-to-end update flow

Once you've cut at least one release (Step 8), the full update cycle is:

```
upstream nickvisionapps/parabolic
        ↓  (daily cron)
sync-upstream.yml merges into your fork's main
        ↓  (push event)
appimage.yml rebuilds the AppImage
        ↓  (manual: you tag a new release)
GitHub Release created with new AppImage asset
        ↓  (user runs ./Parabolic.AppImage --update)
parabolic-updater queries GitHub Releases API
        ↓  (if newer version found)
Yad dialog → user clicks Yes → atomically replaces the AppImage
```

---

## Troubleshooting

### "Workflows aren't being run on this forked repository"

Forks disable Actions by default. Visit `https://github.com/<YOUR_USERNAME>/parabolic/actions` and click **"I understand my workflows, go ahead and enable them"**.

### The `aarch64` build fails with `Runner not found`

GitHub-hosted ARM runners were in public preview at the time of writing. If they're unavailable for your account:

1. Edit `.github/workflows/appimage.yml`
2. Remove the `aarch64` matrix entry (or comment it out)
3. Commit and push

```yaml
matrix:
  include:
    - arch: x86_64
      runner: ubuntu-22.04
      dotnet_rid: linux-x64
    # - arch: aarch64
    #   runner: ubuntu-22.04-arm
    #   dotnet_rid: linux-arm64
```

Alternatively, use a **self-hosted runner** on a Raspberry Pi or any arm64 Linux box — see [GitHub's self-hosted runner docs](https://docs.github.com/en/actions/hosting-your-own-runners).

### The build fails on `blueprint-compiler` not found

The workflow already builds blueprint-compiler from source if the system version is too old. If it still fails, ensure the apt step completed — check the workflow log for the apt-get output. You may need to add additional build deps. Edit `.github/workflows/appimage.yml`'s apt step.

### The AppImage runs but immediately crashes with `error while loading shared libraries: libgtk-4.so.1`

This means the bundled GTK4 libraries are missing. This should not happen because linuxdeploy bundles them automatically — but if you skipped linuxdeploy (e.g., a custom build), you must install GTK4 on the host:

```bash
sudo apt install libgtk-4-1 libadwaita-1-0
```

If you used the standard build path and still see this, check the build log for the `linuxdeploy --plugin gtk` step — it should print which `.so` files it bundled.

### The AppImage runs but yt-dlp is "not found"

The .NET app uses `Nickvision.Desktop.Environment.FindDependency("yt-dlp")` which searches:

1. The executing directory (where `Nickvision.Parabolic.GNOME` lives — i.e. `AppDir/usr/lib/org.nickvision.tubeconverter/`)
2. `$PREFIX/bin` (i.e. `AppDir/usr/bin/`)
3. `$PATH`

Our build script puts `yt-dlp` in both locations. If it's still missing, extract the AppImage and check:

```bash
./Parabolic-*.AppImage --appimage-extract
ls squashfs-root/usr/lib/org.nickvision.tubeconverter/yt-dlp
ls squashfs-root/usr/bin/yt-dlp
```

If either is missing, the download step failed silently. Re-run with `SKIP_DOWNLOAD=0` (or delete `build/deps/`).

### The AppImage is huge

Typical size is **~150–200 MB**. This is normal — it bundles:

- .NET 10 self-contained runtime (~80 MB)
- GTK4 + libadwaita + transitive deps (~30 MB)
- ffmpeg static build (~80 MB)
- yt-dlp standalone (~30 MB)
- deno (~92 MB)
- aria2 (~5 MB)

You can reduce size by:

- Switching from `--self-contained true -p:PublishReadyToRun=true` to `<PublishAot>true</PublishAot>` (Native AOT — smaller, but harder to debug). Edit `publish-and-install.sh` accordingly. **Not recommended** — AOT can fail to compile some .NET reflection patterns.
- Stripping deno (it's the biggest single dep). If you don't use yt-dlp's JS-runtimes deno: feature, you can skip bundling it. Edit `build-appimage.sh` and remove the `deno` block.

### The smoke test fails with "No AppImage produced"

Check the workflow log for the `Run build script` step. Common causes:

- `dotnet publish` failed (NuGet restore error, missing SDK)
- `publish-and-install.sh` exited non-zero (blueprint-compiler too old, missing glib dev tools)
- `linuxdeploy` failed to download (network blip — re-run the workflow)

The workflow uploads the full log; look for the first `ERROR` or red line.

### How do I report upstream changes that broke the build?

Open an issue in **your fork** (not upstream — the upstream doesn't support AppImage). Use this template:

```
Upstream commit: <SHA>
Build error: <paste the relevant log lines>
Likely cause: <e.g. "added a new NuGet package that needs --source">
```

The build script is meant to be self-contained and require minimal maintenance — but every time upstream adds a new dependency, the build may need a one-line tweak.

---

## Reference

- Upstream repo: <https://github.com/nickvisionapps/parabolic>
- AppImage spec: <https://appimage.org>
- linuxdeploy (GTK plugin): <https://github.com/linuxdeploy/linuxdeploy-plugin-gtk>
- .NET 10 RID catalog: <https://learn.microsoft.com/dotnet/core/rid-catalog>
- BtbN FFmpeg static builds: <https://github.com/BtbN/FFmpeg-Builds>
- yt-dlp standalone releases: <https://github.com/yt-dlp/yt-dlp/releases>
- deno releases: <https://github.com/denoland/deno/releases>
- curl-impersonate: <https://github.com/lexiforest/curl-impersonate>
- webp-pixbuf-loader: <https://github.com/aruiz/webp-pixbuf-loader>
