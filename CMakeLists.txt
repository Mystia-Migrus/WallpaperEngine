# *************************************************************
#	该文件为适用于 Direct3D 12 项目构建的 CMake 构建文件
# *************************************************************

# 最低 CMake 版本需求
cmake_minimum_required(VERSION 4.0)
project(WallParperEngine)

# 内部变量配置
set(WINDOWS_SDK_PATH "D:\\Windows Kits\\10")
set(WINDOWS_SDK_VERSION "10.0.19041.0")

set(PROJECT_NAME "WallParperEngine")
set(PROJECT_VERSION "1.0.0")
set(PROJECT_ROOT_PATH ${CMAKE_SOURCE_DIR})
set(PROJECT_BIN_PATH ${CMAKE_BINARY_DIR})
set(PROJECT_TOOLS_ROOT "D:/Programme/repos/BE/utils")
set(PROJECT_FRAME_ROOT "${PROJECT_TOOLS_ROOT}/D3DFrame")
set(PROJECT_D3D12XTK_ROOT "${PROJECT_TOOLS_ROOT}/DirectXTK")
set(PROJECT_D3D12X_ROOT "${PROJECT_TOOLS_ROOT}/directx")
set(WINDOWS_SDK_INCLUDE_ROOT_PATH "${WINDOWS_SDK_PATH}/include/${WINDOWS_SDK_VERSION}")
set(WINDOWS_SDK_LIBRARY_ROOT_PATH "${WINDOWS_SDK_PATH}/lib/${WINDOWS_SDK_VERSION}")

# 外部库
list(APPEND BUILD_ALL_LIBRARIES "${WINDOWS_SDK_LIBRARY_ROOT_PATH}/um/x64")
list(APPEND BUILD_ALL_LIBRARIES "${WINDOWS_SDK_LIBRARY_ROOT_PATH}/ucrt/x64")
list(APPEND BUILD_ALL_LIBRARIES "${WINDOWS_SDK_LIBRARY_ROOT_PATH}/um/x86")
list(APPEND BUILD_ALL_LIBRARIES "${WINDOWS_SDK_LIBRARY_ROOT_PATH}/ucrt/x86")
list(APPEND BUILD_ALL_LIBRARIES "${PROJECT_D3D12XTK_ROOT}/libs")

# 外部头文件
list(APPEND BUILD_ALL_INCLUDES "${WINDOWS_SDK_INCLUDE_ROOT_PATH}/um")
list(APPEND BUILD_ALL_INCLUDES "${WINDOWS_SDK_INCLUDE_ROOT_PATH}/ucrt")
list(APPEND BUILD_ALL_INCLUDES "${WINDOWS_SDK_INCLUDE_ROOT_PATH}/shared")
list(APPEND BUILD_ALL_INCLUDES "${WINDOWS_SDK_INCLUDE_ROOT_PATH}/winrt")
list(APPEND BUILD_ALL_INCLUDES "${WINDOWS_SDK_INCLUDE_ROOT_PATH}/cppwinrt")
list(APPEND BUILD_ALL_INCLUDES "${PROJECT_ROOT_PATH}/includes")
list(APPEND BUILD_ALL_INCLUDES "${PROJECT_TOOLS_ROOT}")
list(APPEND BUILD_ALL_INCLUDES "${PROJECT_FRAME_ROOT}")
list(APPEND BUILD_ALL_INCLUDES "${PROJECT_D3D12XTK_ROOT}/includes")
list(APPEND BUILD_ALL_INCLUDES "${PROJECT_D3D12X_ROOT}")
# 内部库
aux_source_directory(${PROJECT_FRAME_ROOT} BUILD_LIBS_SOURCES)

# 全局编译选项配置
if(MSVC)
    add_compile_options(/FS /utf-8)
endif()

# 添加头文件与链接文件
include_directories(${BUILD_ALL_INCLUDES} ${CMAKE_BINARY_DIR})
link_directories(${BUILD_ALL_LIBRARIES})

# 项目源文件
file(GLOB BUILD_ALL_SOURCES "${PROJECT_ROOT_PATH}/src/*.cpp")
file(GLOB BUILD_ALL_TEST_SOURCES "${PROJECT_ROOT_PATH}/test/*.cpp")

# 项目子程序库
set(PROJECT_SUBDIRECTORY_LIBRARIES "")
file(GLOB SRC_SUBDIRS CONFIGURE_DEPENDS LIST_DIRECTORIES true
    "${PROJECT_ROOT_PATH}/src/*"
)

foreach(SRC_SUBDIR IN LISTS SRC_SUBDIRS)
    if(IS_DIRECTORY "${SRC_SUBDIR}" AND EXISTS "${SRC_SUBDIR}/CMakeLists.txt")
        add_subdirectory("${SRC_SUBDIR}")
    endif()
endforeach()
# *********************************************
# 处理源文件

# 打印基本信息
message(STATUS "--------------- BASE INFO -----------------")
message(STATUS "Windows SDK Path: ${WINDOWS_SDK_PATH}")
message(STATUS "Windows SDK Version: ${WINDOWS_SDK_VERSION}")
message(STATUS "Proejct Tools Path: ${PROJECT_TOOLS_ROOT}")
message(STATUS "Project Path: ${PROJECT_ROOT_PATH}")
message(STATUS "Project Binary Path: ${PROJECT_BIN_PATH}")

message(STATUS "------------- PROJECT INFO ----------------")
message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Version: ${PROJECT_VERSION}")

message(STATUS "---------Building source files...----------")

configure_file(
	"${PROJECT_ROOT_PATH}/config.h.in"
	"${PROJECT_BIN_PATH}/config.h"
)


# 编译内部库
add_library(D3D12Frame STATIC ${BUILD_LIBS_SOURCES})
set_target_properties(D3D12Frame PROPERTIES
    COMPILE_PDB_NAME "D3D12Frame"
    COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BIN_PATH}/pdb/$<CONFIG>"
)

# 编译源文件
# 主程序
add_executable(${PROJECT_NAME} ${BUILD_ALL_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    COMPILE_PDB_NAME "${PROJECT_NAME}"
    COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BIN_PATH}/pdb/$<CONFIG>"
)

target_link_options(${PROJECT_NAME} PRIVATE "/Subsystem:WINDOWS")

# 测试程序
add_executable("${PROJECT_NAME}_Test" ${BUILD_ALL_TEST_SOURCES})

set_target_properties("${PROJECT_NAME}_Test" PROPERTIES
    COMPILE_PDB_NAME "${PROJECT_NAME}_Test"
    COMPILE_PDB_OUTPUT_DIRECTORY "${PROJECT_BIN_PATH}/pdb/$<CONFIG>"
)

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		target_link_libraries(${PROJECT_NAME} ${PROJECT_SUBDIRECTORY_LIBRARIES} D3D12Frame DirectXTKx64_Release.lib)
	else()
		target_link_libraries(${PROJECT_NAME} ${PROJECT_SUBDIRECTORY_LIBRARIES} D3D12Frame DirectXTKx64.lib)
	endif()
else()
	if("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
		target_link_libraries(${PROJECT_NAME} ${PROJECT_SUBDIRECTORY_LIBRARIES} D3D12Frame DirectXTK_Release.lib)
	else()
		target_link_libraries(${PROJECT_NAME} ${PROJECT_SUBDIRECTORY_LIBRARIES} D3D12Frame DirectXTK.lib)
	endif()
endif()