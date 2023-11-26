cmake_minimum_required(VERSION 3.5.0)
project(Net VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "hdr/*.h" "src/*.cpp")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/hdr)
add_library(Net ${SRC_FILES})
if(WIN32)
target_link_libraries(Net PUBLIC ws2_32)
endif()
target_compile_features(Net PUBLIC cxx_std_20)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
