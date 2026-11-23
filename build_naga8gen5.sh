#!/usr/bin/env bash
# xamonxx naga8gen5 — Turnip (Mesa freedreno Vulkan) for Snapdragon 8 Gen 5 / Adreno 829 (iQOO 15R)
# Based on StevenMXZ/Adreno-Tools-Drivers build_turnip.sh (A8xx branch @ 4b79649, GPLv3)
# + performance flags from The412Banner/Banners-Turnip build_turnip_perf.sh.
#
# FLAVOR=perf (default): -O3, ThinLTO, -mcpu=oryon-1 tuning, NDEBUG
# FLAVOR=safe          : same patches, upstream release flags only (fallback if perf fails to build)
set -euo pipefail

green='\033[0;32m'; red='\033[0;31m'; nocolor='\033[0m'
log() { echo -e "${green}$*${nocolor}"; }
die() { echo -e "${red}$*${nocolor}" >&2; exit 1; }

REPO_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
WORKDIR="$REPO_ROOT/turnip_workdir"
INSTALL_DIR="$WORKDIR/install"
NDKVER="android-ndk-r29"
NDK="$WORKDIR/$NDKVER/toolchains/llvm/prebuilt/linux-x86_64/bin"
MIN_API=28
# Mesa commit known to build cleanly with these patches (StevenMXZ CI run 2026-11-23).
MESA_COMMIT="${MESA_COMMIT:-c5ef4bdfabf2e1a76d5e41ab8e5d75ddbc121275}"
FLAVOR="${FLAVOR:-perf}"
DRIVER_NAME="xamonxx naga8gen5 V0.1"
PKG_VERSION="0.1"
ZIP_NAME="xamonxx_naga8gen5V0.1.zip"
[ "$FLAVOR" = "safe" ] && ZIP_NAME="xamonxx_naga8gen5V0.1-safe.zip"

check_deps() {
    for dep in git meson ninja unzip curl flex bison zip glslangValidator python3; do
        command -v "$dep" >/dev/null 2>&1 || die "Missing dependency: $dep"
    done
}

prepare_workdir() {
    mkdir -p "$WORKDIR"
    if [ ! -d "$WORKDIR/$NDKVER" ]; then
        log "Downloading Android NDK r29..."
        curl -fsSL "https://dl.google.com/android/repository/${NDKVER}-linux.zip" -o "$WORKDIR/${NDKVER}-linux.zip"
        unzip -q "$WORKDIR/${NDKVER}-linux.zip" -d "$WORKDIR"
    fi
    rm -rf "$WORKDIR/mesa"
    mkdir -p "$WORKDIR/mesa"
    git -C "$WORKDIR/mesa" init -q
    git -C "$WORKDIR/mesa" remote add origin https://gitlab.freedesktop.org/mesa/mesa.git
    log "Fetching Mesa $MESA_COMMIT..."
    git -C "$WORKDIR/mesa" fetch -q --depth=1 origin "$MESA_COMMIT"
    git -C "$WORKDIR/mesa" checkout -q --detach FETCH_HEAD
}

apply_patch() {
    log "Applying $1..."
    git apply --check "$REPO_ROOT/patches/$1"
    git apply "$REPO_ROOT/patches/$1"
}

apply_unified_patches() {
    local script patch_file
    apply_patch a8xx/0001-device-compatibility.patch
    for script in a8xx/apply_a8xx_gpus.py a8xx/a8xx_gpu_ids.py \
                  a7xx/apply_a7xx_gen1_quirks.py \
                  android/fix_gralloc_flushall.py android/add_aimapper_gralloc.py \
                  android/add_ubwc_swapchain_usage.py; do
        python3 -B "$REPO_ROOT/patches/$script"
    done
    for patch_file in "$REPO_ROOT"/patches/a8xx/mesh-and-compute/*.patch; do
        apply_patch "a8xx/mesh-and-compute/$(basename "$patch_file")"
    done
    apply_patch a8xx/0002-scope-pipeline-and-mesh-to-a8xx.patch
    apply_patch a7xx/0001-oneui-ubwc-hint.patch
    apply_patch a6xx/0001-revert-d32s8-late-z-workaround.patch
    apply_patch a6xx/0002-kgsl-context-recovery.patch
    apply_patch common/tu-vauzi-mutable-ubwc.patch
    for script in a8xx/apply_a8xx_cat4_rpt.py a8xx/a8xx_resolve_cr_cntl.py \
                  a8xx/a8xx_lrz_color_mask.py a8xx/process_heap_usage.py; do
        python3 -B "$REPO_ROOT/patches/$script"
    done

    grep -Fq 'U_GRALLOC_TYPE_AIMAPPER' src/util/u_gralloc/u_gralloc.h
    grep -Fq 'name="Adreno (TM) 829"' src/freedreno/common/freedreno_devices.py || die "Adreno 829 entry missing"
    grep -Fq '0x44030A00' src/freedreno/common/freedreno_devices.py || die "Adreno 829 KGSL chip id missing"
    grep -Fq 'tu_process_heap_used' src/freedreno/vulkan/tu_device.cc
    python3 -m py_compile src/freedreno/common/freedreno_devices.py
}

apply_android_ndk_fixes() {
    log "Applying Android/NDK r29 fixes..."
    sed -i 's/typedef const native_handle_t\* buffer_handle_t;/typedef void* buffer_handle_t;/g' include/android_stub/cutils/native_handle.h || true
    sed -i 's/, hnd->handle/, (void *)hnd->handle/g' src/util/u_gralloc/u_gralloc_fallback.c || true
    sed -i -E 's/([a-z_]+)->handle->/((const native_handle_t *)\1->handle)->/g' src/vulkan/runtime/vk_android.c || true
    sed -i 's/anb->handle->/((const native_handle_t *)anb->handle)->/g' src/vulkan/runtime/vk_android.c || true
    sed -i "/-Werror=gnu-empty-initializer/d" meson.build || true
}

build_android() {
    local cver=36
    [ -f "$NDK/aarch64-linux-android${cver}-clang" ] || cver=35
    [ -f "$NDK/aarch64-linux-android${cver}-clang" ] || cver=34
    [ -f "$NDK/aarch64-linux-android${cver}-clang" ] || die "Android aarch64 clang not found"

    mkdir -p "$WORKDIR/bin"
    ln -sf "$NDK/clang" "$WORKDIR/bin/cc"
    ln -sf "$NDK/clang++" "$WORKDIR/bin/c++"
    export PATH="$WORKDIR/bin:$NDK:$PATH"
    export CC=clang CXX=clang++ AR=llvm-ar RANLIB=llvm-ranlib STRIP=llvm-strip OBJDUMP=llvm-objdump OBJCOPY=llvm-objcopy

    local warn="-Wno-error -Wno-error=gnu-empty-initializer -Wno-gnu-empty-initializer -Wno-deprecated-declarations -Wno-incompatible-pointer-types-discards-qualifiers -Wno-incompatible-pointer-types"
    local opt="" ldopt="" ndebug=false
    if [ "$FLAVOR" = "perf" ]; then
        local cpu="-mcpu=oryon-1"
        if ! "$NDK/aarch64-linux-android${cver}-clang" -mcpu=oryon-1 -x c -c /dev/null -o /dev/null 2>/dev/null; then
            cpu="-march=armv8.6-a+i8mm+bf16 -mtune=cortex-x4"
            log "clang does not know oryon-1, falling back to: $cpu"
        fi
        opt="-O3 -fno-plt -flto=thin $cpu"
        ldopt="-flto=thin -Wl,-O2 -Wl,--gc-sections -Wl,--icf=safe"
        ndebug=true
    fi
    log "FLAVOR=$FLAVOR  OPT='$opt'  LDOPT='$ldopt'"
    export LDFLAGS="-fuse-ld=lld $ldopt"
    export CFLAGS="-D__ANDROID__ $opt $warn"
    export CXXFLAGS="-D__ANDROID__ $opt $warn"

    cat > android-aarch64.txt <<EOF
[binaries]
ar = '$NDK/llvm-ar'
c = ['$NDK/aarch64-linux-android${cver}-clang']
cpp = ['$NDK/aarch64-linux-android${cver}-clang++', '-fno-exceptions', '-fno-unwind-tables', '-fno-asynchronous-unwind-tables', '--start-no-unused-arguments', '-static-libstdc++', '--end-no-unused-arguments']
c_ld = '$NDK/ld.lld'
cpp_ld = '$NDK/ld.lld'
strip = '$NDK/llvm-strip'
pkg-config = ['env', 'PKG_CONFIG_LIBDIR=$NDK/pkg-config', '/usr/bin/pkg-config']

[host_machine]
system = 'android'
cpu_family = 'aarch64'
cpu = 'armv8'
endian = 'little'
EOF

    cat > native.txt <<EOF
[build_machine]
c = ['clang']
cpp = ['clang++']
ar = 'llvm-ar'
strip = 'llvm-strip'
c_ld = 'ld.lld'
cpp_ld = 'ld.lld'
system = 'linux'
cpu_family = 'x86_64'
cpu = 'x86_64'
endian = 'little'
EOF

    local build_dir="build-android-aarch64"
    rm -rf "$build_dir" "$INSTALL_DIR"
    meson setup "$build_dir" \
        --cross-file android-aarch64.txt \
        --native-file native.txt \
        --prefix "$INSTALL_DIR" \
        --libdir lib \
        -Dbuildtype=release \
        -Db_ndebug=$ndebug \
        -Dstrip=true \
        -Dplatforms=android \
        -Dvideo-codecs= \
        -Dplatform-sdk-version=36 \
        -Dandroid-stub=true \
        -Dgallium-drivers= \
        -Dvulkan-drivers=freedreno \
        -Dvulkan-beta=true \
        -Dfreedreno-kmds=kgsl \
        -Degl=disabled \
        -Dandroid-libbacktrace=disabled
    ninja -C "$build_dir" install
    [ -f "$INSTALL_DIR/lib/libvulkan_freedreno.so" ] || die "libvulkan_freedreno.so was not produced"
}

package_driver() {
    local mesa_sha header_version output_dir archive
    mesa_sha="$(git rev-parse --short=10 HEAD)"
    header_version="$(awk '$2 == "VK_HEADER_VERSION" {print $3}' include/vulkan/vulkan_core.h)"
    [[ "$header_version" =~ ^[0-9]+$ ]] || die "Invalid VK_HEADER_VERSION"
    output_dir="$INSTALL_DIR/lib"
    archive="$WORKDIR/$ZIP_NAME"
    local tuning="O3 + ThinLTO + Oryon tuning"
    [ "$FLAVOR" = "safe" ] && tuning="safe release flags"

    python3 - "$output_dir/meta.json" "$DRIVER_NAME" "$PKG_VERSION" "$mesa_sha" "$header_version" "$MIN_API" "$tuning" <<'PYMETA'
import json, sys
out, name, ver, sha, hdr, min_api, tuning = sys.argv[1:]
meta = {
    "schemaVersion": 1,
    "name": name,
    "description": f"Turnip A8xx for Snapdragon 8 Gen 5 / Adreno 829 (iQOO 15R). Mesa {sha}, {tuning}.",
    "author": "xamonxx",
    "packageVersion": ver,
    "vendor": "Mesa",
    "driverVersion": f"Vulkan 1.4.{hdr}",
    "minApi": int(min_api),
    "libraryName": "libvulkan_freedreno.so",
}
open(out, "w", encoding="utf-8").write(json.dumps(meta, indent=2) + "\n")
PYMETA

    rm -f "$archive"
    (cd "$output_dir" && zip -9 -q "$archive" libvulkan_freedreno.so meta.json)
    unzip -tq "$archive"
    log "Built: $archive"
    "$NDK/llvm-readelf" -d "$output_dir/libvulkan_freedreno.so" | grep NEEDED || true
    # Android Vulkan HAL / AdrenoTools load the driver through the exported hw_module symbol "HMI".
    "$NDK/llvm-nm" -D --defined-only "$output_dir/libvulkan_freedreno.so" | grep -wE "HMI" || die "Android HAL entry point HMI missing"
    "$NDK/llvm-nm" -D --defined-only "$output_dir/libvulkan_freedreno.so" | grep -E "vk_icd" || true
}

main() {
    check_deps
    prepare_workdir
    cd "$WORKDIR/mesa"
    log "Mesa: $(git rev-parse HEAD)  FLAVOR: $FLAVOR"
    apply_unified_patches
    apply_android_ndk_fixes
    build_android
    package_driver
}

main "$@"
