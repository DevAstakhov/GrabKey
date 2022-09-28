cmake_minimum_required(VERSION 3.16.0)

project(grabkeylib)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(grabkey)
add_subdirectory(grabkeypp)

include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PACKAGE_FILE_NAME "GrabKey")
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

cpack_add_component("clib"
    DISPLAY_NAME "C library"
    DESCRIPTION "Low level library for file descriptor polling."
    GROUP clib
    )

cpack_add_component("cpplib"
    DISPLAY_NAME "C++ library"
    DESCRIPTION "High level library for parsing and processing ANSI sequence from STDIN."
    GROUP cpplib
    DEPENDS "clib"
    )
