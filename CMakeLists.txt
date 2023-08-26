cmake_minimum_required(VERSION 3.0.0)
project(Snake VERSION 0.1.0 LANGUAGES C CXX)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)
find_package(PNG REQUIRED)

include_directories(${X11_INCLUDE_DIR} ${OPENGL_INCLUDE_DIRS} ${PNG_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/include" )

add_executable(Snake main.cpp)

target_link_libraries(Snake ${OPENGL_LIBRARIES} ${X11_LIBRARIES} ${PNG_LIBRARIES})

