# World Travel ASI Fork

   Fork of [WorldTravelTeam/ASI](https://github.com/WorldTravelTeam/ASI) with fixes for GTA V build 3751+ (April 2026).

   The original mod was discontinued in January 2025. This fork applies fixes
   needed to keep the mod working with newer game builds.

   ## Changes from upstream

   - **`WorldTravelPatches/src/PopZones.h`**: Loosened pattern 2's jump-distance
     byte from `EB 5F` to `EB ?` for GTA V build 3751+. Rockstar inserted ~4 bytes
     of code in the branch target between the original and current build, shifting
     the jump distance.
   - **`WorldTravelPatches/src/PopZones.h`**: Added per-pattern diagnostic logging
     using `sprintf_s`-based message formatting (the bundled spdlog version's `{}`
     placeholders fail silently). Each pattern now logs match count and patch result.
   - **`WorldTravelPatches/src/PopZones.h`**: Replaced fail-fast assertions with
     graceful logging and skip behavior. A broken pattern no longer kills the entire
     init thread, so subsequent patches still install.
   - **`WorldTravel/src/Minimap.cpp`**: Same diagnostic + graceful-failure treatment
     applied to both minimap pattern hooks.

   ## Known issues (not fixed)

   - **Live HUD minimap renders transparent in Liberty City.** Pause map is fine.
     Cause: the `v_fakelibertycity` interior trigger present in the January 2025
     binary release was removed from the May 2025 source. Restoring requires
     reverse-engineering the original binary's use of that string and reimplementing
     the missing function.
   - **LSPDFR logs "No game zone found" warnings at LC coordinates.** Cause: GTA V's
     named-zone system appears to be hardcoded in the executable; LCPP includes no
     named-zone metadata for Liberty City. Fix would require an ASI-level hook on
     `GAMEPLAY::GET_NAME_OF_ZONE` to map LC coordinates to zone codes from
     popzone.ipl. Cosmetic-only impact.

   ## Building

   ### Visual Studio 2022 (original path)

   1. Open `WorldTravelPatches/src/WorldTravelPatches.sln` (or
      `WorldTravel/src/WorldTravel.sln`) in Visual Studio 2022
   2. Right-click solution → Retarget Solution → pick installed Windows SDK
   3. Set configuration to Release / x64
   4. Build (Ctrl+Shift+B)
   5. Output ASI lands in `bin/x64/Release/`

   ### CLion / CMake (Linux dev container)

   The repo ships CMake build files alongside the SLN so CLion can load and
   navigate the project. Two cross-toolchains are wired up:

   - **`cmake/toolchain-clang-cl-xwin.cmake`** — clang-cl + lld-link targeting
     the MSVC ABI, with headers and import libs supplied by
     [`xwin`](https://github.com/Jake-Shadle/xwin). This is the path that can
     produce a real `.asi` from Linux.
   - **`cmake/toolchain-mingw-w64-x86_64.cmake`** — mingw-w64 + posix threads.
     Compiles the source for static analysis and IDE feature coverage; the
     final link fails on the MSVC-only `.lib`s, so this is editor-only.

   #### Setup

   Inside the bundled dev container nothing extra is needed: the Dockerfile
   bakes xwin, the splatted MSVC SDK (VS 2019 manifest, picked because Clang 14
   from Debian 12 can't parse the newer STL), `lld-link`, and a MinHook source
   tree into `/opt/wtasi-toolchain/`. `post-create.sh` runs
   `cmake/strip-mojito-ltcg.sh` once on container creation to write the
   LTCG-stripped mojito-wt to `/opt/wtasi-toolchain/mojito-wt-clean.lib`.
   Container rebuilds redo the strip step but reuse the image-baked toolchain.

   Outside the dev container (e.g. on a plain Linux host you maintain
   yourself), reproduce the same layout under `~/.local/`:

   ```bash
   # xwin
   curl -sSfL https://github.com/Jake-Shadle/xwin/releases/download/0.9.0/xwin-0.9.0-x86_64-unknown-linux-musl.tar.gz \
       | tar -xz -C /tmp
   install -m 755 /tmp/xwin-0.9.0-x86_64-unknown-linux-musl/xwin ~/.local/bin/xwin
   xwin --accept-license --manifest-version 16 --arch x86_64 \
       --cache-dir ~/.xwin-cache splat --output ~/.xwin

   # lld-link from the Debian lld-14 .deb
   curl -sSfL -o /tmp/lld-14.deb \
       http://ftp.debian.org/debian/pool/main/l/llvm-toolchain-14/lld-14_14.0.6-12_amd64.deb
   dpkg-deb -x /tmp/lld-14.deb ~/.local/lld-extract
   ln -sf ~/.local/lld-extract/usr/lib/llvm-14/bin/lld-link ~/.local/bin/lld-link

   # MinHook source (vendored libMinHook-x64-v141-md.lib is MSVC LTCG bitcode
   # that lld-link can't consume; building from upstream yields plain COFF)
   git clone --depth=1 https://github.com/TsudaKageyu/minhook.git /tmp/minhook
   mkdir -p ~/.local/minhook-build/src/hde ~/.local/minhook-build/include
   cp /tmp/minhook/src/{buffer.c,buffer.h,hook.c,trampoline.c,trampoline.h} ~/.local/minhook-build/src/
   cp /tmp/minhook/src/hde/{hde64.c,hde64.h,pstdint.h,table64.h} ~/.local/minhook-build/src/hde/
   cp /tmp/minhook/include/MinHook.h ~/.local/minhook-build/include/
   cp .devcontainer/wtasi-toolchain-files/minhook-CMakeLists.txt ~/.local/minhook-build/CMakeLists.txt

   # Strip the LTCG bundled MinHook out of mojito-wt-md.lib
   ./cmake/strip-mojito-ltcg.sh
   ```

   The CMake toolchain file probes `/opt/wtasi-toolchain/` first and falls
   back to `~/.local/` automatically, so either layout works from the same
   `cmake -DCMAKE_TOOLCHAIN_FILE=…` invocation.

   #### Configure and build

   ```bash
   cmake -S . -B build -G Ninja \
       -DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-clang-cl-xwin.cmake \
       -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

   In CLion: **File → Open** the repo, accept the CMake project, then in
   **Settings → Build, Execution, Deployment → CMake** add a profile with
   the toolchain file in *CMake options*:
   `-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-clang-cl-xwin.cmake`.

   #### What builds

   Both targets land in `build/bin/x64/Release/` after `cmake --build build`:

   - **`WorldTravel.asi`** — links via clang-cl + xwin + locally-built MinHook.
   - **`WorldTravelPatches.asi`** — links the same way, plus the LTCG-stripped
     mojito-wt produced by `cmake/strip-mojito-ltcg.sh`.

   The mojito-wt cleaning step is the non-obvious one: the vendored
   `mojito-wt-md.lib` ships mojito's own (plain COFF) code alongside an
   internal copy of MinHook v141 compiled with `/GL`. Only the bundled MinHook
   members are LTCG bytecode; lld-link rejects them outright. The helper
   script extracts every mojito-own `.obj`, drops the libminhook + d3d11
   members, and re-archives the result with `llvm-lib`. We then satisfy
   MinHook from upstream source instead.
