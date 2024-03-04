cmake_minimum_required(VERSION 3.16.3)
project(simOpenMesh)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_MACOSX_RPATH 1)

find_package(Boost REQUIRED COMPONENTS system serialization)

if(NOT COPPELIASIM_INCLUDE_DIR)
    if(DEFINED ENV{COPPELIASIM_ROOT_DIR})
        set(COPPELIASIM_INCLUDE_DIR $ENV{COPPELIASIM_ROOT_DIR}/programming/include)
    else()
        set(COPPELIASIM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
    endif()
endif()
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
    ${COPPELIASIM_INCLUDE_DIR}/cmake)
find_package(CoppeliaSim 4.5.1.7 REQUIRED)
find_package(Eigen3 REQUIRED)

if(WIN32)
    # install openmesh via vcpkg
    find_package(openmesh REQUIRED)
else()
    include(ExternalProject)
    set(OPENMESH_VERSION 8.1)
    set(OPENMESH_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/OpenMesh-${OPENMESH_VERSION}-prefix)
    set(OPENMESH_SRC_DIR ${OPENMESH_PREFIX}/src/OpenMesh-${OPENMESH_VERSION})
    set(OPENMESH_BUILD_DIR ${OPENMESH_PREFIX}/src/OpenMesh-${OPENMESH_VERSION}-build)
    ExternalProject_Add(OpenMesh-${OPENMESH_VERSION}
        URL https://www.graphics.rwth-aachen.de/media/openmesh_static/Releases/${OPENMESH_VERSION}/OpenMesh-${OPENMESH_VERSION}.tar.gz
        CMAKE_ARGS -DCMAKE_C_FLAGS=-fPIC -DCMAKE_CXX_FLAGS=-fPIC -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE} -DBUILD_APPS=OFF
        BUILD_COMMAND ${CMAKE_COMMAND} --build ${OPENMESH_BUILD_DIR} --config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND ""
    )
    set(OPENMESH_INCLUDE_DIRS ${OPENMESH_SRC_DIR}/src)
    string(TOLOWER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_L)
    if(CMAKE_BUILD_TYPE_L MATCHES debug)
        set(OPENMESH_DEBUG_SUFFIX d)
    endif()
    set(OPENMESH_LIBRARIES
        ${OPENMESH_BUILD_DIR}/Build/lib/${CMAKE_SHARED_LIBRARY_PREFIX}OpenMeshCore${OPENMESH_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX}
        ${OPENMESH_BUILD_DIR}/Build/lib/${CMAKE_SHARED_LIBRARY_PREFIX}OpenMeshTools${OPENMESH_DEBUG_SUFFIX}${CMAKE_STATIC_LIBRARY_SUFFIX})
endif()

include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/external)
include_directories(${OPENMESH_INCLUDE_DIRS})

coppeliasim_add_plugin(
    simOpenMesh
    LEGACY
    SOURCES
    sourceCode/simOpenMesh.cpp
    ${COPPELIASIM_INCLUDE_DIR}/simLib/scriptFunctionData.cpp
    ${COPPELIASIM_INCLUDE_DIR}/simLib/scriptFunctionDataItem.cpp
)
target_compile_definitions(simOpenMesh PRIVATE SIM_MATH_DOUBLE)
if(NOT WIN32)
    add_dependencies(simOpenMesh OpenMesh-${OPENMESH_VERSION})
endif()
target_link_libraries(simOpenMesh PRIVATE ${OPENMESH_LIBRARIES})
coppeliasim_add_lua(lua/simOpenMesh.lua)
coppeliasim_add_lua(lua/simOpenMesh-ce.lua)
