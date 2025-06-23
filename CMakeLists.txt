cmake_minimum_required(VERSION 3.15)
project(opengl_starter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Dependencies
include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLAD (header-only, pre-generated)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG        v0.1.36
)
FetchContent_MakeAvailable(glad)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# Executable
add_executable(opengl_starter main.cpp)

# Include and link
target_include_directories(opengl_starter PRIVATE
  ${glad_SOURCE_DIR}/include
)

target_link_libraries(opengl_starter
  glfw
  glad
)

if (APPLE)
  target_link_libraries(opengl_starter "-framework OpenGL")
elseif (UNIX)
  find_package(OpenGL REQUIRED)
  target_link_libraries(opengl_starter OpenGL::GL)
endif()
