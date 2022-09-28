cmake_minimum_required(VERSION 3.16.0)
project(grabkeypp VERSION 0.2.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Werror")
set(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

set(SOURCES
  src/raw_mode.cpp
  src/detailed_key.cpp
  src/fd_reader.cpp
  src/fd_poller.cpp
  src/kbd_key_parser.cpp
)

set(HEADERS
  include/detailed_key.h
  include/events_config.h
  include/events_processor.h
  include/fd_poller.h
  include/fd_reader.h
  include/IKeyParser.hpp
  include/kbd_key_parser.h
  include/kbd_keys.h
  include/kbd_reader.h
  include/raw_mode.h
)

add_library(grabkeypp ${SOURCES})

set_target_properties(grabkeypp PROPERTIES
    CXX_STANDARD 17
)

target_include_directories(grabkeypp PUBLIC include)
target_link_libraries(grabkeypp PUBLIC grabkey)

set(INCLUDE_INSTALL_DIR "include/grabkey/")
set(LIBRARY_INSTALL_DIR "lib/grabkey/")
set(CMAKE_DESTINATION_DIR "cmake")

include(CMakePackageConfigHelpers)

configure_package_config_file(GrabKeyPpConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfig.cmake
  INSTALL_DESTINATION ${CMAKE_DESTINATION_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

# Packaging

set(COMPONENT_NAME "cpplib")

install(TARGETS grabkeypp
        DESTINATION ${LIBRARY_INSTALL_DIR}
        COMPONENT ${COMPONENT_NAME})

install(FILES ${HEADERS}
        DESTINATION ${INCLUDE_INSTALL_DIR}/grabkeypp
        COMPONENT ${COMPONENT_NAME})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfigVersion.cmake
        DESTINATION ${CMAKE_DESTINATION_DIR}
        COMPONENT ${COMPONENT_NAME})
