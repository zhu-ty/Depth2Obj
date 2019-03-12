
message(
  "
           
               ┌─┐       ┌─┐
            ┌──┘ ┴───────┘ ┴──┐
            │                 │
            │       ───       │
            │  ─┬┘       └┬─  │
            │                 │
            │       ─┴─       │
            │                 │
            └───┐         ┌───┘
                │         │
                │         │
                │         │
                │         └──────────────┐
                │                        │
                │                        ├─┐
                │                        ┌─┘    
                │                        │
                └─┐  ┐  ┌───────┬──┐  ┌──┘         
                  │ ─┤ ─┤       │ ─┤ ─┤         
                  └──┴──┘       └──┴──┘ 
                With the bless of the legend,
                there shall not be any bug.
      
  ")

cmake_minimum_required (VERSION 3.10)
set(name "VideoRender")
project ( ${name} )
# cmake module
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
# c++ version
set (CMAKE_CXX_STANDARD 11)
set (WIN_LIB_PATH "E:/libs")
#OpenCV
if ( WIN32 )
  set(OpenCV_DIR ${WIN_LIB_PATH}/OpenCV/x64/vc14/lib)
  set(OpenCV_CONFIG_PATH ${WIN_LIB_PATH}/OpenCV)
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# opengl
find_package(OpenGL REQUIRED)
if ( WIN32 )
  set(GLFW_INCLUDE_PATH ${WIN_LIB_PATH}/OpenGL/include/GLFW)
  set(GLFW_LIBRARY ${WIN_LIB_PATH}/OpenGL/lib/glfw3.lib )
  set(GLEW_INCLUDE_PATH ${WIN_LIB_PATH}/OpenGL/include)
  set(GLEW_LIBRARY_DEBUG ${WIN_LIB_PATH}/OpenGL/lib/glew32d.lib)
  set(GLEW_LIBRARY_RELEASE ${WIN_LIB_PATH}/OpenGL/lib/glew32.lib)
  set(GLM_INCLUDE_PATH ${WIN_LIB_PATH}/OpenGL/include)
endif()
include_directories(
${GLFW_INCLUDE_PATH}
${GLEW_INCLUDE_PATH}
${GLM_INCLUDE_PATH})
#SubFolder
set (GENOBJ_FOLDER ${PROJECT_SOURCE_DIR}/GenObjCpp)
set (GENOBJ_HEADER
  ${GENOBJ_FOLDER}/ObjectGenerator.h
  ${GENOBJ_FOLDER}/SysUtil.hpp
)
set (GENOBJ_SOURCE
  ${GENOBJ_FOLDER}/ObjectGenerator.cpp
)
include_directories(${GENOBJ_FOLDER})
#MainFolder
file (GLOB this_headers_h "./*.h")
file (GLOB this_headers_hpp "./*.hpp")
set (MY_HEADER_FILES
${this_headers_h}
${this_headers_hpp}
${GENOBJ_HEADER}
)
file (GLOB this_headers_cpp "./*.cpp")
set (MY_SOURCE_FILES
${this_headers_cpp}
${GENOBJ_SOURCE}
)
include_directories(
  ${PROJECT_SOURCE_DIR}  
)

#Link&Exec
add_executable(${name}
  ${MY_HEADER_FILES}
  ${MY_SOURCE_FILES}
  )
target_link_libraries(${name}
  ${OpenCV_LIBS}
  ${OPENGL_LIBRARY}
  ${GLFW_LIBRARY}
  debug  ${GLEW_LIBRARY_DEBUG}
  optimized  ${GLEW_LIBRARY_RELEASE}
  )
