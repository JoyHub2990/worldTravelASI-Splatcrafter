# Stage 1: pull third-party plugin-marketplace repos at image-build time.
# Cloning here (not in postCreate) keeps the install reproducible, bakes the
# artifacts into the image, and lets post-create.sh pre-populate the plugin
# cache without ever needing `claude login` or runtime network egress.
#
# Debian + glibc instead of alpine/git: rootless BuildKit's DNS resolver is
# flaky for back-to-back clones, and glibc tolerates that better than musl.
# Cost: one extra apt layer (~50 MB), reused from the BuildKit cache.
#
# Each clone is its own RUN layer with a 3-attempt retry loop. Splitting per
# layer means a transient failure for one repo doesn't invalidate layers
# already cached. The trailing `exit 1` only fires when all 3 attempts failed.
FROM debian:bookworm-slim AS skill-sources
RUN apt-get update && apt-get install -y --no-install-recommends git ca-certificates \
    && rm -rf /var/lib/apt/lists/*
RUN for i in 1 2 3; do \
        git clone --depth 1 https://github.com/anthropics/claude-plugins-official.git /marketplaces/claude-plugins-official && exit 0; \
        echo "clone failed (attempt $i), retrying in 5s..."; sleep 5; \
    done; exit 1
RUN for i in 1 2 3; do \
        git clone --depth 1 https://github.com/pbakaus/impeccable.git /marketplaces/impeccable && exit 0; \
        echo "clone failed (attempt $i), retrying in 5s..."; sleep 5; \
    done; exit 1

FROM mcr.microsoft.com/devcontainers/cpp:1-debian-12

# Native + cross-compile C/C++ toolchain on top of the cpp devcontainer base.
# The base ships gcc/g++, gdb, cmake and build-essential for native Linux builds;
# we layer on:
#   * LLVM toolchain (clang, clangd, clang-format, clang-tidy, lldb, llvm) —
#     needed for code intelligence (clangd LSP) and as an alternative compiler
#     to gcc, useful when targeting Windows via clang's `--target=` flag.
#   * mingw-w64 (`g++-mingw-w64-x86-64`, `gcc-mingw-w64-x86-64`,
#     `mingw-w64-tools`) — the Windows x64 cross-compiler used to build the
#     `.dll` on Linux without needing Windows or MSVC. Together with `wine64`
#     this also lets you smoke-test a built DLL inside the container.
#     Note: this project's deps include MSVC-format `.lib` import libraries
#     (`ScriptHookV.lib`, `libMinHook-x64-v141-md.lib`) that may not link
#     cleanly with mingw's ld. If you hit ABI issues, the recommended path is
#     to add `xwin` (https://github.com/Jake-Shadle/xwin) on top to download
#     the redistributable MSVC headers + import libs and build with `clang-cl`
#     via CMake. We don't bake xwin in by default because it requires a
#     license-acceptance flag at run time and is large.
#   * Ninja, ccache — faster builds.
#   * wine64 — optional smoke-test runner for the produced DLL. Headless,
#     no X server needed for typical ASI/library testing.
#   * Node LTS — needed only by the Context7 MCP server (`npx
#     @upstash/context7-mcp`); no application code in this repo uses Node.
#
# IMPORTANT: this devcontainer is for code intelligence + sandbox-isolated
# builds of an externally sourced C/C++ project. The hardened runtime (cap
# drops + egress firewall + named volumes) is the primary line of defense if
# the upstream code is later found to be malicious. Build outputs land in
# the workspace bind mount; copy artifacts out explicitly rather than running
# them on the host.
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    jq \
    unzip \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    lldb \
    llvm \
    ninja-build \
    ccache \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    mingw-w64-tools \
    wine64 \
    python3 \
    python3-pip \
    python3-venv \
    pipx \
    && rm -rf /var/lib/apt/lists/*

# Node.js LTS via NodeSource. We install Node here rather than via the
# devcontainers/features/node OCI feature because JetBrains Gateway's feature
# resolver fails to substitute the image into ghcr.io's anonymous auth scope —
# the token request goes out as `repository:user/image:pull` and ghcr.io 403s.
# Baking Node at image-build time also matches the rest of this Dockerfile and
# means the runtime egress firewall doesn't need to allow ghcr.io.
RUN curl -fsSL https://deb.nodesource.com/setup_lts.x | bash - && \
    apt-get install -y --no-install-recommends nodejs && \
    rm -rf /var/lib/apt/lists/*

# Bake user-scope Claude Code skills into the image. Drop new skills under
# .devcontainer/claude/skills/ in the repo; post-create.sh syncs them into
# ~/.claude/skills/ on every container start.
#
# Why a /opt staging area instead of copying straight into ~/.claude/skills/?
# devcontainer.json mounts a named Docker volume on /home/vscode/.claude so
# sessions, login (.credentials.json), settings, plugin cache, and auto-memory
# survive container rebuilds. Once that volume is initialized from the image
# (first start), any subsequent rebuild's ~/.claude/skills/ contents would be
# masked by the volume's frozen copy. Staging in /opt/aether-skills/skills/
# keeps the image's skill set outside the volume scope; post-create.sh copies
# from here into the volume on every start, so skill changes from a rebuilt
# image always reach the user without needing to wipe the volume.
RUN install -d -o vscode -g vscode \
        /home/vscode/.claude \
        /opt/aether-skills \
        /opt/aether-skills/skills
COPY --chown=vscode:vscode claude/skills/ /opt/aether-skills/skills/

# anthropics/claude-plugins-official and pbakaus/impeccable: the two GitHub-hosted
# upstream marketplaces are baked in so post-create.sh can pre-populate
# ~/.claude/plugins/cache/ without hitting GitHub at runtime (the egress firewall
# would block the clone after post-create finishes anyway).
RUN install -d -o vscode -g vscode /opt/claude-marketplaces
COPY --from=skill-sources --chown=vscode:vscode /marketplaces/claude-plugins-official/ \
        /opt/claude-marketplaces/claude-plugins-official/
# Rewrite .name to a non-reserved value. Claude Code rejects any marketplace
# name matching ^(claude|anthropic)-? when registered as a `directory` source
# (anthropics/claude-code#46786) — that namespace is reserved for official
# `github`-source marketplaces from the anthropics org. We have to use a
# directory source because the egress firewall blocks GitHub at runtime, so
# we pick a project-scoped name outside the reserved range. Plugin contents
# and IDs are unchanged; post-create.sh reads .name from this file.
RUN tmp=$(mktemp) && \
    jq '.name = "aether-vendor-plugins"' \
        /opt/claude-marketplaces/claude-plugins-official/.claude-plugin/marketplace.json > "$tmp" && \
    mv "$tmp" /opt/claude-marketplaces/claude-plugins-official/.claude-plugin/marketplace.json && \
    chown vscode:vscode /opt/claude-marketplaces/claude-plugins-official/.claude-plugin/marketplace.json

COPY --from=skill-sources --chown=vscode:vscode /marketplaces/impeccable/ \
        /opt/claude-marketplaces/impeccable/

# ----- Cross-compile toolchain for the worldTravelASI Windows target ---------
# Goal: produce a self-contained image where `cmake --build` from within the
# container can link both .asi files without any further user setup. Everything
# under /opt/wtasi-toolchain/ is image-baked, survives container rebuilds, and
# stays out of the user's home directory (which is a Docker volume).
#
# Three pieces matter here:
#   1. xwin binary + splatted MSVC SDK headers/import libs (Microsoft VS 2019
#      manifest, picked because Clang 14 from Debian 12 cannot parse the C++23
#      STL in newer manifests).
#   2. lld-link from Debian's lld-14 .deb (the base image's apt sources don't
#      carry lld-14, only lldb-14 — so we fetch the .deb directly and lift the
#      binaries out).
#   3. MinHook upstream source. The vendored libMinHook-x64-v141-md.lib in the
#      project bundles MSVC LTCG bytecode that lld-link rejects, so we compile
#      MinHook from source ourselves; the CMakeLists.txt for that build lives
#      at .devcontainer/wtasi-toolchain-files/minhook-CMakeLists.txt.
#
# The mojito-wt-md.lib cleaning step (cmake/strip-mojito-ltcg.sh) is the only
# piece NOT baked here, because it needs the workspace volume that mounts at
# runtime. post-create.sh runs the script during container creation and writes
# the cleaned lib to /opt/wtasi-toolchain/mojito-wt-clean.lib.
RUN install -d -o vscode -g vscode \
        /opt/wtasi-toolchain \
        /opt/wtasi-toolchain/bin \
        /opt/wtasi-toolchain/minhook \
        /opt/wtasi-toolchain/minhook/src \
        /opt/wtasi-toolchain/minhook/src/hde \
        /opt/wtasi-toolchain/minhook/include

# xwin: download the static-musl release binary. We don't run `xwin splat`
# here yet — it's done in a later layer after the Microsoft license terms are
# explicitly accepted via --accept-license.
RUN curl -fsSL \
        https://github.com/Jake-Shadle/xwin/releases/download/0.9.0/xwin-0.9.0-x86_64-unknown-linux-musl.tar.gz \
        -o /tmp/xwin.tar.gz \
    && tar -xzf /tmp/xwin.tar.gz -C /tmp \
    && install -m 755 /tmp/xwin-0.9.0-x86_64-unknown-linux-musl/xwin /opt/wtasi-toolchain/bin/xwin \
    && rm -rf /tmp/xwin.tar.gz /tmp/xwin-0.9.0-x86_64-unknown-linux-musl

# lld-link: fetch the lld-14 deb from Debian's archive and lift just the
# linker binaries. The whole package is ~10 MB; we keep lld, ld.lld, lld-link.
RUN curl -fsSL \
        http://ftp.debian.org/debian/pool/main/l/llvm-toolchain-14/lld-14_14.0.6-12_amd64.deb \
        -o /tmp/lld-14.deb \
    && dpkg-deb -x /tmp/lld-14.deb /tmp/lld-extract \
    && install -m 755 /tmp/lld-extract/usr/lib/llvm-14/bin/lld-link \
                      /tmp/lld-extract/usr/lib/llvm-14/bin/ld.lld \
                      /tmp/lld-extract/usr/lib/llvm-14/bin/lld \
                      /opt/wtasi-toolchain/bin/ \
    && rm -rf /tmp/lld-14.deb /tmp/lld-extract

# MinHook source. Shallow clone to a temp dir, copy only the files we use into
# the toolchain tree, drop the rest. The CMakeLists.txt comes from the repo
# (committed under .devcontainer/wtasi-toolchain-files/) so the build is
# reproducible without depending on whatever upstream MinHook ships.
RUN for i in 1 2 3; do \
        git clone --depth=1 https://github.com/TsudaKageyu/minhook.git /tmp/minhook && exit 0; \
        echo "minhook clone failed (attempt $i), retrying in 5s..."; sleep 5; \
    done; exit 1
RUN cp /tmp/minhook/src/buffer.c \
        /tmp/minhook/src/buffer.h \
        /tmp/minhook/src/hook.c \
        /tmp/minhook/src/trampoline.c \
        /tmp/minhook/src/trampoline.h \
        /opt/wtasi-toolchain/minhook/src/ \
    && cp /tmp/minhook/src/hde/hde64.c \
        /tmp/minhook/src/hde/hde64.h \
        /tmp/minhook/src/hde/pstdint.h \
        /tmp/minhook/src/hde/table64.h \
        /opt/wtasi-toolchain/minhook/src/hde/ \
    && cp /tmp/minhook/include/MinHook.h /opt/wtasi-toolchain/minhook/include/ \
    && cp /tmp/minhook/LICENSE.txt /opt/wtasi-toolchain/minhook/LICENSE.txt \
    && rm -rf /tmp/minhook \
    && chown -R vscode:vscode /opt/wtasi-toolchain/minhook
COPY --chown=vscode:vscode wtasi-toolchain-files/minhook-CMakeLists.txt \
        /opt/wtasi-toolchain/minhook/CMakeLists.txt

# Splat the MSVC SDK. Manifest 16 = VS 2019 era; Clang 14 cannot parse the
# newer (manifest 17) STL because of C++23 features it doesn't support. The
# splat is ~600 MB so this layer is large, but the trade-off is "instant
# build-ready container" vs "5-10 min download on every fresh container".
# --accept-license is acknowledged here on behalf of whoever rebuilds this
# image; the project README explains the implications.
RUN /opt/wtasi-toolchain/bin/xwin --accept-license --manifest-version 16 --arch x86_64 \
        --cache-dir /tmp/xwin-cache \
        splat --output /opt/wtasi-toolchain/xwin \
    && rm -rf /tmp/xwin-cache \
    && chown -R vscode:vscode /opt/wtasi-toolchain/xwin
