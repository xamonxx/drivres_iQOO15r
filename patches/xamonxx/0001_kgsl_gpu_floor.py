#!/usr/bin/env python3
"""xamonxx 0001 — KGSL GPU frequency floor (power constraint) for Turnip.

Run from the Mesa source root AFTER the upstream StevenMXZ patches
(needs kgsl_context_create() from a6xx/0002-kgsl-context-recovery.patch).

What it does
  Each KGSL draw context Turnip creates gets KGSL_CONTEXT_PWR_CONSTRAINT and a
  KGSL_PROP_PWR_CONSTRAINT / KGSL_CONSTRAINT_PWRLEVEL request:
    level 0      -> disabled (identical behaviour to V0.1)
    level 1      -> KGSL_CONSTRAINT_PWR_MAX  (GPU held at max allowed clock while submitting)
    level 2..100 -> floor at N% of max GPU clock (KGSL_CONSTRAINT_PWR_PERC_*)
  The kernel applies the constraint per submitted command batch and lets it expire
  when the game stops submitting. Thermal mitigation still clamps the maximum level,
  so hardware/thermal protection is untouched.

Default level is compiled in with -DXAMONXX_GPU_FLOOR=<n>; it can be overridden at
runtime with the env var / Mesa option XAMONXX_GPU_FLOOR (number or "max").
If the kernel rejects the request (older KGSL without PERC support), the driver logs a
warning and continues without a floor.
"""
import sys
from pathlib import Path

path = Path("src/freedreno/vulkan/tu_knl_kgsl.cc")
src = path.read_text(encoding="utf-8")

if "xamonxx_apply_gpu_floor" in src:
    print("xamonxx gpu floor: already applied")
    sys.exit(0)


def must_replace(text, old, new, what):
    if text.count(old) != 1:
        sys.exit(f"xamonxx gpu floor: anchor for {what} found {text.count(old)}x (expected 1)")
    return text.replace(old, new, 1)


HELPERS = r'''
/* ---- xamonxx naga8gen5: KGSL GPU frequency floor -------------------------- */
#ifndef XAMONXX_GPU_FLOOR
#define XAMONXX_GPU_FLOOR 0
#endif

static int
xamonxx_gpu_floor_level(void)
{
   static int cached = -1;
   if (cached >= 0)
      return cached;

   int level = XAMONXX_GPU_FLOOR;
   const char *opt = os_get_option("XAMONXX_GPU_FLOOR");
   if (opt && *opt)
      level = !strcmp(opt, "max") ? KGSL_CONSTRAINT_PWR_MAX : atoi(opt);
   if (level < 0 || level > KGSL_CONSTRAINT_PWR_PERC_MAX)
      level = 0;

   cached = level;
   if (level)
      mesa_logi("xamonxx naga8gen5: KGSL GPU floor level %d (%s)", level,
                level == KGSL_CONSTRAINT_PWR_MAX ? "max clock" : "percent of max");
   return level;
}

static void
xamonxx_apply_gpu_floor(int fd, uint32_t context_id, int level)
{
   if (!level)
      return;

   struct kgsl_device_constraint_pwrlevel pwr = {
      .level = (unsigned int) level,
   };
   struct kgsl_device_constraint constraint = {
      .type = KGSL_CONSTRAINT_PWRLEVEL,
      .context_id = context_id,
      .data = &pwr,
      .size = sizeof(pwr),
   };
   struct kgsl_device_getproperty prop = {
      .type = KGSL_PROP_PWR_CONSTRAINT,
      .value = &constraint,
      .sizebytes = sizeof(constraint),
   };

   if (safe_ioctl(fd, IOCTL_KGSL_SETPROPERTY, &prop))
      mesa_logw("xamonxx naga8gen5: KGSL rejected GPU floor %d: %s", level,
                strerror(errno));
}
/* -------------------------------------------------------------------------- */

static int
kgsl_context_create('''

# 1) helpers right before kgsl_context_create()
src = must_replace(src, "\nstatic int\nkgsl_context_create(", HELPERS, "kgsl_context_create definition")

# 2) request the constraint flag on the context
old_ioctl = "   int ret = safe_ioctl(dev->physical_device->local_fd, IOCTL_KGSL_DRAWCTXT_CREATE, &req);"
new_ioctl = ("   const int xamonxx_floor = xamonxx_gpu_floor_level();\n"
             "   if (xamonxx_floor)\n"
             "      req.flags |= KGSL_CONTEXT_PWR_CONSTRAINT;\n\n" + old_ioctl)
src = must_replace(src, old_ioctl, new_ioctl, "DRAWCTXT_CREATE ioctl")

# 3) attach the constraint to the new context
old_id = "   *context_id = req.drawctxt_id;\n"
new_id = old_id + ("   xamonxx_apply_gpu_floor(dev->physical_device->local_fd,\n"
                   "                           req.drawctxt_id, xamonxx_floor);\n")
src = must_replace(src, old_id, new_id, "context id assignment")

# 4) headers for atoi/strcmp
src = must_replace(src, "#include <stdint.h>\n", "#include <stdint.h>\n#include <stdlib.h>\n#include <string.h>\n", "stdint include")

path.write_bytes(src.encode("utf-8"))  # keep LF line endings on every host
print("xamonxx gpu floor: applied (default level from XAMONXX_GPU_FLOOR macro)")
