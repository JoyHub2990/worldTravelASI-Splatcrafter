cmake_minimum_required(VERSION 3.20)
project(worldTravelASI CXX)

if(NOT WIN32)
    message(FATAL_ERROR
        "worldTravelASI targets Windows x64 only. Configure with one of the bundled toolchains:\n"
        "  cmake -S . -B build -G Ninja "
        "-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-clang-cl-xwin.cmake "
        "-DCMAKE_BUILD_TYPE=Release\n"
        "  cmake -S . -B build -G Ninja "
        "-DCMAKE_TOOLCHAIN_FILE=cmake/toolchain-mingw-w64-x86_64.cmake "
        "-DCMAKE_BUILD_TYPE=Release")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "worldTravelASI is x64-only (got ${CMAKE_SIZEOF_VOID_P}-byte pointers).")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()

# True when invoked via clang-cl (MSVC ABI). Plain Clang/GCC for MinGW reports GNU-style.
set(WTASI_MSVC_ABI FALSE)
if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC" OR MSVC)
    set(WTASI_MSVC_ABI TRUE)
endif()

# All shared compile/link options for both .asi targets.
add_library(worldtravel_asi_common INTERFACE)
target_compile_definitions(worldtravel_asi_common INTERFACE
    _WINDOWS
    _USRDLL
    LC_EXPORTS
)

if(WTASI_MSVC_ABI)
    target_compile_options(worldtravel_asi_common INTERFACE
        /W3
        /EHsc
        /Zc:__cplusplus
        /D_CRT_SECURE_NO_WARNINGS
        /D_SCL_SECURE_NO_WARNINGS
        # Mirrors the original .vcxproj's -DisableSpecificWarnings 28159
        /wd28159
    )
    # /SUBSYSTEM:WINDOWS for the .asi DLL — matches the .vcxproj.
    target_link_options(worldtravel_asi_common INTERFACE
        /SUBSYSTEM:WINDOWS
    )
else()
    target_compile_options(worldtravel_asi_common INTERFACE
        -Wall
        # MSVC source uses implicit function-pointer-to-void* casts in MH_CreateHook
        # call sites; -fpermissive demotes those to warnings under GCC/Clang-GNU.
        -fpermissive
        -Wno-unknown-pragmas
        -Wno-attributes
    )
    target_link_options(worldtravel_asi_common INTERFACE
        -Wl,--subsystem,windows
        -static-libgcc
        -static-libstdc++
        -static
    )
endif()

# Optional: build MinHook from upstream source instead of using the vendored
# LTCG .lib. The vendored libMinHook-x64-v141-md.lib is MSVC bitcode, which
# only link.exe can finalize — lld-link rejects it. Point WTASI_LOCAL_MINHOOK
# at a checkout of https://github.com/TsudaKageyu/minhook (or the staging
# directory we set up under ~/.local/minhook-build) to bypass this.
set(WTASI_LOCAL_MINHOOK "$ENV{HOME}/.local/minhook-build" CACHE PATH
    "Optional path to a MinHook source tree with a CMakeLists.txt. \
Leave empty to use the vendored .lib.")

if(WTASI_LOCAL_MINHOOK AND EXISTS "${WTASI_LOCAL_MINHOOK}/CMakeLists.txt")
    message(STATUS "Building MinHook from source at ${WTASI_LOCAL_MINHOOK}")
    add_subdirectory("${WTASI_LOCAL_MINHOOK}" "${CMAKE_BINARY_DIR}/_minhook" EXCLUDE_FROM_ALL)
    set(WTASI_HAVE_LOCAL_MINHOOK TRUE)
else()
    set(WTASI_HAVE_LOCAL_MINHOOK FALSE)
endif()

# Optional: bypass the vendored mojito-wt-md.lib in favor of a re-archived
# variant. The shipped lib bundles MinHook v141 LTCG bitcode that lld-link
# rejects; the clean variant is the same lib with those LTCG members stripped.
# Generate it with the script in cmake/strip-mojito-ltcg.sh.
set(WTASI_LOCAL_MOJITO_WT "$ENV{HOME}/.local/mojito-wt-clean/mojito-wt-clean.lib"
    CACHE FILEPATH
    "Optional path to a re-archived mojito-wt-md.lib without LTCG members. \
Leave empty to use the vendored .lib.")

add_subdirectory(WorldTravel/src)
add_subdirectory(WorldTravelPatches/src)
