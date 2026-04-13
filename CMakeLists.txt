cmake_minimum_required(VERSION 3.23)
project(hprint VERSION 1.1.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Header only
add_library(hprint INTERFACE)

target_include_directories(hprint INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(hprint INTERFACE cxx_std_23)

add_library(hp::hprint ALIAS hprint)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS hprint
        EXPORT hprintTargets
)

install(EXPORT hprintTargets
        NAMESPACE hp::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hprint
)

configure_package_config_file(
        cmake/hprintConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/hprintConfig.cmake
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hprint
)

write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/hprintConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)

install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hprint
        ${CMAKE_CURRENT_SOURCE_DIR}/include/hprint.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Demo
add_executable(hprint_demo example/demo.cpp)
target_link_libraries(hprint_demo PRIVATE hprint)