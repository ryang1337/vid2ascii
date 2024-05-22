cmake_minimum_required(VERSION 3.26)
project(vid2ascii)

set(CMAKE_OSX_DEPLOYMENT_TARGET "14.0")

find_package(OpenGL REQUIRED)

include_directories(/usr/local/include /usr/local/Cellar)
link_directories(/usr/local/lib /usr/local/Cellar)

add_executable(vid2ascii vid2ascii.cc)
target_link_libraries(vid2ascii gif ${OPENGL_LIBRARIES} glfw glew)
