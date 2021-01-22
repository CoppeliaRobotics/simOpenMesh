cmake_minimum_required(VERSION 3.15)
project(simExtOpenMesh)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

find_package(Boost REQUIRED COMPONENTS system serialization)

if(NOT LIBPLUGIN_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(LIBPLUGIN_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/libPlugin)
    else()
        set(LIBPLUGIN_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../libPlugin)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
    ${LIBPLUGIN_DIR}/cmake)
find_package(CoppeliaSim 4.1.0.0 REQUIRED)
find_package(Eigen3 REQUIRED)

include(ExternalProject)
set(OPENMESH_VERSION 8.1)
set(OPENMESH_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenMesh-${OPENMESH_VERSION}-prefix)
set(OPENMESH_SRC_DIR ${OPENMESH_PREFIX}/src/OpenMesh-${OPENMESH_VERSION})
set(OPENMESH_BUILD_DIR ${OPENMESH_PREFIX}/src/OpenMesh-${OPENMESH_VERSION}-build)
ExternalProject_Add(OpenMesh-${OPENMESH_VERSION}
    URL https://www.graphics.rwth-aachen.de/media/openmesh_static/Releases/${OPENMESH_VERSION}/OpenMesh-${OPENMESH_VERSION}.tar.gz
    CMAKE_ARGS -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${OPENMESH_BUILD_DIR} --config ${CMAKE_BUILD_TYPE}
    INSTALL_COMMAND ""
)

set(OPENMESH_INCLUDE_DIRS ${OPENMESH_SRC_DIR}/src)
set(OPENMESH_LINK_LIBRARY
    ${OPENMESH_BUILD_DIR}/Build/lib/${CMAKE_SHARED_LIBRARY_PREFIX}OpenMeshCore${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${OPENMESH_BUILD_DIR}/Build/lib/${CMAKE_SHARED_LIBRARY_PREFIX}OpenMeshTools${CMAKE_STATIC_LIBRARY_SUFFIX})

include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
include_directories(${OPENMESH_INCLUDE_DIRS})

coppeliasim_add_plugin(
    simExtOpenMesh
    LEGACY
    SOURCES
    simExtOpenMesh.cpp
    ${COPPELIASIM_COMMON_DIR}/scriptFunctionData.cpp
    ${COPPELIASIM_COMMON_DIR}/scriptFunctionDataItem.cpp
)
add_dependencies(simExtOpenMesh OpenMesh-${OPENMESH_VERSION})
target_link_libraries(simExtOpenMesh ${OPENMESH_LINK_LIBRARY})
