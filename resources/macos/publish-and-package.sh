#!/usr/bin/env bash

# Define colors and logging functions
RED="\033[0;31m"
GREEN="\033[0;32m"
YELLOW="\033[1;33m"
BLUE="\033[0;34m"
CYAN="\033[0;36m"
BOLD="\033[1m"
RESET="\033[0m"

info()    { echo -e "${CYAN}==>${RESET} $1"; }
success() { echo -e "${GREEN}✔${RESET} $1"; }
warn()    { echo -e "${YELLOW}⚠${RESET} $1"; }
error()   { echo -e "${RED}✘${RESET} $1"; exit 1; }

echo -e "${BOLD}${BLUE}==============================================================${RESET}"
echo -e "${BOLD}${BLUE} Nickvision macOS publish-and-package Script${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"

# Initialize script and check arguments
CURRENT_PWD=$(pwd)
set -euo pipefail
if [[ $# -lt 1 ]]; then
    error "Usage: $0 runtime"
fi

# Change pwd to script directory
info "Changing to script directory..."
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"
success "Changed to script directory: $SCRIPT_DIR"

# Load variables
info "Loading variables..."
APP_ID="org.nickvision.tubeconverter"
APP_NAME="Parabolic"
PROJECT="Nickvision.Parabolic.GNOME"
RUNTIME="$1"
APP_BUNDLE="${APP_NAME}.app"
info "Runtime: $RUNTIME"
info "App bundle: $APP_BUNDLE"
success "Loaded variables."

echo -e "${BOLD}${BLUE}==============================================================${RESET}"
echo -e "${BOLD}${BLUE} Publishing and Packaging ${APP_NAME}${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"

# Publish application
info "Publishing application..."
export DOTNET_CLI_TELEMETRY_OPTOUT=1
dotnet publish -c Release \
    "../../$PROJECT/$PROJECT.csproj" \
    --runtime $RUNTIME \
    --self-contained true \
    -p:PublishReadyToRun=true
PUBLISH_DIR="$(find "../../$PROJECT/bin/Release" -type d -name publish | head -n1)"
if [[ ! -d "$PUBLISH_DIR" ]]; then
    error "Publish directory not found!"
fi
success "Published application."

# Create app bundle structure
info "Creating app bundle structure..."
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"
success "Created app bundle structure."

# Copy published files to app bundle
info "Copying published files to app bundle..."
cp -R "$PUBLISH_DIR/"* "$APP_BUNDLE/Contents/MacOS/"
success "Copied published files to app bundle."
cp "Info.plist" "$APP_BUNDLE/Contents/Info.plist"
sed -i '' "s|@APP_ID@|$APP_ID|g" "$APP_BUNDLE/Contents/Info.plist"
sed -i '' "s|@APP_NAME@|$APP_NAME|g" "$APP_BUNDLE/Contents/Info.plist"
sed -i '' "s|@OUTPUT_NAME@|$PROJECT|g" "$APP_BUNDLE/Contents/Info.plist"
success "Created Info.plist."

# Set app icon
info "Setting app icon..."
if [[ -f "../${APP_ID}.png" ]]; then
    mkdir -p AppIcon.iconset
    sips -z 16 16 "../${APP_ID}.png" --out AppIcon.iconset/icon_16x16.png
    sips -z 32 32 "../${APP_ID}.png" --out AppIcon.iconset/icon_16x16@2x.png
    sips -z 32 32 "../${APP_ID}.png" --out AppIcon.iconset/icon_32x32.png
    sips -z 64 64 "../${APP_ID}.png" --out AppIcon.iconset/icon_32x32@2x.png
    sips -z 128 128 "../${APP_ID}.png" --out AppIcon.iconset/icon_128x128.png
    sips -z 256 256 "../${APP_ID}.png" --out AppIcon.iconset/icon_128x128@2x.png
    sips -z 256 256 "../${APP_ID}.png" --out AppIcon.iconset/icon_256x256.png
    sips -z 512 512 "../${APP_ID}.png" --out AppIcon.iconset/icon_256x256@2x.png
    sips -z 512 512 "../${APP_ID}.png" --out AppIcon.iconset/icon_512x512.png
    sips -z 1024 1024 "../${APP_ID}.png" --out AppIcon.iconset/icon_512x512@2x.png
    iconutil -c icns AppIcon.iconset -o "$APP_BUNDLE/Contents/Resources/${APP_ID}.icns"
    rm -rf AppIcon.iconset
    success "Set app icon."
else
    warn "Icon file not found at ../${APP_ID}.png"
fi

# Bundle yt-dlp, aria2c, and ffmpeg
info "Bundling yt-dlp, aria2c, and ffmpeg..."
YT_DLP_PATH="./yt-dlp"
ARIA2C_PATH="$(which aria2c 2>/dev/null || true)"
FFMPEG_PATH="$(which ffmpeg 2>/dev/null || true)"
if [[ -f "$YT_DLP_PATH" ]]; then
    cp "$YT_DLP_PATH" "$APP_BUNDLE/Contents/MacOS/yt-dlp"
    chmod +x "$APP_BUNDLE/Contents/MacOS/yt-dlp"
    success "Bundled yt-dlp."
else
    warn "yt-dlp not found at $YT_DLP_PATH"
fi
if [[ -n "$ARIA2C_PATH" && -f "$ARIA2C_PATH" ]]; then
    cp "$ARIA2C_PATH" "$APP_BUNDLE/Contents/MacOS/aria2c"
    chmod +x "$APP_BUNDLE/Contents/MacOS/aria2c"
    success "Bundled aria2c."
else
    warn "aria2c not found"
fi
if [[ -n "$FFMPEG_PATH" && -f "$FFMPEG_PATH" ]]; then
    cp "$FFMPEG_PATH" "$APP_BUNDLE/Contents/MacOS/ffmpeg"
    chmod +x "$APP_BUNDLE/Contents/MacOS/ffmpeg"
    success "Bundled ffmpeg."
else
    warn "ffmpeg not found"
fi

# Set executable permissions
info "Setting executable permissions..."
chmod +x "$APP_BUNDLE/Contents/MacOS/$PROJECT"
success "Set executable permissions."

# Restore pwd
info "Restoring previous working directory..."
cd "$CURRENT_PWD"
success "Restored working directory to $CURRENT_PWD."

echo -e "${BOLD}${BLUE}==============================================================${RESET}"
echo -e "${BOLD}${GREEN}✔ Published and Packaged ${APP_NAME} Successfully!${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"
