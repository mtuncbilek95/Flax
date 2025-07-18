cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# ----------------------- INCLUDES ---------------------------
include(SetupVulkan)
include(SetupGLFW)
# ------------------------------------------------------------

# ---------------------- FILE ADDITION -----------------------
file(GLOB_RECURSE SUBDIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${SUBDIRECTORIES})
add_library(Runtime STATIC ${SUBDIRECTORIES})
# ------------------------------------------------------------

# ------------------- INCLUDE DIRECTORIES --------------------
target_include_directories(Runtime PRIVATE "${CMAKE_SOURCE_DIR}/Source")
target_link_libraries(Runtime PUBLIC glm::glm fmt spdlog magic_enum stb assimp glfw EnTT::EnTT nlohmann_json tomlplusplus::tomlplusplus)

DefineGlmMacros(Runtime)
AddStbMacros(Runtime)
SetupVulkan(Runtime)
SetupGLFW(Runtime)

if(WIN32)
    include(SetupWindows)
    SetupWin32(Runtime)
endif()
# ------------------------------------------------------------