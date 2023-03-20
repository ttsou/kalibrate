

# Prevent in-tree builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree build. This is bad practice.")
endif(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})


########################################################################
# Project setup
########################################################################
cmake_minimum_required(VERSION 3.5)
project(KAL)
message(STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})
message(STATUS "CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules")


# Find dependencies

# FFT
find_package(FFTW3 REQUIRED)
include_directories(${FFTW3_INCLUDE_DIRS})
link_directories(${FFTW3_LIBRARY_DIRS})


# UHD
find_package(UHD REQUIRED)
if(UHD_FOUND)
  include_directories(${UHD_INCLUDE_DIRS})
  link_directories(${UHD_LIBRARY_DIRS})
endif(UHD_FOUND)


if(UHD_FOUND) # Ubuntu 18.04 requires component 'system' to link UHD
  list(APPEND BOOST_REQUIRED_COMPONENTS "system")
endif(UHD_FOUND)



########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION}")

########################################################################

add_subdirectory(src)
