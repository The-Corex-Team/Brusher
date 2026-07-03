#!/usr/bin/env bash
#
# Build Brusher as a self-contained AppImage using linuxdeploy +
# linuxdeploy-plugin-qt (located in ~/AppImageTools/).
#
# This script:
#   1. Pre-flight checks for required build tools.
#   2. Configures and builds with CMake.
#   3. Resets AppDir/usr, then installs the binary and assets via
#      DESTDIR=AppDir cmake --install.
#   4. Runs linuxdeploy --plugin qt, which bundles Qt libraries and
#      platform plugins (libqwayland.so, libqxcb.so, etc.) found on
#      the build host.
#   5. Produces Brusher-x86_64.AppImage at the repo root.
#
# Run from the repo root:   ./build-appimage.sh

set -euo pipefail

# ---------------------------------------------------------------------------
# Paths and tools
# ---------------------------------------------------------------------------

REPO_ROOT="$(cd "$(dirname "$0")" && pwd)"
APPIMAGE_TOOLS_DIR="${APPIMAGE_TOOLS_DIR:-$HOME/AppImageTools}"

BUILD_DIR="${REPO_ROOT}/build"
APPDIR="${REPO_ROOT}/AppDir"
APPDIR_USR="${APPDIR}/usr"

APPIMAGE_NAME="Brusher-x86_64.AppImage"
APPIMAGE_PATH="${REPO_ROOT}/${APPIMAGE_NAME}"

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

log()  { printf '\033[1;34m[build-appimage]\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m[build-appimage] WARNING:\033[0m %s\n' "$*" >&2; }
err()  { printf '\033[1;31m[build-appimage] ERROR:\033[0m %s\n' "$*" >&2; }
die()  { err "$*"; exit 1; }

# Resolve a tool: prefer on PATH; fall back to APPIMAGE_TOOLS_DIR.
resolve_tool() {
    local name="$1"
    local fallback_name="${2:-$1}"
    if command -v "$name" >/dev/null 2>&1; then
        command -v "$name"
        return 0
    fi
    local fallback="${APPIMAGE_TOOLS_DIR}/${fallback_name}"
    if [[ -x "$fallback" ]]; then
        printf '%s\n' "$fallback"
        return 0
    fi
    return 1
}

# ---------------------------------------------------------------------------
# Pre-flight
# ---------------------------------------------------------------------------

log "Repo root: ${REPO_ROOT}"
log "AppImage tools dir: ${APPIMAGE_TOOLS_DIR}"

# Required build tools. Fail loudly if missing.
command -v cmake >/dev/null 2>&1 \
    || die "cmake not found on PATH. Install with: sudo apt install cmake"

# Prefer qmake6 (Qt 6 explicit), fall back to qmake.
QMAKE_BIN=""
if command -v qmake6 >/dev/null 2>&1; then
    QMAKE_BIN="$(command -v qmake6)"
elif command -v qmake-qt6 >/dev/null 2>&1; then
    QMAKE_BIN="$(command -v qmake-qt6)"
elif command -v qmake >/dev/null 2>&1; then
    QMAKE_BIN="$(command -v qmake)"
fi
if [[ -z "$QMAKE_BIN" ]]; then
    die "qmake not found. Install with: sudo apt install qt6-base-dev"
fi
log "Using qmake: ${QMAKE_BIN}"

LINUXDEPLOY_BIN="$(resolve_tool linuxdeploy linuxdeploy-x86_64.AppImage)" \
    || die "linuxdeploy not found on PATH or in ${APPIMAGE_TOOLS_DIR}"
log "Using linuxdeploy: ${LINUXDEPLOY_BIN}"

# linuxdeploy-plugin-qt is auto-discovered by linuxdeploy when --plugin qt
# is used, as long as it sits next to the linuxdeploy binary. Verify it's
# there; fail if not (per project policy: linuxdeploy only).
LINUXDEPLOY_PLUGIN_QT="${APPIMAGE_TOOLS_DIR}/linuxdeploy-plugin-qt-x86_64.AppImage"
if [[ ! -x "$LINUXDEPLOY_PLUGIN_QT" ]]; then
    die "linuxdeploy-plugin-qt-x86_64.AppImage not found in ${APPIMAGE_TOOLS_DIR}"
fi
log "linuxdeploy-plugin-qt: ${LINUXDEPLOY_PLUGIN_QT}"

# Soft warnings — bundling still works, but specific features may be missing.
if ! dpkg -s qt6-wayland-dev >/dev/null 2>&1; then
    warn "qt6-wayland-dev is not installed."
    warn "  The resulting AppImage will run on X11 but NOT on Wayland."
    warn "  Install with: sudo apt install qt6-wayland-dev"
fi

if ! dpkg -s qt6-base-dev >/dev/null 2>&1; then
    warn "qt6-base-dev is not installed. CMake may fail to find Qt6."
fi

# ---------------------------------------------------------------------------
# Configure + build
# ---------------------------------------------------------------------------

log "Configuring with CMake..."
cmake -S "${REPO_ROOT}" -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr

log "Building..."
cmake --build "${BUILD_DIR}" --parallel

# ---------------------------------------------------------------------------
# Reset AppDir/usr, then install
# ---------------------------------------------------------------------------

log "Resetting AppDir/usr..."
rm -rf "${APPDIR_USR}"

log "Installing into AppDir via DESTDIR..."
DESTDIR="${APPDIR}" cmake --install "${BUILD_DIR}"

# Sanity: the installed binary must exist where the AppRun symlink expects.
[[ -x "${APPDIR_USR}/bin/Brusher" ]] \
    || die "Expected ${APPDIR_USR}/bin/Brusher after install; not found."

# ---------------------------------------------------------------------------
# linuxdeploy + Qt plugin
# ---------------------------------------------------------------------------

log "Running linuxdeploy with --plugin qt..."
"${LINUXDEPLOY_BIN}" \
    --appdir "${APPDIR}" \
    --plugin qt \
    --output appimage

# ---------------------------------------------------------------------------
# Verify bundle
# ---------------------------------------------------------------------------

log "Verifying bundle..."
[[ -f "${APPDIR_USR}/lib/libQt6Core.so.6" ]] \
    || die "Qt6Core was not bundled. Something went wrong in linuxdeploy."

# Count platform plugins. At least one of xcb / wayland / offscreen should
# be present after a successful deployment.
PLATFORM_PLUGIN_DIR="${APPDIR_USR}/plugins/platforms"
if [[ ! -d "$PLATFORM_PLUGIN_DIR" ]] \
   || ! compgen -G "${PLATFORM_PLUGIN_DIR}/libq*.so" >/dev/null; then
    die "No Qt platform plugins bundled. Build host may lack Qt6 platform libs."
fi

WAYLAND_BUNDLED="no"
X11_BUNDLED="no"
[[ -f "${PLATFORM_PLUGIN_DIR}/libqwayland.so" ]] && WAYLAND_BUNDLED="yes"
[[ -f "${PLATFORM_PLUGIN_DIR}/libqxcb.so" ]]     && X11_BUNDLED="yes"

# ---------------------------------------------------------------------------
# Result
# ---------------------------------------------------------------------------

if [[ -f "${APPIMAGE_PATH}" ]]; then
    SIZE_MB=$(du -m "${APPIMAGE_PATH}" | cut -f1)
    log "Done."
    log "  AppImage: ${APPIMAGE_PATH} (${SIZE_MB} MB)"
    log "  Wayland support bundled: ${WAYLAND_BUNDLED}"
    log "  X11 support bundled:     ${X11_BUNDLED}"
    log ""
    log "Run it with:  ./${APPIMAGE_NAME}"
else
    die "linuxdeploy finished but ${APPIMAGE_PATH} was not produced."
fi