#!/bin/bash
# Build sm64coopdx (PersonalUpdates) inside a memory-capped user unit.
#
#   personal/build.sh            incremental build, 10 jobs
#   personal/build.sh clean      remove build/ first
#   JOBS=6 personal/build.sh     fewer parallel compiles
#
# The full build log is written to build/personal-build.log and its last lines are printed.
# The ROM must be linked as baserom.us.z64 in the repo root (it is git-ignored).
set -euo pipefail
cd "$(dirname "$0")/.."
REPO=$PWD
JOBS=${JOBS:-10}
LOG=$REPO/build/personal-build.log

[[ -e baserom.us.z64 ]] || { echo "missing baserom.us.z64 in $REPO" >&2; exit 1; }
[[ ${1:-} == clean ]] && rm -rf build
mkdir -p build

export XDG_RUNTIME_DIR=${XDG_RUNTIME_DIR:-/run/user/$(id -u)}
export DBUS_SESSION_BUS_ADDRESS=${DBUS_SESSION_BUS_ADDRESS:-unix:path=$XDG_RUNTIME_DIR/bus}

# The unit runs make through bash so its output lands in the log instead of the journal.
set +e
systemd-run --user --unit="sm64coopdx-build-$$" --collect --wait --quiet \
    -p MemoryMax=20G -p MemorySwapMax=0 -p WorkingDirectory="$REPO" \
    nice -n 10 bash -c "make -j$JOBS > '$LOG' 2>&1"
STATUS=$?
set -e

grep -E "error:|Error [0-9]+" "$LOG" | head -40 || true
tail -3 "$LOG"
echo "build exit status: $STATUS"
exit $STATUS
