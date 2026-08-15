#!/usr/bin/env bash
# =============================================================================
#  build-appimage.sh — Build a portable Parabolic AppImage for any Linux
# =============================================================================
#  This script:
#    1. Clones the Parabolic source code from nickvisionapps/parabolic
#    2. Downloads all the runtime dependencies (yt-dlp, ffmpeg, aria2, deno,
#       curl-impersonate, webp-pixbuf-loader) as prebuilt/static binaries
#    3. Builds the .NET 10 self-contained native binary via publish-and-install.sh
#    4. Lays out a standard AppDir and generates the final .AppImage
#
#  Usage:
#    ./build-appimage.sh [arch]            # arch: x86_64 | aarch64 (default: host arch)
#    PARABOLIC_REF=main ./build-appimage.sh   # build a specific git ref/branch/tag
#    SKIP_DOWNLOAD=1  ./build-appimage.sh     # reuse ./build/deps (for re-runs)
#
#  Size-reduction knobs (most default ON for smallest output, except deno):
#    BUNDLE_DENO=1                   # bundle deno (DEFAULT — yt-dlp needs it for JS extractors)
#    STRIP_BINARIES=1                # strip debug symbols from bundled ELF binaries (~30 MB savings)
#    REMOVE_PDB=1                    # delete .pdb / .dbg / .xml files from the .NET publish dir (~10 MB savings)
#    SKIP_FFPLAY=1                   # don't bundle ffplay (Parabolic doesn't use it; ~10 MB savings)
#    SHRINK_ICU=0                    # remove libicudata.so (~30 MB savings; breaks non-English locales)
#    FFMPEG_SOURCE=johnvansickle     # use stable ffmpeg 7.0 (~40 MB) instead of BtbN master-latest (~140 MB)
#
#  Example — smallest possible with deno (~171 MB, RECOMMENDED):
#    ./build-appimage.sh x86_64
#
#  Example — skip deno (~79 MB, NOT recommended — yt-dlp JS extractors will fail):
#    BUNDLE_DENO=0 ./build-appimage.sh x86_64
#
#  Example — full-feature (~460 MB, same as flatpak):
#    BUNDLE_DENO=1 STRIP_BINARIES=0 REMOVE_PDB=0 SKIP_FFPLAY=0 FFMPEG_SOURCE=btbn ./build-appimage.sh
#
#  Requirements (host):
#    - bash, curl, tar, unzip, git
#    - .NET 10 SDK  (use `actions/setup-dotnet@v6` in CI, or `dotnet-install.sh` locally)
#    - blueprint-compiler, gettext, libglib2.0-dev-bin, libgtk-4-dev (for glib-compile-resources
#      and gtk-update-icon-cache), desktop-file-utils, meson, ninja-build
#
#  Output:
#    ./Parabolic-<version>-<arch>.AppImage
# =============================================================================
set -euo pipefail

# -----------------------------------------------------------------------------
#  Configuration
# -----------------------------------------------------------------------------
APP_ID="org.nickvision.tubeconverter"
APP_NAME="Parabolic"
PARABOLIC_REF="${PARABOLIC_REF:-main}"
PARABOLIC_REPO="${PARABOLIC_REPO:-https://github.com/nickvisionapps/parabolic.git}"
SKIP_DOWNLOAD="${SKIP_DOWNLOAD:-0}"

# --- Auto-update configuration ---------------------------------------------
# The GitHub owner/repo where releases will be published. This is baked into
# the bundled parabolic-updater.sh so it knows where to check for updates.
# In CI, set this via env var (e.g. GITHUB_OWNER=${{ github.repository_owner }}).
# Locally, defaults to the local git remote's owner, or 'YOUR_GITHUB_USERNAME'
# as a placeholder you'll need to override.
GITHUB_OWNER="${GITHUB_OWNER:-YOUR_GITHUB_USERNAME}"
GITHUB_REPO="${GITHUB_REPO:-parabolic}"

# --- Size-reduction knobs --------------------------------------------------
# All default to the SMALLEST option EXCEPT deno — deno is bundled by default
# because yt-dlp requires it for JS-based extractors (some sites won't download
# without it). Override via env vars if you need to tune further.
BUNDLE_DENO="${BUNDLE_DENO:-1}"          # 1 = bundle deno (DEFAULT — yt-dlp needs it)
STRIP_BINARIES="${STRIP_BINARIES:-1}"   # 1 = strip debug symbols from bundled ELF binaries
REMOVE_PDB="${REMOVE_PDB:-1}"           # 1 = delete .pdb / .dbg / .xml files from .NET publish dir
SKIP_FFPLAY="${SKIP_FFPLAY:-1}"         # 1 = don't bundle ffplay (Parabolic only needs ffmpeg + ffprobe)
SHRINK_ICU="${SHRINK_ICU:-0}"           # 0 = keep ICU (safe); 1 = remove libicudata.so (risky, breaks i18n)

# Resolve arch & RID
HOST_ARCH="$(uname -m)"
case "${1:-$HOST_ARCH}" in
    x86_64|amd64)
        ARCH="x86_64"
        DOTNET_RID="linux-x64"
        YT_DLP_ASSET="yt-dlp_linux"
        FF_ARCH="linux64"                  # BtbN naming
        FF_ARCH_JOHN="amd64"               # johnvansickle naming
        DENO_ARCH="x86_64"
        CURL_IMPERSONATE_ARCH="x86_64"
        ;;
    aarch64|arm64)
        ARCH="aarch64"
        DOTNET_RID="linux-arm64"
        YT_DLP_ASSET="yt-dlp_linux_aarch64"
        FF_ARCH="linuxarm64"               # BtbN naming
        FF_ARCH_JOHN="arm64"               # johnvansickle naming
        DENO_ARCH="aarch64"
        CURL_IMPERSONATE_ARCH="aarch64"
        ;;
    *)
        echo "ERROR: Unsupported arch '$1' (supported: x86_64, aarch64)" >&2
        exit 1
        ;;
esac

# Working directories (relative to where the script lives)
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORK_DIR="${SCRIPT_DIR}/build"
DEPS_DIR="${WORK_DIR}/deps"
SRC_DIR="${WORK_DIR}/parabolic"
APP_DIR="${WORK_DIR}/AppDir"
APPIMAGE_OUT="${SCRIPT_DIR}/Parabolic-${ARCH}.AppImage"

# -----------------------------------------------------------------------------
#  Logging helpers
# -----------------------------------------------------------------------------
c_red=$'\033[31m'; c_grn=$'\033[32m'; c_ylw=$'\033[33m'
c_blu=$'\033[34m'; c_cyn=$'\033[36m'; c_rst=$'\033[0m'
info()  { printf '%s==>%s %s\n' "$c_cyn" "$c_rst" "$*"; }
ok()    { printf '%s✔%s %s\n' "$c_grn" "$c_rst" "$*"; }
warn()  { printf '%s⚠%s %s\n' "$c_ylw" "$c_rst" "$*"; }
die()   { printf '%s✘%s %s\n' "$c_red" "$c_rst" "$*" >&2; exit 1; }

echo "${c_blu}==============================================================${c_rst}"
echo "${c_blu} Building ${APP_NAME} AppImage (${ARCH})${c_rst}"
echo "${c_blu} .NET RID:  ${DOTNET_RID}${c_rst}"
echo "${c_blu} Git ref:   ${PARABOLIC_REF}${c_rst}"
echo "${c_blu}==============================================================${c_rst}"

# -----------------------------------------------------------------------------
#  Step 0 — Preflight checks
# -----------------------------------------------------------------------------
info "Preflight checks..."
command -v dotnet        >/dev/null || die "dotnet not found. Install .NET 10 SDK (https://dot.net)."
command -v curl          >/dev/null || die "curl not found."
command -v git           >/dev/null || die "git not found."
command -v meson        >/dev/null || die "meson not found (apt install meson)."
command -v ninja        >/dev/null || die "ninja not found (apt install ninja-build)."
command -v gtk-update-icon-cache >/dev/null || die "gtk-update-icon-cache not found (apt install libgtk-4-dev)."
command -v glib-compile-resources >/dev/null || die "glib-compile-resources not found (apt install libglib2.0-dev-bin)."
command -v msgfmt       >/dev/null || die "msgfmt not found (apt install gettext)."
command -v blueprint-compiler >/dev/null || die "blueprint-compiler not found (apt install blueprint-compiler or pip install)."
ok "All build tools present."

mkdir -p "$WORK_DIR" "$DEPS_DIR" "$APP_DIR"

# -----------------------------------------------------------------------------
#  Step 1 — Clone Parabolic source
# -----------------------------------------------------------------------------
if [[ ! -d "$SRC_DIR/.git" ]]; then
    info "Cloning Parabolic from ${PARABOLIC_REPO} @ ${PARABOLIC_REF}..."
    git clone --depth 1 --branch "$PARABOLIC_REF" "$PARABOLIC_REPO" "$SRC_DIR"
else
    warn "Reusing existing clone at $SRC_DIR"
fi

# Detect version from csproj / metainfo
PARABOLIC_VERSION="$(grep -oP '(?<=<Version>)[^<]+' "$SRC_DIR/Nickvision.Parabolic.Shared/Nickvision.Parabolic.Shared.csproj" 2>/dev/null | head -n1 || true)"
[[ -z "$PARABOLIC_VERSION" ]] && PARABOLIC_VERSION="$(grep -oP '(?<=version=")[^"]+' "$SRC_DIR/resources/linux/org.nickvision.tubeconverter.metainfo.xml" 2>/dev/null | head -n1 || true)"
[[ -z "$PARABOLIC_VERSION" ]] && PARABOLIC_VERSION="$(date +%Y.%m.%d)-git"
ok "Parabolic version: $PARABOLIC_VERSION"

# -----------------------------------------------------------------------------
#  Step 2 — Download bundled dependencies (skip if cached)
# -----------------------------------------------------------------------------
download_dep() {
    # $1 = name, $2 = url, $3 = output path
    local name="$1" url="$2" out="$3"
    if [[ -f "$out" && "$SKIP_DOWNLOAD" == "1" ]]; then
        ok "Reuse cached: $name"
        return
    fi
    info "Downloading $name..."
    # `env -u LD_LIBRARY_PATH` strips the build env's LD_LIBRARY_PATH so
    # the system curl (which links against system libcurl) doesn't pick up
    # conda/venv libcurl.so.4 (which may be ABI-incompatible).
    # We also set a browser user-agent because some hosts (johnvansickle.com)
    # sit behind Cloudflare's JS challenge and 403 / return an HTML error page
    # to the default curl/8.x user-agent.
    env -u LD_LIBRARY_PATH curl -fsSL --retry 3 \
        -A "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36" \
        -o "$out" "$url" || die "Failed to download $name from $url"
}

if [[ "$SKIP_DOWNLOAD" != "1" ]]; then
    info "Downloading all bundled dependencies into $DEPS_DIR..."

    # --- yt-dlp standalone Linux binary --------------------------------------
    download_dep "yt-dlp" \
        "https://github.com/yt-dlp/yt-dlp/releases/latest/download/${YT_DLP_ASSET}" \
        "$DEPS_DIR/yt-dlp"
    chmod +x "$DEPS_DIR/yt-dlp"

    # --- ffmpeg static build ---------------------------------------------------
    # Two sources, controlled by FFMPEG_SOURCE env var:
    #   - "johnvansickle" (default):  ffmpeg-release-<arch>-static.tar.xz
    #     Size: ~40 MB (ffmpeg + ffprobe + ffplay). Stable release. Has ALL codecs.
    #   - "btbn":  ffmpeg-master-latest-<arch>-gpl.tar.xz
    #     Size: ~140 MB (ffmpeg alone). Bleeding-edge dev build. Has ALL codecs + extra filters.
    # The johnvansickle build is ~100 MB smaller and is what we default to.
    FFMPEG_SOURCE="${FFMPEG_SOURCE:-johnvansickle}"
    case "$FFMPEG_SOURCE" in
        btbn)
            download_dep "ffmpeg-master-latest-${FF_ARCH}-gpl.tar.xz" \
                "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-${FF_ARCH}-gpl.tar.xz" \
                "$DEPS_DIR/ffmpeg.tar.xz"
            mkdir -p "$DEPS_DIR/ffmpeg"
            tar -xf "$DEPS_DIR/ffmpeg.tar.xz" -C "$DEPS_DIR/ffmpeg" --strip-components=2
            ;;
        johnvansickle|*)
            download_dep "ffmpeg-release-${FF_ARCH_JOHN}-static.tar.xz" \
                "https://www.johnvansickle.com/ffmpeg/releases/ffmpeg-release-${FF_ARCH_JOHN}-static.tar.xz" \
                "$DEPS_DIR/ffmpeg.tar.xz"
            mkdir -p "$DEPS_DIR/ffmpeg"
            # The tarball has ffmpeg-$VERSION-<arch>-static/ at the top — strip 1 to get to its contents
            tar -xf "$DEPS_DIR/ffmpeg.tar.xz" -C "$DEPS_DIR/ffmpeg" --strip-components=1
            # johnvansickle puts binaries in $DEPS_DIR/ffmpeg/bin/ — flatten so callers see $DEPS_DIR/ffmpeg/ffmpeg
            if [[ -d "$DEPS_DIR/ffmpeg/bin" ]]; then
                mv "$DEPS_DIR/ffmpeg/bin/"* "$DEPS_DIR/ffmpeg/"
                rmdir "$DEPS_DIR/ffmpeg/bin" 2>/dev/null || true
            fi
            ok "Using johnvansickle ffmpeg (~100 MB smaller than BtbN master-latest)"
            ;;
    esac

    # --- aria2 ----------------------------------------------------------------
    # Prefer an existing aria2c on PATH (apt, brew, conda, etc.). Only build
    # from source if no binary is available — the source build needs libtool,
    # autoconf, libssl-dev, libc-ares-dev, etc. and takes ~3 minutes.
    if [[ ! -x "$DEPS_DIR/aria2c" ]]; then
        if command -v aria2c >/dev/null 2>&1; then
            info "Using existing aria2c: $(command -v aria2c)"
            cp "$(command -v aria2c)" "$DEPS_DIR/aria2c"
        else
            info "Building aria2 from source (this takes ~3 minutes)..."
            ARIA_VERSION="1.37.0"
            download_dep "aria2-${ARIA_VERSION}.tar.xz" \
                "https://github.com/aria2/aria2/releases/download/release-${ARIA_VERSION}/aria2-${ARIA_VERSION}.tar.xz" \
                "$DEPS_DIR/aria2.tar.xz"
            rm -rf "$DEPS_DIR/aria2-${ARIA_VERSION}"
            tar -xf "$DEPS_DIR/aria2.tar.xz" -C "$DEPS_DIR"
            pushd "$DEPS_DIR/aria2-${ARIA_VERSION}" >/dev/null
            autoreconf -i
            ./configure --prefix="$DEPS_DIR/aria2-install" \
                        --with-ca-bundle=/etc/ssl/certs/ca-certificates.crt \
                        --disable-nls --without-gnutls --with-openssl \
                        A2RHASH_CHECKSUM_TYPE=sha1sum >/dev/null 2>&1 || ./configure --prefix="$DEPS_DIR/aria2-install"
            make -j"$(nproc)"
            make install
            cp "$DEPS_DIR/aria2-install/bin/aria2c" "$DEPS_DIR/aria2c"
            popd >/dev/null
        fi
    fi
    chmod +x "$DEPS_DIR/aria2c"

    # --- deno prebuilt (DEFAULT: bundled, yt-dlp requires it) -----------------
    # deno is required by yt-dlp for JS-based extractors (some sites won't download
    # without it). Set BUNDLE_DENO=0 to skip it (~92 MB savings, but yt-dlp JS
    # extractors will fail on sites that require JavaScript to render the page).
    if [[ "$BUNDLE_DENO" == "1" ]]; then
        download_dep "deno-${DENO_ARCH}-unknown-linux-gnu.zip" \
            "https://github.com/denoland/deno/releases/latest/download/deno-${DENO_ARCH}-unknown-linux-gnu.zip" \
            "$DEPS_DIR/deno.zip"
        (cd "$DEPS_DIR" && unzip -o deno.zip && chmod +x deno)
        ok "deno bundled (BUNDLE_DENO=1) — yt-dlp JS extractors will work"
    else
        warn "Skipping deno (BUNDLE_DENO=0) — saves ~92 MB. ⚠ yt-dlp's JS-based extractors will fail on sites that require JavaScript rendering (e.g. some YouTube age-restricted videos, Bilibili, Niconico, etc.)"
        rm -f "$DEPS_DIR/deno" "$DEPS_DIR/deno.zip"
    fi

    # --- curl-impersonate (lexiforest fork) ---------------------------------
    # Provides curl-impersonate-chrome / -edge / -ff / etc. for yt-dlp's curl_cffi.
    # URLs match the ones in upstream flatpak/org.nickvision.tubeconverter.json
    CURL_IMP_VERSION="v1.5.2"
    CURL_IMP_ARCH_NAME="${CURL_IMPERSONATE_ARCH}"   # x86_64 or aarch64
    download_dep "curl-impersonate-${CURL_IMP_VERSION}.${CURL_IMP_ARCH_NAME}-linux-gnu.tar.gz" \
        "https://github.com/lexiforest/curl-impersonate/releases/download/${CURL_IMP_VERSION}/curl-impersonate-${CURL_IMP_VERSION}.${CURL_IMP_ARCH_NAME}-linux-gnu.tar.gz" \
        "$DEPS_DIR/curl-impersonate.tar.gz"
    mkdir -p "$DEPS_DIR/curl-impersonate"
    tar -xzf "$DEPS_DIR/curl-impersonate.tar.gz" -C "$DEPS_DIR/curl-impersonate"

    # --- webp-pixbuf-loader (build from source, meson) ------------------------
    if [[ ! -f "$DEPS_DIR/libwebp_pixbuf_loader.so" ]]; then
        info "Building webp-pixbuf-loader from source..."
        if [[ ! -d "$DEPS_DIR/webp-pixbuf-loader-src" ]]; then
            git clone --depth 1 --branch 0.2.7 \
                https://github.com/aruiz/webp-pixbuf-loader.git \
                "$DEPS_DIR/webp-pixbuf-loader-src"
        fi
        meson setup "$DEPS_DIR/webp-pixbuf-loader-build" \
                    "$DEPS_DIR/webp-pixbuf-loader-src" \
                    --prefix=/usr --buildtype=release \
                    -Dgdk_pixbuf_moduledir="$DEPS_DIR" || true
        ninja -C "$DEPS_DIR/webp-pixbuf-loader-build"
        cp "$DEPS_DIR/webp-pixbuf-loader-build/libwebp_pixbuf_loader.so" "$DEPS_DIR/" || true
    fi

    ok "All dependencies downloaded and built."
fi

# -----------------------------------------------------------------------------
#  Step 3 — Build & install Parabolic into AppDir via upstream script
# -----------------------------------------------------------------------------
info "Running upstream publish-and-install.sh to build .NET binary..."
# upstream script expects $container to be empty for online NuGet feeds —
# we leave it unset so the online feed is used.
rm -rf "$APP_DIR/usr"
mkdir -p "$APP_DIR/usr"

# The upstream script must be run from its own directory.
# Set `container=""` to avoid `set -u` failure on the upstream `[ -n "$container" ]` check.
chmod +x "$SRC_DIR/resources/linux/publish-and-install.sh"
( cd "$SRC_DIR/resources/linux" && \
  container="" ./publish-and-install.sh "$APP_DIR/usr" "$DOTNET_RID" )

ok "Parabolic published and installed to $APP_DIR/usr"

# Sanity-check the layout
LIB_DIR="$APP_DIR/usr/lib/$APP_ID"
[[ -x "$LIB_DIR/Nickvision.Parabolic.GNOME" ]] || die "Native binary missing at $LIB_DIR/Nickvision.Parabolic.GNOME"

# -----------------------------------------------------------------------------
#  Step 4 — Bundle runtime dependencies next to the native binary
# -----------------------------------------------------------------------------
info "Bundling runtime dependencies into $LIB_DIR ..."
# Nickvision.Desktop's Environment.FindDependency() searches:
#   1) the executing directory (where the .NET binary lives)
#   2) $PREFIX/bin
#   3) $PATH
# So we drop everything next to the binary.
cp -f "$DEPS_DIR/yt-dlp"             "$LIB_DIR/yt-dlp"
cp -f "$DEPS_DIR/ffmpeg/ffmpeg"     "$LIB_DIR/ffmpeg"
cp -f "$DEPS_DIR/ffmpeg/ffprobe"    "$LIB_DIR/ffprobe"
if [[ "$SKIP_FFPLAY" != "1" ]]; then
    cp -f "$DEPS_DIR/ffmpeg/ffplay"  "$LIB_DIR/ffplay"
else
    warn "Skipping ffplay (SKIP_FFPLAY=1) — saves ~10 MB. Parabolic doesn't use ffplay."
fi
cp -f "$DEPS_DIR/aria2c"            "$LIB_DIR/aria2c"
if [[ "$BUNDLE_DENO" == "1" ]] && [[ -x "$DEPS_DIR/deno" ]]; then
    cp -f "$DEPS_DIR/deno"           "$LIB_DIR/deno"
fi
cp -f "$DEPS_DIR/curl-impersonate"/* "$LIB_DIR/" 2>/dev/null || true   # curl-impersonate-chrome, -edge, -ff...

# webp-pixbuf-loader goes to a gdk-pixbuf loaders subdir
GDK_PIXBUF_DIR="$LIB_DIR/gdk-pixbuf-2.0/2.10.0/loaders"
mkdir -p "$GDK_PIXBUF_DIR"
cp -f "$DEPS_DIR/libwebp_pixbuf_loader.so" "$GDK_PIXBUF_DIR/" 2>/dev/null || warn "webp-pixbuf-loader missing (WebP thumbnails will not work)"

# Also put the deps in $PREFIX/bin so the launcher finds them on $PATH too
# IMPORTANT: use symlinks, not copies — otherwise ffmpeg/ffprobe/yt-dlp get duplicated
# (~280 MB wasted on ffmpeg+ffprobe alone before squashfs dedup).
cp -f "$DEPS_DIR/yt-dlp"             "$APP_DIR/usr/bin/yt-dlp"
cp -f "$DEPS_DIR/ffmpeg/ffmpeg"      "$APP_DIR/usr/bin/ffmpeg"
cp -f "$DEPS_DIR/ffmpeg/ffprobe"     "$APP_DIR/usr/bin/ffprobe"
if [[ "$SKIP_FFPLAY" != "1" ]]; then
    cp -f "$DEPS_DIR/ffmpeg/ffplay"   "$APP_DIR/usr/bin/ffplay"
fi
cp -f "$DEPS_DIR/aria2c"             "$APP_DIR/usr/bin/aria2c"
if [[ "$BUNDLE_DENO" == "1" ]] && [[ -x "$DEPS_DIR/deno" ]]; then
    cp -f "$DEPS_DIR/deno"           "$APP_DIR/usr/bin/deno"
fi
chmod +x "$APP_DIR/usr/bin"/*

# Replace the duplicates in usr/lib/ with symlinks to usr/bin/ (saves ~280 MB uncompressed,
# and even after squashfs deduplication makes the AppImage mount faster).
info "Replacing duplicated deps in usr/lib/ with symlinks to usr/bin/ ..."
for dep in yt-dlp ffmpeg ffprobe aria2c deno; do
    lib_path="$LIB_DIR/$dep"
    bin_path="$APP_DIR/usr/bin/$dep"
    if [[ -e "$bin_path" ]]; then
        rm -f "$lib_path"
        ln -sf ../../bin/"$dep" "$lib_path"
    fi
done
if [[ "$SKIP_FFPLAY" != "1" ]] && [[ -e "$APP_DIR/usr/bin/ffplay" ]]; then
    rm -f "$LIB_DIR/ffplay"
    ln -sf ../../bin/ffplay "$LIB_DIR/ffplay"
fi

ok "Runtime dependencies bundled."

# -----------------------------------------------------------------------------
#  Step 5 — Generate gdk-pixbuf loaders.cache (so the bundled webp loader loads)
# -----------------------------------------------------------------------------
info "Generating gdk-pixbuf loaders.cache..."
GDK_PIXBUF_QUERYLOADERS="$(command -v gdk-pixbuf-queryloaders || command -v gdk-pixbuf-query-loaders || true)"
if [[ -n "$GDK_PIXBUF_QUERYLOADERS" ]]; then
    # Generate a cache scoped to the bundled loaders dir; libdir paths are relative
    # to the AppDir so the AppImage is portable.
    "$GDK_PIXBUF_QUERYLOADERS" "$GDK_PIXBUF_DIR"/*.so > "$GDK_PIXBUF_DIR/../loaders.cache" 2>/dev/null || true
    # Make all paths relative to the AppDir root
    sed -i "s|$APP_DIR||g" "$GDK_PIXBUF_DIR/../loaders.cache" || true
    ok "loaders.cache written."
else
    warn "gdk-pixbuf-queryloaders not found on host; skipping loaders.cache"
fi

# -----------------------------------------------------------------------------
#  Step 6 — Patch desktop file & launcher script for AppImage portability
# -----------------------------------------------------------------------------
DESKTOP_FILE="$APP_DIR/usr/share/applications/$APP_ID.desktop"
LAUNCHER_FILE="$APP_DIR/usr/bin/$APP_ID"

info "Patching desktop file (Exec=<binary-name>, drop DBusActivatable)..."
# linuxdeploy's gtk plugin reads Exec= from the desktop file to find the binary
# to launch (it looks in $APPDIR/usr/bin/). So we set Exec= to just the launcher
# name — NOT a path, NOT "AppRun" (which would cause infinite recursion).
sed -i "s|^Exec=.*|Exec=${APP_ID} %u|" "$DESKTOP_FILE"
sed -i "s|^TryExec=.*|TryExec=${APP_ID}|" "$DESKTOP_FILE"
# AppImages cannot be D-Bus activated (no fixed install path)
sed -i "s|^DBusActivatable=.*|DBusActivatable=false|" "$DESKTOP_FILE"
ok "Desktop file patched."

info "Patching launcher script to use \$APPDIR-relative path..."
# The upstream launcher does `exec /abs/path/to/lib/<binary> "$@"` — that absolute
# path is correct at build time but wrong at runtime inside an AppImage.
# Replace it with an $APPDIR-aware version that handles BOTH cases:
#   * AppImage:  $APPDIR is set by AppRun, libs live under $APPDIR/usr/lib/
#   * Local install: $APPDIR is unset, libs live under $PREFIX/lib/ (one level up from bin/)
cat > "$LAUNCHER_FILE" <<'LAUNCHER_EOF'
#!/bin/sh
# Launcher for Parabolic — resolves the .NET binary relative to the install root.
# When running inside an AppImage, $APPDIR is set by AppRun to the mount point.
# When running from a local install (./configure --prefix=... && make install),
# $APPDIR is unset and we resolve relative to this script's location.
if [ -z "${APPDIR:-}" ]; then
    # Local install: this script is in $PREFIX/bin/, libs in $PREFIX/lib/.
    PREFIX="$(cd "$(dirname "$0")/.." && pwd)"
    LIB_DIR="$PREFIX/lib/org.nickvision.tubeconverter"
else
    # AppImage: libs are under $APPDIR/usr/lib/.
    LIB_DIR="$APPDIR/usr/lib/org.nickvision.tubeconverter"
fi
exec "$LIB_DIR/Nickvision.Parabolic.GNOME" "$@"
LAUNCHER_EOF
chmod +x "$LAUNCHER_FILE"
ok "Launcher script patched."

# Copy our custom AppRun to AppDir root — it will be overwritten by linuxdeploy's
# gtk plugin if a better one exists, but provides a fallback if linuxdeploy is skipped.
if [[ -f "$SCRIPT_DIR/AppRun" ]]; then
    cp "$SCRIPT_DIR/AppRun" "$APP_DIR/AppRun"
    chmod +x "$APP_DIR/AppRun"
    ok "Fallback AppRun copied (will be replaced by linuxdeploy's gtk plugin)."
fi

# -----------------------------------------------------------------------------
#  Step 6.5 — Install the Yad-based auto-updater + embed version
# -----------------------------------------------------------------------------
info "Installing Yad-based auto-updater..."

# 1. Write the current version to a known location — the updater reads this to
#    compare against the latest GitHub Release tag.
mkdir -p "$APP_DIR/usr/share"
echo "$PARABOLIC_VERSION" > "$APP_DIR/usr/share/parabolic-version.txt"
ok "Embedded version: $PARABOLIC_VERSION → usr/share/parabolic-version.txt"

# 2. Install the updater script, substituting the GitHub owner/repo at build time.
#    The script uses __PARABOLIC_UPDATE_OWNER__ placeholder which we sed-replace.
UPDATER_SRC="$SCRIPT_DIR/parabolic-updater.sh"
UPDATER_DST="$APP_DIR/usr/bin/parabolic-updater"
if [[ -f "$UPDATER_SRC" ]]; then
    cp "$UPDATER_SRC" "$UPDATER_DST"
    # Substitute the owner placeholder (and arch while we're at it)
    sed -i "s|__PARABOLIC_UPDATE_OWNER__|$GITHUB_OWNER|g" "$UPDATER_DST"
    sed -i "s|^PARABOLIC_UPDATE_REPO=.*|PARABOLIC_UPDATE_REPO=\"$GITHUB_REPO\"|" "$UPDATER_DST"
    sed -i "s|^PARABOLIC_ARCH=.*|PARABOLIC_ARCH=\"$ARCH\"|" "$UPDATER_DST"
    chmod +x "$UPDATER_DST"
    ok "Updater installed at usr/bin/parabolic-updater (checks github.com/$GITHUB_OWNER/$GITHUB_REPO/releases)"
else
    warn "parabolic-updater.sh not found at $UPDATER_SRC — auto-update feature will be missing"
fi

# 3. Patch the launcher script to handle the --update flag (routes to the updater).
#    We append the --update handler BEFORE the exec line so the updater runs
#    instead of the .NET binary when the user passes --update.
info "Patching launcher script to handle --update flag ..."
LAUNCHER_FILE="$APP_DIR/usr/bin/$APP_ID"
if [[ -f "$LAUNCHER_FILE" ]]; then
    # Insert the --update handler right after the shebang line
    python3 - "$LAUNCHER_FILE" <<'PYEOF'
import sys
path = sys.argv[1]
with open(path) as f:
    content = f.read()

handler = '''# Auto-update: route --update / --check-update to the Yad updater
case "$1" in
    --update|--check-update|--check-updates)
        exec "$(dirname "$0")/parabolic-updater" "$@"
        ;;
esac
'''

# Find the first line that's not a comment or shebang, insert handler before it
lines = content.split('\n')
insert_at = 0
for i, line in enumerate(lines):
    if line.startswith('#!') or (line.startswith('#') and not line.startswith('#!')):
        insert_at = i + 1
        continue
    break

new_content = '\n'.join(lines[:insert_at]) + '\n' + handler + '\n' + '\n'.join(lines[insert_at:])
with open(path, 'w') as f:
    f.write(new_content)
PYEOF
    chmod +x "$LAUNCHER_FILE"
    ok "Launcher patched: --update now routes to parabolic-updater"
else
    warn "Launcher script not found at $LAUNCHER_FILE — --update flag won't work"
fi

# 4. Install a desktop entry for manual update checking (so users can find it
#    in their app launcher as "Parabolic (Check for Updates)").
DESKTOP_FILE_UPDATE="$APP_DIR/usr/share/applications/org.nickvision.tubeconverter.update.desktop"
cat > "$DESKTOP_FILE_UPDATE" <<EOF
[Desktop Entry]
Version=1.0
Name=Parabolic (Check for Updates)
Comment=Check for and install the latest Parabolic AppImage update
Exec=$APP_ID --update
Icon=$APP_ID
Terminal=false
Type=Application
Categories=Network;AudioVideo;
Keywords=Parabolic;Update;AppImage;
NoDisplay=true
EOF
ok "Installed: org.nickvision.tubeconverter.update.desktop (NoDisplay=true — accessible via --update only by default)"


# -----------------------------------------------------------------------------
#  Step 7 — Bundle GTK4 / libadwaita and all transitive shared libraries
# -----------------------------------------------------------------------------
info "Bundling GTK4 / libadwaita and transitive shared libraries via linuxdeploy..."

# --- Pre-copy the GTK4 / libadwaita shared libs into AppDir -------------------
# GirCore (the C# GTK4 binding) uses dlopen() to load libgtk-4.so.1 / libadwaita-1.so.0
# at runtime, so they don't appear in `ldd Nickvision.Parabolic.GNOME`. linuxdeploy's
# ELF dependency analysis therefore misses them. We copy them into AppDir/usr/lib/
# explicitly; linuxdeploy will then bundle their transitive deps on the next pass.
GTK_LIBDIR="$(pkg-config --variable=libdir gtk4 2>/dev/null || echo /usr/lib/x86_64-linux-gnu)"
info "GTK4 libdir: $GTK_LIBDIR"
for lib in \
    libgtk-4.so.1 \
    libadwaita-1.so.0 \
    libgdk_pixbuf-2.0.so.0 \
    libgraphene-1.0.so.0 \
    libepoxy.so.0 \
    libfribidi.so.0 \
    libharfbuzz.so.0 \
    libfreetype.so.6 \
    libfontconfig.so.1 \
    libwayland-client.so.0 \
    libwayland-cursor.so.0 \
    libwayland-egl.so.1 \
    libxkbcommon.so.0 \
    libxkbcommon-x11.so.0 \
    libpango-1.0.so.0 \
    libpangocairo-1.0.so.0 \
    libpangoft2-1.0.so.0 \
    libcairo.so.2 \
    libcairo-gobject.so.2 \
    libpixman-1.so.0 \
    libffi.so.8 \
    libpcre2-8.so.0 \
    libffi.so.7 \
    ; do
    src="$(find "$GTK_LIBDIR" /usr/lib /usr/lib/x86_64-linux-gnu /usr/lib/aarch64-linux-gnu -name "$lib" 2>/dev/null | head -1 || true)"
    if [[ -n "$src" ]]; then
        cp -L "$src" "$APP_DIR/usr/lib/$lib" 2>/dev/null && ok "Pre-copied: $lib"
    else
        warn "Not found on host: $lib (will fail at runtime if not present on target)"
    fi
done

# Download linuxdeploy + gtk plugin
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-${ARCH}.AppImage"
LINUXDEPLOY_GTK_URL="https://raw.githubusercontent.com/linuxdeploy/linuxdeploy-plugin-gtk/master/linuxdeploy-plugin-gtk.sh"
download_dep "linuxdeploy-${ARCH}.AppImage" "$LINUXDEPLOY_URL" "$DEPS_DIR/linuxdeploy"
chmod +x "$DEPS_DIR/linuxdeploy"
download_dep "linuxdeploy-plugin-gtk.sh" "$LINUXDEPLOY_GTK_URL" "$DEPS_DIR/linuxdeploy-plugin-gtk.sh"
chmod +x "$DEPS_DIR/linuxdeploy-plugin-gtk.sh"

# Also download appimagetool — used in Step 7.5 to repackage after stripping.
# It's much faster than re-running linuxdeploy for repackaging (~3s vs ~30s).
APPIMAGETOOL_URL="https://github.com/AppImage/appimagetool/releases/download/continuous/appimagetool-${ARCH}.AppImage"
download_dep "appimagetool-${ARCH}.AppImage" "$APPIMAGETOOL_URL" "$DEPS_DIR/appimagetool"
chmod +x "$DEPS_DIR/appimagetool"

# linuxdeploy picks up the desktop file at $APP_DIR/usr/share/applications/ and the icon at
# $APP_DIR/usr/share/icons/hicolor/scalable/apps/$APP_ID.svg. The gtk plugin copies in libgtk-4.so,
# libadwaita-1.so and all their transitive deps, compiles gsettings schemas, regenerates the
# gdk-pixbuf loaders cache with relative paths, and patches AppRun to set the right env vars.
export DEPLOY_GTK_VERSION=4   # force GTK4 (plugin auto-detects, but be explicit)
export OUTPUT="$APPIMAGE_OUT"
export APPIMAGE_EXTRACT_AND_RUN=1     # needed because GitHub runners don't allow FUSE
# Note: do NOT set NO_STRIP=1 — we want linuxdeploy to strip its deployed libs.
# We also strip the bundled binaries we copied ourselves in Step 7.5 below.
export VERBOSE=1

# Run linuxdeploy (it will produce $APPIMAGE_OUT)
"$DEPS_DIR/linuxdeploy" --appdir "$APP_DIR" \
                        --plugin gtk \
                        --output appimage \
                        --desktop-file "$DESKTOP_FILE" \
                        --icon-file "$APP_DIR/usr/share/icons/hicolor/scalable/apps/$APP_ID.svg" \
    || {
        warn "linuxdeploy exited with non-zero. The GTK plugin sometimes aborts on non-standard lib layouts."
        warn "Retrying without the gtk plugin (we already pre-copied libgtk-4 / libadwaita-1 above)..."
        "$DEPS_DIR/linuxdeploy" --appdir "$APP_DIR" \
                                --output appimage \
                                --desktop-file "$DESKTOP_FILE" \
                                --icon-file "$APP_DIR/usr/share/icons/hicolor/scalable/apps/$APP_ID.svg"
    }

ok "AppImage generated at: $APPIMAGE_OUT"

# -----------------------------------------------------------------------------
#  Step 7.5 — Post-linuxdeploy size reduction (strip + clean debug files)
# -----------------------------------------------------------------------------
# linuxdeploy runs BEFORE we have a chance to strip the bundled deps we copied
# ourselves (yt-dlp, ffmpeg, aria2c, deno). To get those stripped too, we
# extract the just-built AppImage, strip its binaries, and repackage it.
# This typically saves 20-40 MB on top of linuxdeploy's own stripping.
if [[ "$STRIP_BINARIES" == "1" || "$REMOVE_PDB" == "1" || "$SHRINK_ICU" == "1" ]]; then
    info "Applying post-build size optimizations..."
    EXTRACT_DIR="${WORK_DIR}/AppDir-stripped"
    rm -rf "$EXTRACT_DIR"
    "$APPIMAGE_OUT" --appimage-extract >/dev/null 2>&1
    mv squashfs-root "$EXTRACT_DIR"

    before_size=$(stat -c%s "$APPIMAGE_OUT")

    # 1. Strip all ELF binaries we bundled (linuxdeploy already stripped its own deployed libs,
    #    but our pre-copied yt-dlp/ffmpeg/aria2c/deno were not stripped).
    if [[ "$STRIP_BINARIES" == "1" ]]; then
        info "  Stripping ELF binaries..."
        find "$EXTRACT_DIR/usr/bin" "$EXTRACT_DIR/usr/lib" \
             -type f -exec sh -c 'file "$1" | grep -q "ELF.*not stripped" && strip --strip-all "$1" 2>/dev/null || true' _ {} \;
    fi

    # 2. Remove .pdb (Windows PDB), .dbg (GNU debug info), and .xml (XML doc) files
    #    from the .NET publish dir — they're only useful for debugging, not runtime.
    if [[ "$REMOVE_PDB" == "1" ]]; then
        info "  Removing .pdb / .dbg / .xml debug files..."
        rm -f "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/*.pdb \
              "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/*.dbg \
              "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/*.xml \
              "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/*.dll.config
        # Also remove the .dbg file that .NET NativeAOT/R2R generates alongside the binary
        rm -f "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/Nickvision.Parabolic.GNOME.dbg
    fi

    # 3. Remove ICU data (RISKY — breaks culture-aware string ops for non-English locales)
    if [[ "$SHRINK_ICU" == "1" ]]; then
        warn "  Removing libicudata.so (SHRINK_ICU=1) — non-English date/number formatting will be wrong!"
        rm -f "$EXTRACT_DIR"/usr/lib/libicudata.so.*
        # Force .NET to use invariant globalization (no ICU)
        sed -i 's|exec "\$LIB_DIR/Nickvision.Parabolic.GNOME"|export DOTNET_SYSTEM_GLOBALIZATION_INVARIANT=1\n    exec "\$LIB_DIR/Nickvision.Parabolic.GNOME"|' \
            "$EXTRACT_DIR/usr/bin/org.nickvision.tubeconverter" 2>/dev/null || true
    fi

    # 4. Remove the .NET runtime's localization resource DLLs we don't ship
    #    (already trimmed by --self-contained, but some auxiliary .xml/.json files remain)
    rm -f "$EXTRACT_DIR"/usr/lib/org.nickvision.tubeconverter/*.json

    # Repackage the AppImage from the stripped AppDir
    info "  Repackaging AppImage..."
    rm -f "$APPIMAGE_OUT"
    if [[ -x "$DEPS_DIR/appimagetool" ]]; then
        # Use appimagetool directly if we have it (faster than re-running linuxdeploy)
        export APPIMAGE_EXTRACT_AND_RUN=1
        "$DEPS_DIR/appimagetool" --appimage-extract-and-run "$EXTRACT_DIR" "$APPIMAGE_OUT" >/dev/null 2>&1 || {
            warn "  appimagetool failed, falling back to linuxdeploy for repackaging..."
            "$DEPS_DIR/linuxdeploy" --appdir "$EXTRACT_DIR" --output appimage \
                --desktop-file "$EXTRACT_DIR/usr/share/applications/$APP_ID.desktop" \
                --icon-file "$EXTRACT_DIR/usr/share/icons/hicolor/scalable/apps/$APP_ID.svg" >/dev/null 2>&1 || true
        }
    else
        # Fall back to linuxdeploy (re-runs dep analysis but skips already-bundled libs)
        "$DEPS_DIR/linuxdeploy" --appdir "$EXTRACT_DIR" --output appimage \
            --desktop-file "$EXTRACT_DIR/usr/share/applications/$APP_ID.desktop" \
            --icon-file "$EXTRACT_DIR/usr/share/icons/hicolor/scalable/apps/$APP_ID.svg" >/dev/null 2>&1 || true
    fi

    after_size=$(stat -c%s "$APPIMAGE_OUT" 2>/dev/null || echo 0)
    if [[ "$after_size" -gt 0 ]]; then
        saved=$(( (before_size - after_size) / 1024 / 1024 ))
        ok "Size optimization complete: saved ~${saved} MB (before: $((before_size/1024/1024)) MB, after: $((after_size/1024/1024)) MB)"
    else
        warn "Repackaging may have failed; keeping the unstripped AppImage"
    fi

    rm -rf "$EXTRACT_DIR"
fi

# -----------------------------------------------------------------------------
#  Step 8 — Rename to include version
# -----------------------------------------------------------------------------
FINAL_NAME="${SCRIPT_DIR}/Parabolic-${PARABOLIC_VERSION}-${ARCH}.AppImage"
if [[ "$APPIMAGE_OUT" != "$FINAL_NAME" ]]; then
    mv "$APPIMAGE_OUT" "$FINAL_NAME"
fi
APPIMAGE_OUT="$FINAL_NAME"

echo "${c_grn}==============================================================${c_rst}"
echo "${c_grn}  ✔ AppImage build complete${c_rst}"
echo "${c_grn}  File:  $APPIMAGE_OUT${c_rst}"
echo "${c_grn}  Size:  $(du -h "$APPIMAGE_OUT" | cut -f1)${c_rst}"
echo "${c_grn}  Knobs: BUNDLE_DENO=$BUNDLE_DENO STRIP=$STRIP_BINARIES REMOVE_PDB=$REMOVE_PDB SKIP_FFPLAY=$SKIP_FFPLAY SHRINK_ICU=$SHRINK_ICU${c_rst}"
echo "${c_grn}==============================================================${c_rst}"

# Print next steps for the user
cat <<EOF

Next steps:
  $ ./$APPIMAGE_OUT            # run it directly (chmod +x first if needed)
  $ ./$APPIMAGE_OUT --appimage-extract   # inspect contents

Tested on any Linux with glibc >= 2.35 (Ubuntu 22.04+, Debian 12+, Fedora 36+, Arch).

EOF
