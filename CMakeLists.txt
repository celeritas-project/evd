#-----------------------------------------------------------------------------#
# \file   CMakeLists.txt
# \author Stefano Tognini
# \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
#-----------------------------------------------------------------------------#

#------------------------------------------------------------------------------
# Set up the project
#------------------------------------------------------------------------------
project(Evd LANGUAGES CXX)

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

#------------------------------------------------------------------------------
# Find packages
#------------------------------------------------------------------------------
find_package(ROOT REQUIRED)

#----------------------------------------------------------------------------#
# Generate ROOT dictionary
#----------------------------------------------------------------------------#
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

#------------------------------------------------------------------------------
# Add the executable and link it to the needed libraries
#------------------------------------------------------------------------------
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

#------------------------------------------------------------------------------
# Install the executable to the current directory
#------------------------------------------------------------------------------
install(TARGETS evd DESTINATION .)
