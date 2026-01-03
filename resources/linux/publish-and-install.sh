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
echo -e "${BOLD}${BLUE} Nickvision Linux publish-and-install Script${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"

# Initialize script and check arguments
CURRENT_PWD=$(pwd)
set -euo pipefail
if [[ $# -lt 2 ]]; then
    echo "Usage: $0 prefix runtime"
    exit 1
fi

# Change pwd to script directory
info "Changing to script directory..."
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
cd "$SCRIPT_DIR"
success "Changed to script directory: $SCRIPT_DIR"

# Load variables
info "Loading variables..."
APP_ID="org.nickvision.tubeconverter"
PROJECT="Nickvision.Parabolic.GNOME"
PREFIX="$1"
RUNTIME="$2"
BIN_DIR="$PREFIX/bin"
LIB_DIR="$PREFIX/lib/$APP_ID"
DATA_DIR="$PREFIX/share"
info "Bin directory: $BIN_DIR"
info "Lib directory: $LIB_DIR"
info "Data directory: $DATA_DIR"
success "Loaded variables."

echo -e "${BOLD}${BLUE}==============================================================${RESET}"
echo -e "${BOLD}${BLUE} Publishing and Installing $APP_ID${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"

# Create main directories
info "Creating directories..."
mkdir -p "$BIN_DIR" "$LIB_DIR" "$DATA_DIR"
success "Created directories."

# Publish application
info "Publishing application..."
export DOTNET_CLI_TELEMETRY_OPTOUT=1
if [ -n "$container" ]; then
    dotnet publish -c Release \
        --source "$CURRENT_PWD/nuget-sources" \
        --source "/usr/lib/sdk/dotnet10/nuget/packages" \
        "../../$PROJECT/$PROJECT.csproj" \
        --runtime $RUNTIME \
        --self-contained true \
        -p:PublishReadyToRun=true
else
    dotnet publish -c Release \
        "../../$PROJECT/$PROJECT.csproj" \
        --runtime $RUNTIME \
        --self-contained true \
        -p:PublishReadyToRun=true
fi
PUBLISH_DIR="$(find "../../$PROJECT/bin/Release" -type d -name publish | head -n1)"
if [[ ! -d "$PUBLISH_DIR" ]]; then
    error "Publish directory not found!"
fi
cp -a "$PUBLISH_DIR/." "$LIB_DIR"
success "Published application to $LIB_DIR."

# Create desktop file
info "Creating desktop file..."
DESKTOP_FILE="$DATA_DIR/applications/$APP_ID.desktop"
mkdir -p "$(dirname "$DESKTOP_FILE")"
cp "$APP_ID.desktop.in" "$DESKTOP_FILE"
sed -i "s|@LIB_DIR@|$LIB_DIR|g" "$DESKTOP_FILE"
sed -i "s|@OUTPUT_NAME@|$PROJECT|g" "$DESKTOP_FILE"
success "Created desktop file at $DESKTOP_FILE."

# Create executable launcher script
info "Creating launcher script..."
LAUNCHER_FILE="$BIN_DIR/$APP_ID"
cp "$APP_ID.in" "$LAUNCHER_FILE"
sed -i "s|@LIB_DIR@|$LIB_DIR|g" "$LAUNCHER_FILE"
sed -i "s|@OUTPUT_NAME@|$PROJECT|g" "$LAUNCHER_FILE"
chmod +x "$LAUNCHER_FILE"
success "Created launcher script at $LAUNCHER_FILE."

# Copy metadata file
info "Copying metadata file..."
METADATA_FILE="$DATA_DIR/metainfo/$APP_ID.metainfo.xml"
mkdir -p "$(dirname "$METADATA_FILE")"
cp "$APP_ID.metainfo.xml" "$METADATA_FILE"
success "Copied metadata file to $METADATA_FILE."

# Copy icons
info "Copying icons..."
SCALABLE_ICON_DIR="$DATA_DIR/icons/hicolor/scalable/apps"
SYMBOLIC_ICON_DIR="$DATA_DIR/icons/hicolor/symbolic/apps"
mkdir -p "$SCALABLE_ICON_DIR"
mkdir -p "$SYMBOLIC_ICON_DIR"
cp "../$APP_ID.svg" "$SCALABLE_ICON_DIR/$APP_ID.svg"
cp "../$APP_ID-devel.svg" "$SCALABLE_ICON_DIR/$APP_ID-devel.svg"
cp "../$APP_ID-symbolic.svg" "$SYMBOLIC_ICON_DIR/$APP_ID-symbolic.svg"
success "Copied icons."

# Update gtk icon cache
info "Updating GTK icon cache..."
gtk-update-icon-cache
success "Updated GTK icon cache."

# Update desktop database
info "Updating desktop database..."
update-desktop-database
success "Updated desktop database."

# Restore pwd
info "Restoring previous working directory..."
cd $CURRENT_PWD
success "Restored working directory to $CURRENT_PWD."

echo -e "${BOLD}${BLUE}==============================================================${RESET}"
echo -e "${BOLD}${GREEN}✔ Published and Installed $APP_ID Successfully!${RESET}"
echo -e "${BOLD}${BLUE}==============================================================${RESET}"