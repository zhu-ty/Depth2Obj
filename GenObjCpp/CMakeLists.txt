
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
set(name "GenObjCpp")
project ( ${name} )

# cmake module
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)


# c++ version
set (CMAKE_CXX_STANDARD 11)

set (WIN_LIB_PATH "E:/libs")

#opencv 
if ( WIN32 )
	set(OpenCV_DIR ${WIN_LIB_PATH}/OpenCV/x64/vc14/lib)
	set(OpenCV_CONFIG_PATH ${WIN_LIB_PATH}/OpenCV)
	#set(WIN_HEADER_PATH "${PROJECT_SOURCE_DIR}/winheadfile")
    #include_directories(${WIN_HEADER_PATH})
endif()

# OpenCV
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

file (GLOB headers_h "./*.h")
file (GLOB headers_hpp "./*.hpp")
set (MY_HEADER_FILES
${headers_h}
${headers_hpp}
)

file (GLOB headers_cpp "./*.cpp")
set (MY_SOURCE_FILES
${headers_cpp}
)


include_directories(
	${PROJECT_SOURCE_DIR}	
)

add_executable(${name}
	${MY_HEADER_FILES}
	${MY_SOURCE_FILES}
	)


target_link_libraries(${name}
	${OpenCV_LIBS}
	)
