# WorldTravel ASI Dev Container

Hardened reproducible dev environment for the WorldTravel ASI project — an
externally sourced GTA V mod whose original `.vcxproj`/`.sln` targets MSVC on
Windows. This container provides a sandbox-isolated alternative for code
intelligence, Claude-Code-driven analysis, and cross-compilation to a Windows
x64 DLL **without** running upstream build steps directly on the host.

The image bakes a native + cross-compile C/C++ toolchain (gcc/g++, clang +
clangd + clang-format + clang-tidy, lldb, gdb, cmake, ninja, ccache,
mingw-w64 for x86_64 Windows targets, wine64 for smoke tests), Node LTS (for
Context7 MCP only), and native Claude Code (with MCP servers, plugin
marketplaces, plugins and user-scope skills) into a single image. The
runtime is locked down with capability drops + an egress-only firewall, and
the user's `~/.claude` directory is persisted across rebuilds.

> **Threat model.** Treat upstream source as untrusted. Do not run produced
> binaries on the host without independent review. The container's bind mount
> is the only path the build can write to, and the egress firewall blocks
> exfiltration to anywhere outside the whitelist. Capability drops mean a
> rogue build script can't escalate or fiddle with kernel state.

## Files

| File                | Purpose                                                                                                                                                                  |
|---------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| `Dockerfile`        | Multi-stage image: clones third-party plugin-marketplace repos, then layers the native + cross-compile C++ toolchain and user-scope Claude Code skills onto the cpp base. |
| `devcontainer.json` | Devcontainer spec: features, security flags, workspace mount, port forwarding, post-create hook.                                                                          |
| `post-create.sh`    | One-shot provisioning run on first container start. Installs Claude Code, writes MCP/plugin config, pre-populates the plugin cache, then enables the egress firewall.    |
| `claude/skills/`    | User-scope Claude Code skills, staged into `/opt/aether-skills/skills/` at image build and synced into `~/.claude/skills/` on every `post-create.sh` run.                |

## Image build

Two stages:

1. **`skill-sources`** (`debian:bookworm-slim` + `git`): clones the two
   upstream plugin marketplaces (`anthropics/claude-plugins-official` and
   `pbakaus/impeccable`) in separate `RUN` layers. Each clone has a 3-attempt
   retry loop and is its own layer, so a transient DNS hiccup only invalidates
   the one repo that failed. Debian + glibc here instead of `alpine/git`
   because rootless BuildKit's resolver is intermittently flaky for
   back-to-back clones, and Alpine's musl libc makes that worse than glibc.
2. **Runtime stage** (`mcr.microsoft.com/devcontainers/cpp:1-debian-12`):
   adds the LLVM toolchain (clang, clangd, clang-format, clang-tidy, lldb,
   llvm), Ninja, ccache, mingw-w64 (`g++-mingw-w64-x86-64`,
   `gcc-mingw-w64-x86-64`, `mingw-w64-tools`), wine64, jq/curl/unzip,
   Python 3 + pipx, and Node LTS via NodeSource. Stages skills under
   `/opt/aether-skills/skills/` (post-create.sh syncs them into
   `~/.claude/skills/` on every start — see *Persistence*), and stages the
   two plugin marketplaces under `/opt/claude-marketplaces/`.

Everything that needs network egress happens at build time, so a
post-create-time firewall lockdown can be aggressive.

## Toolchain on `$PATH`

Everything is baked into the image directly — no `ghcr.io/devcontainers/features/*`
OCI features are used, because JetBrains Gateway's feature resolver fails
ghcr.io's anonymous auth scope.

| Tool                                  | Provided by                                            | Used for                                                                |
|---------------------------------------|--------------------------------------------------------|-------------------------------------------------------------------------|
| `gcc` / `g++` / `gdb`                 | `mcr.microsoft.com/devcontainers/cpp:1-debian-12` base | Native Linux C/C++ builds.                                              |
| `cmake`                               | base image                                             | Build system generator (write a `CMakeLists.txt` for ASI/lib targets).  |
| `clang` / `clang++` / `lldb`          | apt (`clang`, `lldb`)                                  | Alternative compiler/debugger; targets via `--target=x86_64-w64-windows-gnu`. |
| `clangd`                              | apt (`clangd`)                                         | LSP for IDE/Claude Code code intelligence.                              |
| `clang-format` / `clang-tidy`         | apt                                                    | Formatting + lint.                                                      |
| `ninja`                               | apt (`ninja-build`)                                    | Fast build executor (CMake's `-G Ninja`).                               |
| `ccache`                              | apt                                                    | Compile cache; symlink as `g++` etc. or set `CMAKE_<LANG>_COMPILER_LAUNCHER=ccache`. |
| `x86_64-w64-mingw32-gcc` / `-g++` / `-windres` / `-dlltool` | apt (`g++-mingw-w64-x86-64`, `gcc-mingw-w64-x86-64`, `mingw-w64-tools`) | Cross-compile to a Windows x64 `.dll` (the ASI target).        |
| `wine64`                              | apt                                                    | Optional smoke test of cross-compiled Windows binaries.                 |
| `node` / `npm`                        | NodeSource apt repo (`setup_lts.x`)                    | Only for `npx @upstash/context7-mcp` (the MCP server).                  |
| `python3` / `pipx`                    | Debian apt                                             | Glue tooling.                                                           |

### Building the project itself

The upstream sources ship as MSVC `.sln`/`.vcxproj` only — there is no
`CMakeLists.txt`. To build inside this container you have two practical paths:

**A — mingw-w64 (default, no extra setup).** Write a `CMakeLists.txt` that
targets a `SHARED` library, then configure with the bundled toolchain file:

```bash
cmake -S . -B build-mingw -G Ninja \
      -DCMAKE_TOOLCHAIN_FILE=/usr/share/mingw-w64/toolchain-x86_64-w64-mingw32.cmake \
      -DCMAKE_BUILD_TYPE=Release
cmake --build build-mingw
```

Caveat: the project's vendored dependencies include MSVC-format import
libraries (`WorldTravel/dependencies/lib/ScriptHookV.lib`,
`libMinHook-x64-v141-md.lib`). mingw's `ld` can usually consume COFF `.lib`
files but C runtime mismatches (MSVC `/MD` vs mingw's CRT) and C++ name
mangling differences may bite. For C-only / `extern "C"` symbols (which
ScriptHookV's API is) this typically works; for C++ symbols across the
boundary it does not. If you only need to call into ScriptHookV's C API and
keep your DLL exports as `extern "C"`, mingw is fine.

**B — `xwin` + `clang-cl` (true MSVC ABI).** For full MSVC ABI compatibility
(name mangling, `/MD` runtime, exception model), download the redistributable
MSVC headers and import libs once with [`xwin`](https://github.com/Jake-Shadle/xwin):

```bash
# inside the container, while the firewall is down (e.g. early in postCreate)
# or rebuild the image with this layer added:
xwin --accept-license splat --output ~/.xwin
```

Then point `clang-cl` at `~/.xwin/{crt,sdk}` via a CMake toolchain file
(`-DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
-DCMAKE_LINKER=lld-link` etc.). This is heavier (~3 GB) and requires
accepting Microsoft's redistributable license, so it's not baked in by
default. Add `dl.xwin.app` and `aka.ms` to the firewall whitelist if you
want xwin to work after the egress lockdown.

> If neither path works for the upstream `.vcxproj` as-is, the alternative is
> to keep the build on a Windows host and use this container only for code
> intelligence + Claude Code analysis. The hardened sandbox is still useful
> for *reading* untrusted source.

## Persistence

`devcontainer.json` mounts a Docker named volume on `/home/vscode/.claude` so
the following survive `Rebuild Container`:

- **Login** (`~/.claude/.credentials.json`) — no re-running `claude` and
  re-doing OAuth after every rebuild.
- **Sessions and conversations** (`~/.claude/projects/<workspace>/<sessionId>.jsonl`).
- **Auto-memory** (`~/.claude/projects/<workspace>/memory/`).
- **Settings + plugin enable state** (`~/.claude/settings.json`) — re-merged
  by `post-create.sh` on each run, so flag additions from the script land
  on top of preserved user changes rather than replacing them.
- **Plugin cache** (`~/.claude/plugins/cache/<marketplace>/<plugin>/<version>/`)
  — refreshed in place by `post-create.sh` on every start.

Volume names are static: `worldtravelasi-claude` for `~/.claude`, and
`worldtravelasi-src` for the workspace itself when JetBrains Gateway's
"Create Dev Container and Clone Resources" flow is used. Inspect / wipe via:

```bash
docker volume ls | grep worldtravelasi
docker volume rm worldtravelasi-claude worldtravelasi-src   # forces a clean re-init on next start
```

Static names instead of `${devcontainerId}` / `${localWorkspaceFolderBasename}`
are deliberate: variable substitution failed under both IntelliJ and CLion's
clone-resources flow (Gateway can't expand them before the local folder
exists), producing `Status 400: invalid mount config for type "volume":
invalid specification: destination can't be '/'`. If you regularly work on
multiple clones of this repo, pick a different `source=…` per clone before
opening additional copies.

Because the volume hides whatever the image had at `/home/vscode/.claude`
after first init, **skills are NOT baked directly into that path**. Instead
the Dockerfile stages them at `/opt/aether-skills/skills/`, and
`post-create.sh` runs `cp -a /opt/aether-skills/skills/. ~/.claude/skills/`
on every start. That way skill updates from a rebuilt image always reach the
user without having to wipe the volume; user-installed skills sitting next to
them in `~/.claude/skills/` are untouched (no `--delete`).

`~/.claude.json` (recent-projects metadata + per-project allowed tools) lives
in the home directory itself, *not* under `~/.claude/`, and is therefore
**not persisted** by the volume. `post-create.sh` re-creates it with the
Context7 MCP server entry on every start.

## JetBrains Gateway / CLion "Clone Resources" mode

CLion's **Create Dev Container and Clone Resources** flow clones the repo
into a Gateway-managed Docker volume *before* the container starts, so
`${localWorkspaceFolder}` is empty at mount-resolution time. With no
`workspaceFolder` / `workspaceMount` set, Gateway computes the workspace
mount target from that empty value and Docker rejects it with
`Status 400: invalid mount config for type "volume": invalid specification:
destination can't be '/'`.

This devcontainer therefore pins both fields explicitly:

```jsonc
"workspaceFolder": "/workspaces/worldTravelASI",
"workspaceMount":  "type=volume,source=worldtravelasi-src,target=/workspaces/worldTravelASI"
```

For the clone-resources flow to find this `.devcontainer/` at all, it must
be **committed and pushed** to the remote you point Gateway at — Gateway
clones from the remote, not from your local working tree. If your fork
doesn't have the devcontainer pushed, either push first or use the
local-clone path: open the existing checkout via **File → Open**, then
**Open Dev Container** from the gutter icon next to the `{` in
`devcontainer.json`. That mode bind-mounts the local folder and ignores the
clone-resources logic entirely (no push required, your local edits to the
devcontainer take effect immediately).

## Hardening

`runArgs` in `devcontainer.json`:

- `--cap-drop=ALL` then re-adds only what's actually needed
  (`CHOWN`, `SETGID`, `SETUID`, `KILL`, `NET_ADMIN`, `FOWNER`).
  `NET_ADMIN` is required because `post-create.sh` configures `iptables`.
- `--security-opt=no-new-privileges` blocks setuid escalations.
- Resource caps: `--memory=6g`, `--cpus=4`, `--pids-limit=2048`.
- `host.docker.internal` mapped to the host gateway for OAuth callback flows.

`post-create.sh` runs an iptables `OUTPUT DROP` and only allows traffic to
explicitly whitelisted hosts (resolved at firewall-setup time):

```
api.anthropic.com   claude.ai            github.com
raw.githubusercontent.com                objects.githubusercontent.com
registry.npmjs.org  context7.com         mcp.context7.com
```

No package-mirror egress at runtime: the C/C++ toolchain (gcc/g++, clang,
clangd, mingw-w64, ninja, ccache, cmake, wine64) is baked into the image at
build time, so apt is never used after first start. **If you need to add a
runtime build dependency, install it during image build** by extending the
Dockerfile rather than poking holes in the firewall — that keeps the
"untrusted source can't fetch a payload" property intact.

## Provisioning at first start (`post-create.sh`)

Runs in this order:

1. **Take ownership** of `$WORKSPACE_DIR` and `~/.claude`.
2. **Sync skills** from the image's `/opt/aether-skills/skills/` staging area
   into the volume-mounted `~/.claude/skills/` (see *Persistence* above).
3. **Install the Claude Code native binary** via the official installer.
4. **Pre-install Context7 MCP** (`@upstash/context7-mcp`) globally so `npx`
   resolves it offline once the firewall blocks the npm registry.
5. **Write user-scope MCP config** to `~/.claude.json` directly via `jq`. This
   sidesteps the `claude mcp add` CLI, which behaves inconsistently in a
   fresh, unauthenticated devcontainer.
6. **Provision plugins** (see below).
7. **Verify state** by listing MCP servers / marketplaces / plugins.
8. **Enable egress firewall**.

## MCP servers

Currently baked in: **Context7** (`@upstash/context7-mcp`).

To add another MCP server, append a `jq` invocation in the same block of
`post-create.sh`. If it ships via npm, also add a global install line in the
`Dockerfile` so `npx` finds it offline.

## Plugins

Both marketplaces are baked into the image at `/opt/claude-marketplaces/` and
`post-create.sh` pre-installs the enabled plugin deterministically. **No
`claude login` is required to provision them**, and the egress firewall does
not need to allow the upstream marketplace hosts at runtime.

For each target plugin, `post-create.sh`:

1. Reads the marketplace's `name` and the plugin's `source` from
   `<marketplace>/.claude-plugin/marketplace.json`.
2. Resolves the version. Precedence:
   `plugin.json.version` → marketplace entry `version` → 12-char git SHA →
   literal `unknown`.
3. Copies the plugin source to
   `~/.claude/plugins/cache/<marketplace-name>/<plugin-name>/<version>/`
   — the on-disk format the Claude Code CLI itself uses.
4. Appends an entry to `~/.claude/plugins/installed_plugins.json` so the CLI
   treats the plugin as already installed (schema observed in
   `anthropics/claude-code` issue #15642).
5. Registers the marketplace as a `directory` source under
   `extraKnownMarketplaces` in `~/.claude/settings.json` and toggles the
   plugin on under `enabledPlugins`.

Currently provisioned:

| Plugin       | Marketplace name | Marketplace source on disk     |
|--------------|------------------|--------------------------------|
| `impeccable` | `impeccable`     | `pbakaus/impeccable` (cloned)  |

The `aether-vendor-plugins` marketplace (renamed clone of
`anthropics/claude-plugins-official`) is also registered as a directory
source so you can `/plugin install <name>@aether-vendor-plugins` without
network access — but no plugin from it is enabled by default. The
Java/Kotlin/TypeScript LSP plugins from earlier variants of this devcontainer
were removed because they have nothing to do for this codebase. C/C++ code
intelligence comes from `clangd` directly on `$PATH`; configure your IDE's
clangd integration to use it, and provide a `compile_commands.json` for
sharper results (CMake emits one with `-DCMAKE_EXPORT_COMPILE_COMMANDS=ON`).

> **Why `aether-vendor-plugins`?** Claude Code reserves any marketplace name
> matching the regex `^(claude|anthropic)-?` for official Anthropic
> marketplaces ([anthropics/claude-code#46786][reserved-names]) — that
> rejects both the literal `claude-plugins-official` (only allowed for
> `github` sources from the `anthropics` org) and any `claude-…` /
> `anthropic-…` suffix variants. Since the egress firewall blocks GitHub at
> runtime we have to register the local clone as a `directory` source, so
> the `Dockerfile` rewrites the `.name` field of the cloned
> `marketplace.json` to `aether-vendor-plugins` (project-scoped, outside the
> reserved namespace). Plugin contents and IDs are unchanged.

[reserved-names]: https://github.com/anthropics/claude-code/issues/46786

### Brittleness disclaimer

`installed_plugins.json` is not part of Claude Code's documented public API.
If a future Claude Code release changes its schema, plugins may fail to load
on a freshly built container. Recovery path:

```bash
rm -rf ~/.claude/plugins/cache ~/.claude/plugins/installed_plugins.json
# inside `claude` after login:
/plugin install impeccable@impeccable
```

### Adding another plugin

1. Pre-clone the marketplace in the `skill-sources` stage of the `Dockerfile`
   and copy it under `/opt/claude-marketplaces/<name>/`.
2. Append a `<marketplace-dir>:<plugin-name>` entry to the `PLUGIN_TARGETS`
   array in `post-create.sh`. The script reads the marketplace name and
   plugin source from `marketplace.json` automatically.
3. If a runtime resource (e.g. an MCP server backing the plugin) needs
   network access, also extend the firewall allowlist in `post-create.sh`.

## Skills (user-scope, `claude/skills/`)

Drop user-scope skills under `claude/skills/`, one directory per skill, each
containing a `SKILL.md` plus any supporting files. Layout:

```
claude/skills/
├── my-skill/
│   ├── SKILL.md
│   └── ...
└── another-skill/
    └── SKILL.md
```

The `Dockerfile` copies this tree into the image staging area at
`/opt/aether-skills/skills/`. `post-create.sh` then runs
`cp -a /opt/aether-skills/skills/. ~/.claude/skills/` on every container
start — this indirection exists so the persisted `/home/vscode/.claude`
Docker volume (see *Persistence*) doesn't freeze skills at their first-init
state. After the next start every skill in this directory appears under
`~/.claude/skills/<name>/` inside the container and is auto-discovered by
Claude Code.

The Java-flavoured skills (`aether-javadoc-thorough`, `java-migration`,
`java-reviewer`, `jpa-patterns`, `maven-dependency-audit`,
`spring-boot-patterns`) and third-party frontend skill collections from
earlier variants of this devcontainer were removed because they have no
applicability to this codebase.

## Forwarded ports

| Port  | Use                            | Auto-forward |
|-------|--------------------------------|--------------|
| 8080  | Claude Code OAuth callback     | ignore       |

## Container env

| Variable                | Value                          | Purpose                                                                                |
|-------------------------|--------------------------------|----------------------------------------------------------------------------------------|
| `CLAUDE_CODE_SANDBOX`   | `true`                         | Tells Claude Code it's running in a sandboxed environment.                             |
| `NO_PROXY` / `no_proxy` | `localhost,127.0.0.1`          | Bypass any inherited proxy for loopback traffic.                                       |
| `WORKSPACE_DIR`         | `${containerWorkspaceFolder}`  | Resolved at container-create time so scripts don't hard-code a mount path.             |

## Re-running provisioning

`post-create.sh` is idempotent and safe to re-run. Useful when:

- you've edited `post-create.sh` itself and want to apply the changes
  without rebuilding the image;
- a Claude Code release shipped that broke the cache layout (run after
  `claude login` so `/plugin install` can also work);
- you've added a new plugin or marketplace and want to provision it now.

```bash
bash "$WORKSPACE_DIR/.devcontainer/post-create.sh"
```

`$WORKSPACE_DIR` is set by `containerEnv` in `devcontainer.json` and points
at whatever path the IDE chose to mount the source under (`/workspaces/worldTravelASI`
by default). Inside the container, use this variable rather than hard-coding
the path.

### Debug mode

`post-create.sh` is silent on the happy path, which makes it hard to tell
whether a long-running step (e.g. the Claude Code installer pulling the
binary) is hung or just slow. Set `DEVCONTAINER_DEBUG=1` to enable verbose
output:

```bash
# Ad-hoc re-run with full tracing:
DEVCONTAINER_DEBUG=1 bash "$WORKSPACE_DIR/.devcontainer/post-create.sh"
```

This activates `bash`'s `set -x` (every command is printed before
execution), drops `curl -s` so the installer download shows progress, and
runs the installer itself under `bash -x` so its internal steps are
visible too. To turn it on for the initial postCreate run, add
`"DEVCONTAINER_DEBUG": "1"` to `containerEnv` in `devcontainer.json`.

### Rebuilding the image

For changes to the `Dockerfile` itself (not just `post-create.sh`), use your
IDE's standard rebuild action — JetBrains Gateway "Rebuild and Restart
Container", VS Code "Dev Containers: Rebuild Container", or
`devcontainer build` from the CLI.
