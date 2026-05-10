# WorldTravel.asi — main gameplay mod (registers ScriptHookV scripts).

file(GLOB WORLDTRAVEL_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(WorldTravel SHARED ${WORLDTRAVEL_SOURCES})

set_target_properties(WorldTravel PROPERTIES
    PREFIX ""
    SUFFIX ".asi"
    OUTPUT_NAME "WorldTravel"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64/$<CONFIG>"
)

target_include_directories(WorldTravel PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include"
)

set(WORLDTRAVEL_DEPS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/lib")

target_link_libraries(WorldTravel PRIVATE worldtravel_asi_common)

# Use the locally-built MinHook target if available (lld-link cannot finalize
# the LTCG bitcode in the vendored *.lib). Fall back to the vendored .lib
# otherwise — that path is what MSVC link.exe uses on a Windows VS 2022 build.
if(WTASI_HAVE_LOCAL_MINHOOK)
    target_link_libraries(WorldTravel PRIVATE minhook)
else()
    target_link_libraries(WorldTravel PRIVATE
        $<$<CONFIG:Debug>:${WORLDTRAVEL_DEPS_LIB}/libMinHook-x64-v141-mdd.lib>
        $<$<NOT:$<CONFIG:Debug>>:${WORLDTRAVEL_DEPS_LIB}/libMinHook-x64-v141-md.lib>
    )
endif()

target_link_libraries(WorldTravel PRIVATE
    $<$<NOT:$<CONFIG:Debug>>:${WORLDTRAVEL_DEPS_LIB}/ScriptHookV.lib>
    kernel32 user32 gdi32 winspool comdlg32 advapi32
    shell32 ole32 oleaut32 uuid odbc32 odbccp32
    # GameVersion.cpp uses PathFindFileNameA. The vcxproj didn't list shlwapi
    # explicitly because MSVC's #pragma comment(lib, ...) elsewhere pulled it
    # in; lld-link doesn't see those, so list it here.
    shlwapi
)
