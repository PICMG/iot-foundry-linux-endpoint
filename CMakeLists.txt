cmake_minimum_required(VERSION 3.15)
project(iot-foundry-linux-endpoint C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Build options
option(PLDM_SUPPORT "Enable PLDM support" ON)

# Compiler flags
add_compile_options(-g -Og -Wall)

# Download IoTFoundry core template sources
set(CORE_REPO "PICMG/iot-foundry-endpoint")
set(CORE_BRANCH "main")
set(CORE_URL "https://github.com/${CORE_REPO}/archive/refs/heads/${CORE_BRANCH}.tar.gz")

# Create directories for core sources
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/core)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/include/core)

# Custom target to download core sources
add_custom_target(download-core
    COMMAND ${CMAKE_COMMAND} -E echo "Downloading IoTFoundry core sources..."
    COMMAND bash -c "TMPDIR=$$(mktemp -d) && \
        wget -q -O $$TMPDIR/repo.tar.gz ${CORE_URL} && \
        mkdir -p $$TMPDIR/repo && \
        tar -xzf $$TMPDIR/repo.tar.gz -C $$TMPDIR/repo --strip-components=1 && \
        cp -a $$TMPDIR/repo/src/. ${CMAKE_SOURCE_DIR}/src/core/ 2>/dev/null || true && \
        cp -a $$TMPDIR/repo/include/. ${CMAKE_SOURCE_DIR}/include/core/ 2>/dev/null || true && \
        rm -rf $$TMPDIR"
    COMMENT "Fetching core template sources from GitHub"
)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/core
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp
)

# libmctp source files (from submodule)
set(LIBMCTP_SOURCES
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/core.c
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/serial.c
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/alloc.c
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/log.c
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/control.c
    ${CMAKE_SOURCE_DIR}/subprojects/libmctp/crc-16-ccitt.c
)

# Collect core sources (from downloaded template)
file(GLOB CORE_SOURCES "${CMAKE_SOURCE_DIR}/src/core/*.c")

# Platform-specific sources
set(PLATFORM_SOURCES
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/platform.c
)

# Create the endpoint executable
add_executable(endpoint
    ${PLATFORM_SOURCES}
    ${CORE_SOURCES}
    ${LIBMCTP_SOURCES}
)

# Make sure core sources are downloaded before building
add_dependencies(endpoint download-core)

# Add PLDM support if enabled
if(PLDM_SUPPORT)
    target_compile_definitions(endpoint PRIVATE PLDM_SUPPORT)
    # Add libpldm include directory when needed
    target_include_directories(endpoint PRIVATE
        ${CMAKE_SOURCE_DIR}/subprojects/libpldm/include
    )
endif()

# Installation rules
install(TARGETS endpoint DESTINATION bin)

# Clean target for core sources
add_custom_target(clean-core
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/src/core/*
    COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_SOURCE_DIR}/include/core/*
    COMMENT "Cleaning downloaded core sources"
)
