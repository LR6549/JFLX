cmake_minimum_required(VERSION 3.20)
project(JFLXLIBS VERSION 1.0.1 LANGUAGES CXX)

# ════════════════════════════════════════════════════════════════════════════
# Global build configuration — single source of truth for every JFLXLIBS
# component. Individual components must not repeat compiler, include, or
# linker configuration; they are defined once here.
# ════════════════════════════════════════════════════════════════════════════

# ── Platform/build configuration module (roots, SDL dependency lists) ──────
if(WIN32 OR ANDROID)
    list(APPEND CMAKE_MODULE_PATH "F:/Dropbox/Dropbox/CompilerUndScripts/CMake/modules")
elseif(UNIX)
    list(APPEND CMAKE_MODULE_PATH "/home/lr6549/Dropbox/CompilerUndScripts/CMake/Modules")
endif()

include(buildConfig)

# ── MSVC runtime — static, matching the per-library configurations ─────────
if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# ════════════════════════════════════════════════════════════════════════════
# Options
# ════════════════════════════════════════════════════════════════════════════
option(JFLXLIBS_BUILD_TESTS    "Build the JFLXLIBS smoke tests" OFF)
option(JFLXLIBS_WARNINGS       "Enable recommended compiler warnings" ON)
option(JFLXLIBS_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

# ════════════════════════════════════════════════════════════════════════════
# Common build settings, shared by all components (INTERFACE target)
# ════════════════════════════════════════════════════════════════════════════
add_library(jflx_common INTERFACE)
add_library(JFLXLIBS::common ALIAS jflx_common)

target_compile_features(jflx_common INTERFACE cxx_std_20)

# Consumers link SDL statically (matches buildConfig's default static linking).
target_compile_definitions(jflx_common INTERFACE SDL_STATIC)

# Public headers live in include/; OS + header-only library roots come from
# buildConfig (OS_INCLUDE_ROOT supplies the SDL3 headers, HPP_LIBS_ROOT the
# external header-only libraries; JFLXLIBS' own headers live in include/).
# The installed interface embeds the SDL include root as well, so an installed
# package is consumable on its own (same machine-absolute-path convention as
# buildConfig.cmake itself).
target_include_directories(jflx_common INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${OS_INCLUDE_ROOT}>
        $<BUILD_INTERFACE:${HPP_LIBS_ROOT}>
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:${OS_INCLUDE_ROOT}>
)

# ── Warning flags — applied to JFLXLIBS components only, never to consumers ─
add_library(jflx_warnings INTERFACE)
if(MSVC)
    target_compile_options(jflx_warnings INTERFACE /W4)
    if(JFLXLIBS_WARNINGS_AS_ERRORS)
        target_compile_options(jflx_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(jflx_warnings INTERFACE -Wall -Wextra)
    if(JFLXLIBS_WARNINGS_AS_ERRORS)
        target_compile_options(jflx_warnings INTERFACE -Werror)
    endif()
endif()

# ── Helper: one component = one object library with shared settings ────────
function(jflx_add_component TARGET)
    add_library(${TARGET} OBJECT ${ARGN})
    target_link_libraries(${TARGET} PRIVATE jflx_common)
    if(JFLXLIBS_WARNINGS)
        target_link_libraries(${TARGET} PRIVATE jflx_warnings)
    endif()
    # PIC so the archive can later be linked into shared libraries.
    set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE ON)
    message(STATUS "JFLXLIBS component: ${TARGET}")
endfunction()

# ════════════════════════════════════════════════════════════════════════════
# Components (object libraries)
#
# SDL3 keeps its global window/renderer configuration in a translation unit so
# the pointers are defined exactly once; the mathematics noise generation and
# binomial-deviation implementations are compiled for the same reason.
# Every other sub-library (sorting, dataStructures, cryptography, positioning,
# strings, utilities, network) is header-only and ships in include/, reached
# through its own global header and JFLXLIBS.hpp.
# ════════════════════════════════════════════════════════════════════════════
jflx_add_component(jflx_sdl3_config
        src/JFLX_SDL3_CONFIG.cpp
        include/SDL3/JFLX_SDL3_CONFIG.hpp
)

jflx_add_component(jflx_sdl3_audio
        src/AudioHandler.cpp
        include/SDL3/AudioHandler.hpp
)

jflx_add_component(jflx_sdl3_text
        src/TextRenderer.cpp
        include/SDL3/TextRenderer.hpp
)

jflx_add_component(jflx_sdl3_texture
        src/TextureHandler.cpp
        include/SDL3/TextureHandler.hpp
)

jflx_add_component(jflx_math_noise_generation
        src/noiseGeneration.cpp
        include/mathematics/noiseGeneration.hpp
)

jflx_add_component(jflx_math_bsa
        src/bsa.cpp
        include/mathematics/bsa.hpp
)

# ════════════════════════════════════════════════════════════════════════════
# Global static archive — collects every component's object files
# ════════════════════════════════════════════════════════════════════════════
add_library(JFLXLIBS STATIC
        $<TARGET_OBJECTS:jflx_sdl3_config>
        $<TARGET_OBJECTS:jflx_sdl3_audio>
        $<TARGET_OBJECTS:jflx_sdl3_text>
        $<TARGET_OBJECTS:jflx_sdl3_texture>
        $<TARGET_OBJECTS:jflx_math_noise_generation>
        $<TARGET_OBJECTS:jflx_math_bsa>
)
add_library(JFLXLIBS::JFLXLIBS ALIAS JFLXLIBS)

# Propagate compile settings (C++20, includes, SDL_STATIC) to consumers.
target_link_libraries(JFLXLIBS PUBLIC jflx_common)

# SDL dependencies actually used by the components, taken from buildConfig's
# dependency lists (audio → mixer, text → ttf, texture → image; base SDL3 for
# all). Static consumers need the transitive codec/system libraries too.
set(_JFLXLIBS_DEPS ${SDL3_DEPS} ${SDL3_image_DEPS} ${SDL3_mixer_DEPS} ${SDL3_ttf_DEPS})
list(REMOVE_DUPLICATES _JFLXLIBS_DEPS)
# Static archives are resolved left-to-right, and libfreetype.a (pulled in
# via SDL3_ttf) references png/z symbols that appear earlier in the list.
# Passing the list twice is the standard idiom to resolve such back-edges.
target_link_libraries(JFLXLIBS PUBLIC
        ${_JFLXLIBS_DEPS}
        ${_JFLXLIBS_DEPS}
)
unset(_JFLXLIBS_DEPS)

# Library search path for consumers (vcpkg roots come from buildConfig).
target_link_directories(JFLXLIBS PUBLIC
        $<BUILD_INTERFACE:${OS_LIB_ROOT}>
        $<INSTALL_INTERFACE:${OS_LIB_ROOT}>
)

message(STATUS "JFLXLIBS: building global static archive lib${PROJECT_NAME}.a")

# ════════════════════════════════════════════════════════════════════════════
# Tests (optional)
# ════════════════════════════════════════════════════════════════════════════
if(JFLXLIBS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ════════════════════════════════════════════════════════════════════════════
# Installation
# ════════════════════════════════════════════════════════════════════════════
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS JFLXLIBS jflx_common
        EXPORT JFLXLIBSTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT JFLXLIBSTargets
        NAMESPACE JFLXLIBS::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/JFLXLIBS
)

configure_package_config_file(
        cmake/JFLXLIBSConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/JFLXLIBSConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/JFLXLIBS
)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/JFLXLIBSConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/JFLXLIBSConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/JFLXLIBSConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/JFLXLIBS
)
