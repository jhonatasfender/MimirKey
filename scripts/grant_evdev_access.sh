#!/usr/bin/env bash
set -euo pipefail

# Usage: grant_evdev_access.sh <username>

UDEV_RULE="/etc/udev/rules.d/70-keymaps-evdev.rules"

echo "[keymaps] Writing udev rule to allow uaccess on input events: $UDEV_RULE"
printf 'KERNEL=="event*", SUBSYSTEM=="input", TAG+="uaccess"\n' | tee "$UDEV_RULE" >/dev/null

echo "[keymaps] Reloading udev rules"
udevadm control --reload-rules || true
udevadm trigger || true

if [ $# -ge 1 ] && [ -n "${1:-}" ]; then
  echo "[keymaps] Adding user '$1' to input group (if not already)"
  usermod -a -G input "$1" || true
fi

echo "[keymaps] Done. You may need to replug the device or restart the app."


