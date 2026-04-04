cmake_minimum_required(VERSION 3.18)
project(cadiback LANGUAGES CXX)

# Generate config.hpp (VERSION, GITID, BUILD) into the build directory.
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" CADIBACK_VERSION)
string(STRIP "${CADIBACK_VERSION}" CADIBACK_VERSION)

find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        OUTPUT_VARIABLE CADIBACK_GITID
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
        RESULT_VARIABLE _git_result)
    if(NOT _git_result EQUAL 0)
        set(CADIBACK_GITID "unknown")
    endif()
else()
    set(CADIBACK_GITID "unknown")
endif()
set(CADIBACK_BUILD "${CMAKE_CXX_COMPILER}")

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/config.hpp.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.hpp")

# ---- CaDiCaL (cmake-based) ----
set(cadical_DIR "" CACHE PATH "cadical install/build prefix (contains lib/cmake/cadical/). Auto-resolved if empty.")

include(FetchContent)
if(NOT TARGET cadical)
    if(cadical_DIR)
        find_package(cadical CONFIG REQUIRED HINTS "${cadical_DIR}")
        message(STATUS "CaDiCaL: using pre-built at ${cadical_DIR}")
    elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../cadical/CMakeLists.txt")
        add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../cadical"
                         "${CMAKE_CURRENT_BINARY_DIR}/cadical-build")
        message(STATUS "CaDiCaL: using sibling repo")
    else()
        FetchContent_Declare(cadical
            GIT_REPOSITORY https://github.com/meelgroup/cadical
            GIT_TAG        master
            GIT_SHALLOW    TRUE)
        FetchContent_MakeAvailable(cadical)
        message(STATUS "CaDiCaL: fetched from GitHub")
    endif()
endif()

add_library(cadiback STATIC cadiback.cpp)

set_target_properties(cadiback PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Public API header is in include/; config.hpp is in the build directory.
target_include_directories(cadiback
    PUBLIC  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include/cadiback>
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_compile_features(cadiback PUBLIC cxx_std_17)

target_compile_definitions(cadiback PRIVATE
    $<$<NOT:$<CONFIG:Debug>>:NDEBUG>)

# Linking cadical PUBLIC gives cadiback (and its users) access to cadical's
# public headers (cadical.hpp, resources.hpp, version.hpp from src/).
target_link_libraries(cadiback PUBLIC cadical)

# ---- Install / cmake package export ----
include(GNUInstallDirs)
install(TARGETS cadiback EXPORT cadibackTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

# Export for build-tree usage (lets dependents point CMAKE_PREFIX_PATH at build/)
export(EXPORT cadibackTargets
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cadibackTargets.cmake")
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/cadiback"
    FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
install(EXPORT cadibackTargets
    FILE cadibackTargets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cadiback")
file(GENERATE
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/cadibackConfig.cmake"
    CONTENT "include(\"\${CMAKE_CURRENT_LIST_DIR}/cadibackTargets.cmake\")\n")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cadibackConfig.cmake"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/cadiback")
