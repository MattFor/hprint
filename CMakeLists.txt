cmake_minimum_required(VERSION 3.23)

project(hprint
        VERSION 1.2.0
        DESCRIPTION "Minimal, header-only C++ utility for printing centered terminal headers"
        HOMEPAGE_URL "https://github.com/MattFor/hprint"
        LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

option(HPRINT_BUILD_EXAMPLES "Build hprint demo" ${PROJECT_IS_TOP_LEVEL})
option(HPRINT_BUILD_TESTS "Build hprint tests" ${PROJECT_IS_TOP_LEVEL})
option(HPRINT_INSTALL "Generate hprint install target" ${PROJECT_IS_TOP_LEVEL})

# Header only
add_library(hprint INTERFACE)
add_library(hp::hprint ALIAS hprint)

target_include_directories(hprint INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_features(hprint INTERFACE cxx_std_23)

# Demo
if (HPRINT_BUILD_EXAMPLES)
    add_executable(hprint_demo example/demo.cpp)
    target_link_libraries(hprint_demo PRIVATE hp::hprint)
endif ()

# Tests
if (HPRINT_BUILD_TESTS)
    enable_testing()

    add_executable(hprint_tests test/test_hprint.cpp)
    target_link_libraries(hprint_tests PRIVATE hp::hprint)

    add_test(NAME hprint_tests COMMAND hprint_tests)
endif ()

# Install
if (HPRINT_INSTALL)
    install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(TARGETS hprint EXPORT hprintTargets)

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
            ARCH_INDEPENDENT
    )

    install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/hprintConfig.cmake
            ${CMAKE_CURRENT_BINARY_DIR}/hprintConfigVersion.cmake
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/hprint
    )
endif ()
