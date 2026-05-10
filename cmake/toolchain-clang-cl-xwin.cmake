# Cross-compile to MSVC ABI from Linux using clang-cl + lld-link, with the MSVC
# headers and import libs supplied by xwin (https://github.com/Jake-Shadle/xwin).
#
# Expected layout (the default `xwin --output ~/.xwin splat` produces this):
#   $XWIN_ROOT/crt/include/                MSVC C/C++ runtime headers
#   $XWIN_ROOT/crt/lib/x86_64/             MSVC C runtime import libs
#   $XWIN_ROOT/sdk/include/{ucrt,um,shared,winrt,cppwinrt}/   Windows SDK headers
#   $XWIN_ROOT/sdk/lib/{ucrt,um}/x86_64/   Windows SDK import libs
#
# Override XWIN_ROOT on the cmake command line if your splat lives elsewhere:
#   cmake -DXWIN_ROOT=/path/to/xwin ...

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR AMD64)

if(NOT DEFINED XWIN_ROOT)
    if(DEFINED ENV{XWIN_ROOT})
        set(XWIN_ROOT "$ENV{XWIN_ROOT}")
    elseif(EXISTS "/opt/wtasi-toolchain/xwin/crt/include")
        # Image-baked path (see .devcontainer/Dockerfile).
        set(XWIN_ROOT "/opt/wtasi-toolchain/xwin")
    else()
        # Fallback for hand-installed setups.
        set(XWIN_ROOT "$ENV{HOME}/.xwin")
    endif()
endif()

if(NOT EXISTS "${XWIN_ROOT}/crt/include")
    message(FATAL_ERROR
        "XWIN_ROOT='${XWIN_ROOT}' does not look like an xwin splat output. "
        "Run: xwin --accept-license --arch x86_64 splat --output \${HOME}/.xwin")
endif()

# Prefer user-local clang-cl (LLVM 14 from Debian); fall back to anything in PATH.
find_program(CLANG_CL_EXE
    NAMES clang-cl clang-cl-14 clang-cl-15 clang-cl-16 clang-cl-17 clang-cl-18
    PATHS /usr/lib/llvm-14/bin /usr/bin
)
find_program(LLD_LINK_EXE
    NAMES lld-link lld-link-14 lld-link-15 lld-link-16 lld-link-17 lld-link-18
    # Prefer the image-baked tool (see .devcontainer/Dockerfile) over any
    # hand-installed binary. /usr/bin is last because Debian's apt repo for
    # this image carries lldb-14 but not lld-14 — so it'll never match there.
    PATHS /opt/wtasi-toolchain/bin "$ENV{HOME}/.local/bin" /usr/lib/llvm-14/bin /usr/bin
    NO_DEFAULT_PATH
)
if(NOT LLD_LINK_EXE)
    # Fall back to the standard search if the curated paths missed it.
    find_program(LLD_LINK_EXE
        NAMES lld-link lld-link-14 lld-link-15 lld-link-16 lld-link-17 lld-link-18
    )
endif()
find_program(LLVM_RC_EXE
    NAMES llvm-rc llvm-rc-14
    PATHS /usr/lib/llvm-14/bin /usr/bin
)
find_program(LLVM_MT_EXE
    NAMES llvm-mt llvm-mt-14
    PATHS /usr/lib/llvm-14/bin /usr/bin
)

if(NOT CLANG_CL_EXE OR NOT LLD_LINK_EXE)
    message(FATAL_ERROR "clang-cl and/or lld-link not found in PATH or /usr/lib/llvm-*/bin.")
endif()

set(CMAKE_C_COMPILER   "${CLANG_CL_EXE}")
set(CMAKE_CXX_COMPILER "${CLANG_CL_EXE}")
set(CMAKE_LINKER       "${LLD_LINK_EXE}")
if(LLVM_RC_EXE)
    set(CMAKE_RC_COMPILER "${LLVM_RC_EXE}")
endif()
if(LLVM_MT_EXE)
    set(CMAKE_MT "${LLVM_MT_EXE}")
endif()

set(CMAKE_C_COMPILER_TARGET   x86_64-pc-windows-msvc)
set(CMAKE_CXX_COMPILER_TARGET x86_64-pc-windows-msvc)

# xwin only splats the redistributable (release) CRT. Force /MD everywhere so
# CMake's compiler test doesn't reach for msvcrtd.lib. This also matches the
# original .vcxproj which links the *-md MinHook variant in Release.
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL" CACHE STRING "" FORCE)

# clang-cl needs to find MSVC and SDK headers/libs explicitly when not running
# under a real VS environment. /imsvc adds an "external" include path (suppresses
# warnings from system headers); /libpath is forwarded to the linker.
set(_xwin_includes
    "/imsvc${XWIN_ROOT}/crt/include"
    "/imsvc${XWIN_ROOT}/sdk/include/ucrt"
    "/imsvc${XWIN_ROOT}/sdk/include/um"
    "/imsvc${XWIN_ROOT}/sdk/include/shared"
)
string(JOIN " " _xwin_includes_str ${_xwin_includes})

# The MSVC STL pinned in the latest xwin manifest demands Clang 19+; LLVM 14
# from Debian fails STL1000. The override is intended exactly for this case
# (https://learn.microsoft.com/en-us/cpp/overview/compiler-versions).
set(_xwin_defs "/D_ALLOW_COMPILER_AND_STL_VERSION_MISMATCH")

set(CMAKE_C_FLAGS_INIT   "${_xwin_includes_str} ${_xwin_defs}")
set(CMAKE_CXX_FLAGS_INIT "${_xwin_includes_str} ${_xwin_defs}")

set(_xwin_libpaths
    "/libpath:${XWIN_ROOT}/crt/lib/x86_64"
    "/libpath:${XWIN_ROOT}/sdk/lib/ucrt/x86_64"
    "/libpath:${XWIN_ROOT}/sdk/lib/um/x86_64"
)
string(JOIN " " _xwin_libpaths_str ${_xwin_libpaths})

set(CMAKE_EXE_LINKER_FLAGS_INIT    "${_xwin_libpaths_str}")
set(CMAKE_SHARED_LINKER_FLAGS_INIT "${_xwin_libpaths_str}")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${_xwin_libpaths_str}")

# Stop CMake from probing for or using the Linux host's headers/libs.
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
