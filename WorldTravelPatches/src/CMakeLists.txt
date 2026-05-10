# WorldTravelPatches.asi — low-level binary patches for Liberty City content.

file(GLOB WORLDTRAVELPATCHES_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(WorldTravelPatches SHARED ${WORLDTRAVELPATCHES_SOURCES})

set_target_properties(WorldTravelPatches PROPERTIES
    PREFIX ""
    SUFFIX ".asi"
    OUTPUT_NAME "WorldTravelPatches"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64/$<CONFIG>"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/x64/$<CONFIG>"
)

target_include_directories(WorldTravelPatches PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/include"
)

target_compile_definitions(WorldTravelPatches PRIVATE
    _SILENCE_STDEXT_ARR_ITERS_DEPRECATION_WARNING
)

set(WTP_DEPS_LIB "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/lib")

target_link_libraries(WorldTravelPatches PRIVATE worldtravel_asi_common)

# Same LTCG situation as WorldTravel: prefer locally-built MinHook so lld-link
# can finalize the link. mojito-wt-md.lib *itself* is plain COFF, but it
# bundles MinHook v141 LTCG objs internally — WTASI_LOCAL_MOJITO_WT points at
# a re-archived copy with those stripped out.
if(WTASI_LOCAL_MOJITO_WT AND EXISTS "${WTASI_LOCAL_MOJITO_WT}")
    target_link_libraries(WorldTravelPatches PRIVATE "${WTASI_LOCAL_MOJITO_WT}")
else()
    target_link_libraries(WorldTravelPatches PRIVATE
        $<$<CONFIG:Debug>:${WTP_DEPS_LIB}/mojito-wt-mdd.lib>
        $<$<NOT:$<CONFIG:Debug>>:${WTP_DEPS_LIB}/mojito-wt-md.lib>
    )
endif()

if(WTASI_HAVE_LOCAL_MINHOOK)
    target_link_libraries(WorldTravelPatches PRIVATE minhook)
else()
    target_link_libraries(WorldTravelPatches PRIVATE
        $<$<CONFIG:Debug>:${WTP_DEPS_LIB}/libMinHook-x64-v141-mdd.lib>
        $<$<NOT:$<CONFIG:Debug>>:${WTP_DEPS_LIB}/libMinHook-x64-v141-md.lib>
    )
endif()

target_link_libraries(WorldTravelPatches PRIVATE
    $<$<NOT:$<CONFIG:Debug>>:version>
    kernel32 user32 gdi32 winspool comdlg32 advapi32
    shell32 ole32 oleaut32 uuid odbc32 odbccp32
)
