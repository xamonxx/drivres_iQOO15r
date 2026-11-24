#!/usr/bin/env python3
"""xamonxx 0002 — per-mode runtime (FPS limiter + ADPF hint + debug evaluation log).

Run from the Mesa source root (pinned commit c5ef4bdfab). Steps:
  1. copy patches/xamonxx/src/xamonxx.{c,h} into src/vulkan/runtime/
  2. add xamonxx.c to the vulkan lite runtime (linked whole into Turnip)
  3. call xamonxx_on_present() once per present on both present paths:
       - vk_common_QueueSignalReleaseImageANDROID  (Android loader swapchain)
       - wsi_common_queue_present                  (Mesa WSI, e.g. headless/wrappers)

Behaviour is chosen at compile time with -DXAMONXX_MODE_ID (0 base, 1 cool60,
2 monster90, 3 monster120) and -DXAMONXX_DEBUG=1 for the debug zip.
The version string comes from env XAMONXX_VERSION (default "dev").
"""
import os
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
RUNTIME = Path("src/vulkan/runtime")


def must_replace(text, old, new, what):
    if text.count(old) != 1:
        sys.exit(f"xamonxx runtime: anchor for {what} found {text.count(old)}x (expected 1)")
    return text.replace(old, new, 1)


def edit(path, fn):
    p = Path(path)
    src = p.read_text(encoding="utf-8")
    if "xamonxx" in src:
        print(f"xamonxx runtime: {path} already patched")
        return
    p.write_bytes(fn(src).encode("utf-8"))  # keep LF line endings on every host


# 1) module sources
shutil.copyfile(HERE / "src" / "xamonxx.h", RUNTIME / "xamonxx.h")
c_src = (HERE / "src" / "xamonxx.c").read_text(encoding="utf-8")
version = os.environ.get("XAMONXX_VERSION", "dev").replace('"', "")
c_src = must_replace(c_src, '#define XAMONXX_VERSION "dev"', f'#define XAMONXX_VERSION "{version}"', "version macro")
(RUNTIME / "xamonxx.c").write_bytes(c_src.encode("utf-8"))

# 2) build wiring
edit(RUNTIME / "meson.build", lambda s: must_replace(
    s,
    "  'vk_ycbcr_conversion.c',\n)\n\nvulkan_lite_runtime_deps",
    "  'vk_ycbcr_conversion.c',\n  'xamonxx.c',\n)\n\nvulkan_lite_runtime_deps",
    "lite runtime file list"))


# 3a) Android loader present path
def patch_android(s):
    s = must_replace(s, '#include "vk_util.h"\n', '#include "vk_util.h"\n#include "xamonxx.h"\n', "vk_android include")
    return must_replace(
        s,
        "   VkResult result;\n\n   if (waitSemaphoreCount == 0) {\n      *pNativeFenceFd = -1;",
        "   VkResult result;\n\n   xamonxx_on_present();\n\n   if (waitSemaphoreCount == 0) {\n      *pNativeFenceFd = -1;",
        "QueueSignalReleaseImageANDROID body")


edit(RUNTIME / "vk_android.c", patch_android)


# 3b) Mesa WSI present path
def patch_wsi(s):
    s = must_replace(s, '#include "vk_util.h"\n', '#include "vk_util.h"\n#include "xamonxx.h"\n', "wsi_common include")
    anchor = ("   struct vk_device *dev = queue->base.device;\n"
              "   uint32_t current_frame = p_atomic_fetch_add(&dev->current_frame, 1);\n")
    return must_replace(s, anchor, anchor + "   xamonxx_on_present();\n", "wsi_common_queue_present body")


edit("src/vulkan/wsi/wsi_common.c", patch_wsi)

print(f"xamonxx runtime: applied (version {version})")
