cmake_minimum_required(VERSION 3.20)
project(ncnn_imgui_demo)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include("cmake/output_dir.cmake")
include("cmake/deps.cmake")

add_executable(ncnn_imgui_demo
  ${CMAKE_SOURCE_DIR}/app.cpp
  ${CMAKE_SOURCE_DIR}/image_render.cpp
  ${CMAKE_SOURCE_DIR}/image_io.cpp
  ${CMAKE_SOURCE_DIR}/ncnn_yolox.cpp
  

  ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
  ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
  ${IMGUI_DIR}/imgui.cpp
  ${IMGUI_DIR}/imgui_draw.cpp
  ${IMGUI_DIR}/imgui_tables.cpp
  ${IMGUI_DIR}/imgui_widgets.cpp
  ${IMGUI_DIR}/imgui_demo.cpp
)
target_link_libraries(ncnn_imgui_demo
  ncnn
  glfw
  ${OPENGL_LIBRARIES}
  ${OpenCV_LIBS}
  portable_file_dialogs
)

if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
  add_definitions(-DGL_SILENCE_DEPRECATION)
endif()

message(STATUS "OPENGL_LIBRARIES: ${OPENGL_LIBRARIES}")

# copy ncnn model files
add_custom_command(TARGET ncnn_imgui_demo
  POST_BUILD

  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/assets/yolox.bin ${CMAKE_BINARY_DIR}/
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/assets/yolox.param ${CMAKE_BINARY_DIR}/

  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/assets/frontground.png ${CMAKE_BINARY_DIR}/
)
