#!/bin/sh
# =============================================================================
#  parabolic-updater.sh — Check for AppImage updates via Yad
# =============================================================================
#  Bundled inside the AppImage at usr/bin/parabolic-updater
#
#  What it does:
#    1. Reads the current version from $APPDIR/usr/share/parabolic-version.txt
#    2. Queries GitHub Releases API for the latest release
#    3. Compares versions
#    4. If newer, shows a Yad dialog asking the user to update
#    5. On "Yes", downloads the new AppImage and atomically replaces the old one
#
#  Yad is preferred but not required. Falls back to:
#    - zenity  (more widely installed)
#    - notify-send + xdg-open (last resort, just notifies the user)
#
#  Usage:
#    parabolic-updater                  # check + ask
#    parabolic-updater --check          # check, exit 0 if up-to-date, exit 2 if update available
#    parabolic-updater --force          # don't ask, just download and replace
#
#  Environment variables (injected at build time by build-appimage.sh):
#    PARABOLIC_UPDATE_OWNER   — GitHub username (e.g. "your-username")
#    PARABOLIC_UPDATE_REPO    — GitHub repo (e.g. "parabolic")
#    PARABOLIC_ARCH           — AppImage arch (e.g. "x86_64")
#
#  When run from inside an AppImage, $APPDIR and $APPIMAGE are set by AppRun.
#  When run standalone, $APPDIR is resolved relative to this script.
# =============================================================================
set -u

# --- Build-time injected configuration -------------------------------------
# These are substituted by build-appimage.sh's sed pass.
PARABOLIC_UPDATE_OWNER="__PARABOLIC_UPDATE_OWNER__"
PARABOLIC_UPDATE_REPO="${PARABOLIC_UPDATE_REPO:-parabolic}"
PARABOLIC_ARCH="${PARABOLIC_ARCH:-x86_64}"

APP_NAME="Parabolic"

# --- Resolve APPDIR (AppImage sets it; standalone doesn't) -----------------
if [ -z "${APPDIR:-}" ]; then
    # Standalone mode: this script is at $APPDIR/usr/bin/parabolic-updater,
    # so APPDIR is two levels up from the script's directory.
    APPDIR="$(cd "$(dirname "$0")/../.." && pwd)"
fi

VERSION_FILE="$APPDIR/usr/share/parabolic-version.txt"
if [ -r "$VERSION_FILE" ]; then
    CURRENT_VERSION="$(cat "$VERSION_FILE" | tr -d '[:space:]')"
else
    CURRENT_VERSION="unknown"
fi

# --- Find a dialog tool (yad > zenity > notify-send) -----------------------
YAD="$(command -v yad 2>/dev/null || true)"
ZENITY="$(command -v zenity 2>/dev/null || true)"
NOTIFY_SEND="$(command -v notify-send 2>/dev/null || true)"

# --- Logging ---------------------------------------------------------------
log() {
    # Log to stderr so Yad dialogs don't pick it up via stdout
    printf '[parabolic-updater] %s\n' "$*" >&2
}

# --- Dialog helpers (work with yad, zenity, or terminal fallback) -----------
dialog_question() {
    # $1 = text, $2 = title
    if [ -n "$YAD" ]; then
        "$YAD" --title="$2" --text="$1" \
              --button="Yes":0 --button="No":1 \
              --width=450 --height=180 --center --on-top
        return $?
    elif [ -n "$ZENITY" ]; then
        "$ZENITY" --question --title="$2" --text="$1" --width=450
        return $?
    else
        # Terminal fallback
        printf '\n%s\n%s [y/N] ' "$2: $1" "Answer" >&2
        read -r answer
        case "$answer" in
            y|Y|yes|YES) return 0 ;;
            *) return 1 ;;
        esac
    fi
}

dialog_info() {
    # $1 = text, $2 = title
    if [ -n "$YAD" ]; then
        "$YAD" --title="$2" --text="$1" \
              --button="OK":0 \
              --width=450 --height=180 --center --on-top
    elif [ -n "$ZENITY" ]; then
        "$ZENITY" --info --title="$2" --text="$1" --width=450
    elif [ -n "$NOTIFY_SEND" ]; then
        "$NOTIFY_SEND" --app-name="$APP_NAME" --icon="dialog-information" "$2" "$1"
    else
        printf '\n%s: %s\n' "$2" "$1" >&2
    fi
}

dialog_error() {
    # $1 = text, $2 = title
    if [ -n "$YAD" ]; then
        "$YAD" --title="$2" --text="$1" \
              --button="OK":0 \
              --image="dialog-error" \
              --width=450 --height=180 --center --on-top
    elif [ -n "$ZENITY" ]; then
        "$ZENITY" --error --title="$2" --text="$1" --width=450
    elif [ -n "$NOTIFY_SEND" ]; then
        "$NOTIFY_SEND" --app-name="$APP_NAME" --icon="dialog-error" --urgency=critical "$2" "$1"
    else
        printf '\n%s: %s\n' "$2" "$1" >&2
    fi
}

dialog_progress() {
    # $1 = text, $2 = title
    # Returns: writes the PID of the progress dialog to stdout
    if [ -n "$YAD" ]; then
        "$YAD" --progress --pulsate --auto-close --auto-kill \
               --title="$2" --text="$1" --width=400 --height=120 --center --on-top --no-buttons &
        echo $!
    elif [ -n "$ZENITY" ]; then
        "$ZENITY" --progress --pulsate --auto-kill \
                  --title="$2" --text="$1" --width=400 &
        echo $!
    else
        # No progress dialog available — just print and return empty PID
        printf '%s\n' "$1" >&2
        echo ""
    fi
}

# --- Argument parsing ------------------------------------------------------
CHECK_ONLY=0
FORCE=0
for arg in "$@"; do
    case "$arg" in
        --update|--check-update|--check-updates)
            # These flags come from the launcher; treat them as default behavior
            # (check + ask). The launcher strips nothing, so we just ignore them.
            ;;
        --check)    CHECK_ONLY=1 ;;
        --force)    FORCE=1 ;;
        --help|-h)
            cat <<EOF
$APP_NAME AppImage updater

Usage:
  Parabolic.AppImage --update           Check for updates. If a newer version exists, ask via Yad dialog.
  Parabolic.AppImage --update --check    Silent check. Exit 0 if up-to-date, exit 2 if update available.
  Parabolic.AppImage --update --force    Skip the prompt; download + install if an update is available.
  Parabolic.AppImage --update --help     This help.

Checks https://github.com/$PARABOLIC_UPDATE_OWNER/$PARABOLIC_UPDATE_REPO/releases/latest
EOF
            exit 0
            ;;
        *)
            log "Unknown argument: $arg (try --update --help)"
            exit 1
            ;;
    esac
done

# --- 1. Query GitHub Releases API ------------------------------------------
API_URL="https://api.github.com/repos/${PARABOLIC_UPDATE_OWNER}/${PARABOLIC_UPDATE_REPO}/releases/latest"
log "Querying: $API_URL"

RELEASE_JSON="$(curl -fsSL \
    -H "Accept: application/vnd.github+json" \
    -H "X-GitHub-Api-Version: 2022-11-28" \
    -A "parabolic-updater/$CURRENT_VERSION (https://github.com/$PARABOLIC_UPDATE_OWNER/$PARABOLIC_UPDATE_REPO)" \
    --connect-timeout 10 --max-time 30 \
    "$API_URL" 2>/dev/null || true)"

if [ -z "$RELEASE_JSON" ]; then
    msg="Could not reach GitHub to check for updates.\n\nCheck your internet connection and try again."
    if [ "$CHECK_ONLY" = "1" ]; then
        log "$msg"
        exit 3
    fi
    dialog_error "$msg" "Update check failed"
    exit 3
fi

# --- 2. Parse latest version + download URL --------------------------------
# Use grep + sed instead of jq for portability (no jq dependency)
LATEST_VERSION="$(printf '%s' "$RELEASE_JSON" \
    | grep -o '"tag_name":[[:space:]]*"[^"]*"' \
    | head -1 \
    | sed -E 's/.*"tag_name":[[:space:]]*"([^"]*)".*/\1/' \
    | sed -E 's/^v//')"
# Strip any "appimage-" prefix the user might have used in tag names
LATEST_VERSION="${LATEST_VERSION#appimage-}"

# Find the AppImage asset matching our arch
APPIMAGE_URL="$(printf '%s' "$RELEASE_JSON" \
    | grep -o '"browser_download_url":[[:space:]]*"[^"]*'"$PARABOLIC_ARCH"'\.AppImage"' \
    | head -1 \
    | sed -E 's/.*"browser_download_url":[[:space:]]*"([^"]*)".*/\1/')"

# Fallback: if no arch-specific asset found, try a generic "latest" filename
if [ -z "$APPIMAGE_URL" ]; then
    APPIMAGE_URL="$(printf '%s' "$RELEASE_JSON" \
        | grep -o '"browser_download_url":[[:space:]]*"[^"]*\.AppImage"' \
        | head -1 \
        | sed -E 's/.*"browser_download_url":[[:space:]]*"([^"]*)".*/\1/')"
fi

if [ -z "$LATEST_VERSION" ] || [ -z "$APPIMAGE_URL" ]; then
    msg="Could not parse the latest release info from GitHub.\n\nThis usually means no release has been published yet, or the asset naming changed.\n\nURL: $API_URL"
    if [ "$CHECK_ONLY" = "1" ]; then
        log "$msg"
        exit 3
    fi
    dialog_error "$msg" "Update check failed"
    exit 3
fi

log "Current: $CURRENT_VERSION"
log "Latest:  $LATEST_VERSION"
log "URL:     $APPIMAGE_URL"

# --- 3. Compare versions ---------------------------------------------------
if [ "$CURRENT_VERSION" = "$LATEST_VERSION" ]; then
    log "Up to date."
    if [ "$CHECK_ONLY" = "1" ]; then exit 0; fi
    dialog_info "You're running the latest version of $APP_NAME ($CURRENT_VERSION)." "Up to date"
    exit 0
fi

if [ "$CURRENT_VERSION" = "unknown" ]; then
    log "WARNING: Current version is unknown. Will prompt user to update."
fi

# Exit 2 in check-only mode to signal "update available"
if [ "$CHECK_ONLY" = "1" ]; then
    log "Update available ($CURRENT_VERSION → $LATEST_VERSION)."
    exit 2
fi

# --- 4. Ask the user (unless --force) --------------------------------------
if [ "$FORCE" != "1" ]; then
    msg="A new version of $APP_NAME is available!\n\nCurrent: <b>$CURRENT_VERSION</b>\nLatest:   <b>$LATEST_VERSION</b>\n\nDownload and install now?"
    if ! dialog_question "$msg" "Update available"; then
        log "User declined the update."
        exit 0
    fi
fi

# --- 5. Find the current AppImage path -------------------------------------
# $APPIMAGE is set by AppImageKit when running from inside an AppImage.
if [ -z "${APPIMAGE:-}" ]; then
    # Try to find it from /proc/self/exe (when run via AppRun)
    APPIMAGE="$(readlink -f /proc/self/exe 2>/dev/null || true)"
    if [ -z "$APPIMAGE" ] || [ ! -f "$APPIMAGE" ]; then
        msg="Cannot determine the current AppImage path.\n\nThis updater must be run from inside an AppImage (use: ./$APP_NAME.AppImage --update)."
        dialog_error "$msg" "Update failed"
        exit 4
    fi
fi

CURRENT_APPIMAGE="$APPIMAGE"
log "Current AppImage: $CURRENT_APPIMAGE"

# Verify the file is writable (we're going to replace it)
if [ ! -w "$CURRENT_APPIMAGE" ]; then
    msg="The current AppImage is not writable:\n  $CURRENT_APPIMAGE\n\nMove it to a location you own (e.g., ~/bin or ~/Applications) and try again."
    dialog_error "$msg" "Update failed"
    exit 5
fi

# --- 6. Download to a temp file --------------------------------------------
TMP_FILE="$(mktemp --suffix=.AppImage 2>/dev/null || mktemp).AppImage"
trap 'rm -f "$TMP_FILE" 2>/dev/null' EXIT

log "Downloading $APPIMAGE_URL → $TMP_FILE ..."

# Start the progress dialog in the background
PROGRESS_PID="$(dialog_progress "Downloading $APP_NAME $LATEST_VERSION..." "Updating")"
PROGRESS_PID="${PROGRESS_PID:-}"

# Download (curl with retries)
if ! curl -fSL --retry 3 --retry-delay 5 \
            --connect-timeout 30 --max-time 600 \
            -o "$TMP_FILE" "$APPIMAGE_URL" 2>/dev/null; then
    # Kill the progress dialog if it's still running
    [ -n "$PROGRESS_PID" ] && kill "$PROGRESS_PID" 2>/dev/null || true
    msg="Download failed. Check your internet connection.\n\nURL: $APPIMAGE_URL"
    dialog_error "$msg" "Update failed"
    exit 6
fi

# Close the progress dialog
[ -n "$PROGRESS_PID" ] && kill "$PROGRESS_PID" 2>/dev/null || true
wait "$PROGRESS_PID" 2>/dev/null || true

# Verify the download is a valid ELF (at least 1 MB)
if [ ! -s "$TMP_FILE" ] || [ "$(stat -c%s "$TMP_FILE")" -lt 1048576 ]; then
    msg="Downloaded file is empty or too small. The release asset may be missing or misnamed."
    dialog_error "$msg" "Update failed"
    exit 7
fi

chmod +x "$TMP_FILE"

# Quick ELF magic check (AppImages start with the AppImage runtime ELF header)
if ! head -c 4 "$TMP_FILE" | od -An -c | grep -q 'ELF' 2>/dev/null; then
    msg="Downloaded file doesn't look like an AppImage (not an ELF binary).\n\nURL: $APPIMAGE_URL"
    dialog_error "$msg" "Update failed"
    exit 8
fi

# --- 7. Atomic replace -----------------------------------------------------
log "Replacing $CURRENT_APPIMAGE with the new version..."

# mv is atomic on the same filesystem (which /tmp usually isn't, but the file
# is already fully downloaded so we just need a rename). If /tmp is on a
# different filesystem, mv falls back to copy+unlink which is not atomic —
# but we already verified the file is complete, so this is fine.
mv "$TMP_FILE" "$CURRENT_APPIMAGE"
trap - EXIT

# --- 8. Done — ask to relaunch ---------------------------------------------
log "Update complete: $CURRENT_APPIMAGE is now version $LATEST_VERSION"

if [ "$FORCE" != "1" ]; then
    if dialog_question "$APP_NAME has been updated to <b>$LATEST_VERSION</b>!\n\nRelaunch now?" "Update complete"; then
        # Exec the new AppImage. Note: the original args ($@) were consumed
        # by the argument parser above, so we just relaunch without args.
        nohup "$CURRENT_APPIMAGE" >/dev/null 2>&1 &
        # Give the new process a moment to start, then exit
        sleep 1
    fi
fi

exit 0
