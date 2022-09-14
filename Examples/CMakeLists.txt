cmake_minimum_required(VERSION 3.16.0)
project(grabkey_examples)

add_executable(grabtest src/grabkey.c)
target_link_libraries(grabtest PRIVATE grabkey pthread)

add_executable(grabtestpp src/grabkeypp.cpp)
target_link_libraries(grabtestpp PRIVATE grabkeypp)
set_target_properties(grabtestpp PROPERTIES CXX_STANDARD 17)