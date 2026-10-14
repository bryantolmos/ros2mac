cmake_minimum_required(VERSION 3.20)
project(macos_sim
    VERSION 0.1.0
    DESCRIPTION "macOS sim dir build"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SIM_TARGET "robot_sim")

# resolve homebrew prefix dynamically
execute_process(
    COMMAND brew --prefix
    OUTPUT_VARIABLE BREW_PREFIX
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
if(NOT BREW_PREFIX)
    set(BREW_PREFIX "/opt/homebrew")
endif()

file(GLOB MUJOCO_CASK_PATHS "${BREW_PREFIX}/Caskroom/mujoco/*")

find_library(MUJOCO_FRAMEWORK
    NAMES mujoco
    PATHS
        ${MUJOCO_CASK_PATHS}
        /Applications/MuJoCo.app/Contents/Frameworks
        ~/Library/Frameworks
        /Library/Frameworks
    REQUIRED
)

message(STATUS "Found MuJoCo framework at: ${MUJOCO_FRAMEWORK}")

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(${SIM_TARGET}
    src/main.cpp
    # src/sim_engine.cpp
    # src/visualizer.cpp
)

target_include_directories(${SIM_TARGET} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MUJOCO_FRAMEWORK}/Headers
)

target_link_libraries(${SIM_TARGET} PRIVATE
    ${MUJOCO_FRAMEWORK}
    glfw
    OpenGL::GL
)

# inject the caskroom directory into the binary runtime load path (LC_RPATH)
get_filename_component(MUJOCO_FRAMEWORK_DIR "${MUJOCO_FRAMEWORK}" DIRECTORY)
set_target_properties(${SIM_TARGET} PROPERTIES
    MACOSX_RPATH TRUE
    INSTALL_RPATH_USE_LINK_PATH TRUE
    BUILD_RPATH "${MUJOCO_FRAMEWORK_DIR};/opt/homebrew/lib"
    INSTALL_RPATH "@loader_path/../lib;${MUJOCO_FRAMEWORK_DIR}"
)

target_compile_options(${SIM_TARGET} PRIVATE
    -Wall
    -Wextra
    -O3
    -Wconversion
    -Wno-deprecated-declarations
)