#!/usr/bin/env bash
# Re-archive WorldTravelPatches/dependencies/lib/mojito-wt-md.lib without the
# bundled libminhook v141 LTCG bytecode members, so lld-link can finalize it.
#
# The vendored mojito-wt-md.lib bundles a copy of MinHook.lib v141 compiled with
# /GL (whole-program optimization). Those members are LTCG bitcode that only
# MSVC link.exe + c2.dll can finalize. The mojito-wt code itself is plain COFF
# — once we strip the LTCG members, the lib links cleanly with lld-link, and
# we just need to provide MinHook from a separate source build (see
# ~/.local/minhook-build).
#
# Usage:
#   cmake/strip-mojito-ltcg.sh
#   # produces ~/.local/mojito-wt-clean/mojito-wt-clean.lib
#
# Then configure CMake; WTASI_LOCAL_MOJITO_WT defaults to that path.

set -euo pipefail

LIB_IN="${1:-$(dirname "$0")/../WorldTravelPatches/dependencies/lib/mojito-wt-md.lib}"
LIB_OUT="${2:-$HOME/.local/mojito-wt-clean/mojito-wt-clean.lib}"
WORK_DIR="$(mktemp -d)"
trap 'rm -rf "$WORK_DIR"' EXIT

if [[ ! -f "$LIB_IN" ]]; then
    echo "Input lib not found: $LIB_IN" >&2
    exit 1
fi

for tool in llvm-ar llvm-lib; do
    if ! command -v "$tool" >/dev/null; then
        echo "Required tool '$tool' not found in PATH (install llvm-14 or newer)." >&2
        exit 1
    fi
done

echo "Reading members from $LIB_IN ..."
llvm-ar t "$LIB_IN" > "$WORK_DIR/members.txt"
total=$(wc -l < "$WORK_DIR/members.txt")
echo "  $total members."

cd "$WORK_DIR"
kept=0
while IFS= read -r member; do
    case "$member" in
        # MinHook v141 LTCG bytecode + d3d11 import descriptors that lld-link
        # can't consume. Stripped here, supplied separately at link time.
        *libminhook*|*d3d11*) continue ;;
    esac
    out="$(echo "$member" | sed 's|.*[\\/]||')"
    [[ -z "$out" ]] && continue
    llvm-ar p "$LIB_IN" "$member" > "$out"
    kept=$((kept + 1))
done < "$WORK_DIR/members.txt"
echo "  $kept members kept."

mkdir -p "$(dirname "$LIB_OUT")"
echo "Re-archiving to $LIB_OUT ..."
llvm-lib /OUT:"$LIB_OUT" /MACHINE:X64 *.obj
echo "Done."
echo
echo "Now configure CMake (the default WTASI_LOCAL_MOJITO_WT picks this path up)."
