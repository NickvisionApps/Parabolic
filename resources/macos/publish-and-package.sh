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

# Bundle dependencies
info "Bundling dependencies..."
YT_DLP_PATH="./yt-dlp"
ARIA2C_PATH="$(which aria2c 2>/dev/null || true)"
FFMPEG_PATH="$(which ffmpeg 2>/dev/null || true)"
FFPROBE_PATH="$(which ffprobe 2>/dev/null || true)"
FFPLAY_PATH="$(which ffplay 2>/dev/null || true)"
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
if [[ -n "$FFPROBE_PATH" && -f "$FFPROBE_PATH" ]]; then
    cp "$FFPROBE_PATH" "$APP_BUNDLE/Contents/MacOS/ffprobe"
    chmod +x "$APP_BUNDLE/Contents/MacOS/ffprobe"
    success "Bundled ffprobe."
else
    warn "ffprobe not found"
fi
if [[ -n "$FFPLAY_PATH" && -f "$FFPLAY_PATH" ]]; then
    cp "$FFPLAY_PATH" "$APP_BUNDLE/Contents/MacOS/ffplay"
    chmod +x "$APP_BUNDLE/Contents/MacOS/ffplay"
    success "Bundled ffplay."
else
    warn "ffplay not found"
fi

# Bundle GTK4 and libadwaita
info "Bundling GTK4 and libadwaita..."
BREW_PREFIX="$(brew --prefix)"
FRAMEWORKS_DIR="$APP_BUNDLE/Contents/Frameworks"
BUNDLE_RESOURCES_DIR="$APP_BUNDLE/Contents/Resources"
mkdir -p "$FRAMEWORKS_DIR"
mkdir -p "$BUNDLE_RESOURCES_DIR"

# Recursively copy a dylib and its Homebrew-installed dependencies into Contents/Frameworks,
# rewriting install names to use @rpath so the dylibs can resolve each other at runtime.
bundle_dylib() {
    local lib_path="$1"
    local lib_name
    lib_name="$(basename "$lib_path")"
    [[ -f "$FRAMEWORKS_DIR/$lib_name" ]] && return
    [[ ! -f "$lib_path" ]] && { warn "Library not found: $lib_path"; return; }
    cp "$lib_path" "$FRAMEWORKS_DIR/$lib_name"
    chmod 755 "$FRAMEWORKS_DIR/$lib_name"
    install_name_tool -id "@rpath/$lib_name" "$FRAMEWORKS_DIR/$lib_name" 2>/dev/null || true
    # Add @loader_path as rpath so this dylib finds its bundled siblings from the same directory
    install_name_tool -add_rpath "@loader_path" "$FRAMEWORKS_DIR/$lib_name" 2>/dev/null || true
    while IFS= read -r dep_line; do
        local dep_path dep_name
        dep_path="$(echo "$dep_line" | awk '{print $1}')"
        if [[ "$dep_path" == "$BREW_PREFIX"* && -f "$dep_path" && "$dep_path" != "$lib_path" ]]; then
            dep_name="$(basename "$dep_path")"
            install_name_tool -change "$dep_path" "@rpath/$dep_name" "$FRAMEWORKS_DIR/$lib_name" 2>/dev/null || true
            bundle_dylib "$dep_path"
        fi
    done < <(otool -L "$lib_path" | tail -n +2)
}

# Bundle libgtk-4 and libadwaita; all transitive dependencies are pulled in automatically
for LIB_NAME in "libgtk-4.1.dylib" "libadwaita-1.0.dylib"; do
    LIB_PATH="$(find "$BREW_PREFIX/lib" -name "$LIB_NAME" | head -n1)"
    if [[ -n "$LIB_PATH" ]]; then
        bundle_dylib "$LIB_PATH"
        success "Bundled $LIB_NAME and its dependencies."
    else
        warn "$LIB_NAME not found under $BREW_PREFIX/lib"
    fi
done

# Fix any Homebrew references in .NET-published native libs and add Frameworks rpath
find "$APP_BUNDLE/Contents/MacOS" \( -name "*.dylib" -o -name "*.so" \) | while read -r lib; do
    install_name_tool -add_rpath "@loader_path/../Frameworks" "$lib" 2>/dev/null || true
    while IFS= read -r dep_line; do
        dep_path="$(echo "$dep_line" | awk '{print $1}')"
        if [[ "$dep_path" == "$BREW_PREFIX"* && -f "$dep_path" ]]; then
            dep_name="$(basename "$dep_path")"
            install_name_tool -change "$dep_path" "@rpath/$dep_name" "$lib" 2>/dev/null || true
        fi
    done < <(otool -L "$lib" | tail -n +2)
done

# Add Frameworks rpath and fix any direct Homebrew references in the main executable
install_name_tool -add_rpath "@executable_path/../Frameworks" "$APP_BUNDLE/Contents/MacOS/$PROJECT" 2>/dev/null || true
while IFS= read -r dep_line; do
    dep_path="$(echo "$dep_line" | awk '{print $1}')"
    if [[ "$dep_path" == "$BREW_PREFIX"* && -f "$dep_path" ]]; then
        dep_name="$(basename "$dep_path")"
        install_name_tool -change "$dep_path" "@rpath/$dep_name" "$APP_BUNDLE/Contents/MacOS/$PROJECT" 2>/dev/null || true
    fi
done < <(otool -L "$APP_BUNDLE/Contents/MacOS/$PROJECT" | tail -n +2)

# Bundle GLib schemas required by GTK4 and libadwaita
info "Bundling GLib schemas..."
SCHEMAS_DEST="$BUNDLE_RESOURCES_DIR/share/glib-2.0/schemas"
mkdir -p "$SCHEMAS_DEST"
find "$BREW_PREFIX/share/glib-2.0/schemas" -name "*.xml" -exec cp {} "$SCHEMAS_DEST/" \; 2>/dev/null || true
glib-compile-schemas "$SCHEMAS_DEST"
success "Bundled GLib schemas."

# Create a launcher script that sets up the GTK environment before running the app.
# DYLD_LIBRARY_PATH is needed because GirCore uses dlopen() by library name at runtime.
info "Creating GTK environment launcher script..."
REAL_BINARY="${PROJECT}.bin"
mv "$APP_BUNDLE/Contents/MacOS/$PROJECT" "$APP_BUNDLE/Contents/MacOS/$REAL_BINARY"
chmod +x "$APP_BUNDLE/Contents/MacOS/$REAL_BINARY"
cat > "$APP_BUNDLE/Contents/MacOS/$PROJECT" << 'LAUNCHER_EOF'
#!/usr/bin/env bash
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUNDLE_CONTENTS="$(cd "$SCRIPT_DIR/.." && pwd)"
export DYLD_LIBRARY_PATH="$BUNDLE_CONTENTS/Frameworks${DYLD_LIBRARY_PATH:+:$DYLD_LIBRARY_PATH}"
export GSETTINGS_SCHEMA_DIR="$BUNDLE_CONTENTS/Resources/share/glib-2.0/schemas"
exec "$SCRIPT_DIR/__REAL_BINARY__" "$@"
LAUNCHER_EOF
sed -i '' "s|__REAL_BINARY__|${REAL_BINARY}|g" "$APP_BUNDLE/Contents/MacOS/$PROJECT"
chmod +x "$APP_BUNDLE/Contents/MacOS/$PROJECT"
success "Created GTK environment launcher script."

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
