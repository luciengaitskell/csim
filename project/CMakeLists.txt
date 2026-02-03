cmake_minimum_required(VERSION 3.21)
project(vehicle_sw LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Chrono
  COMPONENTS Vehicle          # must also be enabled in the superbuild CMakeLists.txt
  OPTIONAL_COMPONENTS Postprocess
  CONFIG REQUIRED
)

add_executable(vehicle_sw src/main.cpp)
target_link_libraries(vehicle_sw PRIVATE ${CHRONO_TARGETS})

if(DEFINED CHRONO_DATA_DIR)
  target_compile_definitions(vehicle_sw PRIVATE CHRONO_DATA_DIR="${CHRONO_DATA_DIR}")
else()
  message(WARNING "CHRONO_DATA_DIR is not defined by chrono in the superbuild.")
endif()
