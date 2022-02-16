#---------------------------------*-CMake-*----------------------------------#
# Copyright 2020-2022 UT-Battelle, LLC, and other Celeritas developers.
# See the Celeritas top-level COPYRIGHT file for details.
# SPDX-License-Identifier: (Apache-2.0 OR MIT)
#----------------------------------------------------------------------------#
project(EVD)

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

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
add_executable(evd main.cpp 
  src/Evd.cpp src/Evd.hh
)

target_include_directories(evd PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(evd PRIVATE
  ROOT::Core
  ROOT::Eve
  ROOT::Rint
  rootdata
)
