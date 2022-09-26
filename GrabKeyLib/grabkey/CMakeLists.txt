cmake_minimum_required(VERSION 3.16.0)

project(grabkey VERSION 0.2.0)

set(SOURCES
    src/terminal.c
)

set(HEADERS
    include/fd_monitor.h
    include/terminal.h
)

include(CheckSymbolExists)

check_symbol_exists(epoll_wait "sys/epoll.h" HAVE_EPOLL)
check_symbol_exists(kqueue "sys/event.h" HAVE_KQUEUE)

if (${HAVE_EPOLL})
    list(APPEND SOURCES src/fd_monitor_epoll.c)
elseif (${HAVE_KQUEUE})
    list(APPEND SOURCES src/fd_monitor_kqueue.c)
else ()
    message(FATAL_ERROR "Neither \"epoll_wait\" nor \"kqueue\" found.")
endif ()

add_library(grabkey ${SOURCES})
target_include_directories(grabkey PUBLIC "include")

set(INCLUDE_INSTALL_DIR "include/grabkey/")
set(LIBRARY_INSTALL_DIR "lib/grabkey/")
set(CMAKE_DESTINATION_DIR "cmake")

include(CMakePackageConfigHelpers)

configure_package_config_file(GrabKeyConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyConfig.cmake
  INSTALL_DESTINATION ${CMAKE_DESTINATION_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Packaging 

set(COMPONENT_NAME "clib")

install(TARGETS grabkey 
        DESTINATION ${LIBRARY_INSTALL_DIR}
        COMPONENT ${COMPONENT_NAME})

install(FILES ${HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/grabkey
        COMPONENT ${COMPONENT_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyConfigVersion.cmake
        DESTINATION ${CMAKE_DESTINATION_DIR}
        COMPONENT ${COMPONENT_NAME})
