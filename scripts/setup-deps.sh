#!/usr/bin/env bash
#
# Fetches everything the Linux/macOS build needs into ./third_party/
#
#   third_party/sgg    the SGG (Simple Game Graphics) library sources
#   third_party/local  SDL2 / SDL2_mixer / GLEW / FreeType / GLM / OpenGL
#                      headers + libraries, but only if your system does not
#                      already provide them
#
# Nothing is installed system-wide and no root access is required.
# Run it once, then run `make`.
#
set -euo pipefail

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
THIRD_PARTY="$REPO_ROOT/third_party"

SGG_URL="https://github.com/cgaueb/sgg.git"
# Pinned so the build does not change under you. Override with SGG_REF=...
SGG_REF="${SGG_REF:-c918482039406d80b794b1f06360b2f6fee4671d}"

# Debian/Ubuntu development packages. These are what you would install with
# apt, and what the no-root fallback unpacks locally.
APT_DEV_PACKAGES=(
    libsdl2-dev
    libsdl2-mixer-dev
    libglew-dev
    libfreetype-dev
    libglm-dev
    libgl-dev libglx-dev libopengl-dev
    libglu1-mesa-dev
    libpng-dev libbrotli-dev libbz2-dev
)

# Runtime libraries the above link against. Most desktops already have these,
# so the fallback only fetches the ones that are genuinely absent - shipping
# our own copy of e.g. libgbm or libwayland would risk fighting the system's
# graphics drivers.
APT_RUNTIME_PACKAGES=(
    libsdl2-2.0-0
    libsdl2-mixer-2.0-0
    libglew2.2
    libglu1-mesa
    libfreetype6
    # SDL2_mixer decoders
    libflac14 libmpg123-0t64 libopusfile0 libvorbisfile3 libxmp4
    libopus0 libogg0
)

say()  { printf '\033[1;36m==>\033[0m %s\n' "$*"; }
warn() { printf '\033[1;33m==>\033[0m %s\n' "$*" >&2; }
die()  { printf '\033[1;31m==>\033[0m %s\n' "$*" >&2; exit 1; }

# ---------------------------------------------------------------------------
# 1. SGG library sources
# ---------------------------------------------------------------------------
fetch_sgg() {
    if [ -d "$THIRD_PARTY/sgg/sgg" ]; then
        say "SGG already present in third_party/sgg"
        return
    fi
    command -v git >/dev/null || die "git is required to fetch SGG"
    say "Fetching SGG into third_party/sgg"
    mkdir -p "$THIRD_PARTY"
    rm -rf "$THIRD_PARTY/sgg"
    git init -q "$THIRD_PARTY/sgg"
    git -C "$THIRD_PARTY/sgg" remote add origin "$SGG_URL"
    git -C "$THIRD_PARTY/sgg" fetch -q --depth 1 origin "$SGG_REF"
    git -C "$THIRD_PARTY/sgg" checkout -q FETCH_HEAD
    say "SGG at $SGG_REF"
}

# ---------------------------------------------------------------------------
# 2. Third-party C/C++ dependencies
# ---------------------------------------------------------------------------
# Returns 0 when the compiler can already find everything on its own.
have_system_deps() {
    command -v pkg-config >/dev/null || return 1
    pkg-config --exists sdl2 SDL2_mixer glew freetype2 || return 1
    # glm and OpenGL ship no reliable .pc file, so probe the headers
    printf '#include <glm/glm.hpp>\n#include <GL/glew.h>\nint main(){}\n' \
        > "${TMPDIR:-/tmp}/sgg_probe_$$.cpp"
    local ok=0
    "${CXX:-c++}" -fsyntax-only \
        $(pkg-config --cflags glew) \
        "${TMPDIR:-/tmp}/sgg_probe_$$.cpp" >/dev/null 2>&1 || ok=1
    rm -f "${TMPDIR:-/tmp}/sgg_probe_$$.cpp"
    return $ok
}

apt_knows() { apt-cache show "$1" >/dev/null 2>&1; }
apt_installed() {
    [ "$(dpkg-query -W -f='${db:Status-Status}' "$1" 2>/dev/null)" = installed ]
}

# No root, no system changes: ask apt for the .deb files and unpack them
# into third_party/local. The Makefile picks that prefix up automatically.
fetch_local_debs() {
    command -v apt-get >/dev/null || return 1
    command -v dpkg-deb >/dev/null || return 1

    local prefix="$THIRD_PARTY/local"
    local debs="$prefix/debs"
    local wanted=() pkg

    # Development packages: always needed, since we only get here when the
    # system does not provide them.
    for pkg in "${APT_DEV_PACKAGES[@]}"; do
        apt_knows "$pkg" && wanted+=("$pkg")
    done
    # Runtime packages: only the missing ones.
    for pkg in "${APT_RUNTIME_PACKAGES[@]}"; do
        apt_installed "$pkg" && continue
        apt_knows "$pkg" && wanted+=("$pkg")
    done

    [ ${#wanted[@]} -gt 0 ] || return 1

    say "Downloading ${#wanted[@]} packages into third_party/local (no root needed)"
    mkdir -p "$debs"
    ( cd "$debs" && apt-get download "${wanted[@]}" ) || {
        warn "apt-get download failed"
        return 1
    }
    local deb
    for deb in "$debs"/*.deb; do
        dpkg-deb -x "$deb" "$prefix"
    done
    say "Unpacked $(ls -1 "$debs"/*.deb | wc -l) packages into third_party/local"
    relink_against_system "$prefix"
}

# A -dev package ships only the linker symlink (libGL.so -> libGL.so.1) and
# leaves the versioned object to the matching runtime package. Where that
# runtime library is already installed system-wide we would otherwise be left
# with a symlink dangling inside our prefix, so re-point those at the real
# system copy.
relink_against_system() {
    local prefix="$1"
    local triplet
    triplet="$(${CXX:-c++} -dumpmachine)"
    local libdir="$prefix/usr/lib/$triplet"
    [ -d "$libdir" ] || return 0

    local link target repaired=0
    for link in "$libdir"/*.so*; do
        [ -L "$link" ] || continue
        [ -e "$link" ] && continue              # resolves fine already
        target="$(readlink "$link")"
        if [ -e "/usr/lib/$triplet/$target" ]; then
            ln -sf "/usr/lib/$triplet/$target" "$link"
            repaired=$((repaired + 1))
        else
            warn "$(basename "$link") -> $target is missing and not installed system-wide"
        fi
    done
    [ "$repaired" -gt 0 ] && say "Re-pointed $repaired linker symlink(s) at system libraries"
    return 0
}

setup_deps() {
    if have_system_deps; then
        say "SDL2 / SDL2_mixer / GLEW / FreeType / GLM found system-wide"
        return
    fi

    if [ -d "$THIRD_PARTY/local/usr/include/SDL2" ]; then
        say "Local dependency prefix already present in third_party/local"
        return
    fi

    warn "System development packages are missing."

    if command -v apt-get >/dev/null; then
        cat >&2 <<EOF

On Debian/Ubuntu the tidiest fix is:

    sudo apt-get install ${APT_DEV_PACKAGES[*]}

Falling back to a local, root-free copy under third_party/ instead.
EOF
        fetch_local_debs && return
    fi

    cat >&2 <<'EOF'

Install the SGG dependencies with your package manager, then re-run make:

  Debian/Ubuntu  libsdl2-dev libsdl2-mixer-dev libglew-dev libfreetype-dev \
                 libglm-dev libgl-dev libglu1-mesa-dev
  Fedora         SDL2-devel SDL2_mixer-devel glew-devel freetype-devel \
                 glm-devel mesa-libGL-devel mesa-libGLU-devel
  Arch           sdl2 sdl2_mixer glew freetype2 glm glu
  macOS (brew)   sdl2 sdl2_mixer glew freetype glm
EOF
    die "missing dependencies"
}

fetch_sgg
setup_deps
say "Done. Now run: make && make run"
