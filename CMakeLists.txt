#---------------------------------*-CMake-*----------------------------------#
# Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
# See the Celeritas top-level COPYRIGHT file for details.
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
#----------------------------------------------------------------------------#
cmake_minimum_required(VERSION 3.9)
project(evd)

if(DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
  message(STATUS "Set default CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")
endif()

#----------------------------------------------------------------------------#
# Find packages
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------#
# Generate ROOT dictionary
include(${ROOT_USE_FILE})

root_generate_dictionary(RootInterface
  ${PROJECT_SOURCE_DIR}/src/RootData.hh
  MODULE rootdata
  LINKDEF ${PROJECT_SOURCE_DIR}/src/RootInterfaceLinkDef.hh
)

add_library(rootdata SHARED
  ${PROJECT_SOURCE_DIR}/src/RootData.hh RootInterface.cxx
)

target_link_libraries(rootdata PUBLIC ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------#
# Add executable
add_executable(evd main.cc
  src/MainViewer.cc
  src/EventViewer.cc
  src/MCTruthViewerInterface.cc
  src/RootDataViewer.cc
)

target_compile_features(evd PUBLIC cxx_std_14)

target_include_directories(evd PRIVATE
  $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${ROOT_INCLUDE_DIR}>
)

target_link_libraries(evd PRIVATE
  ROOT::Core
  ROOT::Tree
  ROOT::Eve
  ROOT::Rint
  rootdata
)
