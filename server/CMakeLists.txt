cmake_minimum_required(VERSION 3.5.0)
project(Server VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "hdr/*.h" "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../net/hdr)

add_executable(Server ${SRC_FILES})
target_link_libraries(Server PRIVATE Net)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
