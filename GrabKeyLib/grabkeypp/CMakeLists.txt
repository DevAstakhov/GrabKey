cmake_minimum_required(VERSION 3.0.0)
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
  include/events_config.h
  include/events_filter.h
  include/events_processor_impl.h
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
    POSITION_INDEPENDENT_CODE 1
    CXX_STANDARD 17
)

target_include_directories(grabkeypp PUBLIC include)
target_link_libraries(grabkeypp PUBLIC grabkey)
target_link_libraries(grabkeypp PRIVATE -lpthread)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(INCLUDE_INSTALL_DIR "include/grabkey/grabkeypp/")
set(LIBRARY_INSTALL_DIR "lib/grabkey/")
set(INSTALL_DESTINATION_DIR "${LIBRARY_INSTALL_DIR}/cmake")

include(CMakePackageConfigHelpers)

configure_package_config_file(GrabKeyPpConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfig.cmake
  INSTALL_DESTINATION ${INSTALL_DESTINATION_DIR}
  PATH_VARS INCLUDE_INSTALL_DIR LIBRARY_INSTALL_DIR)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion)

install(TARGETS grabkeypp DESTINATION ${LIBRARY_INSTALL_DIR})
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfig.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/GrabKeyPpConfigVersion.cmake
        DESTINATION ${INSTALL_DESTINATION_DIR})