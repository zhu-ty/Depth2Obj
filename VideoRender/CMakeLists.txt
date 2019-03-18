
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
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

#OpenCV
if ( WIN32 )
    set(OpenCV_DIR ${WIN_LIB_PATH}/OpenCV/x64/vc14/lib)
    set(OpenCV_CONFIG_PATH ${WIN_LIB_PATH}/OpenCV)
endif()
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
# cuda
find_package(CUDA)
set(CUDA_NVCC_FLAGS
    ${CUDA_NVCC_FLAGS}
#    -O3 
	-gencode=arch=compute_61,code=\"sm_61,compute_61\"
    )
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
#SubFolder_GenObj
set (GENOBJ_FOLDER ${PROJECT_SOURCE_DIR}/GenObjCpp)
set (GENOBJ_HEADER
    ${GENOBJ_FOLDER}/ObjectGenerator.h
    ${GENOBJ_FOLDER}/SysUtil.hpp
)
set (GENOBJ_SOURCE
    ${GENOBJ_FOLDER}/ObjectGenerator.cpp
)
include_directories(${GENOBJ_FOLDER})
#SubFolder_SKEncoder
set (SKENCODER_FOLDER ${PROJECT_SOURCE_DIR}/SKEncoder)
set(SKENCODER_HEADER
    SKEncoder/NvEncoder/NvEncoder/nvEncodeAPI.h
    SKEncoder/NvEncoder/NvEncoder/NvEncoder.h
    SKEncoder/NvEncoder/NvEncoder/NvEncoderCuda.h
    SKEncoder/NvEncoder/Utils/Logger.h
    SKEncoder/NvEncoder/Utils/NvEncoderCLIOptions.h
    SKEncoder/NvEncoder/Utils/NvCodecUtils.h
    SKEncoder/SKEncoder.h
    #SKEncoder/SKCommon.hpp
)
set(SKENCODER_SOURCE
    SKEncoder/NvEncoder/NvEncoder/NvEncoder.cpp
    SKEncoder/NvEncoder/NvEncoder/NvEncoderCuda.cpp
    SKEncoder/SKEncoder.cpp
    )
source_group( "NV_EXTERNAL_FILES" FILES
    SKEncoder/NvEncoder/NvEncoder/nvEncodeAPI.h
    SKEncoder/NvEncoder/NvEncoder/NvEncoder.h
    SKEncoder/NvEncoder/NvEncoder/NvEncoderCuda.h
    SKEncoder/NvEncoder/Utils/Logger.h
    SKEncoder/NvEncoder/Utils/NvEncoderCLIOptions.h
    SKEncoder/NvEncoder/Utils/NvCodecUtils.h
    SKEncoder/NvEncoder/NvEncoder/NvEncoder.cpp
    SKEncoder/NvEncoder/NvEncoder/NvEncoderCuda.cpp
)
include_directories(
    ${SKENCODER_FOLDER}
    ${SKENCODER_FOLDER}/NvEncoder
)
set(NVCUVID_LIB ${PROJECT_SOURCE_DIR}/SKEncoder/lib/x64/nvcuvid.lib)
#MainFolder
file (GLOB this_headers_h "./*.h")
file (GLOB this_headers_hpp "./*.hpp")
set (MY_HEADER_FILES
	${this_headers_h}
	${this_headers_hpp}
	${GENOBJ_HEADER}
	${SKENCODER_HEADER}
)
file (GLOB this_headers_cpp "./*.cpp")
set (MY_SOURCE_FILES
	OVConfig.ini
	${this_headers_cpp}
	${GENOBJ_SOURCE}
	${SKENCODER_SOURCE}
)
source_group( "SHADER" FILES
    SKOpenGL.hpp
	SKOpenGL.cpp
)
include_directories(
    ${PROJECT_SOURCE_DIR}
)

#Link&Exec
cuda_add_executable(${name}
    ${MY_HEADER_FILES}
    ${MY_SOURCE_FILES}
    )
target_link_libraries(${name}
    ${OpenCV_LIBS}
    ${OPENGL_LIBRARY}
    ${GLFW_LIBRARY}
    cuda
    ${NVCUVID_LIB}
    debug  ${GLEW_LIBRARY_DEBUG}
    optimized  ${GLEW_LIBRARY_RELEASE}
    )

