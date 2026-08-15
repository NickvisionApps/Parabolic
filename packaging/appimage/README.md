# Parabolic AppImage Packaging

This folder contains everything needed to build a portable **AppImage** of [Nickvision Parabolic](https://github.com/nickvisionapps/parabolic) — a .NET 10 / GTK4 / libadwaita GUI frontend for `yt-dlp`.

The AppImage is a single executable file that runs on **any Linux distribution with glibc ≥ 2.35** (Ubuntu 22.04+, Debian 12+, Fedora 36+, Arch, openSUSE Leap 15.6+, etc.) without needing Flatpak, Snap, or system packages.

**Default size: ~171 MB** (deno bundled, johnvansickle ffmpeg, stripped binaries) — see the [Size-reduction knobs](#size-reduction-knobs) section below for how to tune this.

> ⚠️ **deno is bundled by default** because yt-dlp requires it for sites that need
> JavaScript rendering (Bilibili, Niconico, some YouTube age-restricted videos, etc.).
> Skipping deno saves ~92 MB but breaks yt-dlp's JS-based extractors.

## Files

| File | Purpose |
|------|---------|
| [`build-appimage.sh`](./build-appimage.sh) | The main build script. Clones Parabolic, downloads all runtime deps, builds the .NET binary, bundles GTK4+libadwaita via linuxdeploy, and produces `Parabolic-<version>-<arch>.AppImage`. |
| [`AppRun`](./AppRun) | Fallback entry-point script for the AppImage. Normally overwritten by `linuxdeploy`'s gtk plugin, which generates a better AppRun that sets `GDK_PIXBUF_MODULE_FILE`, `GSETTINGS_SCHEMA_DIR`, `GI_TYPELIB_PATH`, `GTK_PATH`, etc. automatically. |
| [`parabolic-updater.sh`](./parabolic-updater.sh) | Yad-based auto-updater. Bundled inside the AppImage at `usr/bin/parabolic-updater`. Checks GitHub Releases for a newer version; if found, shows a Yad dialog and atomically replaces the AppImage on user confirmation. |
| [`FORKING-GUIDE.md`](./FORKING-GUIDE.md) | Step-by-step guide to fork the upstream repo, add these files, enable GitHub Actions, and download the resulting AppImage. **Read this first if you're new here.** |
| [`README.md`](./README.md) | This file. |

The GitHub Actions workflows live one level up at [`../../.github/workflows/`](../../.github/workflows/):
- [`appimage.yml`](../../.github/workflows/appimage.yml) — builds the AppImage
- [`sync-upstream.yml`](../../.github/workflows/sync-upstream.yml) — daily cron that merges `nickvisionapps/parabolic:main` into your fork

## Auto-update pipeline

This setup is designed so that **once configured, your AppImage stays in sync with upstream Parabolic automatically**. The pipeline has 3 stages:

### Stage 1 — Upstream sync (daily)

`.github/workflows/sync-upstream.yml` runs daily at 06:00 UTC. It:
1. Fetches `nickvisionapps/parabolic:main`
2. Merges it into your fork's `main` (conflict-free, since the fork only ADDS files)
3. Pushes — which triggers Stage 2

If upstream hasn't changed, no push happens, no rebuild is triggered. To disable auto-sync, just delete `sync-upstream.yml` from your fork.

### Stage 2 — AppImage build (on sync push)

`.github/workflows/appimage.yml` fires automatically when Stage 1 pushes new commits to `main`. It builds a fresh AppImage and uploads it as a workflow artifact (30-day retention). On tag push (`v*.*.*`), it also attaches the AppImage to a GitHub Release.

### Stage 3 — In-app Yad updater (on user demand)

Bundled inside every AppImage is [`parabolic-updater.sh`](./parabolic-updater.sh) — a Yad-based updater that:

1. Reads the current version from `$APPDIR/usr/share/parabolic-version.txt`
2. Queries `https://api.github.com/repos/<your-username>/parabolic/releases/latest`
3. Compares versions
4. If newer, shows a Yad dialog (with zenity + notify-send fallbacks):
   ```
   ┌─────────────────────────────────────────┐
   │  Update available                        │
   │                                          │
   │  A new version of Parabolic is available!│
   │                                          │
   │  Current: 2026.5.0                       │
   │  Latest:   2026.7.4                      │
   │                                          │
   │  Download and install now?               │
   │                                          │
   │            [Yes]    [No]                 │
   └─────────────────────────────────────────┘
   ```
5. On "Yes", downloads the new AppImage and atomically replaces the old one (`mv` over `$APPIMAGE`)

The GitHub owner/repo is baked into the updater at build time via the `GITHUB_OWNER` env var (set automatically to `${{ github.repository_owner }}` in CI — i.e., YOUR username, the fork owner). So the AppImage checks YOUR fork's releases, not upstream's.

### How users check for updates

Users have two ways:

```bash
# 1. CLI flag (the recommended way)
./Parabolic.AppImage --update              # interactive Yad dialog
./Parabolic.AppImage --update --check      # silent, exit 2 if update available
./Parabolic.AppImage --update --force      # skip prompt, just download + replace
```

The `--update` flag is intercepted by the launcher script (`usr/bin/org.nickvision.tubeconverter`) and routed to the bundled `parabolic-updater` instead of launching Parabolic.

```bash
# 2. Desktop entry (auto-installed at $XDG_DATA_HOME/applications/)
# "Parabolic (Check for Updates)" appears in app launchers
```

The desktop entry is `NoDisplay=true` by default (so it doesn't clutter the app menu) — users access it via `./Parabolic.AppImage --update` from a terminal. To make it visible in the app menu, edit `org.nickvision.tubeconverter.update.desktop` and change `NoDisplay=true` to `NoDisplay=false`.

### Update dependencies (host system)

The Yad updater uses these in order of preference:
1. **yad** — preferred; install with `apt install yad` / `dnf install yad` / `pacman -S yad`
2. **zenity** — fallback; pre-installed on most GNOME distros
3. **notify-send + xdg-open** — last resort; just shows a notification with the download URL

If none are available, the updater falls back to a plain terminal prompt.

### Setting up auto-update (one-time)

After forking and pushing the AppImage files (see FORKING-GUIDE.md):

1. **Cut a release** with `git tag v2026.5.0-appimage && git push origin v2026.5.0-appimage`
2. The workflow builds the AppImage and attaches it to a GitHub Release
3. Download the AppImage, `chmod +x`, and run it
4. From now on, run `./Parabolic.AppImage --update` whenever you want to check for updates
5. The daily sync workflow keeps your fork's `main` up-to-date with upstream; tag pushes produce new releases; users update via `--update`

To verify the updater is configured correctly, extract the AppImage and check:
```bash
./Parabolic.AppImage --appimage-extract
grep '^PARABOLIC_UPDATE_OWNER=' squashfs-root/usr/bin/parabolic-updater
# Should show: PARABOLIC_UPDATE_OWNER="<your-github-username>"
```

## Quick start

```bash
# 1. Build locally on Ubuntu 22.04+ (or any distro with the deps)
chmod +x build-appimage.sh AppRun
./build-appimage.sh              # host arch (defaults: ~171 MB output with deno)
./build-appimage.sh x86_64        # explicit arch

# 2. Optionally rebuild WITHOUT deno (~79 MB, breaks yt-dlp JS extractors):
BUNDLE_DENO=0 ./build-appimage.sh x86_64
```

The resulting `Parabolic-<version>-<arch>.AppImage` is dropped into the current directory. See [`FORKING-GUIDE.md`](./FORKING-GUIDE.md) for the GitHub Actions path.

## Size-reduction knobs

The build script exposes 6 environment variables that control how aggressively the AppImage is shrunk. Defaults are tuned to **include everything yt-dlp needs** (deno is ON by default) while still avoiding bloat Parabolic doesn't use (ffplay stripped, debug symbols removed, stable ffmpeg instead of dev build).

| Env var | Default | Effect when changed |
|---------|---------|---------------------|
| `BUNDLE_DENO` | `1` | `1` = bundle deno (~92 MB). **Required by yt-dlp** for JS-based extractors (Bilibili, Niconico, etc.). Set to `0` only if you're sure you only download from sites that don't need JS rendering. |
| `STRIP_BINARIES` | `1` | `1` = strip debug symbols from all bundled ELF binaries (~30 MB savings). Set to `0` for debuggable builds. |
| `REMOVE_PDB` | `1` | `1` = delete .pdb / .dbg / .xml debug files from .NET publish dir (~10 MB savings). Set to `0` to keep them. |
| `SKIP_FFPLAY` | `1` | `1` = don't bundle ffplay (~10 MB savings). Parabolic only uses ffmpeg + ffprobe, never ffplay. |
| `SHRINK_ICU` | `0` | `0` = keep ICU (safe). `1` = remove libicudata.so (~30 MB savings) — **breaks culture-aware formatting for non-English locales**. Don't enable unless you only use English. |
| `FFMPEG_SOURCE` | `johnvansickle` | `johnvansickle` = stable ffmpeg 7.0 release (~40 MB total for ffmpeg + ffprobe). `btbn` = master-latest dev build (~140 MB, has extra filters). |

### Size cheat-sheet

| Configuration | Approximate size |
|---------------|------------------|
| **All defaults (RECOMMENDED — deno bundled)** | **~171 MB** |
| Skip deno (`BUNDLE_DENO=0`) — ⚠ breaks yt-dlp JS extractors | ~79 MB |
| + `FFMPEG_SOURCE=btbn` (instead of johnvansickle) | +100 MB |
| + `SHRINK_ICU=1` (risky) | -30 MB |
| Full-feature flatpak-equivalent (`BUNDLE_DENO=1 STRIP=0 REMOVE_PDB=0 SKIP_FFPLAY=0 FFMPEG_SOURCE=btbn`) | ~460 MB |

### How to override knobs

**Locally** (Ubuntu 22.04+):
```bash
BUNDLE_DENO=1 SKIP_FFPLAY=0 ./build-appimage.sh x86_64
```

**In GitHub Actions** (manual dispatch):
1. Go to your fork's Actions tab → "Build AppImage" workflow → "Run workflow"
2. Toggle `bundle_deno` (default: true) and pick `ffmpeg_source` from the dropdown

**In GitHub Actions** (on push / tag):
Edit the `env:` block at the top of `.github/workflows/appimage.yml`:
```yaml
env:
  BUNDLE_DENO: "0"           # 0 = skip deno (WARNING: breaks yt-dlp JS extractors)
  FFMPEG_SOURCE: btbn          # btbn = master-latest
  SHRINK_ICU: "0"             # 1 = remove ICU (risky)
```

## How the AppImage works

The build pipeline mirrors the official Flatpak manifest but produces a single-file AppImage instead of a Flatpak runtime. Concretely:

```
┌──────────────────────────────────────────────────────────────────────────┐
│  1. Clone Parabolic source (nickvisionapps/parabolic) @ main            │
│                                                                          │
│  2. Download bundled dependencies                                       │
│     • yt-dlp_linux          (standalone PyInstaller binary, ~30 MB)     │
│     • ffmpeg + ffprobe      (johnvansickle static build, ~40 MB total)  │
│     • aria2c                (existing system binary or build source)    │
│     • deno                  (DEFAULT — yt-dlp requires it for JS extractors, ~92 MB) │
│     • curl-impersonate      (lexiforest prebuilt tarball)                │
│     • webp-pixbuf-loader    (meson build from source)                   │
│                                                                          │
│  3. Run upstream publish-and-install.sh                                 │
│     • dotnet publish -c Release --runtime linux-x64 --self-contained     │
│     • Copies publish dir  → AppDir/usr/lib/org.nickvision.tubeconverter │
│     • Generates desktop file, D-Bus service, launcher script            │
│     • Copies icons, metainfo, gresource                                  │
│                                                                          │
│  4. Bundle runtime deps in usr/bin/ (real files)                       │
│     Symlink them into usr/lib/.../  (saves ~280 MB vs duplicating)      │
│                                                                          │
│  5. Generate gdk-pixbuf loaders.cache with relative paths              │
│                                                                          │
│  6. Patch desktop file (Exec=launcher, drop DBusActivatable)            │
│     Patch launcher script to use $APPDIR-relative paths                 │
│                                                                          │
│  7. Pre-copy libgtk-4.so.1, libadwaita-1.so.0 + transitive deps         │
│     (GirCore uses dlopen(), so linuxdeploy's ldd analysis misses them)   │
│                                                                          │
│  8. Run linuxdeploy with --plugin gtk                                    │
│     • Bundles remaining transitive deps                                  │
│     • Compiles GSettings schemas                                         │
│     • Regenerates gdk-pixbuf loaders.cache with relative paths           │
│     • Generates AppRun that sets all GTK env vars                       │
│     • Outputs Parabolic-<version>-<arch>.AppImage                       │
│                                                                          │
│  9. Post-build size optimization (if STRIP_BINARIES=1 or REMOVE_PDB=1):  │
│     • Extract the AppImage                                               │
│     • Strip all ELF binaries linuxdeploy didn't strip                    │
│     • Remove .pdb / .dbg / .xml / .json debug files                     │
│     • Repackage with appimagetool (~10 MB savings)                       │
└──────────────────────────────────────────────────────────────────────────┘
```

### AppDir layout

```
AppDir/
├── AppRun                                     ← generated by linuxdeploy gtk plugin
├── usr/
│   ├── bin/
│   │   ├── org.nickvision.tubeconverter       ← launcher shell script
│   │   ├── yt-dlp                             ← real binary (39 MB)
│   │   ├── ffmpeg                             ← real binary (77 MB, johnvansickle)
│   │   ├── ffprobe                            ← real binary (76 MB, johnvansickle)
│   │   ├── aria2c                             ← real binary
│   │   ├── deno                               ← DEFAULT — yt-dlp requires it
│   │   └── curl-impersonate-chrome, etc.
│   ├── lib/
│   │   ├── org.nickvision.tubeconverter/      ← main application directory
│   │   │   ├── Nickvision.Parabolic.GNOME     ← .NET 10 self-contained binary (17 MB)
│   │   │   ├── org.nickvision.tubeconverter.gresource
│   │   │   ├── plugins/                        ← yt-dlp plugins (srt_fix)
│   │   │   ├── <lang>/parabolic.mo             ← translations (47 languages)
│   │   │   ├── yt-dlp → ../../bin/yt-dlp      ← symlink (saves ~39 MB)
│   │   │   ├── ffmpeg → ../../bin/ffmpeg      ← symlink (saves ~77 MB)
│   │   │   ├── ffprobe → ../../bin/ffprobe    ← symlink (saves ~76 MB)
│   │   │   ├── aria2c → ../../bin/aria2c      ← symlink
│   │   │   └── gdk-pixbuf-2.0/2.10.0/loaders/
│   │   │       ├── libwebp_pixbuf_loader.so
│   │   │       └── loaders.cache              ← relative-path cache
│   │   ├── libgtk-4.so.1, libadwaita-1.so.0   ← bundled by linuxdeploy
│   │   └── <other transitive deps>
│   └── share/
│       ├── applications/org.nickvision.tubeconverter.desktop
│       ├── icons/hicolor/{scalable,symbolic}/apps/*.svg
│       ├── metainfo/org.nickvision.tubeconverter.metainfo.xml
│       └── glib-2.0/schemas/gschemas.compiled  ← compiled by linuxdeploy
└── .DirIcon
```

### Runtime resolution

Parabolic uses `Nickvision.Desktop.Environment.FindDependency(name)` to locate `yt-dlp`, `ffmpeg`, `aria2c`, `deno`, etc. The lookup order is:

1. The executing directory (where `Nickvision.Parabolic.GNOME` lives) — these are now symlinks to `usr/bin/`
2. `$PREFIX/bin` (where the real binaries live)
3. `$PATH`

Because `FindDependency()` follows symlinks transparently, the symlink approach works seamlessly — the app sees the binaries in its own dir AND in `$PATH`.

The .NET binary also detects `DeploymentMode.Local` when run from a non-system-installed prefix (which is always the case inside an AppImage). When `Local`, the app skips the bundled yt-dlp version check (treats it as `0.0.0`) — this is exactly what we want for an AppImage.

## Compatibility matrix

| Distro family | Min version | glibc | Status |
|---------------|-------------|-------|--------|
| Ubuntu        | 22.04 (jammy) | 2.35 | ✅ primary build env |
| Debian        | 12 (bookworm) | 2.36 | ✅ |
| Fedora        | 36 | 2.35 | ✅ |
| Arch / Manjaro | rolling | recent | ✅ |
| openSUSE Leap | 15.6 | 2.31¹ | ⚠️ glibc is older — may need to bundle libc |
| openSUSE Tumbleweed | rolling | recent | ✅ |
| CentOS Stream | 9 | 2.34 | ⚠️ untested |
| Alpine Linux  | any | musl | ❌ .NET runtime needs glibc |

¹ openSUSE Leap 15.6 ships glibc 2.31, but Leap 16 (in development) bumps to 2.38.

## Build variants

The build script respects these environment variables:

| Env var | Default | Effect |
|---------|---------|--------|
| `PARABOLIC_REF` | `main` | Git ref of Parabolic to build (branch / tag / SHA) |
| `PARABOLIC_REPO` | `https://github.com/nickvisionapps/parabolic.git` | Source repo (use a fork for testing) |
| `SKIP_DOWNLOAD` | `0` | `1` = reuse `build/deps/` cache (faster re-runs) |
| `APPIMAGE_EXTRACT_AND_RUN` | unset | `1` = extract AppImage to /tmp before running (used by CI) |
| `BUNDLE_DENO` | `1` | `0` = skip deno (~92 MB savings, ⚠ breaks yt-dlp JS extractors) |
| `STRIP_BINARIES` | `1` | `0` = preserve debug symbols |
| `REMOVE_PDB` | `1` | `0` = keep .pdb / .dbg / .xml files |
| `SKIP_FFPLAY` | `1` | `0` = include ffplay (~10 MB larger) |
| `SHRINK_ICU` | `0` | `1` = remove libicudata.so (risky) |
| `FFMPEG_SOURCE` | `johnvansickle` | `btbn` = master-latest (~100 MB larger) |

## Why AppImage instead of Flatpak?

Parabolic upstream officially distributes via Flathub. AppImage is useful when:

- You can't or don't want to install `flatpak` on your system
- You're on a distro without Flatpak support (e.g. some immutable OSes, NixOS without Flatpak support)
- You want a single-file portable binary you can drop on a USB stick
- You want to run Parabolic in a sandbox without depending on the Flatpak runtime being installed

The trade-offs:

| | Flatpak | AppImage (defaults) | AppImage (full-feature) |
|--|---------|----------|-----------|
| Sandboxing | ✅ built-in (bubblewrap) | ❌ none | ❌ none |
| Auto-updates | ✅ via Flathub | ❌ manual re-download | ❌ manual |
| Disk usage | ✅ runtime shared with other apps | ~171 MB | ~460 MB |
| Install location | `/var/lib/flatpak` | anywhere | anywhere |
| First-run setup | install flatpak + add flathub remote | just `chmod +x` | just `chmod +x` |
| deno (JS extractors) | ✅ included | ✅ included | ✅ included |

## License

Parabolic is MIT-licensed (see [LICENSE](https://github.com/nickvisionapps/parabolic/blob/main/LICENSE)). The build scripts in this folder are also MIT-licensed — feel free to adapt them for other Nickvision apps (Tube Converter → Aura → Denaro → etc.).

Bundled dependencies retain their original licenses:

- yt-dlp: Unlicense
- ffmpeg (johnvansickle static): GPL (with all GPL codecs like x264/x265 included)
- aria2: GPL-2+
- deno (if bundled): MIT
- curl-impersonate: MIT
- webp-pixbuf-loader: LGPL-2.1+

Because the AppImage bundles GPL ffmpeg, the resulting AppImage is **GPL-licensed as a whole** (the Parabolic code inside remains MIT, but the combined work is GPL). This is fine for personal use and redistribution.

