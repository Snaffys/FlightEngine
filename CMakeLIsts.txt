cmake_minimum_required(VERSION 3.26)

file(
  DOWNLOAD
  https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
  EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMFindPackage(
  NAME mr-math
  GITHUB_REPOSITORY 4j-company/mr-math
  GIT_TAG fix/cmake
)

CPMFindPackage(
  NAME glfw3
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.4
  OPTIONS
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BULID_DOCS OFF"
)

project(FlightSimulator)
set(CMAKE_CXX_STANDARD 17)
add_executable(${CMAKE_PROJECT_NAME} "src/Main.cpp" "src/Application.cpp" "src/Camera.cpp" "src/FBO.cpp" "src/glad.c" "src/IBO.cpp" "src/Shader.cpp" "src/stb.cpp" "src/Texture.cpp" "src/VAO.cpp" "src/VBO.cpp" "src/Window.cpp")
target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/headers/" "${CMAKE_CURRENT_SOURCE_DIR}/Libraries/include")
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE mr-math-lib glfw)

target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${mr-math_SRC_DIR}/include)
