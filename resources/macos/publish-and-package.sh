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
if [[ $# -lt 2 ]]; then
    error "Usage: $0 prefix runtime"
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
PREFIX="$1"
RUNTIME="$2"
APP_BUNDLE="${APP_NAME}.app"
info "Prefix: $PREFIX"
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
sed -i '' "s|@APP_NAME@|$APP_NAME|g" "$APP_BUNDLE/Contents/Info.plist"
sed -i '' "s|@OUTPUT_NAME@|$PROJECT|g" "$APP_BUNDLE/Contents/Info.plist"
success "Created Info.plist."

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
