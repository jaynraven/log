cmake_minimum_required(VERSION 3.15)

project(log)

add_definitions("-DUNICODE" "-D_UNICODE")

option(LOG_WITH_TESTS "Compile and run Log test executables" OFF)
option(LOG_WITH_EXAMPLE "Compile Log example" OFF)
option(JSONCPP_STATIC_WINDOWS_RUNTIME "Use static (MT/MTd) Windows runtime" OFF)
option(BUILD_SHARED_LIBS "Build log as a shared library." ON)
option(BUILD_STATIC_LIBS "Build log as a static library." OFF)

include_directories(${PROJECT_SOURCE_DIR}/code)

if(MSVC)
    # Only enabled in debug because some old versions of VS STL generate
    # unreachable code warning when compiled in release configuration.
    add_compile_options($<$<CONFIG:Debug>:/W4>)
    if (JSONCPP_STATIC_WINDOWS_RUNTIME)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

file(GLOB_RECURSE HEAD_FILES code/*.h)
file(GLOB_RECURSE SRC_FILES code/*.c code/*.cc code/*.cpp code/*.hpp)

source_group("HEADER" FILES ${HEAD_FILES})
source_group("SOURCE" FILES ${SRC_FILES})

if(BUILD_SHARED_LIBS)
    add_compile_definitions(LOG_DLL_BUILD)
    set(SHARED_LIB ${PROJECT_NAME}_share)
    add_library(${SHARED_LIB} SHARED ${HEAD_FILES} ${SRC_FILES})
    list(APPEND CMAKE_TARGETS ${SHARED_LIB})
endif()

if(BUILD_STATIC_LIBS)
    set(STATIC_LIB ${PROJECT_NAME}_static)
    add_library(${STATIC_LIB} STATIC ${HEAD_FILES} ${SRC_FILES})
    list(APPEND CMAKE_TARGETS ${STATIC_LIB})
endif()

if (LOG_WITH_TESTS)
    add_executable(log_test_demo ${PROJECT_SOURCE_DIR}/code/src/test.cpp)
endif()

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

install(TARGETS ${CMAKE_TARGETS} 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    OBJECTS DESTINATION ${CMAKE_INSTALL_LIBDIR}
)