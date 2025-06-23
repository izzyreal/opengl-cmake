cmake_minimum_required(VERSION 3.15)
project(opengl_starter LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        0.9.9.8
)
FetchContent_MakeAvailable(glm)

# GLAD (pre-generated loader)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/arrayfire/glad.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(glad)

# Executable
add_executable(opengl_starter main.cpp)

# GLAD static lib from included source
add_library(glad_static STATIC
  "${glad_SOURCE_DIR}/src/glad.c"
)
target_include_directories(glad_static PUBLIC
  "${glad_SOURCE_DIR}/include"
)

# Link libs
target_link_libraries(opengl_starter
  glad_static
  glfw
)

target_include_directories(opengl_starter PRIVATE
  "${glad_SOURCE_DIR}/include"
  "${glm_SOURCE_DIR}"
)

# System OpenGL linking
if (APPLE)
  target_link_libraries(opengl_starter "-framework OpenGL")
elseif (UNIX)
  find_package(OpenGL REQUIRED)
  target_link_libraries(opengl_starter OpenGL::GL)
endif()
