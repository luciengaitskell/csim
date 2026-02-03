cmake_minimum_required(VERSION 3.21)
project(csim_superbuild LANGUAGES NONE)

include(ExternalProject)

# -------- Modules
set(CSIM_ENABLE_CHRONO_VEHICLE ON  CACHE BOOL "Build Chrono Vehicle module")
set(CSIM_ENABLE_CHRONO_SENSOR  OFF CACHE BOOL "Build Chrono Sensor module")
set(CSIM_ENABLE_CHRONO_VSG     OFF CACHE BOOL "Build Chrono VSG module")
set(CSIM_ENABLE_CHRONO_IRRLICHT OFF CACHE BOOL "Build Chrono Irrlicht module")
# --------

set(CHRONO_SRC_DIR ${CMAKE_SOURCE_DIR}/third_party/chrono)

set(DEPS_ROOT         ${CMAKE_BINARY_DIR}/_deps)
set(CHRONO_BUILD_DIR  ${DEPS_ROOT}/chrono-build)
set(CHRONO_INSTALL_DIR ${DEPS_ROOT}/chrono-install)

if(NOT EXISTS "${CHRONO_SRC_DIR}/CMakeLists.txt")
  message(FATAL_ERROR "Chrono submodule not found at ${CHRONO_SRC_DIR}. Run: git submodule update --init --recursive")
endif()

ExternalProject_Add(chrono
  SOURCE_DIR  ${CHRONO_SRC_DIR}
  BINARY_DIR  ${CHRONO_BUILD_DIR}
  INSTALL_DIR ${CHRONO_INSTALL_DIR}

  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DBUILD_TESTING:BOOL=OFF

    -DCH_ENABLE_MODULE_VEHICLE:BOOL=${CSIM_ENABLE_CHRONO_VEHICLE}
    -DCH_ENABLE_MODULE_SENSOR:BOOL=${CSIM_ENABLE_CHRONO_SENSOR}
    -DCH_ENABLE_MODULE_VSG:BOOL=${CSIM_ENABLE_CHRONO_VSG}
    -DCH_ENABLE_MODULE_IRRLICHT:BOOL=${CSIM_ENABLE_CHRONO_IRRLICHT}

  # Force the install step to run when building this external project
  INSTALL_COMMAND ${CMAKE_COMMAND} --build <BINARY_DIR> --target install

  LOG_CONFIGURE 1
  LOG_BUILD 1
  LOG_INSTALL 1
  LOG_OUTPUT_ON_FAILURE 1
)

ExternalProject_Add(project-external
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/project
  BINARY_DIR ${CMAKE_BINARY_DIR}/project-build
  DEPENDS chrono

  CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=ON

    # directory containing chrono-config.cmake
    -DChrono_DIR:PATH=${CHRONO_INSTALL_DIR}/lib/cmake/Chrono

  INSTALL_COMMAND ""
)

# Export compile_commands.json to the top build directory
add_custom_target(compile_commands ALL
  DEPENDS project-external
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
          ${CMAKE_BINARY_DIR}/project-build/compile_commands.json
          ${CMAKE_BINARY_DIR}/compile_commands.json
)

